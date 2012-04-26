#include "./H/messageQueue.h"

/**
 * Message queue constructor.
 *
 * Implements a simle FIFO queue. Initializes semaphores and mutex
 * for protecting its inner data and also initializes this inner data.
 *
 * @param   queueLogFile  Log file that is used for debug messages.
 * @param   size          Maximum number of messages in queue.
 * @author  Pechy
 * @see     semaphores
 */
MessageQueue::MessageQueue(LogFile * queueLogFile, Size size)
  :Debugable(queueLogFile)
{
  SetDebugOptions(llError|llWarning|llFatalError);
  WriteString(llDebug, __FILE__ ":%d:MessageQueue() constructor,"
                       " size=%d.", __LINE__, size);
  this->size = size;
  messages = new GMessage*[size];

    // initializing the two needed semaphores
  semaphore_init(&semFull, 0);
  semaphore_init(&semEmpty, this->size);

    // mutex for avoiding of race-condition with 
    // manipulating with queue
  pthread_mutex_init(&mutex, NULL);

  firstMsg = 0;
  lastMsg = size - 1;
  msgCount = 0;
  WriteString(llDebug, __FILE__ ":%d:MessageQueue() end of constructor.",
                       __LINE__);
  SetDebugOptions(llAll);
}

/**
 * Message queue destructor.
 *
 * Deletes its internal data structures.
 *
 * @author  Pechy
 * @see     
 */
MessageQueue::~MessageQueue()
{
  DELETE(messages);
}

/**
 * Append message to queue.
 *
 * Appends GMessage to the end of messaage queue.
 *
 * @param   msg         GMessage 
 * @param   requestType Wheteher we should fall asleep when message
 *                      queue is already full - default is yes.
 * @return  If requestType is SOFT and queue is full then KO else OK.
 * @author  Pechy
 * @see     
 */
Err
MessageQueue::Append(GMessage *msg, int requestType)
{

    // sem_down MUST be BEFORE mutex_lock !!!!
    /* this is not absolutely correct, for requestType=SOFT is not
     * guaranteed that it won't block, but it is not so critical for
     * us
     */
  if ( requestType != SOFT || msgCount != this->size)
    semaphore_down(&semEmpty);

    // we must be careful here...
  pthread_mutex_lock(&mutex);

  if ( requestType == SOFT) {
    if ( msgCount == this->size) {
      pthread_mutex_unlock(&mutex);
      return( MSGQ_FULL);
    }
  }

  lastMsg = (++lastMsg) % size;
  messages[lastMsg] = msg;
  msgCount++;
  
  pthread_mutex_unlock(&mutex);
  semaphore_up(&semFull);

  return OK;
}

/**
 * Get random message from queue.
 *
 * Chooses a random message in queue and returns it. It also deletes
 * the message from queue.
 *
 * @param   msg    Out - gets filled with pointer to a GMessage.
 * @return  OK
 * @author  Pechy
 * @see     
 */
Err MessageQueue::GetRandomMsg(GMessage **msg)
{
  Size index = random() % msgCount; 
  GMessage * message = NULL;
  Err result = GetMsg(&message, index);
  *msg = message;
  return result;
}

/**
 * et message from queue.
 *
 * Returns the number-th message and removes it from queue, the 1st 
 * message has number 0 !!!!
 *
 * @param   msg     Gets filled with a pointer to the message.
 * @param   number  Index of message we want from queue. 
 * @return  OK
 * @author  Pechy
 * @see     
 */
Err MessageQueue::GetMsg(GMessage **msg, Size number)
{
  Size i;

  semaphore_down(&semFull);

    // we must be careful here...
  pthread_mutex_lock(&mutex);

  *msg = messages[(firstMsg + number) % size];
  if (number < (msgCount / 2)) {
    for(i = firstMsg + number; i > firstMsg; i--) {
      messages[i % size] = messages[(i - 1) % size];
    };
    firstMsg = (firstMsg + 1) % size;
   }
  else {
    for(i = firstMsg + number; i != lastMsg; i = (i + 1) % size) {
      messages[i] = messages[(i + 1) % size];
    };
    lastMsg = (lastMsg - 1 + size) % size;
  };

  msgCount--;

  pthread_mutex_unlock(&mutex);

  semaphore_up(&semEmpty);

  return OK;
}

/**
 * Get message from queue.
 *
 * Returns the number-th message from queue but leaves it in there.
 *
 * @param   msg    Gets filled with pointer to message retured.
 * @param   number Message offset in queue.
 * @return  OK
 * @author  Pechy
 * @see     
 */
Err MessageQueue::ShowMsg(GMessage **msg, Size number)
{
  if (number >= msgCount)
      fprintf(stderr, "number bigger or equal then msgCount");
  WriteString(llWarning, __FILE__ ":%d:Number bigger or equal then"
    " msgCount in ShowMsg()", __LINE__);

    // we must be careful here even if we do *not* modify the queue, because
    // somebody *could*
  pthread_mutex_lock(&mutex);

  *msg = messages[(firstMsg + number) % size];

  pthread_mutex_unlock(&mutex);
  return OK;
}

/**
 * Returns the number of messages in queue
 *
 * @return  Number of messages in queue.
 * @author  Pechy
 * @see     
 */
Size MessageQueue::MsgCount()
{
  return msgCount;
}

/**
 * Number of messages it is possible to append to queue.
 *
 * @return  Number of free slots.
 * @author  Pechy
 * @see     
 */
Size MessageQueue::FreeSlotsCount()
{
  return (size - msgCount);
}

/**
 * Is the queue full?.
 *
 * If number of messages is the same as number of slots then the
 * queue is full.
 *
 * @return  true if queue is full false otherwise.
 * @author  Pechy
 * @see     
 */
bool MessageQueue::IsQueueFull()
{
  return (size == msgCount);
}
