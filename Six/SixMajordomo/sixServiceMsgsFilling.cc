#include "./H/sixMajordomo.h"

/**
 * Sets GMessage as Test.
 *
 * Sets the Parameters-field of GMessage to given msgField. 
 *
 *
 * @param   gMsg message to fill
 * @param   msgFld field tu put in gMsg
 * @return  error code 
 * @author  Marek
 * @see     
 */
Err 
SixMajordomo::FillTest(GMessage *gMsg, MsgField *msgFld)
{
  if(gMsg->SetAsMsgField(GM_PARAMETERS, msgFld) != OK){
    return KO;
  }
  
  return OK;
}

/**
 * Sets replyToInfo of GMessage.
 *
 * Sets two fields of the gMsg (replyToTID, replyToAC).
 *
 *
 * @param   gMsg message to fill
 * @return  error code 
 * @author  Marek 
 * @see     GMessage
 */
Err 
SixMajordomo::FillReplyToInfo(GMessage *gMsg)
{
  Queue *acQueue;
  GMessage *myAC;
  
DBG;
  if(gMsg == NULL)
    return KO;
    
DBG;
  if((acQueue = GetACs(ACTYPE_THIS_SIX, 1)) == NULL)
    return KO;
DBG;
  if((myAC = (GMessage *) acQueue->First()) == NULL){
    DELETE(acQueue);
    return KO;
  }
  myAC->DebugHeaders(logFile, "What does myAC look like ?");
  myAC->SetLogFile(logFile);
  gMsg->SetLogFile(logFile);
DBG;
  
  if(gMsg->SetAsGMessage(GM_REPLY_TO_AC, myAC) != OK){
    delete myAC;
    return KO;
  }
  delete myAC;
DBG;
    //if the queue is longer, delete whole queue
  while((myAC = (GMessage *)acQueue->Next()) != NULL){
    DELETE(myAC);
  }
  DELETE(acQueue);
DBG;     
  if (gMsg->SetAsID(GM_REPLY_TO_TID, tID) != OK) 
    return KO;
DBG;
  return OK;
}


