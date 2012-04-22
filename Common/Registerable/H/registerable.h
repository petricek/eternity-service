#ifndef REGISTERABLE_H
#define REGISTERABLE_H

class Registerable;

#include "./../../GMessage/H/gMessage.h"

/**
 * Registerable class.
 *
 * Descendands of this class are able to register with MessageIDRepository
 * so that it can receive the reply.<p>
 * This class forces its offsprings to implement method used for clean
 * register.<p>
 * All objects that have to save some info before terminating should 
 * inherit from this class 
 *
 * @author  Venca
 * @see     MessageIDRepository
 */
class Registerable
{

  public:
  /**@name methods */
  /*@{*/
    /// Pass data to object
    virtual void HereYouAre(GMessage * data) = 0;
  /*@}*/

};


#endif

