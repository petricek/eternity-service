  // has to be the FIRST include !
#include "./../H/common.h"

#include "./H/cipherer.h"

/**
 * Cipherer constructor.
 *
 * Cipherer constructor. For now, nothing is needed to run here
 * (random generator is initialized in main.cc).
 *
 *
 * @param   ciphererLogFile Log file.
 * @author  Pechy
 * @see
 */
Cipherer::Cipherer(LogFile * ciphererLogFile)
  : Debugable::Debugable(ciphererLogFile)
{

}

/**
 * Cipherer destructor.
 *
 * Cipherer destructor. For now, nothing is needed to run here.
 *
 * @author  Pechy
 * @see     Cipherer()
 */
Cipherer::~Cipherer()
{

}
