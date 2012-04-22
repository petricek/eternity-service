  // has to be the FIRST include !
#include "./../../Common/H/common.h"

#include <stdio.h>
#include <sys/errno.h>
#include <string.h>

#include <pthread.h>

#include "./../../Common/H/labels.h"
#include "./../../Common/LogFile/H/logFile.h"
#include "./../../Common/RunningObject/H/runningObject.h"

#include "./H/mix.h"

/**
 * Mix constructor.
 *
 * Creates all the objects necessary to run Mix server.
 *
 * @param   configName Config file name form where to read configuration 
 *                     information.
 * @author  Venca, Pechy
 * @see     All other objects
 */
Mix::Mix(string configName)
{
  //printf("%s",configName.c_str());
  configFile = new ConfigFile(configName);
  string log = configFile->GetValue(CFG_LOGFILE);
  if(log.empty())
    fprintf(stderr, "error - no log file specified");

  logFile = new LogFile(log.c_str());

  if (InitializeRandomGenerator() == KO) {
    logFile->WriteString(llWarning, __FILE__ ":%d:Error in seeding "
                         "random generator.", __LINE__);
  }

  logFile->WriteString(llDebug, __FILE__ ":%d:Creating queues.", __LINE__);
  receiverToTranslator = new MessageQueue(logFile, MSGQ_DEFAULT_BUFFER_LEN);
  translatorToLinker = new MessageQueue(logFile, MSGQ_DEFAULT_BUFFER_LEN);
  chopperTranslatorToPadder=new MessageQueue(logFile,MSGQ_DEFAULT_BUFFER_LEN);
  paddingGeneratorToPadder=new MessageQueue(logFile,MSGQ_PADDING_BUFFER_LEN);
  linkerChopperToMajordomo=new MessageQueue(logFile, MSGQ_DEFAULT_BUFFER_LEN);
  receiverToChopper = new MessageQueue(logFile, MSGQ_DEFAULT_BUFFER_LEN);
  padderToSender = new MessageQueue(logFile, MSGQ_DEFAULT_BUFFER_LEN);
  majordomoToSender = new MessageQueue(logFile,MSGQ_DEFAULT_BUFFER_LEN);

  logFile->WriteString(llDebug, __FILE__ ":%d:Creating static objects.",
		       __LINE__);
  cipherer = new Cipherer(logFile);
  certificateRepository = new CertificateRepository(configFile, logFile);
  addrMgr = new AddressManager(this, logFile);
  msgCreator = new MessageCreator(configFile, logFile, certificateRepository,
                                  cipherer, keyManager);
  
  keyManager = new KeyManager(configFile, logFile, cipherer, 
                              chopperTranslatorToPadder);

  messageIDRepository = new MessageIDRepository(logFile);
  //scheduler = new Scheduler(this, logFile);
  killer = new Killer(logFile);
  messageIDRepository = new MessageIDRepository(logFile);

  logFile->WriteString(llDebug, __FILE__ ":%d:Creating running objects.",
		       __LINE__);

  int tmpInt;
  tmpInt = atoi((configFile->GetValue(CFG_FROM_MIX_PORT)).c_str());
  receiverNet = new Receiver(configFile, logFile, killer, receiverToTranslator, 
                             "mix", (u_short) tmpInt);

  tmpInt = atoi((configFile->GetValue(CFG_FROM_SIX_PORT)).c_str());
  receiverSix = new Receiver(configFile, logFile, killer, receiverToChopper, 
                             "", (u_short) tmpInt);

  translator = new Translator(this, logFile);
  padder = new Padder(this, logFile);
  paddingGenerator = new PaddingGenerator(this, logFile);
  linker = new Linker(this, logFile);
  majordomo = new Majordomo(this, logFile);
  chopper = new Chopper(this, logFile);
  senderNet = new Sender(logFile, killer, padderToSender, "");
  senderSix = new Sender(logFile, killer, majordomoToSender, "");
  sigMgr = new SignalManager(logFile);

  logFile->WriteString(llDebug, __FILE__ ":%d:Creating structure for"
		       " runningObject's ID's.", __LINE__);
  runningObject = new RunningObject();

  logFile->WriteString(llDebug, __FILE__ ":%d:Constructor Mix() finished.", 
                       __LINE__);
}

/**
 * Mix destructor.
 *
 * Deletes all objects created in constructor. This should happen only
 * after catching SIGTERM signal indicating the shutdown.
 *
 * @return  
 * @author  Venca, Pechy
 * @see     All other objects
 */
Mix::~Mix()
{

  logFile->WriteString(llDebug, __FILE__ ":%d:Deleting running objects.",
		       __LINE__); 

  DELETE(receiverNet);
  DELETE(receiverSix);
  DELETE(translator);
  DELETE(padder);
  DELETE(linker);
  DELETE(senderNet);
  DELETE(senderSix);
  DELETE(majordomo);
  DELETE(chopper);
  DELETE(sigMgr);

  logFile->WriteString(llDebug, __FILE__ ":%d:Deleting static objects.",
		       __LINE__); 
  DELETE(addrMgr);
  DELETE(configFile);
  DELETE(msgCreator);
  DELETE(messageIDRepository);
  DELETE(cipherer);
  DELETE(keyManager);
  DELETE(certificateRepository);

  logFile->WriteString(llDebug, __FILE__ ":%d:Deleting objects.", __LINE__); 
  DELETE(receiverToTranslator);
  DELETE(padderToSender);
  DELETE(translatorToLinker);
  DELETE(chopperTranslatorToPadder);
  DELETE(paddingGeneratorToPadder);
  DELETE(linkerChopperToMajordomo);
  DELETE(receiverToChopper);
  DELETE(majordomoToSender);
  
  logFile->WriteString(llDebug, __FILE__ ":%d:Deleting structure for"
		       " RunningObject's ID's.", __LINE__); 
  DELETE(runningObject);

    // has to be the last object to DELETE !!!
  logFile->WriteString(llDebug, __FILE__ ":%d:Deleting logFile.", __LINE__); 
  DELETE(logFile);
}

/**
 * Creates and starts all running objects (ie. threads).
 *
 * Just creates all runnig objects and then falls asleep waiting for them.
 *
 * @return  void
 * @author  Venca, Pechy
 * @see     All running objects
 */
void
Mix::Run(void)
{
  pthread_sigmask(SIG_BLOCK, NULL, NULL);

  logFile->WriteString(llDebug, __FILE__ ":%d:Starting threads.", __LINE__);

  runningObject->SaveRunningObjectID(RECEIVER_NET, StartObject(receiverNet));
  runningObject->SaveRunningObjectID(RECEIVER_SIX, StartObject(receiverSix));
  runningObject->SaveRunningObjectID(TRANSLATOR, StartObject(translator));
  runningObject->SaveRunningObjectID(PADDER, StartObject(padder));
  runningObject->SaveRunningObjectID(SENDER_NET, StartObject(senderNet));
  runningObject->SaveRunningObjectID(SENDER_SIX, StartObject(senderSix));
  runningObject->SaveRunningObjectID(SIGNAL_MANAGER, StartObject(sigMgr));
  runningObject->SaveRunningObjectID(LINKER, StartObject(linker));
  runningObject->SaveRunningObjectID(MAJORDOMO, StartObject(majordomo));
  runningObject->SaveRunningObjectID(CHOPPER, StartObject(chopper));

    logFile->WriteString(llDebug, __FILE__ ":%d:Threads created, starting"
      "waiting for them for ever.", __LINE__);

  runningObject->WaitForRunningObject(RECEIVER_NET);
  runningObject->WaitForRunningObject(RECEIVER_SIX);
  runningObject->WaitForRunningObject(TRANSLATOR);
  runningObject->WaitForRunningObject(PADDER);
  runningObject->WaitForRunningObject(SENDER_NET);
  runningObject->WaitForRunningObject(SENDER_SIX);
  runningObject->WaitForRunningObject(SIGNAL_MANAGER);
  runningObject->WaitForRunningObject(MAJORDOMO);
  runningObject->WaitForRunningObject(CHOPPER);
  runningObject->WaitForRunningObject(LINKER);
}

/**
 * Starts one running object.
 *
 * In this method, the running object is started. For now, the thread
 * is started, but the method could be modified to start normal
 * process (in the system that doesn't support threads). Be portable!
 *
 * @param   object Running object created by Run().
 * @return  RunningObjectID Running object ID, we can then wait for it.
 * @author  Pechy
 * @see     Run(), threads discussion
 */
RunningObjectID
Mix::StartObject( Runable *object)
{
    // initalizing pthread attribute object for request servant threads
    // we must have bigger stack then is by default (def. is 8K)
  pthread_attr_t pthreadAttr;
  if (pthread_attr_init( &pthreadAttr) == -1) {
    logFile->WriteString(llError, __FILE__ ":%d:pthread_attr_init() %s.",
      __LINE__, strerror(errno));
    exit( 1);
  }

    // adjust attributes for servant threads
  pthread_attr_setstacksize( &pthreadAttr, (size_t) THREAD_STACK_SIZE);

  pthread_t id;
  if (pthread_create(&id, &pthreadAttr, RunObject, (void *) object) != 0) {
    logFile->WriteString(llError, __FILE__ ":%d:pthread_create() - %s",
                         __LINE__, strerror(errno));

    return (RunningObjectID) 0;
  } 
  else
    return (RunningObjectID) id;
}

/**
 * Initializes a random generator.
 *
 * We use normal random() system function. This function is not the
 * best one, but is still useable. We use /dev/urandom for seeding the
 * generator (srandom()). The /dev/urandom should be good enough, if
 * not, we can easily modify the method to use /dev/random.
 *
 * @param   void
 * @return  KO if the generator can't be initialized, OK otherwise.
 * @author  Pechy
 * @see     random(4), srandom(3), random(3) man pages.
 */
Err
Mix::InitializeRandomGenerator()
{
  int readBytes, 
      err = KO;
  unsigned long randLong = 1;

  string randomLongString = configFile->GetValue(CFG_DEBUG_RANDOM_LONG_SEED);

  if (randomLongString != "") {
    logFile->WriteString(llDebug, __FILE__ ":%d:Read debug random "
                         "seed %s form " "configFile.", __LINE__, 
			 randomLongString.c_str());
    randLong = atol(randomLongString.c_str());
    err = OK;
  }
  else {
    FILE *file;
    if ((file = fopen("/dev/urandom", "r")) == NULL) {
      logFile->WriteString(llWarning, __FILE__ ":%d:Error opening "
                           "/dev/urandom.", __LINE__);
    }
    else {
      if ((readBytes = fread(&randLong, 1, sizeof(long), file)) == 0) {
        logFile->WriteString(llWarning, __FILE__ ":%d:Error reading "
	                     "/dev/urandom.", __LINE__);
      }
      else {
        logFile->WriteString(llDebug, __FILE__ ":%d:Read %d bytes from "
	                     "/dev/urandom.", __LINE__, readBytes);
        err = OK;
      }

    fclose(file);
    }
  }

  logFile->WriteString(llDebug, __FILE__ ":%d:Seeding with %lu.", 
                       __LINE__, randLong);
  srandom(randLong);

  return err;
}
