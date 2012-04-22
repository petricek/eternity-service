#include "./H/sixMajordomo.h"


/**
 * Handles the message from Mix.
 *
 * Extracts the command and data fields from the inMsg and calls switch methods.
 *
 *
 * @param   inMsg message that came from receiverToMajordomo queue
 * @return  void 
 * @author  Pavel, Marek 
 * @see     GMessage, Debugable
 */
Void SixMajordomo::HandleMixMessage(GMessage *inMsg)
{
  Err result;
  
DBG;  
  Char *version = six->GetMixSixVersion();
  if(version == NULL)
  {
    WriteString(llError, __FILE__":%d:Cannot get mix-six protocol version");
    return;
  }

DBG;  
      //msgHandling for LBL_MIXSIX_PROTOCOL_VERSION_BETA
  if(strcmp(version, LBL_MIXSIX_PROTOCOL_VERSION_BETA) == 0){
    outMsgQueue = new Queue();
      
    MsgField *commandFld, *dataFld;

      // get Command and Data fields
    commandFld = inMsg->GetAsMsgField(GM_COMMAND);
    dataFld    = inMsg->GetAsMsgField(GM_PARAMETERS);

      // choose a method to serve this Command
    result = SwitchMixCommand(commandFld, dataFld);

      // answer (or send elsewhere)
    if(result == OK)
      FeedSenderWithOutMsgQueue();

    if(outMsgQueue != NULL)
      delete outMsgQueue; 
    if(commandFld != NULL)
      delete commandFld;
    if(dataFld != NULL)
      delete dataFld;
  } //end of MIXSIX_PROTOCOL_VERSION_BETA
  else
    WriteString(llError, __FILE__":%d:Unknown mix-six protocol version.");

  DELETE(version);
}


/**
 * Chooses handling method.
 *
 * According to field Command (in message poped from receiverToMajordomo) 
 * chooses and calls handling method.
 *
 *
 * @param   commandFld command extracted from the message
 * @param   dataFld data extracted from the message
 * @return  error code 
 * @author  Pavel
 */
Err SixMajordomo::SwitchMixCommand(MsgField *commandFld, MsgField *dataFld)
{
  Char *command;
  Err result;

DBG;  
    // field command cannot be empty
  if(commandFld == NULL)
    return KO;

    // get command string
  command = commandFld->GetAsString();
  if(command == NULL)
    return KO; 
DBG;  
  WriteString(llDebug, "SixMajordomo::SwitchMixCommand(): command = %s", command);
    // call appropriate command handling method
    // commands from MIX
  result = OK;
  if(strcmp(command, CMD_TEST) == 0)
    result = CommandTest(dataFld);
  else if(strcmp(command, CMD_DATA_RECEIVED) == 0)
    result = CommandDataReceived(dataFld);
  else if(strcmp(command, CMD_ONION_GENERATED) == 0)
    result = CommandOnionGenerated(dataFld);
  else 
  {
      //tohle by melo byt az v potomcixh
   // WriteString(llWarning, __FILE__":%d: unknown Mix command", __LINE__);
    result = KO;
  }
 
 
  delete command;

  return result;
}


/**
 * Serves command Test from Mix.
 *
 * Handles command Test got from Mix.
 *
 *
 * @param   dataField contents of data
 * @return  error code
 * @author  Marek
 * @see     GMessage, MsgField
 */
Err SixMajordomo::CommandTest(MsgField *dataFld)
{
  Err result;
  GMessage *mixMsg;

  mixMsg = CreateTestMsg(dataFld);

    //put it into outMsgQueue
  if(mixMsg == NULL){
    WriteString(llError, "SixMajordomo::CommandTest():"
		  "Cannot create mixMsg");
    return KO;
  }
  else
  {
    result = outMsgQueue->Insert(mixMsg);
    if(result != OK)
      delete mixMsg;
    return result;
  }
}


/**
 * Serves "onion_generated" command from Mix.
 *
 * Gets the onion and creates the AC of this Six.
 *
 *
 * @param   dataFld contents of dataMsg
 * @return  error code
 * @author  Marek
 * @see     GMessage, MsgField
 */
Err SixMajordomo::CommandOnionGenerated(MsgField *dataFld)
{
  GMessage *inMsg;
  MsgField *onion;
  GMessage *ac;
  Err result;

  DBG;
  WriteString(llDebug, "Majordomo::CommandOnionGenerated(): entering");

  DBG;
  if ((inMsg = dataFld->GetAsGMessage()) == NULL)
    return KO;

  DBG;
  if ((onion = inMsg->GetAsMsgField(GM_ONION)) == NULL){
    delete inMsg;
    return KO;
  }

  DBG;
  delete inMsg;

  DBG;
  result =GenerateThisSixAC(onion, &ac);
  DELETE(onion);

  if(result != OK)
    return KO;

  DBG;
  result = HandleGeneratedAC(ac);
  DELETE(ac);

  return result;
}


/**
 * Handles generated AC.
 *
 * In Six does nothing. Descendants may save or publish 
 * the AC by overriding this method.
 *
 *
 * @param   generatedAC
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
Err SixMajordomo::HandleGeneratedAC(GMessage *)
{
  return OK;
}

