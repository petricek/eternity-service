#ifndef RUNABLE_H
#define RUNABLE_H

#include "./../../Debugable/H/debugable.h"

/**
 * Runable class.
 *
 * This class forces its offsprings to implement method Run used for 
 * execution in a separate thread.
 * All objects that have to run as a thread should inherit from this class.
 *
 * @author  Pechy Venca
 * @see     Debugable LogFile SignalManager
 */
class Runable: public Debugable {

  public: 
  /**@name methods */
  /*@{*/
    /// Constructor
    Runable(LogFile * logFile = NULL);
    /// Do not send me signals (Signal manager will take care of them)
    void MaskAllSignals();
    /// Main thread function
    virtual void *Run(void * param) = 0;
  /*@}*/
};

  /// helper function
void *RunObject(void *object);

#endif
