#ifndef MIX_OFFSPRING_H
#define MIX_OFFSPRING_H

class Mix;

/**
 * Mix offspring.
 *
 * Maintains a pointer to it's parent so that descendands of this
 * class can access some members of Mix. It is useful for objects that
 * communicate heavily with other objects inside of Mix.
 *
 * @author  Venca
 * @see     Mix, Majordomo
 */
class MixOffspring
{

  protected:

  /**@name attributes */
  /*@{*/
    Mix * mix;
  /*@}*/
  
  public:

  /**@name methods */
  /*@{*/
    MixOffspring(Mix * mix); 
  /*@}*/
};

#include "./../../../Mix/Mix/H/mix.h"

#endif
