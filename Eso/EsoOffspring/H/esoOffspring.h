#ifndef ESO_OFFSPRING_H
#define ESO_OFFSPRING_H

class Eso;

/**
 * Eso offspring.
 *
 * Maintains a pointer to the object that created this one.
 *
 * @author  Venca
 * @see     Eso
 */
class EsoOffspring
{
    // properties
  protected:

  /**@name attributes */
     /*@{*/
    Eso * eso;
     /*@}*/

    // methods
  public:

  /**@name methods */
     /*@{*/
    EsoOffspring(Eso * eso); 
     /*@}*/
};

#include "../../Eso/H/eso.h"

#endif
