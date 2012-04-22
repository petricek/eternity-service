#ifndef SIGNAL_MANAGER_H
#define SIGNAL_MANAGER_H

class SignalManager;

#include <pthread.h>

#include "./../../H/common.h"
#include "./../../Runable/H/runable.h"

/**
 * Signal manger class.
 *
 * Implements system specific signal handling and dispatching signals 
 * to appropriate threads.
 *
 * @author  Pechy
 * @see     Scheduler Killer
 */
class SignalManager: public Runable
{

  public:
  /**@name methods */
  /*@{*/
    /// Constructor
    SignalManager(LogFile * signalManagerLogFile);
    /// Main thread method
    void * Run(void *);
  /*@}*/

  private:
  /**@name attributes */
  /*@{*/
    /// Synchronization
    pthread_mutex_t mutex;
  /*@}*/

};
#endif
