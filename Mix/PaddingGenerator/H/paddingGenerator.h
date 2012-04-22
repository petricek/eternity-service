#ifndef PADDING_GENERATOR_H
#define PADDING_GENERATOR_H

#define DEFAULT_PADDING_HOPS 50

//#include <.h>
#include "../../../Common/H/common.h"
#include "../../../Common/H/labels.h"
#include "../../../Common/Runable/H/runable.h"
#include "../../../Common/GMessage/H/gMessage.h"
#include "../../MixOffspring/H/mixOffspring.h"

/**
 * Padding messages generator.
 *
 * It just generates padding messages that are used to fill gaps
 * between traffic peaks. Makes a reservoir of messages for Padder to
 * send when there are no messages waiting.
 * 
 * @author  Venca
 * @see     Padder, traffic analysis discussion, padding discussion
 */
class PaddingGenerator : public Runable, public MixOffspring
{
  protected:

  /**@name methods */
  /*@{*/
    void SendPaddingMessage();
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
    PaddingGenerator(Mix * parent, LogFile * logFile);
    virtual void * Run(void * );
  /*@}*/
};
#endif
