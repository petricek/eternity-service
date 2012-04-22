#ifndef ACS_OFFSPRING_H
#define ACS_OFFSPRING_H

class Acs;

/**
 * Offspring of Acs class.
 *
 * Objects that inherit from this class are offsprings of a different
 * object. The purpose of it is to maintain the relation between
 * father and his sons.
 *
 * @author  Venca
 * @see     Acs, Majordomo
 */
class AcsOffspring
{
  protected:

  /**@name attributes */
  /*@{*/
      /// Pointer to its parent (Acs)
    Acs * acs;
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
      /// Constructor
    AcsOffspring(Acs * acs); 
  /*@}*/
};

#include "../../Acs/H/acs.h"

#endif
