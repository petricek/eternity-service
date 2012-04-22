#include "H/acsOffspring.h"

/**
 * Acs offspring constructor.
 *
 * It just remembers pointer to its father so that it is possible to access
 * its members like other objects.
 *
 * @author  Venca
 * @see     Acs
 */
AcsOffspring::AcsOffspring(Acs * acs)
{
  this->acs = acs;
}
