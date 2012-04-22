#ifndef PADDER_H
#define PADDER_H

class Padder;

  // has to be the FIRST include !
#include "./../../../Common/H/common.h"

#include "./../../../Common/Runable/H/runable.h"
#include "./../../../Common/MessageQueue/H/messageQueue.h"
#include "./../../../Common/Schedulable/H/schedulable.h"
#include "./../../../Common/Shutdownable/H/shutdownable.h"

#include "./../../MixOffspring/H/mixOffspring.h"
#include <queue>

  /// Message type
enum MessageType {
  mtPadding,
  mtMessage
};

/**
 * Mix traffic padder.
 *
 * Pads the traffic (generates additional messages, delays despatching
 * messages, ...) All these techniques make traffic analysis harder.
 *
 * @author  Pechy, Venca
 * @see     PaddingGenerator
 */
class Padder:  public Runable, public MixOffspring, public Schedulable, public Shutdownable
{
  public:

  /**@name attributes */
  /*@{*/
  Size intervalLength;
  Size load;
  /*@}*/

  /**@name methods */
  /*@{*/
    Padder(Mix * parent, LogFile *padderLogFile);
    virtual ~Padder();

    virtual void *Run(void *);
    void AdjustRestTime(MessageType messageType);

    virtual void Shutdown();
    virtual void HandleEvent(int eventID); 
  /*@}*/
};
#endif
