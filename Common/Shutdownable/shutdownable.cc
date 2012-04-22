#include "./H/shutdownable.h"

/**
 * Shutdownable constructor.
 *
 * Reisters with killer so that he can notify us when shutdown comes.
 * That means call our method Shutdown().
 *
 * @param   killer    Killer that will notify us when shutdown comes.
 * @author  Venca
 * @see     Killer, ShutDown()
 */
Shutdownable::Shutdownable(Killer * killer)
{
  killer->RegisterForShutdown(this);
}
