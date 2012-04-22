#include "./H/majordomo.h"

/**
 * Sends message to number of Esos.
 *
 * Sends the message to count randomly chosen Esos
 * (creates count "send_data" mixMsgs and puts them
 *  into outMsgQueue)
 *
 *
 * @param   command command to be sent to Esos
 * @param   msg message to be sent to Esos
 * @param   count count of Esos to send the message to
 * @return  error code
 * @author  Marek
 * @see     SixMajordomo
 */
Err
Majordomo::SendToEsos(Char *command, GMessage *msg, Int count)
{
  Queue *acQueue;
  GMessage *ac;  
  GMessage *outMsg;


  if (msg == NULL){
    return KO;
  }

    //get <width> access certificates of other Esos
  if ((acQueue = GetACs(ACTYPE_OTHER, count)) == NULL){
    return KO;
  }
  
    //create messages for all of the ACs in the queue
  ac =  (GMessage*) acQueue->First();
  while (ac != NULL){

      //create "send data" msg for Mix 
    outMsg = CreateSendDataMsg(command, outMsg, ac); 

      //put it into outMsgQueue
    if(outMsg == NULL){
      return KO;
      WriteString(llError, "Majordomo::SendToEsos(): Cannot create outMsg ");
    }
    else
      outMsgQueue->Insert(outMsg);

    delete ac; 
    ac = (GMessage*) acQueue->Next();
  }   
  delete acQueue;
  
  return OK;
}    
  
