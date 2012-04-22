#ifndef RECEIVER_REQUEST_THREAD_H
#define RECEIVER_REQUEST_THREAD_H

  // has to be the FIRST include !
#include "./../../../Common/H/common.h"

/**
 * Receiver arguments for thread.
 *
 * Contains all args needed by thread that handles retrieval of one message 
 * from net. 
 * Description: We need a class below for arguments that we need pass to a
 *   function for phthread_create. We have to use an auxiliary function
 *   and then to call the receiver->RequestServant() inside that
 *   function.
 *
 * @author  Pechy
 * @see     Receiver, Sender
 */
class ArgsForRequestThread
{
  public:
  /**@name attributes */
  /*@{*/
    /// Pointer to parent
    Receiver *receiver;
    /// Socket to read from
    int sockIn;
  /*@}*/

  public:
  /**@name methods */
  /*@{*/
    /// Constructor
    ArgsForRequestThread() {};
    /// Destructor
    ~ArgsForRequestThread() {};
  /*@}*/
};

  /// thread that handles retrieval of one message
void *ReceiverRequestThread(void *args);

#endif
