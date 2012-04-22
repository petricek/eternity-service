  // has to be the FIRST include !
#include "./../H/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include <string>

#include "./H/receiver.h"
#include "../H/labels.h"
#include "./H/receiverRequestThread.h"


  /// length of listen()'s queue
#define RECEIVER_QUEUE 10

/**
 * Generic receiver constructor.
 *
 * Remembers all the attributes.
 * must get pointer to rcvToTran queue, because this queue is declared
 * in Mix class so Receiver can't see it as we want to be generic.
 *
 * @param   receiverConfigFile  Configuration file.
 * @param   receiverLogFile     Log file for debug messages.
 * @param   killer              Killer objectthat will call us when
 *                              shutdown occurs.
 * @param   fromReceiver        Output queue into which to insert
 *                              received messages.
 * @param   origin              GM_ORIGIN is set to origin unless it
 *                              is empty string.
 * @param   portWhereToListen   Port on which the receiver shoul listen
 *                              to incomming messages.
 * @author  Pechy
 * @see     Sender()
 */
Receiver::Receiver( ConfigFile * receiverConfigFile, 
                    LogFile * receiverLogFile, 
                    Killer * killer,
                    MessageQueue * fromReceiver,
                    string origin,
		    u_short portWhereToListen)
  :Runable(receiverLogFile), Shutdownable(killer)
{
  MaskAllSignals();

  WriteString(llDebug, __FILE__ ":%d:Entering Receiver->Receiver()",
    __LINE__);

  this->origin = origin;
  this->port = portWhereToListen;
  this->configFile = receiverConfigFile;
  this->fromReceiverQueue = fromReceiver;

  // Venca
  pthread_mutex_init(&mutex, NULL);


  WriteString(llDebug, __FILE__ ":%d:Got string '%s'.", __LINE__, 
    origin.c_str());
}

/**
 * Generic receiver destructor.
 *
 * Empty for now.
 *
 * @author  Pechy
 * @see     Receiver()
 */
Receiver::~Receiver()
{
}

/**
 * Prepare for clean shut down.
 *
 * This method should implement all the tasks necessary for preparation
 * for a clean shutdown like saving inconsistent data to disk etc. 
 *
 * @return  void
 * @author  Venca
 * @see     Shutdownable, Killer
 */
void Receiver::Shutdown()
{
  // Maybe we should close sockets here
}

/**
 * Main receiver method.
 *
 * this function is the function that is run in thread. It listens on
 * some port and whenever there comes a connection it disptches a
 * new thread that takes care of receiving the whole message and 
 * passing it to the next object.
 *
 * @return  (void *)NULL
 * @author  Pechy
 * @see     Sender
 */
void *Receiver::Run( void *)
{
  WriteString(llDebug, __FILE__ ":%d:Entering Receiver->Run()", __LINE__);

  int sock;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    WriteString(llError, __FILE__ ":%d:socket() %s.", __LINE__, 
                strerror(errno));
  }
  else {
    //WriteString(llDebug, __FILE__ ":%d:Returned socket %d.", 
     //           __LINE__, sock);
    ;
  }

    // initalizing pthread attribute object for request servant threads
    // setting thread to detachable - the thread won't no longer exists
    // after its finishing (ie., no ,,zombies'')
  pthread_attr_t pthreadAttr;
  if (pthread_attr_init( &pthreadAttr) == -1) {
    WriteString(llError, __FILE__ ":%d:pthread_attr_init() %s.",
      __LINE__, strerror(errno));
    exit( 1);
  }

    // adjust attributes for servant threads
  pthread_attr_setdetachstate( &pthreadAttr, PTHREAD_CREATE_DETACHED);
  pthread_attr_setstacksize( &pthreadAttr, (size_t) RECEIVER_STACK_SIZE);

    // initializing structure for receiver address
  struct sockaddr_in receiverAddr;
  receiverAddr.sin_family = AF_INET;
  receiverAddr.sin_port = htons(this->port);
  receiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  //bzero(&(receiverAddr.sin_zero), 8);

  if (bind(sock, (struct sockaddr *) &receiverAddr,  
	   sizeof(struct sockaddr)) == -1) {
    WriteString(llFatalError, __FILE__ ":%d:bind() %s to socket %d with "
                "port %u.", __LINE__, strerror(errno), sock, 
		 this->port);
  }
  else {
    WriteString(llDebug, __FILE__ ":%d:bind() to socket %d OK with "
                "port %u.", __LINE__, sock, this->port);
  }

  if (listen(sock, RECEIVER_QUEUE) == -1) {
    WriteString(llFatalError, __FILE__ ":%d:listen() %s.",
      __LINE__, strerror(errno));
  }
  else {
    //WriteString(llDebug, __FILE__ ":%d:listen() OK.", __LINE__);
    //WriteString(llDebug, __FILE__ ":%d:Receiver listening on port %u.",
    //            __LINE__, (u_int) this->port);
  }

    // the main loop where Receiver is accepting connections from the Net
  for ( ; ; ) {

    //WriteString(llDebug, __FILE__ ":%d:Receiver in loop for accept().",
     // __LINE__);

    int acceptedSock;
    int sinSize = sizeof(struct sockaddr_in);
    struct sockaddr_in incomingAddr;
    if ((acceptedSock = accept(sock, (struct sockaddr *) &incomingAddr, 
	                       &sinSize)) == -1) {
      WriteString(llWarning, __FILE__ ":%d:accept() %s.", __LINE__, 
                  strerror(errno));
      continue;
    }
    else {
       WriteString(llDebug, __FILE__ ":%d:Accept() from %s, socket is %d.",
                   __LINE__, inet_ntoa(incomingAddr.sin_addr), acceptedSock);
    }

    ArgsForRequestThread *argsForRequestThread = new ArgsForRequestThread();
      // we need this to identify a sender for X-Origin field
    argsForRequestThread->sockIn = acceptedSock;
    argsForRequestThread->receiver = this;

    // Venca
    pthread_mutex_lock(&mutex);
  
    pthread_t thread;
    if (pthread_create(&thread, &pthreadAttr, ReceiverRequestThread, 
		       (void *) argsForRequestThread) != 0) {
      WriteString(llWarning, __FILE__ ":%d:pthread_create() %s.", __LINE__, 
                  strerror(errno));
      continue;
    }
    else {
      //WriteString(llDebug, __FILE__ ":%d:pthread_create() OK.", __LINE__);
    }
  }
}
