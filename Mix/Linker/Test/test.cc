#include "../Common/LogFile/H/logFile.h"
#include "../Common/H/commands.h"
#include "../Common/H/labels.h"
#include "../Common/H/common.h"
#include "../Common/ConfigFile/H/configFile.h"
#include "../Common/GMessage/H/gMessage.h"
#include "../Common/MessageQueue/H/messageQueue.h"
#include <string>
#include <list>


#define MAX_PARTIAL_MESSAGES 100
#define CHUNK_LENGTH 1000

#include <vector>
#include <map>


//-------------------------------------------------------------------------
// ChunkData
// ~~~~~~~~~
// Represents one chunk of data
//-------------------------------------------------------------------------
class ChunkData
{
  public:
    bool present;
    Size length;
    char * data;
  public:
    ChunkData();
};

ChunkData::ChunkData()
{
  data = NULL;
  length = 0;
  present = false;
}

typedef vector<ChunkData *> PresentChunks;
//-------------------------------------------------------------------------
// PartialMessage
// ~~~~~~~~~~~~~~
// Represents all by now received chunks from message
//-------------------------------------------------------------------------
class PartialMessage
{
   // properties
  public:
     // received chunks
    //PresentChunks * presentChunks;
    ChunkData ** presentChunks;

     // Info about complete message
    Size messageSize;
    Size totalChunks;

   // methods
  public:
    PartialMessage(Size msgSize, Size numOfChunks);
};

typedef map<ID, PartialMessage *, less<ID> > PartialMessages;
     // messages in progress
    // methods

PartialMessages * partialMessages=NULL;
//-------------------------------------------------------------------------
// StoreChunk
// ~~~~~~~~~~
//-------------------------------------------------------------------------
GMessage * StoreChunk(GMessage * chunk,LogFile * logFile)
{

  chunk->DebugHeaders(logFile,"Chunk to link contains:");
  logFile->WriteString(llDebug,"StoreChunk():%d" , __LINE__);
  
  GMessage *gMsgChunkData = chunk->GetAsGMessage(GM_DATA);
  gMsgChunkData->DebugHeaders(logFile,"*data to link contains:");
  logFile->WriteString(llDebug,"StoreChunk():%d" , __LINE__);
    // Get values from chunk
  Size chunkID = (*(ID *)(gMsgChunkData->GetAsBytes(GM_CHUNK_ID)));
  logFile->WriteString(llDebug,"StoreChunk():%d" , __LINE__);
  ID messageID = (*(ID *)(gMsgChunkData->GetAsBytes(GM_MESSAGE_ID)));
  logFile->WriteString(llDebug,"StoreChunk():%d" , __LINE__);
  Size totalChunks = (*(Size *)(gMsgChunkData->GetAsBytes(GM_TOTAL_CHUNKS)));
  logFile->WriteString(llDebug,"StoreChunk():%d" , __LINE__);
  Size messageSize = (*(Size *)(gMsgChunkData->GetAsBytes(GM_MESSAGE_LENGTH)));
  logFile->WriteString(llDebug,"StoreChunk():%d:, messageSize:%ld" , __LINE__, messageSize);
  logFile->WriteString(llDebug,"StoreChunk():%d" , __LINE__);

  // If it is first chunk that arrived from this messageID
  if(partialMessages->find(messageID) == partialMessages->end())
  {
    // Create a new partial message for chunks that will arrive
    (*partialMessages)[messageID] = new PartialMessage(messageSize, totalChunks);
    logFile->WriteString(llDebug, 
                __FILE__ ":%d:Message founded - id: %ld", __LINE__, messageID );
  }
  logFile->WriteString(llDebug,"StoreChunk():%d" , __LINE__);
  
    // Store this chunk in partial message
  ChunkData * currentChunk = new ChunkData; 
  logFile->WriteString(llDebug,"StoreChunk():%d" , __LINE__);
  Size len = 0;
  char * data = NULL;
  data = (char *)gMsgChunkData->GetAsBytes(GM_DATA, &len);
  logFile->WriteString(llDebug,"StoreChunk():%d" , __LINE__);
  currentChunk->present = true;
  logFile->WriteString(llDebug,"StoreChunk():%d" , __LINE__);
  currentChunk->length = len;
  logFile->WriteString(llDebug,"StoreChunk():%d" , __LINE__);
  currentChunk->data   = data;
  logFile->WriteString(llDebug,"StoreChunk():%d" , __LINE__);
  PartialMessage * currentMessage = NULL;
  currentMessage = (*partialMessages)[messageID];
  logFile->WriteString(llDebug,"StoreChunk():%d" , __LINE__);
  logFile->WriteString(llDebug,"StoreChunk():%d" , __LINE__);
  (currentMessage->presentChunks)[chunkID] = currentChunk;

  logFile->WriteString(llDebug,__FILE__ ":%d:Message chunk received - id: %ld chunk: %ld", 
              __LINE__, messageID, chunkID);

    // Test if this message is complete
  
  Size chunkNum = 0;
  while (chunkNum < totalChunks && 
        ((currentMessage->presentChunks))[chunkNum]->present == true)
  {
    chunkNum++;
  }
  logFile->WriteString(llDebug,"StoreChunk():%d" , __LINE__);

  Size completeLen = 0;
  char * completeData = NULL;

    // If all the chunks are present
  if (chunkNum == totalChunks)
  {
    logFile->WriteString(llDebug,"StoreChunk():%d" , __LINE__);
    completeData = new char[messageSize];
    char * lastFull = completeData;
    
    chunkNum = 0;

     // Join all the chunks into one
    while (chunkNum < totalChunks - 1)
    {
      Size   chunkLen  = (currentMessage->presentChunks)[chunkNum]->length;
      char * chunkData = (currentMessage->presentChunks)[chunkNum]->data;
      logFile->WriteString(llDebug,__FILE__ "StoreChunk():%d: Joining Chunk number:%ld,len:%ld" , __LINE__, chunkNum, chunkLen);
      memcpy(lastFull, chunkData, chunkLen);
      DELETE(chunkData);

      chunkNum++;
      lastFull += chunkLen;
    }

    Size   chunkLen  = (currentMessage->presentChunks)[chunkNum]->length;
    char * chunkData = (currentMessage->presentChunks)[chunkNum]->data;
    logFile->WriteString(llDebug,__FILE__ "StoreChunk():%d: Joining last Chunk number:%ld,len:%ld/%ld" ,
                         __LINE__, chunkNum, chunkLen, messageSize - (lastFull - completeData));
    memcpy(lastFull, chunkData, messageSize - (lastFull - completeData));
    DELETE(chunkData);

    chunkNum++;
    lastFull += chunkLen;

    logFile->WriteString(llDebug,"StoreChunk():%d:, messageSize:%ld" , __LINE__, messageSize);

      // create message for Majordomo
    GMessage * message = new GMessage();
    logFile->WriteString(llDebug,"StoreChunk():%d:, messageSize:%ld" , __LINE__, messageSize);
    message->SetAsBytes(GM_DATA, completeData, messageSize);
    logFile->WriteString(llDebug,"StoreChunk():%d" , __LINE__);
    //message->SetAsString(GM_X_DESTINATION, (char *) (xDestination.c_str()));
    //message->SetAsString(GM_ORIGIN, (char *) (origin.c_str()));
    //message->SetAsString(GM_PROTOCOL_VERSION, (char *)(protocolVersion.c_str()));
    //message->SetAsString(GM_DEBUG, (char *)(debug.c_str()));

    logFile->WriteString(llDebug,__FILE__ ":%d:Message linked - id: %ld",
                __LINE__, messageID);

    DELETE(completeData);
    //DELETE(partialMessages)[messageID];

    logFile->WriteString(llDebug,__FILE__ ":%d:Message deleted nad passed - id: %ld", 
                __LINE__, messageID);

    message->DebugHeaders(logFile, "Completely linked message contains:");
    return message; 
  }
  else
  {
    logFile->WriteString(llDebug,"StoreChunk():%d" , __LINE__);
    return NULL;
  }
}

//-------------------------------------------------------------------------
// PartialMessage
// ~~~~~~~~~~~~~~
//-------------------------------------------------------------------------
PartialMessage::PartialMessage(Size msgSize,Size numOfChunks)
{
  presentChunks = new (ChunkData *)[numOfChunks];
  for(int i= numOfChunks; i > 0;i--)
  {
    presentChunks[i-1] = new ChunkData;
  }

  totalChunks = numOfChunks;
  messageSize = msgSize; 
}

//-------------------------------------------------------------------------
// CreateChunks
// ~~~~~~~~~~~~
//-------------------------------------------------------------------------
list< GMessage * > 
CreateChunks(GMessage * parameters, LogFile * logFile)
{                 
  list< GMessage * > chunks;
  GMessage * chunk;
  Size len;
  ID lastMessageID = 7;
  char * data;

   // This data has to be chopped
  data = (char *) parameters->GetAsBytes(GM_DATA, &len);
  MsgField *onion = parameters->GetAsMsgField(GM_ONION);
  MsgField *hops = parameters->GetAsMsgField(GM_ADD_HOPS);

  char * pChunkData = NULL;

  pChunkData = data; 
  Size chunkNum = 0; 
  Size totalChunks = ((len - 1) / CHUNK_LENGTH) + 1; 

  GMessage * chunkData = NULL;

   // chop CHUNK_LENGTH bytes and make a chunk from it
  while( pChunkData <= data + len - CHUNK_LENGTH ) 
  {
    chunk = new GMessage();

     // Add onion and hops
    chunk->SetAsMsgField(GM_ONION, onion);
    chunk->SetAsMsgField(GM_ADD_HOPS, hops);


    chunkData = new GMessage();

     // Data
    chunkData->SetAsBytes(GM_DATA, pChunkData, CHUNK_LENGTH);
    chunkData->SetAsBytes(GM_MESSAGE_ID,    (char *)&lastMessageID,sizeof(ID));
    chunkData->SetAsBytes(GM_CHUNK_ID,      (char *)&chunkNum,     sizeof(Size));
    chunkData->SetAsBytes(GM_MESSAGE_LENGTH,(char *)&len,          sizeof(Size));
    chunkData->SetAsBytes(GM_TOTAL_CHUNKS,  (char *)&totalChunks,  sizeof(Size));

    chunk->SetAsGMessage(GM_DATA, chunkData);
    logFile->WriteString(llDebug,"Chunk number: %d",chunkNum);
    chunk->DebugHeaders(logFile,"Just ceated:");
    chunkData->DebugHeaders(logFile,"data contains:");

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
    //printf("zbytek: %ld\n", len - (pChunkData - data));
    chunkData->SetAsBytes(GM_DATA, rest, CHUNK_LENGTH);
    chunkData->SetAsBytes(GM_MESSAGE_ID,    (char *)&lastMessageID,sizeof(ID));
    chunkData->SetAsBytes(GM_CHUNK_ID,      (char *)&chunkNum,     sizeof(Size));
    chunkData->SetAsBytes(GM_MESSAGE_LENGTH,(char *)&len,          sizeof(Size));
    chunkData->SetAsBytes(GM_TOTAL_CHUNKS,  (char *)&totalChunks,  sizeof(Size));
    chunk = new GMessage();
    chunk->SetAsGMessage(GM_DATA, chunkData);

     // Add onion and hops
    chunk->SetAsMsgField(GM_ONION, onion);
    chunk->SetAsMsgField(GM_ADD_HOPS, hops);
    logFile->WriteString(llDebug,"Chunk number: %d",chunkNum);
    chunk->DebugHeaders(logFile,"Just ceated last chunk:");
    chunkData->DebugHeaders(logFile,"data contains:");

    chunks.push_front(chunk);
  }

  lastMessageID++;

  //DELETE(message);

  return chunks; 
}


void main()
{
      LogFile * logFile = new LogFile("test.log");
      GMessage * parameters = new GMessage();
      MessageQueue * chopperToLinker = new MessageQueue(logFile, 1000);
      parameters->SetAsString(GM_ONION,"Cibule");
      parameters->SetAsString(GM_ADD_HOPS,"Par hopu");
      char * data = new char[20001];
      Size len = 20001;
      for(int i=0;i<len;i++)
      {
        data[i] = i % 10 + 64; 
      }
      parameters->SetAsBytes(GM_DATA,data,len);

      //parameters->DebugHeaders(logFile);
      list< GMessage* > chunks;
      chunks = CreateChunks(parameters, logFile);
      DELETE(parameters);

      while( ! chunks.empty() ) {
	//(chunks.front())->DebugHeaders(logFile, "What is in chunk?");
	// message = mix->msgCreator->CreateChunk(chunks.front());
	chopperToLinker->Append(chunks.front());
	chunks.pop_front();
      }
      
      GMessage * message = NULL;
      GMessage * completeMsg = NULL;
      partialMessages = new PartialMessages;
      while(1)
      {
        chopperToLinker->GetMsg(&message);
        completeMsg = StoreChunk(message,logFile);
	if(completeMsg != NULL)
	{
          completeMsg->DebugHeaders(logFile,"Slepeno:");
          break;
	}
      }
}

