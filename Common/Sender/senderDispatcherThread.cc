  // has to be the FIRST include !
#include "./../H/common.h"

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
#include <pthread.h>

#include "./H/sender.h"
#include "../H/labels.h"
#include "./H/senderDispatcherThread.h"

  /// how many different recipients are there in one chunk?
#define RECIPIENT_COUNT 2

/**
 * Call Sender's method for dispatching one message.
 *
 * Works around the problem with passing method to pthread_create()
 * by encapsulating this method into a normal function.
 *
 * @param   args   Passed to sender->DispatcherThread()
 * @return  NULL - not used
 * @author  Pechy
 * @see     pthread_create(), DispatcherThread() 
 */
void *SenderDispatcherThread(void *args)
{
  pthread_sigmask(SIG_BLOCK, NULL, NULL);

  ArgsForDispatcherThread *argsAxlr;
  argsAxlr = (ArgsForDispatcherThread *) args;

  argsAxlr->sender->DispatcherThread(argsAxlr->msg);
  DELETE(argsAxlr);


  return NULL;
}

/**
 * Connect to one of the recipients and send him the message.
 *
 * In our implementation, we use threads for despatching msgs. This 
 * method is the body of that thread.
 * It expects:
 * <pre>
 *   GM_RECIPIENT_1
 *     GM_ADDRESS
 *     GM_PORT
 *  [GM_RECIPIENT_2
 *     GM_ADDRESS
 *     GM_PORT     ]
 *   GM_CHUNK
 * </pre>
 * The chosen recipient is inserted into the GM_CHUNK and that is
 * transmitted over network.
 *
 * @param   msg     Message to send
 * @return  void
 * @author  Pechy
 * @see     Sender
 */
void Sender::DispatcherThread(GMessage *msg)
{

  WriteString(llDebug, __FILE__ ":%d:What message is going to be sent "
              "by sender?", __LINE__);
  msg->DebugHeaders(logFile);

  for ( int i = 0; i < RECIPIENT_COUNT; ++i) {

    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      WriteString(llError, __FILE__ ":%d:socket() %s.", __LINE__, 
		  strerror(errno));

      DELETE(msg);
      pthread_exit(NULL);
    }

    char id[10];
    sprintf(id, "%d", i+1);

    GMessage *gMsgRcpt = NULL;
    if ((gMsgRcpt = msg->GetAsGMessage((char *)(string(GM_RECIPIENT) + 
                                       "-" + id).c_str())) == NULL) {
      WriteString(llWarning, __FILE__ ":%d:Got NULL recipient gMsg", __LINE__);
      continue;
    }

    WriteString(llDebug, __FILE__ ":%d:What is in Recipient I will try "
                "to connect to?", __LINE__);
    gMsgRcpt->DebugHeaders(logFile);

    struct sockaddr_in theirAddr;
    theirAddr.sin_family = AF_INET;
    u_short *tmpPort;
    if ((tmpPort = ((u_short *) gMsgRcpt->GetAsBytes(GM_PORT))) == NULL) {
      WriteString(llError, __FILE__ ":%d:Bad port!", __LINE__);
      DELETE(gMsgRcpt);
      continue;
    }
    else {
      theirAddr.sin_port = htons(*tmpPort);
      DELETE(tmpPort);
      WriteString(llDebug, __FILE__ ":%d:Port in u_short is %u", __LINE__, 
		  ntohs(theirAddr.sin_port));
    }

    char *tmpIP;
    if ((tmpIP = gMsgRcpt->GetAsString(GM_ADDRESS)) == NULL) {
      WriteString(llError, __FILE__ ":%d:Bad address!", __LINE__);
      DELETE(gMsgRcpt);
      continue;
    }
    else {
      WriteString(llDebug, __FILE__ ":%d:Address is %s.", __LINE__, tmpIP);

      if ((theirAddr.sin_addr.s_addr = ConvertToNumericIP(tmpIP)) == 0) {
	WriteString(llError, __FILE__ ":%d:Address %s unknown.", __LINE__, 
	            tmpIP);

	DELETE(tmpIP); DELETE(gMsgRcpt);
	continue;
      }
      else {
	WriteString(llDebug, __FILE__ ":%d:converted %s is %s", __LINE__, 
		    tmpIP, inet_ntoa(theirAddr.sin_addr));
      }
    }

      // make convention happy..., but in C++ everything is NULL
    //bzero(&(theirAddr.sin_zero), 8);
    //bzero(theirAddr.sin_zero, 8);

    int sockAddrSize;
    sockAddrSize = sizeof(struct sockaddr);

    if (connect(sock, (struct sockaddr *) &theirAddr, sockAddrSize) == -1) {
      int Errno = errno;
      WriteString(llError, __FILE__ ":%d:connect() %s on socket %d "
                  "and port %u, errno is %d.", __LINE__, strerror(Errno), sock, 
		  ntohs(theirAddr.sin_port), Errno);
    }
    else {

      WriteString(llDebug, __FILE__ ":%d:connect() succeded on socket %d " 
                  "and port %u.", __LINE__, sock, ntohs(theirAddr.sin_port));

        // GM_CHUNK is gMsg, there is everything except GM_RECIPIENT
      GMessage *threadGMsg;
      if ((threadGMsg = msg->GetAsGMessage( GM_CHUNK)) == NULL) {
	WriteString(llError, __FILE__ ":%d:Bad chunk !", __LINE__);
	DELETE(gMsgRcpt); DELETE(tmpIP); 
	close(sock);
	continue;
      }

      threadGMsg->SetAsGMessage( GM_RECIPIENT, gMsgRcpt);
      WriteString(llDebug, __FILE__ ":%d:What is in chunk with recipient?", 
                  __LINE__); 
      threadGMsg->DebugHeaders(logFile);
	// we mustn't forget these two deletes
      DELETE(msg); DELETE(gMsgRcpt);

        // if origin is nonempty, the sender is in a Six and has to 
	// identify itself to the Mix
      if(origin.empty())
        threadGMsg->Delete(GM_ORIGIN);
      else
        threadGMsg->SetAsString(GM_ORIGIN, (char *) (this->origin).c_str());

	// feed the Net and free the threadGMsg when done
      if (WriteMsgToSocket(sock, threadGMsg) == OK) { 
        WriteString(llDebug, __FILE__ ":%d:Message feeded to %s.", __LINE__, 
                    tmpIP); 
      }
      else {
        WriteString(llWarning, __FILE__ ":%d:Message NOT feeded to %s.", 
	            __LINE__, tmpIP); 
      }

      DELETE(tmpIP);

      // Venca
      pthread_mutex_unlock(&mutex);

      pthread_exit(NULL);
    }

    if (close(sock) == -1)
      WriteString(llError, __FILE__ ":%d:close() %s, closing %d socket.",
	  __LINE__, strerror(errno), sock);

    DELETE(gMsgRcpt); DELETE(tmpIP);
  }

    // message not sent
  DELETE(msg);
  WriteString(llWarning, __FILE__ ":%d:Two recipients is too few!", __LINE__); 

  // Venca
  pthread_mutex_unlock(&mutex);

  pthread_exit(NULL);
}

/**
 * Write message to connected socket.
 *
 * Writes body of the message to the socket, the socket must be already
 * created and connected. This method is called from the thread is
 * managing the actual sending.
 *
 * @param   sock    Socket to write to.
 * @param   msg     GMessage to send.
 * @return  OK if successful else KO.
 * @author  Pechy
 * @see     SenderDispatcher()
 */
Err Sender::WriteMsgToSocket(int sock, GMessage *msg)
{
  Size msgLen;

  char *msgInByteStream = (char *) msg->StoreToBytes(&msgLen);
  WriteString(llDebug, __FILE__ ":%d:Message in WriteMsgToSocket is "
              "%lu bytes long.", __LINE__, msg->SizeOf());

    // we don't need msg any more
  DELETE( msg);

  int dispatchedChunkLen = 0;
    // oh, oh, it's ugly
  int decreasingLen = (int) msgLen;

  while (decreasingLen) {
    if ((dispatchedChunkLen = write(sock, msgInByteStream + 
			      msgLen - decreasingLen, 
			      decreasingLen)) == -1) {

      WriteString(llWarning, __FILE__ ":%d:write() %s", __LINE__, 
		  strerror(errno));

      if (close(sock) == -1)
	WriteString(llError, __FILE__ ":%d:close() %s, closing %d socket.",
	    __LINE__, strerror(errno), sock);

      DELETE(msgInByteStream);
      return(KO);
    }

    decreasingLen -= dispatchedChunkLen;
    WriteString(llDebug, __FILE__ ":%d:Written %d bytes.", __LINE__, 
                dispatchedChunkLen);
    assert(decreasingLen >= 0);
  }

  WriteString(llDebug, __FILE__ ":%d:Written %d bytes long message to "
              "the socket %d.", __LINE__, msgLen, sock);

  if (close(sock) == -1)
    WriteString(llError, __FILE__ ":%d:close() %s, closing %d socket.",
	__LINE__, strerror(errno), sock);

    // this string was allocated by new (in msg->StoreToBytes()), we
    // have to use DELETE(), not free()
  DELETE(msgInByteStream);
  return(OK);
}

/**
 * Converts string address into the unsigned long.
 *
 * In the Path field in msg we can have both DNS or IP addresses. Both 
 * types are as a *string*. We need to convert the address to an 
 * unsigned long.
 *
 * @param   addr   String representation of address (IP or hostname)
 * @return  IP address represented by a long value.
 * @author  Pechy
 * @see     SenderDispatcher()
 */
unsigned long Sender::ConvertToNumericIP(char *addr)
{
  struct hostent *he;

  if (addr[0] >= '1' && addr[0] <= '9')
    return(inet_addr(addr));
  else {
    if ((he = gethostbyname(addr)) == NULL) {
	// h_errno is declared in netdb.h
      WriteString(llError, __FILE__ ":%d:gethostbyname() - %s",
        __LINE__, hstrerror(h_errno));
      return(0);
    }

    return(*((unsigned long *) he->h_addr));
  }
}
