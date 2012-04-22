#ifndef SIX_H
#define SIX_H

  // has to be the FIRST include !
#include "./../../../Common/H/common.h"

#include "./../../../Common/Cipherer/H/cipherer.h"
#include "./../../../Common/ConfigFile/H/configFile.h"
#include "./../../../Common/LogFile/H/logFile.h"
#include "./../../../Common/Runable/H/runable.h"
#include "./../../../Common/TransactionManager/H/transactionManager.h"

class Six;

#include <pthread.h>

#include "./../../../Common/MessageQueue/H/messageQueue.h"
#include "./../../../Common/RunningObject/H/runningObject.h"


class Receiver;
class Killer;
class Sender;
class SixMajordomo;

/**
 * Ancestor of some Services running above Mix.
 *
 * Abstract class that provides capability to communication
 * with Mix server. Sender, Receiver and msgQueues 
 * to and from majordomo are created and common information
 * from the SixConfigFile is got. 
 *
 * @author  Pavel, Marek
 * @see     Sender, Receiver, MessageQueue, ConfigFile, LogFile, Debugable
 */
class Six: public Debugable
{


protected:
  /**@name attributes */
  /*@{*/
    ///MixSix protocol version got from ConfigFile
  string mixSixProtoVer;
    ///Path to the Six SystemData directory got from ConfigFile.
  string path;
    ///Object needed for waiting for running objects (now threads, but possibly processes).
  RunningObject *runningObject;
  /*@}*/

  /**@name methods */
  /*@{*/
    ///In this method majordomo must be assigned a valid SixMajordomo ancestor pointer.
  virtual void CreateMajordomo();
    ///In this method running objects are created.
  virtual void CreateRunningObjects();
    ///In this method waiting for running objects to stop is started.
  virtual void WaitForRunningObjects();
    ///Returns ID of a running object.
  RunningObjectID StartObject(Runable *object);
  /*@}*/

public:  
  /**@name attributes */
  /*@{*/
    ///Queue connecting receiver to majordomo.
  MessageQueue *receiverToMajordomo;
    ///Queue connecting majordomo to sender.
  MessageQueue *majordomoToSender;

    ///Cipherer for ciphering in- and outcoming messages.
  Cipherer *cipherer;
    ///Class responsible for shutdown of all objects.
  Killer *killer;
    ///Log file.
  ConfigFile *configFile;

    ///Class responsible for receiving commands from Mix.
  Receiver *receiver;
    ///Class responsible for handling all messages.
  SixMajordomo *majordomo;
    ///Class responsible for sending commands to Mix.
  Sender *sender;
  /*@}*/

  /**@name methods */
  /*@{*/
    ///Constructor.
  Six(string configName);
    ///Destructor.
  virtual ~Six();
    ///Method that runs as a thread and processes all requests.
  virtual void Run();
  
    ///Method that returns MixSix communication protocol version got from ConfigFile.
  Char *GetMixSixVersion();
    ///Method that adds path prefix got from ConfigFile before pathSuffix got as a parameter.
  Char *GetPath(Char *pathSuffix = NULL);
  /*@}*/
};

#include "./../../../Common/Receiver/H/receiver.h"
#include "./../../../Common/Sender/H/sender.h"
#include "./../../../Common/Killer/H/killer.h"
#include "./../../SixMajordomo/H/sixMajordomo.h"

#endif
