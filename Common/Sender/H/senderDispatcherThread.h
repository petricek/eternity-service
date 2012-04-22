#ifndef SENDER_DISPATCHER_THREAD_H
#define SENDER_DISPATCHER_THREAD_H

  // has to be the FIRST include !
#include "./../../H/common.h"
#include "./../../GMessage/H/gMessage.h"

#include "./../../Runable/H/runable.h"

/**
 * Thread that dispatches message.
 *
 * contains all args needed by function that is an argument in 
 * pthread_create and that takes care of transmitting one message to network.
 * <p>
 * Description: We need a class below for arguments that we need pass to a
 *   function for phthread_create. We have to use an auxiliary function
 *   and then to call the sender->DispatchMsg() inside that function.
 *
 * @author  Pechy
 * @see     Sender Receiver
 */
class ArgsForDispatcherThread
{
  public:
  /**@name attributes */
  /*@{*/
    /// Pointer to parent
    Sender *sender;
    /// Gmessage to send
    GMessage *msg;
  /*@}*/

  public:
  /**@name methods */
  /*@{*/
    /// Constructor
    ArgsForDispatcherThread() {};
    /// Destructor
    ~ArgsForDispatcherThread() {};
  /*@}*/
};

  /// a prototype that will be passed to pthread_create()
void *SenderDispatcherThread(void *args);

#endif
