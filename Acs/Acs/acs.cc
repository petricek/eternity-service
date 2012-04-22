  // has to be the FIRST include !
#include "./../../Common/H/common.h"

#include <stdio.h>
#include <sys/errno.h>
#include <string.h>

#include "./H/acs.h"

#include "./../../Common/LogFile/H/logFile.h"
#include "./../../Common/H/labels.h"

/**
 * Acs constructor.
 *
 * Creates all the objects necessary for Acs to run.
 *
 * @author  Venca
 * @param   configName Name of configuration file that should be opened and 
 *                     from where we should read all the configuration
 *                     information.
 * @see     Majordomo, Sender, Receiver
 */
Acs::Acs(string configName)
{
  configFile = new ConfigFile(configName);
  string log = configFile->GetValue(CFG_LOGFILE);
  if(log.empty())
    fprintf(stderr, "error - no log file specified");

  //configFile->PrintAll();
  logFile = new LogFile(log.c_str());

  logFile->WriteString(llDebug, __FILE__ ":%d:Creating queues.", __LINE__);
  receiverToMajordomo = new MessageQueue(logFile, MSGQ_DEFAULT_BUFFER_LEN);
  majordomoToSender = new MessageQueue(logFile, MSGQ_DEFAULT_BUFFER_LEN);
  
  logFile->WriteString(llDebug, __FILE__ ":%d:Queues created.", __LINE__);

  logFile->WriteString(llDebug, __FILE__ ":%d:Creating static objects.",
		       __LINE__);
  certificateRepository = new CertificateRepository(configFile, logFile);
  if(certificateRepository->LoadCertificates() == KO)
    logFile->WriteString(llDebug, __FILE__ ":%d:LoadCertificates failed.", __LINE__);

  killer = new Killer(logFile);

  addressManager = new AddressManager(this, logFile);

  logFile->WriteString(llDebug, __FILE__ ":%d:Creating running objects.",
		       __LINE__);
  u_short port = atoi(configFile->GetValue(CFG_INCOMING_PORT).c_str());
  receiver = new Receiver(configFile, logFile, killer, receiverToMajordomo, 
                          "", port);
  majordomo = new Majordomo(this, logFile);

  string origin = configFile->GetValue(CFG_NAME);
  sender = new Sender(logFile, killer, majordomoToSender, origin);

  logFile->WriteString(llDebug, __FILE__ ":%d:Creating structure for"
		       " runningObject's ID's.", __LINE__);
  runningObject = new RunningObject();

  logFile->WriteString(llDebug, __FILE__ ":%d:Acs() finished.", __LINE__);
}

/**
 * Acs destructor.
 *
 * Deletes all the objects receiver, sender, majordomo ... 
 * <p>
 * Writes info about exiting into log file and closes it.
 *
 * @author  Venca
 */
Acs::~Acs()
{

  logFile->WriteString(llDebug, __FILE__ ":%d:Deleting running objects ...",
		       __LINE__); 

  DELETE(receiver);
  DELETE(sender);
  DELETE(majordomo);
  DELETE(killer);
  DELETE(addressManager);

  logFile->WriteString(llDebug, __FILE__ ":%d:Deleting static objects ...",
		       __LINE__); 
  DELETE(certificateRepository);

  logFile->WriteString(llDebug, __FILE__ ":%d:Deleting objects", __LINE__); 
  DELETE(receiverToMajordomo);
  DELETE(majordomoToSender);
  
  logFile->WriteString(llDebug, __FILE__ ":%d:Deleting structure for"
		       " RunningObject's ID's", __LINE__); 
  DELETE(runningObject);

    // has to be the last object to delete !!!
  logFile->WriteString(llDebug, __FILE__ ":%d:Deleting logFile !", __LINE__); 
  DELETE(logFile);
  DELETE(configFile);
}

  // Brings all the objets, that have to run, to life.
  // Each in a separate thread
void Acs::Run(void)
{
  logFile->WriteString(llDebug, __FILE__ ":%d:Starting threads.", __LINE__);

  runningObject->SaveRunningObjectID(RECEIVER, StartObject(receiver));
  runningObject->SaveRunningObjectID(SENDER, StartObject(sender));
  runningObject->SaveRunningObjectID(MAJORDOMO, StartObject(majordomo));

    logFile->WriteString(llDebug, __FILE__ ":%d:Threads created, starting"
      "waiting for them for ever.", __LINE__);

  runningObject->WaitForRunningObject(RECEIVER);
  runningObject->WaitForRunningObject(SENDER);
  runningObject->WaitForRunningObject(MAJORDOMO);

}

/**
 * Starts object in aseparate thread.
 *
 * Creates a thread in which the specified  object will run its method Run().
 *
 * @param   object Runable object that implements virtual method Run that
 *                 will be executed in the new thread
 * @return  Id of the running object so that we can wait for the newly
 *          created thread to finish
 * @author  Venca
 * @see     Runable Acs Run()
 */
RunningObjectID Acs::StartObject( Runable *object)
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
