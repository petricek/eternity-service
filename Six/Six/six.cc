  // has to be the FIRST include !
#include "./../../Common/H/common.h"

#include <stdio.h>
#include <sys/errno.h>
#include <string.h>

#include "./H/six.h"

#include "./../../Common/LogFile/H/logFile.h"
#include "./../../Common/H/labels.h"

/**
 * Creates all the objects necessary to run.
 *
 * In Six constructor the information from ConfigFile is read
 * and the basic Six structure 
 * <pre>
 *   Receiver
 *     receiverToMajordomo queue
 *
 *     majordomoTosender queue
 *   Sender
 * </pre>
 * is created. 
 * Majordomo must be created in the descedants of Six.
 *
 *
 * @param   configName name of the ConfigFile 
 * @author  Pavel 
 * @see     Receiver, Sender, MessageQueue, SixMajordomo 
 */
Six::Six(string configName)
:Debugable()
{
  configFile = new ConfigFile(configName);
  
    // create logFile
  string log = configFile->GetValue(CFG_LOGFILE);
  if(log.empty())
    fprintf(stderr, "Fatal error - no log file specified in config file\n"
                    "Six will not function properly");
  logFile = new LogFile(log.c_str());
DBG;
    // get path
  path = configFile->GetValue(CFG_SIX_PATH);
  if(path.empty())
    WriteString(llFatalError, "No six path specified. "
                          "Six will not function properly");

    // get mix-six protocol version
  mixSixProtoVer = configFile->GetValue(CFG_MIX_SIX_PROTOCOL_VERSION);
  if(path.empty())
    WriteString(llFatalError, "No communication protocol version specified"
                            "Six will not function properly.");
 
    // create message queues
  logFile->WriteString(llDebug, __FILE__ ":%d:Creating queues.", __LINE__);
  receiverToMajordomo = new MessageQueue(logFile, MSGQ_DEFAULT_BUFFER_LEN);
  majordomoToSender = new MessageQueue(logFile, MSGQ_DEFAULT_BUFFER_LEN);
  logFile->WriteString(llDebug, __FILE__ ":%d:Queues created.", __LINE__);

    // create Killer and Cipherer
  logFile->WriteString(llDebug, __FILE__ ":%d:Creating static objects.", __LINE__);
  cipherer = new Cipherer(logFile);
  logFile->WriteString(llDebug, __FILE__ ":%d:Cipherer created", __LINE__);
  killer = new Killer(logFile);
  logFile->WriteString(llDebug, __FILE__ ":%d:Killer created", __LINE__);
  logFile->WriteString(llDebug, __FILE__ ":%d:Static objects created.", __LINE__);
 
    // create Majordomo
  //CreateMajordomo();
  logFile->WriteString(llDebug, __FILE__ ":%d:Creating running objects.", __LINE__);

    // create receiver and sender
  u_short inPort = atoi(configFile->GetValue(CFG_INCOMING_PORT).c_str());
  receiver = new Receiver(configFile, logFile, killer, 
                          receiverToMajordomo, "", inPort);
  string origin = configFile->GetValue(CFG_NAME);
  string tmp = configFile->GetValue(CFG_MIX_ADDRESS);
  u_short port = atoi(configFile->GetValue(CFG_MIX_PORT).c_str());
  sender = new Sender(logFile, killer, majordomoToSender, origin, tmp, port);

  
  logFile->WriteString(llDebug, __FILE__ ":%d:Creating structure for"
		       " runningObject's ID's.", __LINE__);
/*
    // create transactionManager
  Char *tmpPath;
  transactionManager = new TransactionManager(tmpPath = GetPath("transaction.log"), logFile);
  DELETE(tmpPath);
*/
    // create runningObject
  runningObject = new RunningObject();


  logFile->WriteString(llDebug, __FILE__ ":%d:Six() finished.", __LINE__);
}

/**
 * Six destructor.
 *
 * Deletes all members.
 *
 *
 * @author  Pavel 
 * @see     Receiver, Sender, MessageQueue 
 */
Six::~Six()
{

  logFile->WriteString(llDebug, __FILE__ ":%d:Deleting running objects ...",
		       __LINE__); 

  delete receiver;
  delete sender;
  delete majordomo;

  logFile->WriteString(llDebug, __FILE__ ":%d:Deleting static objects ...",
		       __LINE__); 

  delete cipherer;
  delete configFile;
  delete killer;

  logFile->WriteString(llDebug, __FILE__ ":%d:Deleting objects", __LINE__); 
  delete receiverToMajordomo;
  delete majordomoToSender;
  
  logFile->WriteString(llDebug, __FILE__ ":%d:Deleting structure for"
		       " RunningObject's ID's", __LINE__); 
  delete runningObject;
//  delete transactionManager;

    // has to be the last object to delete !!!
  logFile->WriteString(llDebug, __FILE__ ":%d:Deleting logFile !", __LINE__); 
}

/**
 * Function running Six.
 *
 * Brings all the objets, that have to run, to life.
 * (Each in a separate thread.)
 *
 * @return  void 
 * @author  Pavel 
 * @see     RunningObject 
 */
void Six::Run()
{
  CreateRunningObjects();
  WaitForRunningObjects();
}


/**
 * Creates majordomo.
 *
 * This method must be overloaded in Six descendants.
 *
 *
 * @return  void 
 * @author  Pavel
 * @see     SixMajordomo
 */
void Six::CreateMajordomo()
{
  fprintf(stderr, "This shouldn't be called. Six::CreateMajordomo has to" \
                  " be redifined in its descendant\n");
  majordomo = NULL;
}


/**
 * Creates running objects.
 *
 * Brings to life all running objects: Sender, Receiver, Majordomo.
 *
 *
 * @return  void 
 * @author  Pavel
 * @see     Receiver, SixMajordomo, Sender
 */
void Six::CreateRunningObjects()
{
  logFile->WriteString(llDebug, __FILE__ ":%d:Starting threads.", __LINE__);

  runningObject->SaveRunningObjectID(RECEIVER, StartObject(receiver));
  runningObject->SaveRunningObjectID(SENDER, StartObject(sender));
  runningObject->SaveRunningObjectID(MAJORDOMO, StartObject(majordomo));
}


/**
 * Waits for running objects forever.
 *
 * Calls method of runningObject WaitForRunningObject, that waits for 
 * running object forever.
 *
 *
 * @return  void 
 * @author  Pavel
 * @see     RunningObject
 */
void Six::WaitForRunningObjects()
{
   logFile->WriteString(llDebug, __FILE__ ":%d:Threads created, starting"
      "waiting for them for ever.", __LINE__);

  runningObject->WaitForRunningObject(RECEIVER);
  runningObject->WaitForRunningObject(SENDER);
  runningObject->WaitForRunningObject(MAJORDOMO);
}


/**
 * Returns SixMix protocol version.
 *
 * Returns the protocol version got from the configFile in constructor.
 *
 *
 * @return  char* the version 
 * @author  Pavel
 * @see     ConfigFile
 */
Char *Six::GetMixSixVersion()
{
  Char *version = new Char[strlen(mixSixProtoVer.c_str())];
  if(version == NULL)
    return NULL;
  strcpy(version, mixSixProtoVer.c_str());

  return version; 
}


/**
 * Returns the six path with appended pathSuffix.
 *
 * Creates string concatenating the sixPath (the path to SystemData of Six directory)
 * and the path suffix and returns it.
 *
 *
 * @param   pathSuffix suffix to append behind the six path
 * @return  concatenated pathSuffix to sixPath 
 * @author  Marek, Pavel 
 * @see     ConfigFile 
 */
Char *Six::GetPath(Char *pathSuffix)
{
  Int pathLength = MyStrLen(path.c_str()) + MyStrLen(pathSuffix) + 1;
  Char *wholePath = new Char[pathLength];
  if(wholePath == NULL)
    return NULL;
  strcpy(wholePath, path.c_str());
  if(pathSuffix != NULL)
    strcat(wholePath, pathSuffix);

  return wholePath;
}


/**
 * Starts runable object.
 *
 * This function is used to start objects running in separate threads.
 *
 *
 * @param   object object to start (must be descendant of Runable)
 * @return  runningObjectID 
 * @author  Pavel 
 * @see     Runable, RunningObject, RunningObjectID
 */
//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
RunningObjectID Six::StartObject( Runable *object)
{
  pthread_t id;

  if (pthread_create(&id, NULL, RunObject, (void *) object) != 0) {
    logFile->WriteString(llError, __FILE__ ":%d:pthread_create() - %s",
                         __LINE__, strerror(errno));

    return (RunningObjectID) 0;
  } 
  else
    return (RunningObjectID) id;
}

