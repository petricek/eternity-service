#ifndef SENDER_H
#define SENDER_H

class Sender;

  // has to be the FIRST include !
#include "./../../H/common.h"
#include "./../../Runable/H/runable.h"
#include "./../../Shutdownable/H/shutdownable.h"

#include "./../../MessageQueue/H/messageQueue.h"

/**
 * Sender class.
 *
 * Sends messages across network to other Mix or Six
 * Expects a GMessage containing:
 * <PRE>
 *   GM_RECIPIENT_1
 *   GM_RECIPIENT_2
 *   GM_CHUNK
 *     GM_DEBUG
 *     GM_PROTOCOL_VERSION
 *     GM_REST_OF_PATH
 *     GM_DATA
 * </PRE>
 * 
 * Makes a message like this and transmits it over network to IP from
 * field Recipient.
 * <PRE>
 *   GM_CHUNK
 *     GM_RECIPIENT
 *     GM_DEBUG
 *     PROTOCOL_VERSION
 *     GM_REST_OF_PATH
 *     GM_DATA
 * </PRE>
 *
 * The GM_RECIPIENT is set to GM_RECIPIENT_1 or GM_RECIPIENT_2 
 * depending on which was used as destination address.
 *
 * @author  Pechy
 * @see     Receiver 
 */
class Sender: public Runable, public Shutdownable
{
  protected:

  /**@name attributes */
  /*@{*/
    /// Mutex to serialize threads
    pthread_mutex_t mutex;

    /// Incoming queue
    MessageQueue *toSenderQueue;
    /// What set as GM_ORIGIN
    string origin;
    /// IP address to send by default
    string IP;
    /// Port to send by default
    unsigned short port;
  /*@}*/

  /**@name methods */
  /*@{*/
    /// string -> long representation
    unsigned long ConvertToNumericIP(char *addr);
    /// Feed the raw data to socket
    Err WriteMsgToSocket(int sock, GMessage *msg);
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
    /// Constructor
    Sender(LogFile * senderLogFile,
           Killer * killer,
           MessageQueue * toSenderQueue,
	   string origin, string IP = "", unsigned short port = 0);
    /// Destructor
    virtual ~Sender();

    /// Main thread function
    void *Run(void *);
    /// Take care of this one message
    Err DispatchMsg(GMessage *msg);
    /// method that runs in a separate thread and sends the message
    void DispatcherThread(GMessage *msg);
    /// Prepare for clean shutdown
    virtual void Shutdown();

    /// Insert default address and port to recipient
    GMessage * MakeMessageForSendingToMix(GMessage *chunk);
  /*@}*/

};
#endif
