#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#define MSGQ_OUT_OF_RANGE KO
#define MSGQ_FULL KO
#define MSGQ_DEFAULT_BUFFER_LEN 50 

/** for requestType
    SOFT always tries whether the queue is full, in that case it returns
    QUEUE_FULL, HARD would go to sleep */
  /*@{*/
#define HARD 0
#define SOFT 1
  /*@}*/

  // has to be the FIRST include !
#include "./../../H/common.h"

#include <pthread.h>

#include "./../../GMessage/H/gMessage.h"
#include "./../../Debugable/H/debugable.h"
#include "./../../Semaphore/H/semaphore.h"

/**
 * Queue of message pointers.
 *
 * Holds a queue of pointers to messages. It provides access to stored messages in FIFO order
 * and even random access.
 *
 * @author  Marek, Pechy, Venca
 * @see
 */
class MessageQueue:Debugable
{

  private:

  /**@name attributes */
  /*@{*/
      ///
    Size size;
      ///
    Size firstMsg;
      ///
    Size lastMsg;
      ///
    Size msgCount;
    
      /// for synchronization
    struct Semaphore semEmpty, semFull;
      /// for synchronization
    pthread_mutex_t mutex;

      /// array of pointers to messages
    GMessage **messages;
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
      /// Creates a message that can store up to size messages
    MessageQueue(LogFile * queueLogFile, Size size = MSGQ_DEFAULT_BUFFER_LEN);
    /// Destructor
    ~MessageQueue();

      /// Appends a message to the end of queue
    Err Append(GMessage *msg, int requestType = HARD);
      /// Returns the number-th message from queue (removes it from queue)
    Err GetMsg(GMessage **msg, Size number = 0);
      /// Returns a random message from queue (removes it from queue)
    Err GetRandomMsg(GMessage **msg);
      /// Returns the number-th message from queue (leaves it in queue)
    Err ShowMsg(GMessage **msg, Size number = 0);
      /// Returns the number of messages in queue
    Size MsgCount();
      /// Returns the number of free slots in queue
    Size FreeSlotsCount();
      /// Is the queue full ?
    bool IsQueueFull();
  /*@}*/
};
#endif
