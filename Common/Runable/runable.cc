  // has to be the FIRST include !
#include "./../H/common.h"

#include <pthread.h>

#include "./H/runable.h"

/**
 * Runable constructor.
 *
 * Initializes its parent class.
 *
 * @param   logFile  Log file for writing debug messages.
 * @author  Venca
 * @see     Debugable, LogFile
 */
Runable::Runable(LogFile * logFile)
  :Debugable(logFile)
{
  
}

/**
 * Mask all signals.
 *
 * All threads except SignalManager *have* to mask all signals for 
 * library to direct all signals to SignalManager.
 *
 * @return  void
 * @author  Pechy
 * @see     SignalManager, thread discussion, Shutdownable
 */
void Runable::MaskAllSignals()
{
  pthread_sigmask(SIG_BLOCK, NULL, NULL);
}

/**
 * Run object in a separate thread.
 *
 * Runs method Run of object. Used for creation of a new running
 * object. Works around the problem with passing a method of an
 * object to pthread_create().
 *
 * @param   object  Runable object to execute.
 * @return  what them method Run() of Runnable object returns.
 * @author  Venca
 * @see     pthread_create(), RunningObject, Run()
 */
void *RunObject(void *object)
{
  ((Runable *) object)->Run( NULL);
  return NULL;
};

