#ifndef LINKER_H
#define LINKER_H

class Linker;

#define MAX_PARTIAL_MESSAGES 100

#include "../../../Common/Runable/H/runable.h"
#include "../../../Common/Shutdownable/H/shutdownable.h"

#include "../../MixOffspring/H/mixOffspring.h"

#include <vector>
#include <map>

/**
 * Chunk of data.
 *
 * Represents one chunk of data as it was created by Chopper
 *
 * @author  Venca
 * @see     Linker, PartialMessage, PartialMessages, Chopper
 */
class ChunkData
{
  public:

  /**@name methods */
  /*@{*/
    bool present;
    Size length;
      // the real chunk's data 
    char * data;
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
    ChunkData();
  /*@}*/
};

/**
 * Chunks that already arrived.
 *
 * Stores those chunks of message that alredy arrived. When  the
 * message is complete it can be joined together. 
 *
 * @author  Venca
 * @see     ChunkData, Linker
 */
typedef vector<ChunkData *> PresentChunks;

/**
 * Partially received message.
 *
 * Represents all the received chunks, size of complete message,
 * total number of chunks.
 *
 * @author  Venca
 * @see     Linker, ChunkData, PartialMessage, PartialMessages
 */
class PartialMessage
{
  public:
     /// received chunks
    ChunkData ** presentChunks;

     /// Info about complete message
    Size messageSize;
    Size totalChunks;

  public:
    PartialMessage(Size msgSize, Size numOfChunks);
    ~PartialMessage();
};

/**
 * Partially received messages.
 *
 * Represents all the received messages.
 *
 * @author  Venca
 * @see     Linker, ChunkData, PartialMessage, PartialMessages
 */
typedef map<ID, PartialMessage *, less<ID> > PartialMessages;

/**
 * Messages linker.
 *
 * Collects chunks of messages (created by chopper) as they arrive,
 * stores them until the message is complete and then joins the chunks
 * so that it reconstruct the original message.
 *
 * @author  Venca
 * @see     ChunkData, PartialMessage, PartialMessages
 */
class Linker:public Runable,public MixOffspring,public Shutdownable
{
  protected:
  /**@name attributes */
  /*@{*/
     // messages in progress
    PartialMessages * partialMessages;
  /*@}*/

  public:
  /**@name methods */
  /*@{*/
    Linker(Mix * parent, LogFile * linkerLogFile);

    void * Run(void *);

      // add chunk to an incomplete message
    GMessage * StoreChunk( GMessage *chunk);
    virtual void Shutdown(); 
  /*@}*/
};
#endif
