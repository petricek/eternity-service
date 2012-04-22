#ifndef SHUTDOWNABLE_H
#define SHUTDOWNABLE_H

class Shutdownable;

#include "./../../Killer/H/killer.h"

/**
 * Shutdownable class.
 *
 * This class forces its offsprings to implement method used for clean
 * shutdown. 
 * All objects that have to save some info before terminating should 
 * inherit from this class 
 *
 * @author  Venca
 * @see     Killer
 */
class Shutdownable
{

  public:
  /**@name methods */
  /*@{*/
    /// Constructor
    Shutdownable(Killer * killer);
    /// This method is called by Killer.
    virtual void Shutdown() = 0;
  /*@}*/

};


#endif

