#ifndef ESO_H
#define ESO_H

  // POZOR - jen pro ladeni - obchazi bankere
//#define NO_BANKER

#define ESO_CONFIG_FILE "/home/eternity/conf/eso.conf"

  // has to be the FIRST include !
#include "./../../../Common/H/common.h"

#include "./../../../Common/ConfigFile/H/configFile.h"
#include "./../../../Common/LogFile/H/logFile.h"
#include "./../../../Common/Runable/H/runable.h"
#include "./../../../Six/Six/H/six.h"
#include "./../../Common/H/constants.h"

class Eso;

#include <pthread.h>

#include "./../../../Common/MessageQueue/H/messageQueue.h"
#include "./../../../Common/RunningObject/H/runningObject.h"



class Receiver;
class Killer;
class Sender;
class Majordomo;
class CertificateRepository;
class TCBWrapper;
class Cipherer;
class ACManager;
class Banker;
class Allocator;
class Scheduler;
class Finder;
class TimeSynchronizer;

/**
 * Main class that represents the whole Eso service.
 *
 * This class inherits its functionality from Six, so its
 * capable of communicating with Mix servers. It holds all classes
 * needed to run the storage service of ES.
 *
 * @author Venca
 * @see    Six, MessageQueu
 */
class Eso: public Six
{
  public:
  /**@name attributes*/
  /*@{*/
      /// Trusted computing base wrapper.
    TCBWrapper *tcbWrapper;
      /// Manager of Banks' access certificates.
    ACManager *bankACManager;
      /// Manager of other Esos' access certificates.
    ACManager *esoACManager;
      /// Manager of this Eso's access certificates.
    ACManager *thisEsoACManager;
      /// Space allocating class.
    Allocator *allocator;
      /// Class responsible for money handling.
    Banker    *banker;
      /// Timeouts generating class.
    Scheduler *scheduler;
      /// Search and download controler.
    Finder    *finder;
      /** Class that keeps the right time and synchronizes current
          time with other Esos. */
    TimeSynchronizer *timeSynchronizer;
  /*@}*/

  public:
  /**@name attributes*/
  /*@{*/
      /// Constructor.
    Eso(string configName = ESO_CONFIG_FILE);      
      /// Destructor.
    virtual ~Eso();                               

      /** Creates all Runable objects, running them as threads.
          Overrides Sixes virtual method. */
    virtual void CreateRunningObjects();
      /** Waits for all running objects to stop.
          Overrides Sixes virtual method. */
    virtual void WaitForRunningObjects();
      /** Sets Six::majordomo to new Eso's Majordomo.
          Overrides Sixes virtual method. */
    virtual void CreateMajordomo();
    
      /** Does some really cool initializations needed especially 
       * when debugging. */
    Err Initialize();
  /*@}*/
};

#include "./../../../Common/Receiver/H/receiver.h"
#include "./../../../Common/Sender/H/sender.h"
#include "./../../../Common/Killer/H/killer.h"
#include "./../../../Common/ACManager/H/acManager.h"
#include "./../../Majordomo/H/majordomo.h"
#include "./../../TCBWrapper/H/tcbWrapper.h"
#include "./../../../Common/Cipherer/H/cipherer.h"
#include "./../../Banker/H/banker.h"
#include "./../../Allocator/H/allocator.h"
#include "./../../Scheduler/H/scheduler.h"
#include "./../../Finder/H/finder.h"
#include "./../../TimeSynchronizer/H/timesynch.h"

#endif
