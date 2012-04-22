#include "./H/sixMajordomo.h"


/**
 * Incoming message handling.
 *
 * Chooses a method to handle the command from the incomming message.
 *
 *
 * @param   command command received from service 
 * @param   dataFld data received from service 
 * @param   acid ID of AC to which the data was sent   
 * @return  error code
 * @author  Pavel
 */
Err SixMajordomo::SwitchDataReceivedCommand(const Char     *command, 
                                         const MsgField *dataFld,
					 const ACID      /*acid*/)
{
  Err result;
  
  if(command == NULL)
   return KO; 

  if(strcmp(command, CE_TEST))
    result = DRCTest(dataFld);
  else
    result = KO;

  return result;
}


/**
 * Handles Test message from service.
 *
 * Replies to the Test message got from service.
 *
 *
 * @param   dataFld data got from service
 * @return  error code
 * @author  Pavel
 */
Err SixMajordomo::DRCTest(const MsgField *dataFld)
{
  GMessage *inMsg;  
  Err result;
  TID replyToTID;
  GMessage *replyToAC;
  MsgField *params;
  GMessage *outMsg;
  GMessage *mixMsg;

    //parse dataField into GMessage
  if (dataFld == NULL)
    return KO;
  if ((inMsg = ((MsgField *)dataFld)->GetAsGMessage()) == NULL) 
    return KO;

   //get ReplyTo info from inMsg
  result = OK;
  if((inMsg->GetAsID(GM_REPLY_TO_TID, &replyToTID)) != OK)
    result = KO; 
  if((replyToAC = inMsg->GetAsGMessage(GM_REPLY_TO_AC)) == NULL)
    result = KO;
  if((params = inMsg->GetAsMsgField(GM_PARAMETERS)) == NULL)
    result = KO;

  if((outMsg = new GMessage()) == NULL)
        result = KO;

  if (result == OK){

       //fill outMsg as ReplyFile
   if(FillTest(outMsg, params) == OK){

        //create "send data" msg for Mix 
      mixMsg = CreateSendDataMsg(CMD_TEST, outMsg, replyToAC); 

        //put it into outMsgQueue
      if(mixMsg == NULL){
        WriteString(llError, "Majordomo::DRCTest():"
                              "Cannot create mixMsg");
        result = KO;
      }
      else
        if((result = outMsgQueue->Insert(mixMsg)) != OK)
	  delete mixMsg;
    }
  }

  if(replyToAC != NULL)
    delete replyToAC;
  if(params != NULL)
    delete params;
  if(outMsg != NULL)
    delete outMsg;
    
  return result;
}

