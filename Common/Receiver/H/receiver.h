#ifndef RECEIVER_H
#define RECEIVER_H

class Receiver;

  // has to be the FIRST include !
#include "../../H/common.h"

#include "../../Runable/H/runable.h"
#include "../../ConfigFile/H/configFile.h"
#include "../../MessageQueue/H/messageQueue.h"
#include "../../Shutdownable/H/shutdownable.h"

  /// thread stack is not very large by default, so let's adjust that.
#define RECEIVER_STACK_SIZE 10000

/**
 * Message receiver.
 *
 * a class for receiver, it is a object that is responsible for accepting
 * messages from the Net and passing it to Translator.
 * <PRE>
 * In:
 *  byte stream from network
 * Out:
 *  GMessage containing
 *    GM_DEBUG
 *    GM_PROTOCOL_VERSION
 *    GM_REST_OF_PATH
 *    GM_DATA
 *    GM_RECIPIENT
 * </PRE>
 * @author  Pechy
 * @see     Sender
 */
class Receiver: public Runable, public Shutdownable
{
  protected:

  /**@name attributes */
  /*@{*/
    /// Mutex to serialize threads
    pthread_mutex_t mutex;

    /// What to insert into message
    string origin;
    /// Configuration file
    ConfigFile * configFile;
    /// Outgoing message queue
    MessageQueue * fromReceiverQueue;
    /// Port where to listen
    u_short port;
    /// Type of receiver
    int receiverType;
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
    /// Constructor
    Receiver(ConfigFile * receiverConfigFile, 
             LogFile * receiverLogFile, 
             Killer * killer, 
             MessageQueue * fromReceiver,
	     string origin,
	     u_short port);
    /// Destructor
    ~Receiver();

    /// Thread method
    void *Run( void *info);
    /// Handles one message
    void RequestServant(int sockIn);
    /// Prepare for shutdown
    virtual void Shutdown();
  /*@}*/
};
#endif
