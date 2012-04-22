#ifndef SIX_OFFSPRING_H
#define SIX_OFFSPRING_H

class Six;

/**
 * Maintains a pointer to the object that created this one.
 *
 * Maintains a pointer to it's parent so that descendands of this class can access
 * some members of Six. It is useful for objects that communicate heavily with other objects inside
 * of Six.
 *
 *
 * @author  Pavel 
 * @see     Six 
 */
class SixOffspring
{
  protected:
  /**@name attributes */
  /*@{*/
      ///Pointer to Six.
    Six * six;
  /*@}*/
  public:
  /**@name methods */
  /*@{*/
      ///Constructor.
    SixOffspring(Six *six); 
  /*@}*/
};

#include "../../Six/H/six.h"

#endif
