#ifndef KILLER_H
#define KILLER_H

class Killer;

#include "./../../H/common.h"
#include "./../../Shutdownable/H/shutdownable.h"
#include "./../../Debugable/H/debugable.h"

#include <list>

/**
 * Shuts down all shutdownable objects.
 *
 * All Shutdownable objects register with Killer when created and he shuts 
 * them down when necessary. In fact it calls the Shutdown() method of all Shutdownable 
 * objects that have to implement it.
 * Calls Shutdown() method of all shutdownable objects - that is 
 * why he stores pointers to all of them. They pass it to him when 
 * created.
 *
 * @author  Venca
 * @see     Shutdownable
 */
class Killer:Debugable
{
  protected:
  /**@name attributes */
  /*@{*/
    /// list of objects to notify
    list< Shutdownable * > victims;
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
  /// Constructor
    Killer(LogFile * logFile);
    /// Join the victims
    void RegisterForShutdown(Shutdownable * victim);
    /// Shutdown all victims
    void ShutdownAll();
  /*@}*/

};
#endif
