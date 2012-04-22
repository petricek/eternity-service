#include "./H/majordomo.h"

/**
 * Sets the outMsg as RequestForHeaders .
 *
 * Fills all needed fields in a message that is sent as a reply to
 * CMD_REQ_FOR_HEADERS command.
 *
 *
 * @param   parametr
 * @return  error code
 * @author  Marek
 * @see     co_se_toho_tyka
 */
Err
Majordomo::FillRequestForHeaders(GMessage *outMsg, MsgField *aFAM,
                GMessage *transportKey, Int depth, Int width)
{
  Err result;
  
  string strD = six->configFile->GetValue(CFG_MAX_SEARCH_DEPTH);
  Int depthD = atoi(strD.c_str());
  string strW = six->configFile->GetValue(CFG_MAX_SEARCH_WIDTH);
  Int widthD = atoi(strD.c_str());

  result = OK;
  if (FillReplyToInfo(outMsg) != OK) 
    result = KO;       
  if(outMsg->SetAsMsgField(GM_FAM, aFAM) != OK)
    result = KO;
  if(outMsg->SetAsInt(GM_SEARCH_DEPTH, min(depth, depthD) - 1) != OK)
    result = KO;
  if(outMsg->SetAsInt(GM_SEARCH_WIDTH, min(width, widthD)) != OK)
    result = KO;
  if(outMsg->SetAsGMessage(GM_TRANSPORT_KEY, transportKey) != OK)
    result = KO;
  
  return result;
}
 
/**
 * Sets the outMsg as RequestForFile.
 *
 * Fills all needed fields in a message that is sent as a reply to
 * CMD_REQ_FOR_FILE command.
 *
 *
 * @param   parametr
 * @return  error code
 * @author  Marek
 * @see     co_se_toho_tyka
 */
Err
Majordomo::FillRequestForFile(GMessage *outMsg, MsgField * aFFID,GMessage *transportKey, Int depth, Int width)
{
  Err result;

  string strD = six->configFile->GetValue(CFG_MAX_SEARCH_DEPTH);
  Int depthD = atoi(strD.c_str());
  string strW = six->configFile->GetValue(CFG_MAX_SEARCH_WIDTH);
  Int widthD = atoi(strD.c_str());
  
  result = OK;
  if (FillReplyToInfo(outMsg) != OK) 
   result = KO;       
  if(outMsg->SetAsMsgField(GM_FFID, aFFID) != OK)
    result = KO; 
  if(outMsg->SetAsInt(GM_SEARCH_DEPTH, min(depth, depthD) - 1) != OK)
    result = KO;
  if(outMsg->SetAsInt(GM_SEARCH_WIDTH, min(width, widthD)) != OK)
    result = KO;
  if(outMsg->SetAsGMessage(GM_TRANSPORT_KEY, transportKey) != OK)
    result = KO;
  
  return result;
}
/**
 * Sets the outMsg as ReplyHeader .
 *
 * Fills all needed fields in a message that is sent as a 
 * CMD_REPLY_HEADER command.
 *
 *
 * @param   parametr
 * @return  error code
 * @author  Marek
 * @see     co_se_toho_tyka
 */
Err
Majordomo::FillReplyHeader(GMessage *outMsg, TID inReplyToTID, GMessage *aHeader)
{
  Err result;

  result = OK;
  if(outMsg->SetAsID(GM_IN_REPLY_TO_TID, inReplyToTID) != OK)
    result = KO;
  if(outMsg->SetAsGMessage(GM_HEADER, aHeader) != OK)
    result = KO;
    
  return result;
}

/**
 * Sets the outMsg as ReplyFile .
 *
 * Fills all needed fields in a message that is sent as a 
 * CMD_REPLY_FILE command.
 *
 *
 * @param   parametr
 * @return  error code
 * @author  Marek
 * @see     co_se_toho_tyka
 */
Err
Majordomo::FillReplyFile(GMessage *outMsg, TID inReplyToTID, MsgField *aFile)
{
  Err result;
  
  result = OK;
  if(outMsg->SetAsID(GM_IN_REPLY_TO_TID, inReplyToTID)!= OK)
    result = KO;
  if(outMsg->SetAsMsgField(GM_FILE, aFile) != OK)
    result = KO;
    
  return result;
}

/**
 * Sets the outMsg as ReplyAllocation.
 *
 * Fills all needed fields in a message that is sent as a reply to
 * CE_REQ_FOR_STOR command.
 *
 *
 * @param   parametr
 * @return  error code
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err
Majordomo::FillReplyAllocation(GMessage *outMsg, TID inReplyToTID, MsgField *payPlan)
{
  Err result;

  result = OK;
  if (FillReplyToInfo(outMsg) != OK) 
    result = KO;       
  if(outMsg->SetAsID(GM_IN_REPLY_TO_TID, inReplyToTID) != OK)
    result = KO;
#ifndef NO_BANKER
  if(outMsg->SetAsMsgField(GM_PAY_PLAN, payPlan) != OK)
    result = KO;
#endif
  outMsg->DebugHeaders(logFile, "What is going to be in reply to CE_REQ_FOR_STOR?");
    
  return result;
}

/**
 * Sets the outMsg as ReplyFileStored.
 *
 * Fills all needed fields in a message that is sent as a reply to
 * CE_DATA_TO_STOR command.
 *
 *
 * @param   parametr
 * @return  error code
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err
Majordomo::FillReplyFileStored(GMessage *outMsg, TID inReplyToTID)
{
  Err result;
  result = OK;
  if (FillReplyToInfo(outMsg) != OK) 
    result = KO;       
  if(outMsg->SetAsID(GM_IN_REPLY_TO_TID, inReplyToTID) != OK)
    result = KO;
  if(outMsg->SetAsString(GM_RESULT, LBL_YES) != OK)
    result = KO;
  outMsg->DebugHeaders(logFile, "What is going to be in reply to CE_DATA_TO_STOR?");
    
  return result;
}

