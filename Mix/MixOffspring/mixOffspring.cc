#include "./H/mixOffspring.h"

/**
 * Remembers pointer to its father.
 *
 * Remembers pointer to its father. Some objects need pointers to
 * almost all of other objects, so we made them mix's offsprings for
 * not to have to include all pointers as parameters.
 *
 * @param   mix Mix main object.
 * @author  Venca
 * @see     Mix
 */
MixOffspring::MixOffspring(Mix * mix)
{
  this->mix = mix;
}
