#ifndef CHOPPER_H
#define CHOPPER_H

class Chopper;

  // Chopper tries to get CFG_FIXED_CHUNK_SIZE from config file
#define DEFAULT_FIXED_CHUNK_SIZE 20480

#include "../../../Common/Debugable/H/debugable.h"
#include "../../../Common/Shutdownable/H/shutdownable.h"

#include "../../MixOffspring/H/mixOffspring.h"

#include <list>

/**
 * Messages chopper.
 *
 * Chops large messages into uniform chunks that can be untraceably sent
 * across network.
 *
 * @author  Venca
 * @see     Linker
 */
class Chopper:public Runable, public MixOffspring, public Shutdownable
{

  protected:

  /**@name attributes */
  /*@{*/
    ID lastMessageID;
    Size fixedChunkSize;
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
    Chopper(Mix * parent, LogFile * chopperLogFile);
    virtual void * Run(void *);
    virtual void Shutdown();

    list< GMessage * > CreateChunks(GMessage * message);
  /*@}*/
};
#endif
