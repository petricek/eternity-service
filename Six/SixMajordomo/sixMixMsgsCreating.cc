#include "./../../Common/GMessage/H/arrayGMessage.h"
#include "./H/sixMajordomo.h"

/**
 * Creates "test" message for Mix.
 *
 * Creates and fills the fields of Test message for Mix.
 * <pre>
 * End situation (what is returned):
 * [mixMsg]Command: test
 *         Parameters: dataFld
 * </pre>
 *
 *
 * @param   dataFld dataField to insert to message 
 * @return  created GMessage
 * @author  Marek 
 * @see     
 */
GMessage *
SixMajordomo::CreateTestMsg(MsgField *dataFld)
{
  GMessage *mixMsg;
  Err result;
  
  if((mixMsg = new GMessage()) == NULL){
    delete mixMsg;
    return NULL;
  }

  result = OK;
  if (mixMsg->SetAsString(GM_COMMAND, CMD_TEST) != OK)
    result = KO;
  if (mixMsg->SetAsMsgField(GM_PARAMETERS, dataFld) != OK)
    result = KO;

  if (result != OK){
    delete mixMsg;
    return NULL;
  }

  return mixMsg;
}

      
/**
 * Creates "send_data" message for Mix.
 *
 * Creates and fills the fields of "send_data"  message for Mix.
 * <pre>
 * end situation (what is returned):
 * [outMsg]Command: send_data
 *         Parameters: 
 * [msg3]    AC: ac
 *           Add-Hops: hops
 *           Data:
 * [msg2]      ACID:
 *             Body: {CIPHERED WITH PUB_KEY OF ac}
 * [msg1]         Command: commandSer
 *                Data: dataSer         
 *                Version: eso_others protocol version
 * </pre>
 *
 *
 * @param   commandSer command to send to Service 
 * @param   dataSer data to send to Service 
 * @param   ac ac of recipient
 * @param   hops number of hops to add 
 * @return  created GMessage
 * @author  Marek 
 * @see Cipherer, GMessage, MsgField    
 */
GMessage *
SixMajordomo::CreateSendDataMsg(Char *commandSer, 
                        GMessage *dataSer, GMessage *ac, 
                        Size hops)
{
  GMessage *outMsg, *msg1, *msg2, *msg3;
  ID acid;
  GMessage *pubKey;
  MsgField *decryptedMsgField;
  MsgField *encryptedMsgField;
  Err result;
  Cipherer *cip;
DBG;
ac->DebugHeaders(logFile, "CreateSendDataMsg got ac:");
    //create message for Service
  if((msg1 = new GMessage()) == NULL){
    return NULL;
  }
dataSer->DebugHeaders(logFile, __FILE__":Data to wrap for sending");
DBG;
    //insert into the message for Service
    // the "command" and "data" and "version"fields
  result = OK;
  result = msg1->SetAsGMessage(GM_DATA, dataSer);
  if (msg1->SetAsString(GM_COMMAND, commandSer) != OK)
    result = KO;
  if(msg1->SetAsString(GM_VERSION, LBL_ESO_OTHERS_PROTOCOL_VERSION_BETA))
    result = KO;
  
    //:test correctness of inserting 
  if (result != OK){
   delete msg1;
   return NULL;
  }  
DBG;
    //get the ACID and the PubKey of the AC
 result = OK;
DBG;
 result = ac->GetAsID(GM_ACID, &acid);
DBG;
 pubKey = ac->GetAsGMessage(GM_PUBLIC_KEY);   
    //test correctness of getting ACID and PubKey
DBG;
 if ((pubKey == NULL) || (result != OK)) {
    delete msg1;
    return NULL;
  }
DBG;
  msg1->DebugHeaders(logFile, "CSD : msg1");
    //wrap the inner GMsg and encrypt it
    //: create outer GMessage
  if((msg2 = new GMessage()) == NULL){
    delete msg1;
    delete pubKey;
    return NULL;
  }
  DBG; 
    //:transform the inner GMessage into MsgField 
  if ((decryptedMsgField = msg1->StoreToMsgField()) == NULL){
    delete msg1;
    delete pubKey;
    return NULL;
  }
  delete msg1;
DBG; 
    //:encrypt the MsgField 
    //::get a Cipherer to encrypt it with
  cip = six->cipherer;
  if (cip == NULL){
    delete decryptedMsgField;
    delete pubKey;
    return NULL;
  }
DBG;
    //::encrypt the MsgField
  if (cip->EncryptWithPublicKey(pubKey, decryptedMsgField, &encryptedMsgField)
          != OK){
    delete pubKey;
    delete decryptedMsgField; 
    if (encryptedMsgField)
      delete encryptedMsgField;
    return NULL;
  }
  delete pubKey;
  delete decryptedMsgField;
DBG;
    //:fill in the fields of the outer GMessgae 
  result = OK;
  result = msg2->SetAsMsgField(GM_BODY, encryptedMsgField);
  delete encryptedMsgField;
  if(msg2->SetAsID(GM_ACID, acid) != OK)
    result = KO;
    //:test correctness of the filling
  if(result != OK){
    delete msg2;
    return NULL;
  } 
DBG; 
   //the situation now is:
   //[msg2]: ACID:
   //        Body: [msg1]: ....
  
   //create and fill in ParametersFld of msg for MIX
   //:create 
  if ((msg3 = new GMessage) == NULL){
    delete msg2;
    return NULL;
  }
DBG;

MsgField *onion = ac->GetAsMsgField(GM_ONION);
msg2->DebugHeaders(logFile, "CSDM : msg2");
    //:fill in
  result = OK;
  result = msg3->SetAsGMessage(GM_DATA, msg2);
  delete msg2;
  if(msg3->SetAsMsgField(GM_ONION, onion) != OK)
    result = KO;
  DELETE(onion);
  if(msg3->SetAsSize(GM_ADD_HOPS, hops) != OK)
    result = KO;
  DBG; 
   //:test the correctness
  if (result != OK){
    delete msg3;
    return NULL;
  }
 DBG; 
    //create and fill in the message for MIX
    //:create
  if ((outMsg = new GMessage) == NULL){
    delete msg3;
    return NULL;
  }
 DBG;   
 msg3->DebugHeaders(logFile, "CSD : msg3");
    //:fill in
  result = OK;
  result = outMsg->SetAsString(GM_COMMAND, CMD_SEND_DATA);
  if(outMsg->SetAsGMessage(GM_PARAMETERS,  msg3) != OK)
    result = KO;
  delete msg3;
DBG;
    //:test the correctness
  if (result != OK){
    delete outMsg;
    return NULL;
  } 
DBG;
outMsg->DebugHeaders(logFile, "CSD: outMsg");
  return outMsg;  
}

      
/**
 * Creates "get_certificates" message for Mix.
 *
 * Creates and fills the fields of "get_certificates" message for Mix.
 * <pre>
 * [mixMsg]Command: get_certificates
 *         Parameters: 
 * [msg]     Certificate_Type: 
 *           Count: hops
 *           Request_ID:
 * </pre>
 *
 *
 * @param   type type of ACs wanted 
 * @param   count count of ACs wanted 
 * @return  created GMessage
 * @author  Marek 
 * @see     
 */
GMessage *SixMajordomo::CreateGetACsMsg(Char *type, Int count)
{
  GMessage *mixMsg;
  GMessage *msg;
  Err result;

  if((msg = new GMessage()) == NULL)
    return NULL;

  result = OK;
  if(msg->SetAsID(GM_REQUEST_ID, tID) != OK)
    result = KO;
  if(msg->SetAsInt(GM_COUNT, count) != OK)
    result = KO;
  if(msg->SetAsString(GM_CERTIFICATE_TYPE, type) != OK)
    result = KO;
  if(result != OK){
    delete msg;
    return NULL;
  }

  if((mixMsg = new GMessage()) == NULL){
    delete msg;
    return NULL;
  }

  result = OK;
  if (mixMsg->SetAsString(GM_COMMAND, CMD_GET_CERTIFICATES) != OK)
    result = KO;
  if (mixMsg->SetAsGMessage(GM_PARAMETERS, msg) != OK)
    result = KO;

  delete msg;
  if (result != OK){
    delete mixMsg;
    return NULL;
  }

  return mixMsg;
}


/**
 * Creates "generate_onion" message for Mix.
 *
 * Creates and fills the fields of "generate_onion" message for Mix.
 * <pre>
 * End situation (what is returned):
 * [mixMsg]Command: generate_onion
 *         Parameters: 
 * [msg]     Certificate_Type: 
 *           Count: hops
 *           Request_ID:
 * </pre>
 *
 *
 * @param   hopCount count of hops in AC wanted 
 * @return  created GMessage
 * @author  Marek 
 * @see     
 */
GMessage *
SixMajordomo::CreateGenerateOnionMsg(Size hopCount = CFG_HOPS_IN_AC)
{
  GMessage *mixMsg;
  GMessage *msg;
  Err result;

  if((msg = new GMessage()) == NULL)
    return NULL;

  result = OK;
  if(msg->SetAsID(GM_REQUEST_ID, tID) != OK)
    result = KO;
  if(msg->SetAsSize(GM_HOP_COUNT, hopCount) != OK)
    result = KO;
  if(result != OK){
    delete msg;
    return NULL;
  }

  if((mixMsg = new GMessage()) == NULL){
    delete msg;
    return NULL;
  }

  result = OK;
  if (mixMsg->SetAsString(GM_COMMAND, CMD_GENERATE_ONION) != OK)
    result = KO;
  if (mixMsg->SetAsGMessage(GM_PARAMETERS, msg) != OK)
    result = KO;

  delete msg;
  if (result != OK){
    delete mixMsg;
    return NULL;
  }

  return mixMsg;
}
   
/**
 * Creates "publish_certificates" message for Mix.
 *
 * Creates and fills the fields of "publish_certificates" message for Mix.
 * <pre>
 * End situation (what is returned):
 * [mixMsg]Command: publish_certificates
 *         Parameters: 
 * [msg]     Certificate-1: 
 * </pre>
 *
 *
 * @param   ac ac to publish 
 * @return  created GMessage
 * @author  Marek 
 * @see     
 */
GMessage *
SixMajordomo::CreatePublishCertificateMsg(GMessage *ac)
{
  GMessage *mixMsg;
  ArrayGMessage *msg;
  MsgField *tmp;
  Err result;

  ac->DebugHeaders(logFile, "AC to publish");

  if((msg = new ArrayGMessage()) == NULL)
    return NULL;

  result = OK;
  tmp = ac->StoreToMsgField();
  if(tmp == NULL)
    result = KO;
  result = msg->AddField(GM_CERTIFICATE, tmp);
  DELETE(tmp);
  if(result != OK){
    delete msg;
    return NULL;
  }

  if((mixMsg = new GMessage()) == NULL){
    delete msg;
    return NULL;
  }

  result = OK;
  if (mixMsg->SetAsString(GM_COMMAND, CMD_PUBLISH_CERTIFICATES) != OK)
    result = KO;
  if (mixMsg->SetAsGMessage(GM_PARAMETERS, msg) != OK)
    result = KO;

  delete msg;
  if (result != OK){
    delete mixMsg;
    return NULL;
  }

  return mixMsg;
}
