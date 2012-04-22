#ifndef LEAK_DETECTOR_H
#define LEAK_DETECTOR_H

/**
 * Memory leak detector.
 *
 * A parent of all classes that we want to track. It finds out if all 
 * instances are deleted correctly or if there are some memory leaks. 
 *
 * @author  Venca
 * @see     GMessage MsgField
 */
class LeakDetector
{
  public:
  /**@name operators */
  /*@{*/
    /// Overloaded new
  void * operator new(size_t size);
    /// Overloaded delete
  void   operator delete(void * what);
  /*@}*/

};
#endif
