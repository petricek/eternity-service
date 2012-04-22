#ifndef ACS_H
#define ACS_H

#define ACS_CONFIG_FILE "/home/eternity/conf/acs.conf"

  // has to be the FIRST include !
#include "./../../../Common/H/common.h"

#include "./../../../Common/ConfigFile/H/configFile.h"
#include "./../../../Common/LogFile/H/logFile.h"

#include "./../../../Common/Runable/H/runable.h"
#include "./../../../Common/RunningObject/H/runningObject.h"

class Acs;

#include <pthread.h>

#include "./../../../Common/MessageQueue/H/messageQueue.h"

class Receiver;
class Killer;
class Sender;
class Majordomo;
class CertificateRepository;
class AddressManager;

/**
 * Access certificates server.
 *
 * Stores and distributes access certificates of other services like
 * Eso Bank Mix.  It handles requests for publishing and retreival
 * of certificates.
 *
 * @author  Pechy, Venca
 * @see     Mix, Acs, Eso, Client
 */
class Acs
{

  // properties

  public:

    /**@name attributes */
    /*@{*/
      /// Queue connecting Receiver and Majordomo
    MessageQueue *receiverToMajordomo;
      /// Queue connecting Majordomo and Sender 
    MessageQueue *majordomoToSender;
  
      /// Repository containing certificates
    CertificateRepository *certificateRepository;
      /// Manager of certificates
    AddressManager *addressManager;
      /// Class responsible for shutdown of all objects
    Killer *killer;
      /// Configuration file
    ConfigFile *configFile;
      /// Log file
    LogFile *logFile;
 
      /// Message receiver
    Receiver *receiver;
      /// Requests handler
    Majordomo *majordomo;
      /// Message sender
    Sender *sender;

      /// Object that is used to wait for the threads to finish
    RunningObject *runningObject;
    /*@}*/

  
    /**@name methods */
    /*@{*/
      /// Constructor
    Acs(string configName = ACS_CONFIG_FILE);
      /// Destructor
    virtual ~Acs();
      /// This method runs as a thread and processes all the requests
    virtual void Run();
    /*@}*/

  private:
    /**@name methods */
    /*@{*/
      /// Used to dispatch an object in a thread
    RunningObjectID StartObject( Runable *runableObject);
    /*@}*/
};

  // static objects
#include "./../../../Common/CertificateRepository/H/certificateRepository.h"

  // types of running objects
#include "./../../../Common/Receiver/H/receiver.h"
#include "./../../../Common/Sender/H/sender.h"
#include "./../../../Common/Killer/H/killer.h"
#include "./../../Majordomo/H/majordomo.h"
#include "./../../AddressManager/H/addressManager.h"

#endif
