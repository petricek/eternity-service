
  // has to be the *FIRST* include !
#include "./../../Common/H/common.h"

#include <stdlib.h>

#include "./../../Common/H/commands.h"

#include "./H/majordomo.h"

#define DEFAULT_ADD_HOPS 5
#define DEFAULT_ONION_LEN 5
#define DEFAULT_LOOPS 20
#define DEFAULT_REQUEST_ID 12321
#define DEFAULT_DATA "Celebrate who sees these words !!!"
#define CERTAINITY_COEFICIENT 2

//  Sample resource debugging: 
//  --------------------------
//  fprintf(stderr, "Resource : %ld, line %d\n", 
//  ResourceUsage(), __LINE__);

/**
 * Asks Mix for onion.
 *
 * Creates GenerateOnion message for Client and waits for reply.
 *
 *
 * @param   onionLen Requested length of onion.
 * @param   requestID RequestID to send to client 
 * @return  GMessage - the onion.
 * @author  Pechy, Venca
 * @see     
 */
GMessage *
Majordomo::GetOnion(Size onionLen, Size requestID)
{
    // create message for sending to Mix
  GMessage *parameters = new GMessage();
  parameters->SetAsBytes(GM_HOP_COUNT, (void *) &onionLen, sizeof (Size));
  parameters->SetAsBytes(GM_REQUEST_ID, (void *) &requestID, sizeof (Size));

  GMessage *chunk = new GMessage();
  chunk->SetAsGMessage(GM_PARAMETERS, parameters);
  DELETE(parameters);
  chunk->SetAsString(GM_COMMAND, CMD_GENERATE_ONION);

  FeedSenderWithMsgForMix(chunk);

  GMessage *message = NULL;
  ((Client *) six)->receiverToMajordomo->GetMsg(&message);

  WriteString(llDebug, __FILE__ ":%d:Something received.", __LINE__);

  char *command = message->GetAsString(GM_COMMAND);
  if (strcmp(CMD_ONION_GENERATED, command) != 0) {
    WriteString(llFatalError, __FILE__ ":%d:Didn't get an onion, but should (got %s).", command,  
                __LINE__);
    exit(1);
  }
  DELETE(command);

  parameters = message->GetAsGMessage(GM_PARAMETERS); 
  DELETE(message);
  Size *returnedRequestID = (Size *) parameters->GetAsBytes(GM_REQUEST_ID);
  WriteString(llDebug, __FILE__ ":%d:Request-ID is %lu, should be %lu.", 
              __LINE__, *returnedRequestID, requestID);
  if (*returnedRequestID != requestID) {
    WriteString(llError, __FILE__ ":%d:Request-ID doesn't match.", __LINE__);
  }
  DELETE(returnedRequestID);

  GMessage *onion = NULL;
  WriteString(llDebug, __FILE__ ":%d:What is in parameters?", __LINE__);
  parameters->DebugHeaders(logFile);
  onion = parameters->GetAsGMessage(GM_ONION); 
  DELETE(parameters);
   
  WriteString(llDebug, __FILE__ ":%d:Really got an onion.", __LINE__);
  return onion;
}

/**
 * Send data to onion.
 *
 * Sends data to onion or mix - method used by test methods in Client.
 *
 *
 * @param   data Data to send.
 * @param   destination Onion to send the data to. 
 * @param   hops number of hops to add.
 * @return  void 
 * @author  Pechy, Venca
 * @see     
 */
void 
Majordomo::SendData(MsgField * data, GMessage * destination, 
                         string destinationType, Size hops)
{


  WriteString(llDebug, __FILE__ ":%d:Entering SendData.", __LINE__);

  GMessage * parameters = new GMessage();
  
  parameters->SetAsBytes(GM_ADD_HOPS, (void *) &hops, sizeof (Size));
  parameters->SetAsGMessage((char *)destinationType.c_str(), destination);
  //DELETE(destination);
  parameters->SetAsMsgField(GM_DATA, data);
  DELETE(data);
  WriteString(llDebug, __FILE__ ":%d:What is in parameters?", __LINE__); 
  parameters->DebugHeaders(logFile);

  GMessage * chunk = new GMessage();
  
  chunk->SetAsGMessage(GM_PARAMETERS, parameters);
  DELETE(parameters);
  chunk->SetAsString(GM_COMMAND, CMD_SEND_DATA);

  GMessage * messageForSender = new GMessage();

  messageForSender->SetAsGMessage(GM_CHUNK, chunk);
  DELETE(chunk);
  GMessage *recipient = new GMessage();
    // got port number where receiver will listen to the incoming messages
  string tmp;
  tmp = ((Client *) six)->configFile->GetValue(CFG_MIX_ADDRESS);
  recipient->SetAsString(GM_ADDRESS, (char *) tmp.c_str());
  WriteString(llDebug, __FILE__ ":%d:Got mix address %s.", __LINE__,
               tmp.c_str()); 
  u_short port = atoi(((Client *) six)->configFile->GetValue(CFG_MIX_PORT).c_str());
  recipient->SetAsBytes(GM_PORT, &port, sizeof(u_short));
  WriteString(llDebug, __FILE__ ":%d:What is in recipient?", __LINE__); 
  recipient->DebugHeaders(logFile);
  WriteString(llDebug, __FILE__ ":%d:Got mix port number %s.", __LINE__,
              tmp.c_str()); 
  messageForSender->SetAsGMessage(GM_RECIPIENT_1, recipient);
  DELETE(recipient);

  WriteString(llDebug, __FILE__ ":%d:What is in messageForSender?", __LINE__); 
  messageForSender->DebugHeaders(logFile);
  ((Client *) six)->majordomoToSender->Append(messageForSender);
}

/**
 *  Asks mix for some certificates and then returns them.
 *
 * Constructs message of this type:
 * <pre>
 *  Command: get_certificates
 *  Parameters:
 *  Count:
 *  Certificate-Type:
 *  Request-ID:
 * </pre>
 *
 * @param   type Type of certificates to get
 * @param   count Count of certificates to get.
 * @return  GMessage - got certificates. 
 * @author  Venca, Pechy
 * @see     
 */
GMessage *
Majordomo::GetCertificates(string type, Size count)
{
  DBG;
  GMessage * message = new GMessage();
  DBG;
  message->SetAsString(GM_COMMAND, CMD_GET_CERTIFICATES);
  DBG;
  GMessage * parameters = new GMessage();
  DBG;
  parameters->SetAsBytes(GM_COUNT, (void *)&count, sizeof(Size));
  DBG;
  parameters->SetAsString(GM_CERTIFICATE_TYPE, (char *)type.c_str());
  DBG;
  Size id = random();
  parameters->SetAsBytes(GM_REQUEST_ID, (void *)&id, sizeof(Size));
  DBG;
  parameters->DebugHeaders(logFile, "parameters");
  DBG;
  message->SetAsGMessage(GM_PARAMETERS, parameters);
  DBG;
  message->DebugHeaders(logFile,"message");
  DBG;
  DELETE(parameters);
  DBG;

  GMessage * senderMsg = ((Client *) six)->sender->MakeMessageForSendingToMix(message);
  DBG;
  ((Client *) six)->majordomoToSender->Append(senderMsg);

    // Wait for reply and find out if it is OK.
  ((Client *) six)->receiverToMajordomo->GetMsg(&message);
  DBG;
  message->DebugHeaders(logFile, "message received back");
  DBG;

  char * command = NULL;
  command = message->GetAsString(GM_COMMAND);
  if (strcmp(CMD_CERTIFICATES_RECEIVED, command) != 0) {
    WriteString(llFatalError, __FILE__ ":%d:Didn't get certificates, but should.", 
		__LINE__);
    exit(1);
  }
  DELETE(command);
DBG; 
  parameters = NULL;
  parameters = message->GetAsGMessage(GM_PARAMETERS); 
  DELETE(message);
  GMessage * newData = NULL;
  newData = parameters->GetAsGMessage(GM_CERTIFICATES);
  parameters->DebugHeaders(logFile, "parameters received back");
  DELETE(parameters);
 DBG; 
  return newData;
}

