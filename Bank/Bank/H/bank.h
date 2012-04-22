// ***********************************************************************
// 
// File: eso.h
// Name: eso
// Author: Venca
// Last modified: 1.1.1999
// 
// Description: This file contains interface to eso object that receives
// messages from network and decides if they should be delivered locally
// or sent over the network. It also generates padding to make traffic 
// analysis harder.
//
// ***********************************************************************

#ifndef BANK_H
#define BANK_H


#define ACMGR_TABLE_THIS_SIX "ACManager/thisSix.acs"
#define BANK_CONFIG_FILE "/home/eternity/conf/bank.conf"

  // has to be the FIRST include !
#include "./../../../Common/H/common.h"

#include "./../../../Common/ConfigFile/H/configFile.h"
#include "./../../../Common/LogFile/H/logFile.h"
#include "./../../../Common/Runable/H/runable.h"
#include "./../../../Six/Six/H/six.h"

class Bank;

#include <pthread.h>

#include "./../../../Common/MessageQueue/H/messageQueue.h"
#include "./../../../Common/RunningObject/H/runningObject.h"



class Receiver;
class Killer;
class Sender;
class Majordomo;
class CertificateRepository;
class Cipherer;
class ACManager;
class PayManager;
class KeyManager;

/**
 * Main class managing Bank service.
 *
 * This class derives its behaviour from general Six extending its
 * functionality with some features we would like the Bank to have.
 *
 *
 * @author  Marek
 * @see     Six
 */
class Bank: public Six
{
  public:

  /**@name attributes */
  /*@{*/
    ACManager *thisSixACManager;
    KeyManager *keyMgr;
    PayManager *payMgr;
  /*@}*/

  /**@name methods */
  /*@{*/
      /// Constructor
    Bank(string configName = BANK_CONFIG_FILE);      
      /// Destructor
    virtual ~Bank();                                
      
      /** 
       * Sets Six::majordomo to new Eso's Majordomo.
       * Overrides Sixes virtual method. 
       * */
    virtual void CreateMajordomo();
      /**
       * Does some really cool initializations needed especially 
       * for debugging purposes. 
       * */
    Err Initialize();
  /*@}*/
};

  // types of running objects
#include "./../../../Common/Receiver/H/receiver.h"
#include "./../../../Common/Sender/H/sender.h"
#include "./../../../Common/Killer/H/killer.h"
#include "./../../../Common/ACManager/H/acManager.h"
#include "./../../Majordomo/H/majordomo.h"
#include "./../../../Common/Cipherer/H/cipherer.h"
#include "./../../PayManager/H/payManager.h"
#include "./../../KeyManager/H/keyManager.h"

#endif
