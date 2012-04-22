  // has to be the FIRST include !
#include "./../H/common.h"

#include "./../H/labels.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>

#include "./H/sender.h"
#include "./H/senderDispatcherThread.h"

/**
 * Generic sender constructor.
 *
 * Remembers all the parameters for future use and initializes
 * its parent classes.
 *
 * @param   senderLogFile Log file for debug messages.
 * @param   killer        Who will notify us about shutdown.
 * @param   origin        What will we insert as GM_ORIGIN into
 *                        outgoing messages.
 * @param   IP            Where to send messages by default
 *                        (used when sending from Six to underlying Mix).
 * @param   port          Port number to which to connect.
 * @author  Pechy
 * @see     Receiver, Mix
 */
Sender::Sender(LogFile * senderLogFile, 
               Killer * killer, 
               MessageQueue * toSenderQueue,
	       string origin, string IP = "", unsigned short port = 0)
  :Runable(senderLogFile), Shutdownable(killer)
{
  this->toSenderQueue = toSenderQueue;
  this->origin = origin;
  this->IP = IP;
  this->port = port;
  // Venca
  pthread_mutex_init(&mutex, NULL);


}

/**
 * Generic Sender destructor.
 *
 * Empty by now.
 * 
 * @author  Pechy
 * @see     Sender()
 */
Sender::~Sender()
{
 
}

/**
 * Prepare for clean shutdown.
 *
 * This method should implement all the tasks necessary for
 * preparation for a clean shutdown like saving inconsistent data
 * to disk etc. Now it is empty as AddressManger is completely
 * stateless as it stores certificate to disk immediatelly after it
 * is generated.
 *
 * @author  Venca
 * @see     Shutdownable, Killer
 */
void
Sender::Shutdown()
{
  // Maybe should close sockets here
}

/**
 * Generic Sender Run() method.
 *
 * Neverending cycle, in each loop just one message passed from
 * Padder is read and the method DispatchMsg() is called. This method 
 * should send message to the apropriate destination. Now DispatchMsg() 
 * creates thread for serving this.
 *
 * @return  never
 * @author  Pechy
 * @see     DispatchMsg()
 */
void *
Sender::Run(void *)
{
  MaskAllSignals();

  GMessage *msg;

  WriteString(llDebug, __FILE__ ":%d:Entering Sender::Run()", 
		       __LINE__);
  for ( ; ; ) {

    toSenderQueue->GetMsg(&msg);
      // the main function of the Sender thread
    DispatchMsg(msg);
      // do not DELETE(msg) here, thread still needs it
  }
  return NULL;
}

/**
 * Send a message over the network.
 *
 * Sender requested for sending a message - This method creates a 
 * thread, that should do all the work. It takes GM_RECIPIENT and
 * sends the message to the address specified in it and just before
 * it he inserts GM_RECIPIENT into the GM_CHUNK.
 *
 * @param   msg    GMessage to send.
 * <pre>
 * GM_RECIPIENT
 *   GM_DDADDRESS
 *   GM_PORT
 * GM_CHUNK
 * </pre>
 * @return  OK if message was sent successfully.
 * @author  Pechy
 * @see     SenderDispatcherThread()
 */
Err
Sender::DispatchMsg(GMessage *msg)
{
  WriteString(llDebug, __FILE__ ":%d:entering DispatchMsg()...", __LINE__);

    // initalizing pthread attribute object
  // Venca
  pthread_mutex_lock(&mutex);


  pthread_attr_t pthreadAttr;
  if (pthread_attr_init( &pthreadAttr) == -1) {
      // strange error...
    WriteString(llWarning, __FILE__ ":%d:pthread_attr_init() - %s",
      __LINE__, strerror(errno));
    //pthreadAttr = NULL;
    return KO;
  }

    // we are not interested in informing that the thread finished
  if (pthread_attr_setdetachstate(&pthreadAttr, 
                                  PTHREAD_CREATE_DETACHED) != 0) {
    fprintf(stderr, "pthread_attr_setdetachstate failed.\n");
    DELETE(msg);
    return KO;
  }

    // initializing structure for receiver address
  ArgsForDispatcherThread *args = new ArgsForDispatcherThread();
  args->sender = this;
  args->msg = msg;

  pthread_t thread;
  if (pthread_create(&thread, &pthreadAttr, SenderDispatcherThread, 
                     (void *) args) != 0) {

    WriteString(llWarning, __FILE__ ":%d:pthread_create() - %s", __LINE__, 
                strerror(errno));
    return( KO);
  }

  return OK;
}

/**
 * Add recipient to message.
 *
 * Gets chunk and adds to it the GM_RECIPIENT field so that sender
 * knows how to deliver it and where.
 *
 * @param   chunk    Message we want to send to Mix.
 * @return  GMessage containing the original message with added
 *          GM_RECIPIENT field.
 * <pre>
 * GM_RECIPIENT
 *   GM_DDADDRESS
 *   GM_PORT
 * GM_CHUNK
 * </pre>
 * @author  Pechy
 * @see     SenderDispatcherThread()
 */
GMessage *
Sender::MakeMessageForSendingToMix(GMessage *chunk)
{
  if (IP == "" || port == 0) {
    WriteString(llWarning, __FILE__ ":%d:MakeMessageForSendingToMix() "
                "failed.", __LINE__);
    WriteString(llWarning, __FILE__ ":%d:IP = %s, Port = %d",
                           __LINE__, IP.c_str(), port);
    return NULL;
  }

  GMessage *messageForSender = new GMessage();
  messageForSender->SetAsGMessage(GM_CHUNK, chunk);

  GMessage *recipient = new GMessage();
  recipient->SetAsString(GM_ADDRESS, (char *) IP.c_str());
  recipient->SetAsBytes(GM_PORT, &port, sizeof(u_short));
  messageForSender->SetAsGMessage(GM_RECIPIENT_1, recipient);
  DELETE(recipient);

  return messageForSender;
}
