#include "./H/sixMajordomo.h"


/**
 * Saves ACs of given type.
 *
 * Calls method of ACManager of given type to save ACs.
 *
 *
 * @param   type type of the AC
 * @param   ac ac to save 
 * @return  error code
 * @author  Marek
 * @see     ACManager
 */
Err 
SixMajordomo::InsertAC(Char *type, GMessage *ac)
{
  ACManager *acMgr;
  DBG;
  acMgr = GetPtrToACManager(type);
  DBG;  
  if(acMgr == NULL)
    return KO;
  DBG;
  return acMgr->InsertAC(ac);
}

/**
 * Returns ACs of given type.
 *
 * Calls method of ACManager of given type to return
 * count ACs of given type not included in notIn queue.
 *
 *
 * @param   type type of the AC
 * @param   count count of ACs wanted 
 * @param   notIn queue of ACs not wanted to include 
 * @return  queue of returned ACs 
 * @author  Marek
 * @see     ACManager
 */
Queue *
SixMajordomo::GetACs(Char *type, const int count, Queue *notIn) 
{
  ACManager *acMgr;

  acMgr = GetPtrToACManager(type);

  if(acMgr == NULL)
    return NULL;
    
  return acMgr->GetACs(count, notIn);
}


/**
 * Get AC of given type and ACID.
 *
 * Calls method of ACManager of given type to get AC of given ACID.
 *
 *
 * @param   type type of the AC
 * @param   acid acid to find 
 * @return  found ac
 * @author  Marek
 * @see     ACManager
 */
GMessage *
SixMajordomo::GetACByACID(Char *type, ACID acID)
{
  ACManager *acMgr;

  acMgr = GetPtrToACManager(type);

  if(acMgr == NULL)
    return NULL;
    
  return acMgr->GetACByACID(acID);
}
/**
 * Returns next timeout of ACManager of given type.
 *
 * Calls method of ACManager of given type to return next timeout.
 *
 *
 * @param   type type of the AC
 * @param   t returned time 
 * @return  error code
 * @author  Marek
 * @see     ACManager
 */
Err 
SixMajordomo::NextACTimeOut(Char *type, ESTime *t)
{
  ACManager *acMgr;

  acMgr = GetPtrToACManager(type);

  if(acMgr == NULL)
    return KO;
    
  *t = acMgr->NextTimeOut();
  return OK;
}


/**
 * Cretates timeout message.
 *
 * Calls method of ACManager of given type to save ACs of given type.
 *
 * Creates timeout message understood by TO_CHECK_AC_COUNT command serve.
 *
 *
 * @param   type type of the AC
 * @return  acTimeOutMsg 
 * @author  Marek
 * @see     ACManager
 */
GMessage *
SixMajordomo::CreateNextACTimeOutMsg(Char *type)
{
  GMessage *timeOutMsg;

  if((timeOutMsg = new GMessage()) == NULL)
    return NULL;

  if(timeOutMsg->SetAsString(GM_CERTIFICATE_TYPE, type) != OK) {
    DELETE(timeOutMsg);
    return NULL;
  }

  return timeOutMsg;
}


/**
 * Test count of ACs of given type.
 *
 * Calls method of ACManager of given type to test count of ACs.
 *
 *
 * @param   type type of the AC
 * @param   missing returned count of missing ACs 
 * @return  error code
 * @author  Marek
 * @see     ACManager
 */
Err 
SixMajordomo::TestACCount(Char *type, Int *missing)
{
  ACManager *acMgr;
  DBG;
  WriteString(llDebug, "SixMajordomo::TestACCount(): enetring method");

  acMgr = GetPtrToACManager(type);

  if(acMgr == NULL)
    return KO;
  
  DBG;  
  return acMgr->TestACCount(missing);
}

/**
 * Generates request for ACs of given type.
 *
 * Test count of ACs of given type. If there are some missing,
 * generates request for missing them and puts it into outMsgQueue.
 *
 *
 * @param   type type of the AC
 * @return  error code
 * @author  Marek
 * @see     ACManager
 */
Err 
SixMajordomo::GenerateRequestForMissingACs(Char *acType)
{
  Int missing;
  GMessage *mixMsg = NULL;

  DBG;
  WriteString(llDebug, "SixMajordomo::CommandControllACCount():"
                           "Entering method");
 
  missing = 0;
  if (TestACCount(acType, &missing) != OK){
    return KO;
  }
  DBG;
  WriteString(llDebug, "SixMajordomo: missing %d", missing);

  if(missing <= 0)
    return OK;

    // to generate this six ACs, generate onions
  if(strcmp(acType, ACTYPE_THIS_SIX) == 0)
  {
      // create get_onion
    mixMsg = CreateGenerateOnionMsg();
   if(mixMsg == NULL){
      WriteString(llError, __FILE__":%d:Cannot create generate_onion message.", __LINE__);
    }
    else{
      DBG;
      mixMsg->DebugHeaders(logFile, "mix Msg (generate_onion) vypada");
    } 

      // multipy get_onion message
    GMessage *tmp;
    for(int i=1; i <= missing; i++) {
  DBG;
      tmp = new GMessage(mixMsg);
      outMsgQueue->Insert(tmp);
    }
  
    if(mixMsg != NULL)
      delete mixMsg;
  }
    // to get ACs of other types, create get_certificates message
  else {
    mixMsg = CreateGetACsMsg(acType, missing);
    if(mixMsg == NULL)
      WriteString(llError, __FILE__":%d:Cannot generate get_certificates message.", __LINE__);
    else {
      mixMsg->DebugHeaders(logFile, "mix Msg (get_certificates) vypada");
      outMsgQueue->Insert(mixMsg);
    }
  }

  return OK;
}


/**
 * Asks for onion, if necessary.
 *
 * Test count of ACs of THIS_SIX. If there are some missing,
 * generates request for onion and puts it into outMsgQueue.
 *
 *
 * @param   type type of the AC
 * @param   ac ac to save 
 * @return  error code
 * @author  Marek
 * @see     ACManager
 */
Err 
SixMajordomo::AskForOnion()
{
 DBG;
  outMsgQueue = new Queue();
  GenerateRequestForMissingACs(ACTYPE_THIS_SIX);
  FeedSenderWithOutMsgQueue();
  return OK;
}


