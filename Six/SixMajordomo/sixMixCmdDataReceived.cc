#include "./H/sixMajordomo.h"


/**
 * Handles the Data_received command.
 *
 * Handling the Data_received command including decryption of the field
 * Body contained in field Data.
 * <pre>
 * Handles this message:
 *   Data:
 *     ACID:
 *     Body: (encrypted by ACPubKey identified by ACID)
 *       Version: (eso-others protocol version)
 *       Command:
 *       Data:
 *</pre>
 *
 *
 * @param   paramFld parameters field from the inMsg
 * @return  error code 
 * @author  Pavel
 * @see     
 */
Err 
SixMajordomo::CommandDataReceived(MsgField *paramFld)
{
  GMessage *paramMsg, *dataMsg;
  MsgField *bodyFld;
  ACID      acid;

DBG;
  if(paramFld == NULL)
    return KO;

DBG;
    // paramFld is GMessage
  paramMsg = paramFld->GetAsGMessage();
  if(paramMsg == NULL)
    return KO;

DBG;
    // remove Data level
  dataMsg = paramMsg->GetAsGMessage(GM_DATA);
  if(dataMsg == NULL)
  {
    DELETE(paramMsg);
    return KO;
  }

DBG;
    // decrypt the Body field
  if(DecryptDataMsg(dataMsg, &bodyFld, &acid) != OK)
  {
    delete dataMsg;
    DELETE(paramMsg);
    return KO;
  }

DBG;
    // handle the Body field
  Err result = HandleDataReceivedBody(bodyFld, acid);

  delete bodyFld;
  delete dataMsg;
  DELETE(paramMsg);

  return result;
}


/**
 * Decrypts the field Body.
 *
 * Decrypts the field Body (stored in given GMessage) according to field
 * ACID. Returns the decrypted field Body.
 *
 *
 * @param   dataMsg given GMessage
 * @param   decryptedBodyField returned decrypted field 
 * @param   acid acid to decrypt with 
 * @return  error code 
 * @author  Pavel
 * @see     GMessage
 */
Err 
SixMajordomo::DecryptDataMsg(const GMessage *dataMsg,
                              MsgField      **decryptedBodyFld,
			      ACID           *acid)
{
  MsgField *encryptedBodyFld;

DBG;
  if(dataMsg == NULL)
    return KO;
  ((GMessage *)dataMsg)->DebugHeaders(logFile);

DBG;
    // get the ACID field (identifies the Eso's pub key used to
    // encrypt Body field)
  if(dataMsg->GetAsID(GM_ACID, acid) != OK)
    return KO;
  
DBG;
    // get the Body field (encrypted)
  encryptedBodyFld = dataMsg->GetAsMsgField(GM_BODY);
  if(encryptedBodyFld == NULL)
    return KO;

DBG;
    // decrypt the Body field using TCB
  *decryptedBodyFld = DecryptByACPriv(*acid, encryptedBodyFld);

  delete encryptedBodyFld;

  return (*decryptedBodyFld == NULL) ? KO : OK;
}


/**
 * Handles the command receiver from Service.
 *
 * Handles the command received from the receiver (not from a Mix) according
 * to Eso-Others protocol.
 *
 *
 * @param   bodyFld body of the message
 * @param   acid acid to which the message was sent 
 * @return  err 
 * @author  pavel
 * @see     
 */
Err 
SixMajordomo::HandleDataReceivedBody(const MsgField *bodyFld, 
                                      const ACID      acid)
{
  Char *version;
  GMessage *inMsg;

DBG;
  if(bodyFld == NULL)
    return KO; 
 
DBG;
  if((inMsg = ((MsgField *)bodyFld)->GetAsGMessage()) == NULL)
    return KO; 
inMsg->DebugHeaders(logFile, "Decrypted message");

DBG;
    // get version of Eso-Others protocol
  version = inMsg->GetAsString(GM_VERSION);
  if(version == NULL)
  {
    delete inMsg;
    return KO;
  }

DBG;
    // handle the rest according to given version
  Err result = SwitchDataReceivedVersion(version, inMsg, acid);

  delete version;
  delete inMsg;
  
  return result;
}


/**
 * Switches a method according to the version.
 *
 * Chooses a method to handle the incomming message according to its
 * version.
 *
 *
 * @param   version version of received GMessage 
 * @param   body body of the received GMessage
 * @param   acid ID of certificate to which the message came
 * @return  error code
 * @author  Pavel
 * @see     
 */
Err 
SixMajordomo::SwitchDataReceivedVersion(const Char     *version, 
                                         const GMessage *body,
					 const ACID      acid)
{
  Err result;
  
  if((version == NULL) || (body == NULL))
    return KO;

  if(strcmp(version, LBL_ESO_OTHERS_PROTOCOL_VERSION_BETA) == 0)
  {
    Char *command;
    MsgField *dataFld;

DBG;
      // get Data and Command fields
    dataFld = body->GetAsMsgField(GM_DATA);
    command = body->GetAsString(GM_COMMAND);

DBG;
      // the Command field is needed, the Data field not
    if(command == NULL)
    {
      if(dataFld != NULL)
        delete dataFld;
      return KO;
    }

DBG;
      // handle Data according to Command
    if((result = SwitchDataReceivedCommand(command, dataFld, acid)) != OK)
      WriteString(llWarning, __FILE__":%d:Command not served successfully.", __LINE__);

DBG;
    if(dataFld != NULL)
      delete dataFld;
    delete command;
  }
//  else if(strcmp(version, other version ...
  else
  {
    WriteString(llWarning, __FILE__":%d: unknown data_received version", __LINE__);
    return KO;
  }

DBG;
  return result;
}


