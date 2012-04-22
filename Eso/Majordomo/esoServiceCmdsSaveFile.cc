#include "./../Eso/H/eso.h"
#include "./H/majordomo.h"

/**
 * Serves CE_REQ_FOR_STOR command.
 *
 * Handles the CE_REQ_FOR_STOR command, according to Eso-Others interface.
 * dataFld comming has this structure: <pre>
 *   Reply-To-ID: ID of a message assigned by sender (Client)
 *   Reply-To-AC: AC to reply to
 *   File-Size: Size of file Client whishes to store
 *   Storage-Time: RelTime for which the file should be stored
 * </pre>
 *
 * @param   dataFld data comming with command
 * @return  error code
 * @author  Pavel
 * @see     FillReplyAllocation(), Allocator, Banker, Scheduler
 */
Err
Majordomo::DRCRequestForStorage(const MsgField *dataFld)
{
  Err            result;
  TID            replyToTID;
  Size           fileSize;
  GMessage      *mixMsg;
  GMessage      *outMsg;
  GMessage      *replyToAC;
  MsgField      *payPlanFld;
  RelTime       *storageTime;

DBG;
    // get all fields needed
  if(DRCRFSGetFields(dataFld, &replyToAC,
                              &replyToTID,
                              &fileSize, 
			      &storageTime) != OK)
    return KO;

DBG;
    // try to allocate requested space
  if(((Eso *) six)->allocator->AllocSpace(tID, fileSize) != OK)
  {
    delete replyToAC;
    delete storageTime;
    return KO;               // exit: KO, response: none
  }
#ifndef NO_BANKER
DBG;
    // generate payment plan
  if((payPlanFld = ((Eso *) six)->banker->MakePayPlan(fileSize, *storageTime, tID)) == NULL)
  {
      // free allocated space
    ((Eso *) six)->allocator->FreeSpace(tID);
    delete replyToAC;
    delete storageTime;
    return KO;               // exit: KO, response: none
  }
#else
  payPlanFld = NULL;
#endif  
  delete storageTime;

#ifndef NO_SCHEDULER  
DBG;
    // set timeout interesting for Allocator and Banker
  GMessage *timeoutInfo = new GMessage();
  if(timeoutInfo == NULL)
  {
#ifndef NO_BANKER
    delete payPlanFld;
#endif  
    delete replyToAC;
    return KO;
  }
DBG;
  if(timeoutInfo->SetAsID(GM_TID, tID) != OK)
  {
#ifndef NO_BANKER
    delete payPlanFld;
#endif  
    delete replyToAC;
    return KO;
  }
  RelTime timeout(TO_ALLOC_REL_TIMEOUT);
  
DBG;
  if(((Eso *) six)->scheduler->SetTimeOut(TO_ALLOC, timeoutInfo, timeout) != OK)
    WriteString(llWarning, "Majordomo::DRCRequestForStorage():"
          "Cannot set timeout TO_ALLOC");
  delete timeoutInfo;
#endif  // NO_SCHEDULER

    // reply YES
    
DBG;
    //fill outMsg
  outMsg = new GMessage();
  if(FillReplyAllocation(outMsg, replyToTID, payPlanFld) != OK)
  {
#ifndef NO_BANKER
    delete payPlanFld;
#endif  
    delete replyToAC;
    return KO;
  }
#ifndef NO_BANKER
  delete payPlanFld;
#endif  
   
DBG;
    // create "send data" msg for Mix 
  mixMsg = CreateSendDataMsg(EC_REPLY_ALLOC, outMsg, replyToAC); 
  delete replyToAC;
  delete outMsg;
  if(mixMsg == NULL){
    WriteString(llError, "Majordomo::DRCRequestForStorage():"
	  "Cannot create mixMsg");
    return KO;
  }
  
DBG;
    // insert the result in outMsgQueue
  if((result = outMsgQueue->Insert(mixMsg)) != OK)
    delete mixMsg;

  return result;
}


/**
 * Serves CE_DATA_TO_STOR command.
 *
 * Handles the CE_DATA_TO_STOR command, according to Eso-Others interface. <pre>
 *   In-Reply-To-TID: ID of a EC_REPLY_ALLOC message
 *   Reply-To-ID: ID of a message assigned by sender (Client)
 *   Reply-To-AC: AC to reply to
 *   FFID:
 *   Keywords:
 *   Data:
 * </pre>
 *
 * @param   dataFld data comming with command
 * @return  error code
 * @author  Pavel
 * @see     FillReplyFileStored(), Allocator, Banker, Scheduler, TCBWrapper
 */
Err 
Majordomo::DRCDataToStore(const MsgField *dataFld, const ACID acid)
{
  Err            result;
  TID            inReplyToTID;
  TID            replyToTID;
  GMessage      *mixMsg;
  GMessage      *outMsg;
  GMessage      *replyToAC;
  MsgField      *ffid;
  MsgField      *keywords;
  MsgField      *dataToStoreFld;

  WriteString(llDebug, __FILE__":%d:DRCDataToStore!!!!!", __LINE__);
DBG;
    // parse incomming GMessage
  if(DRCDTSGetFields(dataFld, &replyToAC,
			      &replyToTID,
			      &inReplyToTID, 
                              &ffid, 
			      &keywords, 
			      &dataToStoreFld) != OK)
    return KO;
 DBG;   
    // check validity of allocation
  if(! ((Eso *) six)->allocator->AllocValid(inReplyToTID, dataToStoreFld->SizeOf()))
  {
    delete replyToAC;
    delete ffid;
    delete keywords;
    delete dataToStoreFld;
    return KO;               // exit: KO, response: none
  }
DBG;
    // save the data
  result = ((Eso *) six)->tcbWrapper->SaveFile(acid, tID, ffid, keywords, dataToStoreFld);
  delete ffid;
  delete keywords;
  delete dataToStoreFld;
  if(result != OK)
    return KO;               // exit: KO, response: none
DBG;
    // free allocated space
  if(((Eso *) six)->allocator->FreeSpace(inReplyToTID) != OK)
    WriteString(llWarning, "Cannot free allocated space, after file saved.");

DBG;
#ifndef NO_BANKER
    // tell Banker, data arrived
  if(((Eso *) six)->banker->SchedulePays(inReplyToTID, tID) != OK)
    WriteString(llWarning, "Majordomo::DRCDataToStore(): Banker::SchedulePays KO");
#endif
    // reply YES
DBG;
    
  outMsg = new GMessage();
    //fill outMsg 
  if(FillReplyFileStored(outMsg, replyToTID) != OK)
    return KO;
  DBG; 
    //create "send data" msg for Mix 
  mixMsg = CreateSendDataMsg(EC_REPLY_STOR, outMsg, replyToAC); 
  delete replyToAC;
  delete outMsg;
DBG;
  if(mixMsg == NULL){
    WriteString(llError, "Majordomo::DRCDataToStore():"
	  "Cannot create mixMsg");
    return KO;
  }
 DBG; 
    // insert the result in outMsgQueue
  if((result = outMsgQueue->Insert(mixMsg)) != OK)
    delete mixMsg;
DBG;
  return result;
}

/**
 * Parses GMessage comming with CE_REQ_FOR_STOR command.
 *
 * GMessage comming with command CE_REQ_FOR_STOR should have this structure: <pre>
 *   Reply-To-ID: ID of a message assigned by sender (Client)
 *   File-Size: Size of file Client whishes to store
 *   Storage-Time: RelTime for which the file should be stored
 * </pre>
 *
 * @param   dataFld data comming with command
 * @param   replyToAC out parameter for Reply-To access certificate
 * @param   replyToTID out parameter for Reply-To message ID
 * @param   fileSize out parameter for size of file whished to stored
 * @param   storageTime out parameter for time for which the file should be stored
 * @return  error code
 * @author  Pavel
 * @see     DRCRequestForStorage()
 */
Err
Majordomo::DRCRFSGetFields(const MsgField *dataFld,
			       GMessage      **replyToAC,
                               TID            *replyToTID,
                               Size           *fileSize,
			       RelTime       **storageTime)
{
    // check if dataFld is valid
  if(dataFld == NULL)
    return KO;               // exit: KO, response: none
    
    // convert dataFld to GMessage
  GMessage *dataMsg = ((MsgField *)dataFld)->GetAsGMessage();
  if(dataMsg == NULL)
    return KO;

    // get ReplyToTID
  Err result = dataMsg->GetAsID(GM_REPLY_TO_TID, replyToTID); 
  if(result != OK)
  {
    WriteString(llWarning, "Majordomo: Request_for_storage: ReplyToTID not found");
    delete dataMsg;
    return KO;
  }

    // get FileSize
  result = dataMsg->GetAsID(GM_FILE_SIZE, fileSize);
  if(result != OK)
  {
    WriteString(llWarning, "Majordomo: Request_for_storage: FileSize not found");
    delete dataMsg;
    return KO;
  }

    // get ReplyToAC
  if((*replyToAC = dataMsg->GetAsGMessage(GM_REPLY_TO_AC)) == NULL)
  {
    WriteString(llWarning, "Majordomo: Request_for_storage: ReplyToAC not found");
    delete dataMsg;
    return KO;
  }

    // get StorageTime
  if((*storageTime = new RelTime()) == NULL)
  {
    delete replyToAC;
    delete dataMsg;
  }
  result = dataMsg->GetAsBasicTime(GM_STORAGE_TIME, *storageTime);
  if(result != OK)
  {
    WriteString(llWarning, "Majordomo: Request_for_storage: StorageTime not found");
    delete storageTime;
    delete replyToAC;
    delete dataMsg;
    return KO;
  }

  delete dataMsg;
  
  return OK;
}

/**
 * Parses the GMessage comming with command CE_DATA_TO_STORE.
 *
 * GMessage comming with command CE_DATA_TO_STORE should have this structure:
 * <pre>
 *   Reply-To-ID: ID of a message assigned by sender (Client)
 *   FFID:
 *   Keywords:
 *   Data:
 * </pre>
 *
 *
 * @param   dataFld data comming with command
 * @param   replyToAC out parameter for Reply-To access certificate
 * @param   replyToTID out parameter for Reply-To message ID
 * @param   inReplyToTID out parameter for In-Reply-To message ID
 * @param   ffid out parameter for file find ID
 * @param   keywords out parameter for keywords
 * @param   dataToStoreFld out parameter for data to store
 * @return  error code
 * @author  Pavel
 * @see     DRCDataToStore()
 */
Err
Majordomo::DRCDTSGetFields(const MsgField *dataFld,
			       GMessage      **replyToAC,
			       TID            *replyToTID,
			       TID            *inReplyToTID,
			       MsgField      **ffid,
			       MsgField      **keywords,
                               MsgField      **dataToStoreFld)
{
  WriteString(llDebug, __FILE__":%d:DRCDTSGetFields", __LINE__);
DBG;
    // check if dataFld is valid
  if(dataFld == NULL)
    return KO;
DBG;
    // convert dataFld to GMessage 
  GMessage *dataMsg = ((MsgField *)dataFld)->GetAsGMessage();
  if(dataMsg == NULL)
    return KO;
DBG;
    // get InReplyToTID
  if(dataMsg->GetAsID(GM_IN_REPLY_TO_TID, inReplyToTID) != OK)
  {
    delete dataMsg;
    return KO;
  }

DBG;
    // get ReplyToTID
  if(dataMsg->GetAsID(GM_REPLY_TO_TID, replyToTID) != OK)
  {
    delete dataMsg;
    return KO;
  }
DBG;
    // get ReplyToAC
  if((*replyToAC = dataMsg->GetAsGMessage(GM_REPLY_TO_AC)) == NULL)
  {
    delete dataMsg;
    return KO;
  }
DBG;

    // get FFID
  if((*ffid = dataMsg->GetAsMsgField(GM_FFID)) == NULL)
  {
    delete replyToAC;
    delete dataMsg;
    return KO;
  }
DBG;

    // get Keywords
  if((*keywords = dataMsg->GetAsMsgField(GM_KEYWORDS)) == NULL)
  {
    delete ffid;
    delete replyToAC;
    delete dataMsg;
    return KO;
  }
DBG;

    // get Data
  if((*dataToStoreFld = dataMsg->GetAsMsgField(GM_DATA)) == NULL)
  {
    delete keywords;
    delete ffid;
    delete replyToAC;
    delete dataMsg;
    return KO;
  }
DBG;
  delete dataMsg;

DBG;
  return OK;
}
