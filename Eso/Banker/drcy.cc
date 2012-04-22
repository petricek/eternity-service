/**
 * DRCTimeForPayment (cmd to_payment)
 *
 * Dlouhy_komentar
 *
 * @param   dataInMsgFld
 * @return  
 * @author  Ivana
 * @see     
 */
Err Majordomo::DRCTimeForPayment(MsgField *dataInMsgFld)
{
  GMessage *inMsg=NULL, *dataOutMsg=NULL, *outMsg=NULL, *bankAC=NULL;
  ACID bankACID;
  
  //check argument first
  if( dataInMsgFld == NULL ||
     ( inMsg = dataInMsgFld->GetAsGMessage()) == NULL ){
    WriteString(llWarning, "Majordomo::DRCTimeForPayment():" 
      "No inMsg from scheduller");
    return KO;
  }
  
  //lets generate "data" GMessage
  dataOutMsg = eso->banker->TimeForPayment(inMsg, tID, &bankACID);
  DELETE(inMsg);
  if( dataOutMsg == NULL ) return KO;

  //find bank with given ACID
  if( (bankAC = GetACByACID(ACTYPE_BANK, bankACID)) == NULL ){
    DELETE(dataOutMsg);
    WriteString(llWarning, "Majordomo::DRCTimeForPayment():" 
      "Cannot find bank with ACID: %ld", bankACID);
    return KO;
  }
    
  //create final outMsg
  if( FillReplyToInfo(dataOutMsg) != OK || 
      (outMsg = CreateSendDataMsg(CMD_EB_REQ_FOR_PAY, dataOutMsg, bankAC,
        2*(CFG_SEND_DATA_HOPS))) == NULL ){
    DELETE(dataOutMsg);
    WriteString(llWarning, "Majordomo::DRCTimeForPayment():" 
      "Cannot fill 'ReplyTo' info or encrypt msg for bank");
    return KO;
  }
  DELETE(dataOutMsg);
  
  //insert generated request to a queue, which will be sent to bank
  if ( outMsgQueue->Insert(outMsg) != OK ){
    DELETE(outMsg);
    return KO;
  }

  return OK;
}


/**
 * DRCChallengeFromBank (cmd be_chall)
 *
 * Dlouhy_komentar
 *
 * @param   dataInMsgFld
 * @return  
 * @author  Ivana
 */
Err Majorodomo::DRCChallengeFromBank(MsgField *dataInMsgFld)
{
  GMessage *inMsg=NULL, *dataOutMsg=NULL, *outMsg=NULL, *bankAC=NULL;
  ACID bankACID;

  //check arguments first
  if ( dataInMsgFld == NULL ||
       ( inMsg = dataInMsgFld->GetAsGMessage()) == NULL ){
    WriteString(llWarning, "Majordomo::DRCChallengeFromBank():" 
      "No inMsg for creating answer for bank");
    return KO;
  }
  
  //lets generate "data" GMessage
  dataOutMsg = eso->banker->ChallengeFromBank(inMsg, tID, &bankACID);
  DELETE(inMsg);
  if( dataOutMsg == NULL ) return KO;

  //find bank with given ACID
  if( (bankAC = GetACByACID(ACTYPE_BANK, bankACID)) == NULL ){
    DELETE(dataOutMsg);
    WriteString(llWarning, "Majordomo::DRCChallengeFromBank():" 
      "Cannot find bank with ACID: %ld", bankACID);
    return KO;
  }
    
  //create final outMsg
  if( FillReplyToInfo(dataOutMsg) != OK || 
      (outMsg = CreateSendDataMsg(CMD_EB_RESP_TO_CHALL, dataOutMsg, bankAC,
        2*(CFG_SEND_DATA_HOPS))) == NULL ){
    DELETE(dataOutMsg);
    WriteString(llWarning, "Majordomo::DRCChallengeFromBank():" 
      "Cannot fill 'ReplyTo' info or encrypt msg for bank");
    return KO;
  }
  DELETE(dataOutMsg);
    
  //insert generated answer to a queue, which will be sent to bank
  if ( outMsgQueue->Insert(outMsg) != OK ){
    DELETE(outMsg);
    return KO;
  }

  return OK;
}


/**
 * DRCAnswerFromBank (cmd be_reply_to_resp)
 *
 * Dlouhy_komentar
 *
 * @param   dataInMsgFld
 * @return  
 * @author  Ivana
 * @see     
 */
Err Majordomo::DRCAnswerFromBank(MsgField *dataInMsgFld)
{
  GMessage *inMsg=NULL;

  //check arguments first
  if ( dataInMsgFld == NULL ||
       ( inMsg = dataInMsgFld->GetAsGMessage()) == NULL ){
    WriteString(llWarning, "Majordomo::DRCAnswerFromBank():" 
      "No inMsg in answer from bank");
    return KO;
  }
  eso->banker->AnswerFromBank(inMsg);
  DELETE(inMsg);
  
  return OK;
}


/**
 * DRCTimeOutPayment (cmd to_curr_pay)
 *
 * Dlouhy_komentar
 *
 * @param   dataInMsgFld
 * @return  
 * @author  Ivana
 * @see     
 */
Err Majordomo::DRCTimeOutPayment(MsgField *dataInMsgFld)
{
  GMessage *inMsg=NULL, *dataOutMsg=NULL, *outMsg=NULL, *bankAC=NULL;
  ACID bankACID;
  Int request;
  Err e;

  //check arguments first
  if ( dataInMsgFld == NULL  ||  outMsgQ == NULL  ||
       ( inMsg = dataInMsgFld->GetAsGMessage()) == NULL ){
    WriteString(llWarning, "Majordomo::DRCTimeOutPayment():" 
      "No inMsg from scheduler for creating request/answer to bank");
    return KO;
  }
  
  //lets generate "data" GMessage
  dataOutMsg = eso->banker->TimeOutPayment(inMsg, tID, &bankACID, &request);
  DELETE(inMsg);
  if( dataOutMsg == NULL ) return KO;

  //find bank with given ACID
  if( (bankAC = GetACByACID(ACTYPE_BANK, bankACID)) == NULL ){
    DELETE(dataOutMsg);
    WriteString(llWarning, "Majordomo::DRCTimeOutPayment():" 
      "Cannot find bank with ACID: %ld", bankACID);
    return KO;
  }
    
  ///create final outMsg
  e = FillReplyToInfo(dataOutMsg);
  if( request )
    outMsg = CreateSendDataMsg(CMD_EB_REQ_FOR_PAY, dataOutMsg, bankAC,
      2*(CFG_SEND_DATA_HOPS));
  else
    outMsg = CreateSendDataMsg(CMD_EB_RESP_TO_CHALL, dataOutMsg, bankAC,
      2*(CFG_SEND_DATA_HOPS));
  DELETE(dataOutMsg);
  if( e != OK || outMsg == NULL ){
    if( outMsg ) DELETE(outMsg);
    WriteString(llWarning, "Majordomo::DRCTimeOutPayment():" 
      "Cannot fill 'ReplyTo' info or encrypt msg for bank");
    return KO;
  }
    
  //insert generated request/answer to a queue, which will be sent to bank
  if ( outMsgQueue->Insert(outMsg) != OK ){
    DELETE(outMsg);
    return KO;
  }
  
  return OK;
}

