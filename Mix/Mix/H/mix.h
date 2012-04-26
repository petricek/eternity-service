#ifndef MIX_H
#define MIX_H

#define MIX_CONFIG_FILE "/home/eternity/conf/mix.conf"

#define MSGQ_PADDING_BUFFER_LEN 50

  // has to be the FIRST include !
#include "./../../../Common/H/common.h"

#include "./../../../Common/ConfigFile/H/configFile.h"
#include "./../../../Common/LogFile/H/logFile.h"

class Mix;

#include <pthread.h>

#include "./../../../Common/MessageQueue/H/messageQueue.h"
#include "./../../../Common/Runable/H/runable.h"
#include "./../../../Common/RunningObject/H/runningObject.h"

  /// thread stack is not very large by default, so let's adjust that
#define THREAD_STACK_SIZE 32768

class Cipherer;
class KeyManager;
class CertificateRepository;
class AddressManager;
class MessageCreator;
class MessageIDRepository;
class Receiver;
class Linker;
class Translator;
class Padder;
class PaddingGenerator;
class Sender;
class Majordomo;
class Chopper;
class SignalManager;
class Killer;

/**
 * Mix server.
 *
 * Implements the ability to route messages in the spirit of onion
 * routing slightly enhanced. The message that mix receives is
 * decrypted so that it is possible to find out where to send the
 * message next. The piece of information about where to send the
 * message consists of two (that is what we chose but any finite
 * number would go) recipients. This allows Mix for a much bigger
 * reliability when delivering messages routed through many hops. 
 *
 * It also implements some techniques suitable for making traffic analysis 
 * harder. These techniques are:
 * <ul>
 *   <li> Header padding
 *   <li> Data padding
 *   <li> Onion routing
 *   <li> Traffic padding
 * </ul>
 *
 * @author  Pechy, Venca
 * @see     Acs, Eso, Client, Bank, Translator, Padder, PaddingGenerator
 */
class Mix
{
  public:

  /**@name attributes */
  /*@{*/
      // 7 queues
    MessageQueue *receiverToTranslator;
    MessageQueue *chopperTranslatorToPadder;
    MessageQueue *paddingGeneratorToPadder;
    MessageQueue *padderToSender;
    MessageQueue *translatorToLinker;
    MessageQueue *translatorToPadder;
    MessageQueue *linkerChopperToMajordomo;
    MessageQueue *receiverToChopper;
    MessageQueue *majordomoToSender;
  
      // 10 static objects
    KeyManager *keyManager;
    Cipherer *cipherer;
    AddressManager *addrMgr;
    CertificateRepository *certificateRepository;
    MessageIDRepository *messageIDRepository;
    MessageCreator *msgCreator;
    //Scheduler *scheduler;
    Killer *killer;
    ConfigFile *configFile;
    LogFile *logFile;
 
      // 10 running objects (8 types)
    Receiver *receiverNet;
    Receiver *receiverSix;
    Translator *translator;
    Padder *padder;
    PaddingGenerator *paddingGenerator;
    Linker *linker;
    Majordomo *majordomo;
    Chopper *chopper;
    SignalManager *sigMgr;
    Sender *senderNet;
    Sender *senderSix;

      /// object needed for waiting for running objects (now threads, but possibly processes)
    RunningObject *runningObject;
  /*@}*/

  /**@name methods */
  /*@{*/
    Mix(string configName = MIX_CONFIG_FILE);
    virtual ~Mix();
    virtual void Run(void);
    Err InitializeRandomGenerator();
  /*@}*/

  private:
  /**@name methods */
  /*@{*/
    RunningObjectID StartObject( Runable *runableObject);
  /*@}*/
};

  // static objects
#include "./../../../Common/Cipherer/H/cipherer.h"
//#include "./../../../Common/Scheduler/H/scheduler.h"
#include "./../../../Common/MessageIDRepository/H/messageIDRepository.h"
#include "./../../../Common/CertificateRepository/H/certificateRepository.h"
#include "./../../../Common/KeyManager/H/keyManager.h"

#include "./../../AddressManager/H/addressManager.h"
#include "./../../MessageCreator/H/messageCreator.h"

  // running objects
#include "./../../../Common/Receiver/H/receiver.h"
#include "./../../../Common/Sender/H/sender.h"
#include "./../../../Common/SignalManager/H/signalManager.h"
#include "./../../../Common/Killer/H/killer.h"

#include "./../../Translator/H/translator.h"
#include "./../../Padder/H/padder.h"
#include "./../../PaddingGenerator/H/paddingGenerator.h"
#include "./../../Linker/H/linker.h"
#include "./../../Majordomo/H/majordomo.h"
#include "./../../Chopper/H/chopper.h"

#endif
