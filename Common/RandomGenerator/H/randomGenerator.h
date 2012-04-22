#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include "./../../Debugable/H/debugable.h"
#include "./../../GMessage/H/msgField.h"

#define RANDOM_PATH "./"

/**
 * Generator of random data.
 *
 * Generates a message field that contains 
 *
 * @author  Pavel
 * @see     
 */
class RandomGenerator: public Debugable
{
  public:

  /**@name methods */
  /*@{*/
    RandomGenerator(LogFile *aLogFile = NULL);
    MsgField *RandomBytes(Size count);
    Size RandomSize();
  /*@}*/
};

#endif
