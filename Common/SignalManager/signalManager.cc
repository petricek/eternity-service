  // has to be the FIRST include !
#include "./../H/common.h"

#include <signal.h>

#include "./H/signalManager.h"

/**
 * Handler for SIGALRM signal.
 *
 * Because of lack of sigwait() call in thread library in former
 * versions of FreeBSD, we have to use signal handlers. This should
 * be changed in the future.
 *
 * @param   int
 * @return  void
 * @author  Pechy
 * @see     thread discussion, Shutdownable
 */
void processSIGALRM( int)
{
  // we can't call this function, because processSIGALRM is not a method
  // WriteString(llDebug, __FILE__ ":%d:Signal catched...", __LINE__);

  // do nothing here...
}

/**
 * Handler for SIGQUIT signal
 *
 * Because of lack of sigwait() call in thread library in former
 * versions of FreeBSD, we have to use signal handlers. This should
 * be changed in the future.
 *
 * @param   int
 * @return  void
 * @author  Pechy
 * @see     processSIGALRM()
 */
void processSIGHUP( int)
{
  // do nothing here...
  fprintf(stderr, "SIGHUP catched.\n");
}

/**
 * Signal Manager constructor.
 *
 * Just initializes mutex.
 *
 * @param   signalManagerLogFile Log file.
 * @return  
 * @author  Pechy
 * @see     thread discussion
 */
SignalManager::SignalManager(LogFile * signalManagerLogFile)
  :Runable(signalManagerLogFile)
{
  pthread_mutex_init( &mutex, NULL);
}

/**
 * Signal Manager desstructor.
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     SignalManager()
 */
SignalManager::~SignalManager()
{
}

/**
 * The main loop for Signal Manager thread.
 *
 * Implementation of SignalManager thread, this is just a
 * template here, we intercept SIGALRM signal for now only,
 * to show how to do it.
 * <p>
 * Because lacking sigwait() function call, we have to do signal handling
 * using classical handlers, so we twice lock mutex to fall asleep for
 * ever.
 *
 * @param   void *
 * @return  void *
 * @author  Pechy
 * @see     thread discussion
 */
void *SignalManager::Run(void *)
{
    // set handler
  signal( SIGALRM, processSIGALRM);
  signal( SIGHUP, processSIGHUP);

  WriteString(llDebug, __FILE__ ":%d:SignalManager in Run()", __LINE__);

  pthread_mutex_lock( &mutex);
  //WriteString(llDebug, __FILE__ ":%d:After first lock in Run()", __LINE__);

    // sleep *FOREVER*
  pthread_mutex_lock( &mutex);

    // never gets here
  return NULL;
}
