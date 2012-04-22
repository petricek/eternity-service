#include "./H/majordomo.h"

/**
 * Chooses a method to handle the command with.
 *
 * According to the received command chooses a method to handle the
 * command with.
 *
 *
 * @param   command Command in the incoming message.
 * @param   dataFld Data in the incoming message.
 * @param   acid ID of the certificate to which the data was sent.
 * @return  error code
 * @author  Marek
 * @see     
 */
Err 
Majordomo::SwitchDataReceivedCommand(const Char     *command, 
                                         const MsgField *dataFld,
					 const ACID      acid)
{
  Err result;

  if( SixMajordomo::SwitchDataReceivedCommand(command, dataFld, acid) == OK )
    result = OK;
  else if(strcmp(command, CB_PAY))
    result = DRCMoneyFromClient((MsgField *)dataFld);
  else if(strcmp(command, EB_REQ_FOR_PAY))
    result = DRCRequestFromEso((MsgField *)dataFld);
  else if(strcmp(command, EB_RESP_TO_CHALL))
    result = DRCAnswerFromEso((MsgField *)dataFld);
  else
  {
    WriteString(llWarning, __FILE__":%d: unknown data_received command", __LINE__);
    result = KO;
  }

  return result;
}


/**
 * Serves command from client.
 *
 * Handles command that Client sends after successfull storage of his
 * file on Eso, to inform Bank about the stored file owner.
 *
 *
 * @param   dataFld     Data comming along with command
 * @return  error code
 * @author  Ivana
 * @see     
 */
Err
Majordomo::DRCMoneyFromClient(MsgField *dataFld)
{
  GMessage *inMsg = NULL, *outMsg = NULL;
  GMessage *replyToAC = NULL, *toMixMsg = NULL;
  TID replyToTID;
  
  if( dataFld == NULL || (inMsg = dataFld->GetAsGMessage()) == NULL){
    WriteString(llWarning, __FILE__":%d: No inMsg from client", __LINE__);
    return KO;
  }

  if( inMsg->GetAsID(GM_REPLY_TO_TID, &replyToTID) != OK || 
      (replyToAC = inMsg->GetAsGMessage(GM_REPLY_TO_AC)) == NULL ){
    DELETE(inMsg);
    WriteString(llWarning, __FILE__":%d: No ReplyToInfo from client", __LINE__);
    return KO;
  }

  outMsg = ((Bank *)six)->payMgr->MoneyFromClient(inMsg);
  DELETE(inMsg);
  if( outMsg == NULL ){
    DELETE(replyToAC);
    return KO;
  }
  
  if( outMsg->SetAsID(GM_IN_REPLY_TO_TID, replyToTID) != OK ){
    DELETE(replyToAC);
    DELETE(outMsg);
    WriteString(llWarning, __FILE__":%d: cannot fill ReplyToTID", __LINE__);
    return KO;
  }

  toMixMsg = CreateSendDataMsg(BC_ANSW_PAY, outMsg, replyToAC);
  DELETE(replyToAC);
  DELETE(outMsg);
  
  if( toMixMsg == NULL ){
    WriteString(llWarning, __FILE__":%d: cannot encrypt msg", __LINE__);
    return KO;
  }
    
  if( outMsgQueue->Insert(toMixMsg) != OK ){
    DELETE(toMixMsg);
    WriteString(llWarning, __FILE__":%d: cannot insert msg into outMsgQ", __LINE__);
    return KO;
  }

  return OK;
}


/**
 * Serves Eso's request for money.
 *
 * Finds appropriate oAuth to send back to Eso to verify it is the
 * correct file owner not faking file storage.
 *
 *
 * @param   dataFld    data comming along with command
 * @return  error code
 * @author  Ivana
 * @see     co_se_toho_tyka
 */
Err
Majordomo::DRCRequestFromEso(MsgField *dataFld)
{
  GMessage *inMsg = NULL, *outMsg = NULL;
  GMessage *replyToAC = NULL, *toMixMsg = NULL;
  TID replyToTID;
  
  if( dataFld == NULL || (inMsg = dataFld->GetAsGMessage()) == NULL){
    WriteString(llWarning, __FILE__":%d: No inMsg from eso", __LINE__);
    return KO;
  }

  if( inMsg->GetAsID(GM_REPLY_TO_TID, &replyToTID) != OK || 
      (replyToAC = inMsg->GetAsGMessage(GM_REPLY_TO_AC)) == NULL ){
    DELETE(inMsg);
    WriteString(llWarning, __FILE__":%d: No ReplyToInfo from eso", __LINE__);
    return KO;
  }

  outMsg = ((Bank *)six)->payMgr->RequestFromEso(inMsg);
  DELETE(inMsg);
  if( outMsg == NULL ){
    DELETE(replyToAC);
    return KO;
  }
  
  if( outMsg->SetAsID(GM_IN_REPLY_TO_TID, replyToTID) != OK ){
    DELETE(replyToAC);
    DELETE(outMsg);
    WriteString(llWarning, __FILE__":%d: cannot fill ReplyToTID", __LINE__);
    return KO;
  }

  toMixMsg = CreateSendDataMsg(BE_CHALL, outMsg, replyToAC);
  DELETE(replyToAC);
  DELETE(outMsg);
  
  if( toMixMsg == NULL ){
    WriteString(llWarning, __FILE__":%d: cannot encrypt msg", __LINE__);
    return KO;
  }
    
  if( outMsgQueue->Insert(toMixMsg) != OK ){
    DELETE(toMixMsg);
    WriteString(llWarning, __FILE__":%d: cannot insert msg into outMsgQ", __LINE__);
    return KO;
  }

  return OK;
}


/**
 * Serves Eso's answer to reply to its request for money.
 *
 * Compares if Eso sent the same MAC as Bank holds for requested
 * payment.
 *
 *
 * @param   dataFld    data comming along with command
 * @return  error code
 * @author  Ivana
 * @see     
 */
Err
Majordomo::DRCAnswerFromEso(MsgField *dataFld)
{
  GMessage *inMsg = NULL, *outMsg = NULL;
  GMessage *replyToAC = NULL, *toMixMsg = NULL;
  TID replyToTID;
  
  if( dataFld == NULL || (inMsg = dataFld->GetAsGMessage()) == NULL){
    WriteString(llWarning, __FILE__":%d: No inMsg from eso", __LINE__);
    return KO;
  }

  if( inMsg->GetAsID(GM_REPLY_TO_TID, &replyToTID) != OK || 
      (replyToAC = inMsg->GetAsGMessage(GM_REPLY_TO_AC)) == NULL ){
    DELETE(inMsg);
    WriteString(llWarning, __FILE__":%d: No ReplyToInfo from eso", __LINE__);
    return KO;
  }

  outMsg = ((Bank *)six)->payMgr->AnswerFromEso(inMsg);
  DELETE(inMsg);
  if( outMsg == NULL ){
    DELETE(replyToAC);
    return KO;
  }
  
  if( outMsg->SetAsID(GM_IN_REPLY_TO_TID, replyToTID) != OK ){
    DELETE(replyToAC);
    DELETE(outMsg);
    WriteString(llWarning, __FILE__":%d: cannot fill ReplyToTID", __LINE__);
    return KO;
  }

  toMixMsg = CreateSendDataMsg(BE_REPLY_TO_RESP, outMsg, replyToAC);
  DELETE(replyToAC);
  DELETE(outMsg);
  
  if( toMixMsg == NULL ){
    WriteString(llWarning, __FILE__":%d: cannot encrypt msg", __LINE__);
    return KO;
  }
    
  if( outMsgQueue->Insert(toMixMsg) != OK ){
    DELETE(toMixMsg);
    WriteString(llWarning, __FILE__":%d: cannot insert msg into outMsgQ", __LINE__);
    return KO;
  }

  return OK;
}

