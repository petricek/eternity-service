  // has to be the FIRST include !
#include "./../../Common/H/common.h"

#include "./../../Common/H/labels.h"

#include "./H/linker.h"

/**
 * Chunk of Data constructor.
 *
 * Initializes length of chunk data to 0 present sign to false and 
 * pointer to data to NULL.
 *
 * @author  Venca
 * @see     PartialMessage
 */
ChunkData::ChunkData()
{
  data = NULL;
  length = 0;
  present = false;
}

/**
 * Mix linker constructor.
 *
 * Allocates a new object of type PartialMessages for storing chunks
 * for different messages.
 *
 * @param   parent Pointer to Mix object.
 * @param   parent linkerLogFile Log file.
 * @author  Venca
 * @see     PartialMessage
 */
Linker::Linker(Mix * parent, LogFile * linkerLogFile)
  :Runable(linkerLogFile), MixOffspring(parent), Shutdownable(parent->killer)
{
  partialMessages = new PartialMessages;
  WriteString(llDebug, __FILE__ ":%d:Linker created.", __LINE__); 
}

/**
 * Mix linker Run() method.
 *
 * Processes all messages from the incoming queue and inserts them
 * into its internal structures. Has to watch if a message is already
 * completed and then it kinks all its chunks together and passes the
 * final message further to Majordomo for processing.
 *
 * @return  void pointer
 * @author  Venca
 * @see     Chopper, Majordomo
 */
void * Linker::Run(void *)
{
  GMessage * message;

  MaskAllSignals();

  WriteString(llDebug, __FILE__ ":%d:Entering Linker->Run().",__LINE__); 

  for ( ; ; ) {

    mix->translatorToLinker->GetMsg(&message);
    message = StoreChunk(message); 

    if(message != NULL) {
      mix->linkerChopperToMajordomo->Append(message);
    }
  }

  return NULL;
}

/**
 * Store chunk of message into internal structures.
 *
 * Actually performs the task of storing message into its internal
 * structures, checking for Message ID conflicts and checking if the
 * message is already complete. If that is true than a completely
 * linked message is returned.
 *
 * @param   chunk Message from network that represents one chunk of
 *                original message and as such has to be linked with
 *                all the others to reconstruct it.
 * @return  Completely linked message if any else NULL.
 * @author  Venca
 * @see     Majordomo::Run(), Chopper, Chopper::CreateChunks()
 */
GMessage * Linker::StoreChunk(GMessage * chunk)
{

  char *xDestination = chunk->GetAsString(GM_X_DESTINATION);
  char *protocolVersion = chunk->GetAsString(GM_PROTOCOL_VERSION);
  char *origin = chunk->GetAsString(GM_ORIGIN);
  //char *debug = chunk->GetAsString(GM_DEBUG);

  WriteString(llDebug, __FILE__":%d:What is in the chunk we are to link?",
              __LINE__);
  chunk->DebugHeaders(logFile);
  
  GMessage *gMsgChunkData = chunk->GetAsGMessage(GM_DATA);
  DELETE(chunk);
  WriteString(llDebug, __FILE__":%d:What is in a GM_DATA section extracted "
                                "from the chunk?", __LINE__);
  gMsgChunkData->DebugHeaders(logFile);

    // Get values from chunk
  ID *chunkID = (ID *) gMsgChunkData->GetAsBytes(GM_CHUNK_ID);
  ID *messageID = (ID *) gMsgChunkData->GetAsBytes(GM_MESSAGE_ID);
  Size *totalChunks = (Size *) gMsgChunkData->GetAsBytes(GM_TOTAL_CHUNKS);
  Size *messageSize = (Size *) gMsgChunkData->GetAsBytes(GM_MESSAGE_LENGTH);

  Size len = 0;
  char *data = NULL;
  data = (char *)gMsgChunkData->GetAsBytes(GM_DATA, &len);
  DELETE(gMsgChunkData);

  WriteString(llDebug, __FILE__":%d:Length of a message the chunk "
              "belongs to is %lu." , __LINE__, *messageSize);

    // If it is first chunk that arrived from this messageID
  if (partialMessages->find(*messageID) == partialMessages->end())
  {
      // Create a new partial message for chunks that will arrive
    (*partialMessages)[*messageID] = new PartialMessage(*messageSize, 
                                                        *totalChunks);
    WriteString(llDebug, __FILE__ ":%d:Foundation of a new message with "
                "id %lu", __LINE__, *messageID);
  }
  
    // Store this chunk in partial message
  ChunkData *currentChunk = new ChunkData(); 

  currentChunk->present = true;
  currentChunk->length = len;
  currentChunk->data = data;
  PartialMessage * currentMessage = NULL;
  currentMessage = (*partialMessages)[*messageID];
  (currentMessage->presentChunks)[*chunkID] = currentChunk;

  WriteString(llDebug,__FILE__ ":%d:Message chunk received - id: "
              "%lu, chunk: %lu", __LINE__, *messageID, *chunkID);
  DELETE(chunkID);

    // Test if this message is complete
  
  Size chunkNum = 0;
  while (chunkNum < *totalChunks && 
        ((currentMessage->presentChunks))[chunkNum]->present == true) {
    chunkNum++;
  }

  char * completeData = NULL;

    // If all the chunks are present
  if (chunkNum == *totalChunks)
  {
    completeData = new char[*messageSize];
    char * lastFull = completeData;
    
     // Join all the chunks into one
    chunkNum = 0;
    while (chunkNum < *totalChunks - 1) {
      Size   chunkLen  = (currentMessage->presentChunks)[chunkNum]->length;
      char * chunkData = (currentMessage->presentChunks)[chunkNum]->data;
      WriteString(llDebug,__FILE__ "StoreChunk():%d: Joining Chunk "
                  "number:%lu,len:%lu" , __LINE__, chunkNum, chunkLen);
      memcpy(lastFull, chunkData, chunkLen);
      DELETE(chunkData);

      chunkNum++;
      lastFull += chunkLen;
    }

    DELETE(totalChunks);

      // process the last, probably not fully aligned chunk
    Size chunkLen  = (currentMessage->presentChunks)[chunkNum]->length;
    char * chunkData = (currentMessage->presentChunks)[chunkNum]->data;
    WriteString(llDebug,__FILE__ ":%d: Joining last Chunk "
                "number: %lu, len: %lu/%lu", __LINE__, chunkNum, chunkLen, 
		 *messageSize - (lastFull - completeData));
    memcpy(lastFull, chunkData, *messageSize - (lastFull - completeData));
    DELETE(chunkData);

    chunkNum++;
    lastFull += chunkLen;

      // create message for Majordomo
    WriteString(llDebug,":%d:, messageSize is %lu" , __LINE__, *messageSize);
    GMessage * message = new GMessage();
    message->SetAsBytes(GM_DATA, completeData, *messageSize);
    DELETE(completeData); DELETE(messageSize);
    message->SetAsString(GM_X_DESTINATION, xDestination);
    DELETE(xDestination);
    message->SetAsString(GM_ORIGIN, origin);
    DELETE(origin);
    message->SetAsString(GM_PROTOCOL_VERSION, protocolVersion);
    DELETE(protocolVersion);
    //message->SetAsString(GM_DEBUG, debug.c_str);

    WriteString(llDebug,__FILE__ ":%d:Message id %lu linked.", __LINE__, 
                *messageID);
    DELETE(messageID);

    //DELETE(partialMessages)[messageID];

    WriteString(llDebug,__FILE__ ":%d:What contains the completely "
                "linked message?", __LINE__);
    message->DebugHeaders(logFile);
    return message; 
  }
  else {
    DELETE(messageID);
    DELETE(messageSize);
    DELETE(totalChunks);
    return NULL;
  }
}

/**
 * Partial message constructor.
 *
 * Constricts an internal structure for linker that will hold all
 * the chunks of a message.
 *
 * @author  Venca
 * @param   msgSize     Total size of the original message
 * @param   numOfChunks Number of chunks into which the original message
 *                      was split.
 * @see     PartialMessages, StoreChunk()
 */
PartialMessage::PartialMessage(Size msgSize, Size numOfChunks)
{
  presentChunks = new (ChunkData *)[numOfChunks];

  for (int i = numOfChunks; i > 0; i--) {
    presentChunks[i - 1] = new ChunkData;
  }

  totalChunks = numOfChunks;
  messageSize = msgSize;
}

/**
 * Prepare for clean shutdown.
 *
 * This method should implement all the tasks necessary for
 * preparation for a clean shutdown like saving inconsistent data to
 * disk etc. Now it is empty as Linker is completely stateless as the
 * loss of some partially received messages is not fatal.
 *
 * return   void
 * @author  Venca
 * @see     Shutdownable, Killer
 */
void Linker::Shutdown()
{
  WriteString(llDebug, __FILE__ ":%d:Linker in Shutdown()", __LINE__);
}
