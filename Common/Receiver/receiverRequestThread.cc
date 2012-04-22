  // has to be the FIRST include !
#include "./../H/common.h"

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>

#include "./../MessageQueue/H/messageQueue.h"

#include "./H/receiver.h"
#include "../H/labels.h"
#include "./H/receiverRequestThread.h"

#include "./../Runable/H/runable.h"


  // for thread's buffer - BEWARE of thread's stack size !!!!
  // look for RECEIVER_STACK_SIZE
  // maybe this number is too high
  /// Receiver buffer size - does not limit the total size of message
#define MAX_RECEIVER_BUF_SIZE 8192

/**
 * Take care of receiving one message.
 *
 * This function is a parameter to the pthread_create() function call.
 * It works around the fact that it is not possible to pass a method
 * to pthread_create().
 *
 * @param   args
 * @return  NULL - not used by now.
 * @author  Pechy
 * @see     Receiver
 */
void *ReceiverRequestThread(void *args)
{
  pthread_sigmask(SIG_BLOCK, NULL, NULL);

  ArgsForRequestThread *argsAxlr;
  argsAxlr = (ArgsForRequestThread *) args;

  argsAxlr->receiver->RequestServant(argsAxlr->sockIn);
  return NULL;
}

/**
 * Receive one message from network.
 *
 * Reads data from the specified socket and allocates memory for it.
 * After this is done it makes a GMessage from it and inserts a
 * field called GM_ORIGIN with value specified in constructor.
 * Then the message is passed to the outgoing queue.
 *
 * @param   sockIn  Socket from which we can read tha incoming data.
 * @return  void
 * @author  Pechy
 * @see     Receiver
 */
void Receiver::RequestServant(int sockIn)
{
  int Errno,
      countAll = 0,
      count = 0;

  char buf[MAX_RECEIVER_BUF_SIZE],
       *bufHelp = NULL,
       *bufWhole = NULL;

  while (1) {

    if ((count = read(sockIn, buf, MAX_RECEIVER_BUF_SIZE)) == -1) {
      Errno = errno;
      if (Errno == EAGAIN) {
   /*     WriteString(llDebug, __FILE__ ":%d:Sock=%d %s.", __LINE__, 
                    sockIn, strerror(Errno));
		    */
      }
      else {
        WriteString(llWarning, __FILE__ ":%d:Recv() in thread sock=%d
                    failed, %s, exitting from thread.", __LINE__, sockIn, 
                    strerror(Errno));
	if (bufWhole)
	  DELETE(bufWhole);
	// Venca
	pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
      }
    } 
    else {
      if (count) {
        WriteString(llDebug, __FILE__ ":%d:Socket %d, read %d bytes.", __LINE__,
                    sockIn, count);
	bufHelp = new char[countAll + count];

        bcopy(bufWhole, bufHelp, countAll);
	bcopy(buf, bufHelp + countAll, count); 
	countAll += count;

	if (bufWhole) {
	  DELETE(bufWhole);
	}
	bufWhole = bufHelp;
      }
      else {
        WriteString(llDebug, __FILE__ ":%d:Sock=%d everything read, breaking"
                    " from the cycle....", __LINE__, sockIn);
          // all data are here, exit from while cycle
	break;
      }
    }
  }

  WriteString(llDebug, __FILE__ ":%d:Read %d bytes.", __LINE__, countAll);

  if (close(sockIn) == -1) {
    WriteString(llError, __FILE__ ":%d:close() %s, closing %d socket.",
        __LINE__, strerror(errno), sockIn);
  }

    // create gMsg and fill first field, ie. GM_X_ORIGIN
  GMessage *msg = new GMessage();
  if (msg->LoadFromBytes(bufWhole, countAll) != OK) {
    WriteString(llWarning, __FILE__ ":%d:Message not in proper format, "
                "throwing it out (%d bytes).", __LINE__, countAll);

    DELETE(bufWhole);
    DELETE(msg);
  }
  else {
    DELETE(bufWhole);

    WriteString(llDebug, __FILE__ ":%d:What receiver did read?", __LINE__);
    msg->DebugHeaders(logFile);

      // add receiver identification 
    if (this->origin != "") {
      msg->SetAsString(GM_ORIGIN, (char *) origin.c_str());
    }

    fromReceiverQueue->Append(msg);
  }

  //SetDebugOptions(savedOptions);

  // Venca
  pthread_mutex_unlock(&mutex);

  pthread_exit(NULL);
}
