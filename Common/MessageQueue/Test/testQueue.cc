#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <stdlib.h>
#include <unistd.h>

#include "../H/message.h"
#include "../H/messageQueue.h"
#include "./../../../Common/H/logFile.h"

LogFile *logFile = new LogFile("./mix.log");
MessageQueue *msgQ = new MessageQueue( 2);
Message *msg = new Message();

#define COUNT 6

int rnd()
{
    // toto simuluje hod mince: 1 nebo 0
  return( ((rand() + (RAND_MAX / 2.0)) > RAND_MAX) ? 1 : 0);
}

void *pusher1(void *dummy)
{
  int i;

  for ( int j = 0; j < COUNT; ) {
    if ( rnd()) {
      logFile->WriteString(llDebug, __FILE__ ":%d:Pusher #1 neco"
                           " prida do fronty SOFT.", __LINE__);
      if ( msgQ->Append( msg, SOFT) == MSGQ_FULL)
        logFile->WriteString(llDebug, __FILE__ ":%d:Pusher #1 by"
                             " sel spat if HARD !!", __LINE__);
      else
        ++j;
    }
    else {
      logFile->WriteString(llDebug, __FILE__ ":%d:Pusher #1 neco"
                           " prida do fronty HARD.", __LINE__);
      msgQ->Append( msg, HARD);
      ++j;
    }
    logFile->WriteString(llDebug, __FILE__ ":%d:Pusher #1 pred"
                         " usleep(30000).", __LINE__);
    usleep( 30000);
  }
}

void *pusher2(void *dummy)
{
  int i;

  for ( int j = 0; j < COUNT; ) {
    if ( rnd()) {
      logFile->WriteString(llDebug, __FILE__ ":%d:Pusher #2 neco"
                           " prida do fronty SOFT.", __LINE__);
      if ( msgQ->Append( msg, SOFT) == MSGQ_FULL)
        logFile->WriteString(llDebug, __FILE__ ":%d:Pusher #2 by"
                             " sel spat if HARD !!", __LINE__);
      else
        ++j;
    }
    else {
      logFile->WriteString(llDebug, __FILE__ ":%d:Pusher #2 neco"
                           " prida do fronty HARD.", __LINE__);
      msgQ->Append( msg, HARD);
      ++j;
    }

    logFile->WriteString(llDebug, __FILE__ ":%d:Pusher #2 pred"
                         " usleep(30000).", __LINE__);
    usleep( 30000);
  }
}

int main()
{
  pthread_t thread1, thread2;
  pthread_attr_t attr;
  int i;

  logFile->WriteString(llDebug, __FILE__ ":%d:Zacatek testu.",
                       __LINE__);

  pthread_attr_init( &attr);
  pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED);

  if (pthread_create(&thread1, NULL, pusher1, (void *) NULL) != 0) {
    perror("1");
  }
  else {
    logFile->WriteString(llDebug, __FILE__ ":%d:Pusher #1 vytvoren.",
                         __LINE__);
  }

  if (pthread_create(&thread2, NULL, pusher2, (void *) NULL) != 0) {
    perror("1");
  }
  else {
    logFile->WriteString(llDebug, __FILE__ ":%d:Pusher #2 vytvoren.", 
                         __LINE__);
  }

  for ( int j = 0; j < 2 * COUNT; ) {
    if ( i = rnd())
      logFile->WriteString(llDebug, __FILE__ ":%d:Popper nic"
                           " neodebere z fronty.", __LINE__);
    else {
      logFile->WriteString(llDebug, __FILE__ ":%d:Popper neco"
                           " odebere z fronty.", __LINE__);
      msgQ->GetMsg( &msg);
      ++j;
    }
    logFile->WriteString(llDebug, __FILE__ ":%d:Popper pred usleep(30000).",
                         __LINE__);
    usleep( 30000);
  }

  logFile->WriteString(llDebug, __FILE__ ":%d:Konec testu.",
                       __LINE__);
}
