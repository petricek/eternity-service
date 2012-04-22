#ifndef SCHEDULABLE_H
#define SCHEDULABLE_H

/**
 * Schedulable class.
 *
 * Schedulable forces its offsprings to implement HandleEvent method
 * that is used as a callback function when the event we are waiting for 
 * happens.
 * All objects that want to schedule some actions for the future have to 
 * inherit from this class.
 *
 * @author  Venca
 * @see     Scheduler
 */
class Schedulable {

  public: 
  /**@name methods */
  /*@{*/
    /// Notify about event that occured
    virtual void HandleEvent(int handleID) = 0;
  /*@}*/

};
#endif

