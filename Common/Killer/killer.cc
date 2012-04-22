#include "./H/killer.h"

/**
 * Killer constructor.
 *
 * Initializes parent classes.
 *
 * @param   logFile  Killer is Debugable so he needs a log file to write to.
 * @author  Venca
 * @see     Shutdownable, Shutdown()
 */
Killer::Killer(LogFile * logFile)
  :Debugable(logFile)
{
}

/**
 * Register yourself for information about shutdown.
 *
 * Adds victim to a list of objects that should be notified by
 * calling their method Shutdown when we are going down. Victim has to
 * be Shutdownable - has to implement Shutdown().
 *
 * @param   victim  object to notify.
 * @author  Venca
 * @see     Shutdownable
 */
void Killer::RegisterForShutdown(Shutdownable * victim)
{
  victims.push_back(victim);
}

/**
 * Call Shutdown() of all victims.
 *
 * Victims - objects that asked for it.
 *
 *
 * @author  Venca
 * @see     Shutdownable
 */
void Killer::ShutdownAll()
{
  while( ! victims.empty() )
  {
    victims.front()->Shutdown();
    victims.pop_front();
  }
}
