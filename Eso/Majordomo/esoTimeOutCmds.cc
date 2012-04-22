#include "./../../Common/H/labels.h"
#include "./H/majordomo.h"

/**
 * Serves the TO_DEL_FINDER_TBL command.
 *
 * Processes timeout command for deleting tables in Finder.
 *
 *
 * @param   dataFld data comming with timeout 
 * @return  error code
 * @author  Marek, Pavel
 * @see     Finder, Scheduler
 */
Err
Majordomo::CommandDelFinderTbl(MsgField * /*dataFld*/)
{
/*
  ESTime *aTime;
  GMessage *timeOutMsg;
  Finder *fi;   
  Scheduler *sch;
  RelTime *rt;
  Err result;
  MsgFld *mf;

    //disable debugging warnings;
  mf = dataFld;

  if (((Eso *) six) == NULL)
    return KO;
  if((fi = ((Eso *) six)->finder) == NULL)
    return KO;
  if((sch = ((Eso *) six)->scheduler) == NULL)
    return KO;

    //aTime = currentTime
  if((aTime  = new ESTime()) == NULL)
    return KO; 
  
  if(fi->DeleteRecordsByExpiration(*aTime) != OK){
    delete aTime;
    return KO;
  }

  if((rt = new RelTime(cfg_finder_timeout)) == NULL){
    delete aTime;
    return KO;
  }

  *aTime = *aTime + *rt;  
  delete rt;

  if((timeOutMsg = new GMessage()) == NULL){
    delete aTime;
    return KO;
  }

  result = OK;
  if(timeOutMsg->SetAsString(GM_PARAMETERS, "") != OK)
    result = KO;  
  
  if (result == OK)
    sch->SetTimeout(TO_DEL_FINDER_TBL, *aTime, timeOutMsg);

  delete aTime;
  delete timeOutMsg;
*/
  return OK;
}

/**
 * Serves TO_CHECK_AC_COUNT command.
 * 
 * Processes timeout command for controlling count of ACs.
 *
 *
 * @param   dataFld data comming with timeout
 * @return  error code
 * @author  Marek, Pavel
 * @see     ACManager, Scheduler, GenerateRequestForMissingACs(), CreateNextACTimeOutMsg()
 */
Err
Majordomo::CommandControllACCount(MsgField *dataFld)
{
  Scheduler *sch;
  GMessage *inMsg;  
  Char *acType;
  //Int missing;
  ESTime nextTO;
  GMessage *timeOutMsg;

DBG;
  if (((Eso *) six) == NULL)
    return KO;
  if((sch = ((Eso *) six)->scheduler) == NULL)
    return KO;

DBG;
  if(dataFld == NULL)
    return KO;

DBG;    
  if ((inMsg = dataFld->GetAsGMessage()) == NULL)
    return KO;

DBG;
  if((acType = inMsg->GetAsString(GM_CERTIFICATE_TYPE)) == NULL){
    delete inMsg;
    return KO;
  }
  delete inMsg;

DBG;
  if(GenerateRequestForMissingACs(acType) != OK)
    return KO;

DBG;
  if((NextACTimeOut(acType, &nextTO)) != OK)
    return KO;
 
  Char *tmp;
  WriteString(llDebug, "NextACTimeOut will be set to %s\n", tmp = nextTO.Print());
  DELETE(tmp);
DBG;
  if((timeOutMsg = CreateNextACTimeOutMsg(acType)) == NULL) 
    return KO;
  
DBG;
  Err result = sch->SetTimeOut(TO_CHECK_AC_COUNT, timeOutMsg, nextTO);
  DELETE(timeOutMsg);
  
  return result;
}

/**
 * Serves TO_ALLOC
 * 
 * Processes timeout command for allocation. Deletes apropriate records
 * in Allocator and Banker.
 *
 *
 * @param   dataFld data comming with timeout
 * @return  error code
 * @author  Marek, Pavel
 * @see     Allocator, Banker, 
 */
Err
Majordomo::CommandTimeoutAllocation(MsgField *dataFld)
{
  Err        result;
  TID        removeTID;
  Allocator *al;
  Banker    *bn;

  if(dataFld == NULL)
    return KO;
  
  if (((Eso *) six) == NULL)
    return KO;
  if((al = ((Eso *) six)->allocator) == NULL)
    return KO;
#ifndef NO_BANKER
  if((bn = ((Eso *) six)->banker) == NULL)
    return KO;
#endif

    // parse dataFld (get removeTID out of it)
  GMessage *timeOutMsg = dataFld->GetAsGMessage();
  if(timeOutMsg == NULL)
    return KO;
  result = timeOutMsg->GetAsID(GM_TID, &removeTID);
  delete timeOutMsg;
  if(result != OK)
    return KO;

  result = OK;

    // free allocated space
  if((result = al->FreeSpace(removeTID)) != OK)
    WriteString(llWarning, "Majordomo::CommandTimeoutAllocation():"
                " cannot remove allocation %ld", removeTID);
#ifndef NO_BANKER		
    // tell banker not to keep payment plan any more
  bn->DelPayPlan(removeTID);
#endif

  return result;
}

