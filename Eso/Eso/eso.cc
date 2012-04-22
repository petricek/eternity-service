  // has to be the FIRST include !
#include "./../../Common/H/common.h"

#include <stdio.h>
#include <sys/errno.h>
#include <string.h>

#include "./H/eso.h"

#include "./../../Common/LogFile/H/logFile.h"
#include "./../../Common/H/labels.h"

/**
 * Eso constructor.
 *
 * Creates all the objects necessary to run
 *
 *
 * @param   configName name of file with configuration information for Eso
 * @author  Marek, Pavel
 * @see     Six, LogFile, Allocator, Banker, TCBWrapper, Finder, Scheduler, TimeSynchronizer, ACManager
 */
Eso::Eso(string configName)
: Six(configName)
{
  logFile->WriteString(llDebug, __FILE__ ":%d:Creating static objects.",
		       __LINE__);

  allocator = new Allocator(this, logFile);
  logFile->WriteString(llDebug, __FILE__ ":%d:Allocator created",
		       __LINE__);

  banker = new Banker(logFile, this);
  logFile->WriteString(llDebug, __FILE__ ":%d:Banker created",
		       __LINE__);

  tcbWrapper = new SWTCBWrapper(this, logFile);
  logFile->WriteString(llDebug, __FILE__ ":%d:TCBWrapper created",
		       __LINE__);

  finder = new Finder(logFile, this);
  logFile->WriteString(llDebug, __FILE__ ":%d:Finder created",
		       __LINE__);
  
  timeSynchronizer = new TimeSynchronizer(logFile, this);
  logFile->WriteString(llDebug, __FILE__ ":%d:TimeSynchronizer created",
		       __LINE__);

  scheduler = new Scheduler(logFile, this);
  logFile->WriteString(llDebug, __FILE__ ":%d:Scheduler created",
		       __LINE__);
  
  Char *tmp;
  bankACManager = new ACManager(ACTYPE_BANK, tmp = GetPath(ACMGR_TABLE_BANK),
                                1, 4, logFile);
  DELETE(tmp);
  esoACManager = new ACManager(ACTYPE_ESO, tmp = GetPath(ACMGR_TABLE_ESO),
                                1, 1, logFile);
  DELETE(tmp);
  
  int thisEsoACCount;
  string thisEsoACCountStr = configFile->GetValue(CFG_THIS_ESO_AC_COUNT);
  if(thisEsoACCountStr.empty())
    thisEsoACCount = 1;
  else
    thisEsoACCount = atoi(thisEsoACCountStr.c_str());
  thisEsoACManager = new ACManager(ACTYPE_THIS_SIX, tmp = GetPath(ACMGR_TABLE_THIS_ESO),
                                   1, thisEsoACCount, logFile);
  DELETE(tmp);
  logFile->WriteString(llDebug, __FILE__ ":%d: ACManagers created",
		       __LINE__);

    // create Majordomo
  CreateMajordomo();

  logFile->WriteString(llDebug, __FILE__ ":%d:Initializing"
		       , __LINE__);

  string result = configFile->GetValue(CFG_INITIALIZE);
  if(strcmp(result.c_str(), "Yes") == 0)
    Initialize();

  logFile->WriteString(llDebug, __FILE__ ":%d:Eso() finished.", __LINE__);

}

/**
 * Eso destructor.
 *
 * Deletes all the objects - frees memory
 *
 *
 * @author  Pavel
 */
Eso::~Eso()
{
  logFile->WriteString(llDebug, __FILE__ ":%d:Deleting static objects ...",
		       __LINE__); 

  delete allocator;
  delete banker;
  delete bankACManager;
  delete esoACManager;
  delete scheduler;
  delete timeSynchronizer;
  delete tcbWrapper;
  delete thisEsoACManager;
}

/**
 * Eso Majordomo creator.
 *
 * Overrides Six::CreateMajordomo method to assign correct majordomo to
 * this type of Six (Eso).
 *
 *
 * @return  void
 * @author  Pavel
 * @see     Six
 */
void 
Eso::CreateMajordomo()
{
  majordomo = new Majordomo(this, logFile);
}


/**
 * Creates running objects.
 *
 * Calls methods that create all threads for both Six and Eso
 *
 *
 * @return  void
 * @author  Pavel
 * @see     Six, Six::CreateRunningObjects()
 */
void 
Eso::CreateRunningObjects()
{
  Six::CreateRunningObjects();
  runningObject->SaveRunningObjectID(SCHEDULER, StartObject(scheduler));
}


/**
 * Waites for running objects.
 *
 * Waits for all threads to stop running, for both Six and Eso
 *
 *
 * @return  void
 * @author  Pavel
 * @see     Six, Six::CreateRunningObjects()
 */
void
Eso::WaitForRunningObjects()
{
  Six::WaitForRunningObjects();
  runningObject->WaitForRunningObject(SCHEDULER);
}


/**
 * Initializes Eso.
 *
 * Calls appropriate initializations, used on startup, mostly for debugging
 *
 *
 * @return  error code
 * @author  Marek, Pavel
 * @see     Majordomo, Scheduler, TimeSynchronizer
 */
Err
Eso::Initialize()
{
   //testing: majordomo asks for onion
  ((Majordomo*) majordomo)->AskForOnion();
    
    // check thisEsoACs
  GMessage *timeOutMsg = ((Majordomo *) majordomo)->CreateNextACTimeOutMsg(ACTYPE_THIS_SIX);
  RelTime timeOut;
  timeOut.SetTime("00:00:00:00:00:01");
  scheduler->SetTimeOut(TO_CHECK_AC_COUNT, timeOutMsg, timeOut);
  DELETE(timeOutMsg);
    
    // check bank ACs
  timeOutMsg = ((Majordomo *) majordomo)->CreateNextACTimeOutMsg(ACTYPE_BANK);
  timeOut.SetTime("00:00:00:00:01:01");
  scheduler->SetTimeOut(TO_CHECK_AC_COUNT, timeOutMsg, timeOut);
  DELETE(timeOutMsg);
    
  if (timeSynchronizer->Init() == KO) return KO; 

  return OK;
}
