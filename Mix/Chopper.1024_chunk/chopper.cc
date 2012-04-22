  // has to be the FIRST include !
#include "./../../Common/H/common.h"

#include "./../../Common/H/labels.h"
#include "./../../Common/H/commands.h"

#include "./H/chopper.h"

#include <list>

/**
 * Mix chopper constructor.
 *
 * Initializes lastMessageID with a random number so that probability
 * of conflicting Message ID's is minimized. This Message ID is later
 * used for marking chunks of one message as that they  belong
 * together. It also initializes its parent classes.
 *
 * @param   parent Pointer to Mix objects.
 * @param   chopperLogFile Log file pointer.
 * @author  Venca
 * @see     ~Chopper()
 */
Chopper::Chopper(Mix * parent, LogFile * chopperLogFile)
  :Runable(chopperLogFile), MixOffspring(parent), Shutdownable(mix->killer)
{
   // Init message ID counter
  lastMessageID = random();
  logFile->WriteString(llDebug, __FILE__ ":%d: initialized lastMessageID "
                       "= %lu ", __LINE__, lastMessageID);
}

/**
 * Mix chopper Run() method.
 *
 * This methods processes all messages from the incoming queue.  If
 * the message is for local Mix then it is passed to Majordomo for
 * further processing. If it is destined to be sent over the network
 * then the message has to be prepared for transmission. That means
 * splitting it into uniform chunks so that they cannot be
 * distinguished  and then wraping some additional peels onto its
 * onion - in this way the sender is protected against traffic
 * analysis too.
 *
 * @author  Venca
 * @see     MessageCreator, Linker, traffic analysis discussion
 */
void * Chopper::Run(void *)
{
  logFile->WriteString(llDebug, __FILE__ ":%d:Entering Chopper->Run().",
                       __LINE__);

  MaskAllSignals();

  for ( ; ; ) {

    GMessage *message;
    mix->receiverToChopper->GetMsg(&message);

    logFile->WriteString(llDebug, __FILE__ ":%d:Chooper got message.",
                       __LINE__);

    message->DebugHeaders(logFile);

    char *command = message->GetAsString(GM_COMMAND);
    logFile->WriteString(llDebug, __FILE__ ":%d:Chooper got command %s.",
                       __LINE__, command);

    if (!strcmp(command, CMD_SEND_DATA)) {
      GMessage * parameters = message->GetAsGMessage(GM_PARAMETERS);
      //char * origin = (char *) message->GetAsString(GM_ORIGIN);
      //parameters->SetAsString(GM_ORIGIN, origin);
      //DELETE(origin);
      DELETE(message);

      parameters->DebugHeaders(logFile,"Parameters form six");
      list< GMessage* > chunks;
      chunks = CreateChunks(parameters);
      DELETE(parameters);

      while( ! chunks.empty() ) {
	(chunks.front())->DebugHeaders(logFile, "What is in chunk?");
	message = mix->msgCreator->CreateChunk(chunks.front());
	  // Pechy thinks this line isn't necessary
	//message->SetAsString(GM_X_DESTINATION, LBL_MIX_REMOTE);
	mix->chopperTranslatorToPadder->Append(message);
	chunks.pop_front();
      }
    }
    else {
      message->SetAsString(GM_X_DESTINATION, LBL_MIX_LOCAL);
      logFile->WriteString(llDebug, __FILE__ ":%d:Chooper added "
                           "XDestination ,,mix_local''.", __LINE__);
      mix->linkerChopperToMajordomo->Append(message);
    }

    DELETE(command);
  }
  return NULL;
}

/**
 * Prepare for clean shutdown.
 *
 * This method should implement all the tasks necessary for
 * preparation for a clean shutdown like saving inconsistent data to
 * disk etc. Now it is empty as Chopper is completely stateless as the
 * loss of some partially received messages is not fatal to the
 * system.
 *
 * @author  Venca
 * @see     Shutdownable, Killer
 */
void Chopper::Shutdown()
{

}

/**
 * Create chunks from message.
 *
 * Chops a message.
 *
 * <PRE>
 * [Onion|Mix]:
 * AddHops:
 * Data:
 * </PRE>
 *
 * and splits it into multiple chunks of the form:
 *
 * <PRE>
 * [Onion|Mix] 
 * AddHops
 * Data:
 *   Message-ID:
 *   Chunk-ID:
 *   Total-Chunks:
 *   Message-Length:
 *   Data:
 * </PRE>
 *
 * @author  Venca
 * @param   parameters The GMessage has the following structure:
 * <PRE>
 * [Onion|Mix]:
 * AddHops:
 * Data:
 * </PRE>
 *
 * @return  list of chunks of the structure:
 * <PRE>
 * [Onion|Mix] 
 * AddHops
 * Data:
 *   Message-ID:
 *   Chunk-ID:
 *   Total-Chunks:
 *   Message-Length:
 *   Data:
 * </PRE>
 *
 * @see     Linker, StoreChunk(), traffic analysis discussion
 */
list< GMessage * > Chopper::CreateChunks(GMessage * parameters)
{                 
  list< GMessage * > chunks;
  GMessage * chunk;
  Size len;

   // This data has to be chopped, parameters are deleted outside of
   // this method
  parameters->DebugHeaders(logFile, "parameters");
  char *data = (char *) parameters->GetAsBytes(GM_DATA, &len);
  //char *origin = (char *) parameters->GetAsString(GM_ORIGIN);
  MsgField *onion = parameters->GetAsMsgField(GM_ONION);

  if(onion == NULL)
  {
    GMessage *mixcert = parameters->GetAsGMessage(GM_MIX);
    mixcert->DebugHeaders(logFile,"mixCert");
    GMessage * properties = new GMessage();
    char * destination = mixcert->GetAsString(GM_DESTINATION);
    properties->SetAsString(GM_DESTINATION, destination);
    DELETE(destination);
    properties->SetAsGMessage(GM_MIX_CERTIFICATE,mixcert);
    DELETE(mixcert);

    GMessage * gmOnion = NULL;
    gmOnion = mix->msgCreator->CreateOnionFromMixCertificate(properties);
    onion = gmOnion->StoreToMsgField();

    DELETE(gmOnion);
    DELETE(properties);
  }
  MsgField *hops = parameters->GetAsMsgField(GM_ADD_HOPS);

  char * pChunkData = NULL;

  pChunkData = data; 
  Size chunkNum = 0; 
  Size totalChunks = ((len - 1) / CHUNK_LENGTH) + 1;

  GMessage * chunkData = NULL;

   // chop CHUNK_LENGTH bytes and make a chunk from it
  while( pChunkData < data + len - CHUNK_LENGTH ) 
  {
    chunk = new GMessage();

     // Add onion and hops
    chunk->SetAsMsgField(GM_ONION, onion);
    chunk->SetAsMsgField(GM_ADD_HOPS, hops);

    chunkData = new GMessage();

     // Data
    chunkData->SetAsBytes(GM_DATA, pChunkData, CHUNK_LENGTH);
    chunkData->SetAsBytes(GM_MESSAGE_ID, (char *)&lastMessageID,sizeof(ID));
    chunkData->SetAsBytes(GM_CHUNK_ID, (char *)&chunkNum, sizeof(Size));
    chunkData->SetAsBytes(GM_MESSAGE_LENGTH,(char *)&len, sizeof(Size));
    chunkData->SetAsBytes(GM_TOTAL_CHUNKS, (char *)&totalChunks,sizeof(Size));

    chunk->SetAsGMessage(GM_DATA, chunkData);
    logFile->WriteString(llDebug,__FILE__":%d:Chunk number is %d.", __LINE__,
                         chunkNum);
    chunk->DebugHeaders(logFile,"What is in just created chunk?");
    chunkData->DebugHeaders(logFile,"What is in data?");
    DELETE(chunkData);

      // probably push_front doesn't create a copy, so do NOT DELETE(chunk)
      // after the statement
    chunks.push_front(chunk);

    chunkNum++;
    pChunkData += CHUNK_LENGTH;
  }

  if(pChunkData < data + len)
  {
        // Last incomplete chunk
    chunkData = new GMessage();
    char * rest = new char[CHUNK_LENGTH];
    memset(rest, 'w', CHUNK_LENGTH);
    memcpy(rest, pChunkData, len - (pChunkData - data));
    chunkData->SetAsBytes(GM_DATA, rest, CHUNK_LENGTH);
    DELETE(rest);
    chunkData->SetAsBytes(GM_MESSAGE_ID, (char *)&lastMessageID, sizeof(ID));
    chunkData->SetAsBytes(GM_CHUNK_ID, (char *)&chunkNum, sizeof(Size));
    chunkData->SetAsBytes(GM_MESSAGE_LENGTH,(char *)&len, sizeof(Size));
    chunkData->SetAsBytes(GM_TOTAL_CHUNKS, (char *)&totalChunks,sizeof(Size));

    chunk = new GMessage();
    chunk->SetAsGMessage(GM_DATA, chunkData);
    logFile->WriteString(llDebug,__FILE__":%d:What is in just created "
                        "chunk data?", __LINE__);
    chunkData->DebugHeaders(logFile);
    DELETE(chunkData);

     // Add onion and hops
    chunk->SetAsMsgField(GM_ONION, onion);
    chunk->SetAsMsgField(GM_ADD_HOPS, hops);
    logFile->WriteString(llDebug,__FILE__":%d:Chunk number is %d",
                         __LINE__, chunkNum);
    logFile->WriteString(llDebug,__FILE__":%d:What is in just created "
                        "last chunk?", __LINE__);
    chunk->DebugHeaders(logFile);

    chunks.push_front(chunk);
  }

  DELETE(hops); DELETE(onion); DELETE(data);
  //DELETE(origin);

  lastMessageID++;

  return chunks; 
}
