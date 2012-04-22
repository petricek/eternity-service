
  // has to be the *FIRST* include !
#include "./../../Common/H/common.h"

#include <stdlib.h>
#include <unistd.h>

#include "./../../Common/H/labels.h"
#include "./../../Common/H/commands.h"

#include "./H/majordomo.h"

#define DEFAULT_ADD_HOPS 5
#define DEFAULT_ONION_LEN 5
#define DEFAULT_LOOPS 20
#define DEFAULT_REQUEST_ID 12321
#define DEFAULT_DATA "Celebrate who sees these words !!!"
#define CERTAINITY_COEFICIENT 2

  // Global parameters                                                                                          
extern char * configName;
extern char * fileKeys;
extern char * fileName;
extern char * storeTime;
extern CommandOption commandOption;

/**
 * TestW.
 *
 * W-test of Client.
 *
 *
 * @author  Venca, Pechy
 * @see     Client
 *
 */
void * 
Majordomo::TestW(void * )
{
/*
    //Pavel changed
    ArrayGMessage *certificates = (ArrayGMessage *)GetCertificates(ACTYPE_ESO, 1);
    MsgField *certFld = certificates->GetField(GM_CERTIFICATE, 1);
    DELETE(certificates);
    GMessage *certificate = certFld->GetAsGMessage();
    certificate->DebugHeaders(logFile);
    DELETE(certFld);
*/
  WriteString(llDebug, __FILE__ ":%d:Entering TestW.", __LINE__); 
  GMessage * chunk = new GMessage();
  chunk->SetAsString(GM_COMMAND, CMD_GET_CERTIFICATES);
  GMessage * parameters = NULL;
  parameters = new GMessage();

  Size tmpCount = 1;
  parameters->SetAsBytes(GM_COUNT, &tmpCount, sizeof(Size));

  parameters->SetAsString(GM_CERTIFICATE_TYPE, ACTYPE_ESO);
  parameters->DebugHeaders(logFile, "parameters");
  chunk->SetAsGMessage(GM_PARAMETERS, parameters);
  DELETE(parameters);
  chunk->DebugHeaders(logFile,"chunk");

  GMessage * messageForSender;
  messageForSender = ((Client *) six)->sender->MakeMessageForSendingToMix(chunk);
  WriteString(llDebug, __FILE__ ":%d:What is in messageForSender?", __LINE__); 
  messageForSender->DebugHeaders(logFile);
  DELETE(chunk);

  ((Client *) six)->majordomoToSender->Append(messageForSender);

    // Wait for reply and find out if it is OK.
  GMessage *message;
  ((Client *) six)->receiverToMajordomo->GetMsg(&message);
  message->DebugHeaders(logFile, "message received back");

  char * command = NULL;
  command = message->GetAsString(GM_COMMAND);
  if (strcmp(CMD_CERTIFICATES_RECEIVED, command) != 0) {
    WriteString(llFatalError, __FILE__ ":%d:Didn't get data, but should.", 
		__LINE__);
    exit(1);
  }
  DELETE(command);
  
  parameters = NULL;
  parameters = message->GetAsGMessage(GM_PARAMETERS); 
  DELETE(message);
  parameters->DebugHeaders(logFile, "parameters received back");
  GMessage *certificates = parameters->GetAsGMessage(GM_CERTIFICATES);
  DELETE(parameters);
  GMessage *certificate = certificates->GetAsGMessage(GM_CERTIFICATE_1);
  DELETE(certificates)
  certificate->DebugHeaders(logFile, "Eso certificate received");

  GMessage *onion = certificate->GetAsGMessage(GM_ONION);
  onion->DebugHeaders(logFile, "What is in onion from certificate?");
     
    ///Marek changed (testovani baleni a rozbalovani zprav)
    GenerateAsymetricKey();
    File *testFile = new File("/home/eternity/conf/eso.conf");
    GMessage *msg = CreateSimpleReqForStor(testFile);
    msg->DebugHeaders(logFile, "Message to Eso:");
    DBG;
    chunk = CreateSendDataMsg(CE_REQ_FOR_STOR, msg, certificate);
    DBG;
    DELETE(testFile);
    DBG;
    DELETE(msg);
    
    


/*
  DELETE(certificate);
  chunk = new GMessage();
  chunk->SetAsString(GM_COMMAND, CMD_SEND_DATA);

  parameters = new GMessage();
  parameters->SetAsGMessage(GM_ONION, onion);
  DELETE(onion);
  Size tmpAddHops = 0;
  parameters->SetAsBytes(GM_ADD_HOPS, &tmpAddHops, sizeof(Size));

  parameters->SetAsString(GM_DATA, "Fak!");

  chunk->SetAsGMessage(GM_PARAMETERS, parameters);
  DELETE(parameters);
*/
  DBG;
  messageForSender = six->sender->MakeMessageForSendingToMix(chunk);
  DELETE(chunk);
  
  ((Client *) six)->majordomoToSender->Append(messageForSender);

  WriteString(llDebug, __FILE__ ":%d:Sent data to Eso.", __LINE__);
  return NULL;
}

/**
 * TestX.
 *
 * X-test of Client.
 *
 *
 * @author  Venca, Pechy
 * @see     Client
 *
 */
void * Majordomo::TestX(void * )
{
  Size defaultAddHops;
  Size defaultOnionLen;
  Size defaultLoops;

  string tmpStr;
  tmpStr = ((Client *) six)->configFile->GetValue(CFG_DEBUG_LOOPS);
  defaultLoops = ((tmpStr != "") ? atol(tmpStr.c_str()) : DEFAULT_LOOPS);
  tmpStr = ((Client *) six)->configFile->GetValue(CFG_DEBUG_ONION_LENGTH);
  defaultOnionLen = (tmpStr != "") ? atol(tmpStr.c_str()) : DEFAULT_ONION_LEN;
  tmpStr = ((Client *) six)->configFile->GetValue(CFG_DEBUG_ADD_HOPS_TO_SEND_DATA);
  defaultAddHops = (tmpStr != "") ? atol(tmpStr.c_str()) : DEFAULT_ADD_HOPS;

  int useRandom;
  tmpStr = ((Client *) six)->configFile->GetValue("debug_use_random");
  useRandom = (tmpStr != "") ? 1 : 0;

  //char *data = DEFAULT_DATA;
#define DATA_SIZE 1000
  char *data = DEFAULT_DATA;

  for (Size i = 0; i < defaultLoops; ++i) {

    Size realOnionLen;

      // if we have "debug_use_random" in configFile, generate onion
      // length with randomness (within 0 - CFG_DEBUG_ONION_LENGTH)
    if (useRandom)
      realOnionLen = (random() % defaultOnionLen);
    else
      realOnionLen = defaultOnionLen;

    WriteString(llDebug, __FILE__ ":%d:Request for onion with %lu hops.", 
		__LINE__, realOnionLen); 

    GMessage *onion;
    onion = GetOnion(realOnionLen, DEFAULT_REQUEST_ID); 
    // DELETE(onion);

    BytesMsgField *dataMsgField = new BytesMsgField;
//    dataMsgField->SetAsBytes(data, DATA_SIZE);
    dataMsgField->SetAsString(data);

      // see useRandom above
    Size realAddHops;
    if (useRandom)
      realAddHops = (random() & defaultAddHops);
    else 
      realAddHops = defaultAddHops;

    WriteString(llDebug, __FILE__ ":%d:Send data with %lu hops.", __LINE__, 
                realAddHops); 
    SendData(dataMsgField, onion, GM_ONION, realAddHops);

      // Wait for reply and find out if it is OK.
    GMessage * message = NULL;
    ((Client *) six)->receiverToMajordomo->GetMsg(&message);
    WriteString(llDebug, __FILE__ ":%d:What message did client got?", 
      __LINE__);
    message->DebugHeaders(logFile);

    char * command = NULL;
    command = message->GetAsString(GM_COMMAND);
    if (strcmp(CMD_DATA_RECEIVED, command) != 0) {
      WriteString(llFatalError, __FILE__ ":%d:Didn't get "
                  "CMD_DATA_RECEIVED, but should.", __LINE__);
      exit(1);
    }
    DELETE(command);
    
    GMessage * parameters = NULL;
    parameters = message->GetAsGMessage(GM_PARAMETERS); 
    DELETE(message);
    /*
    Size dataLen;
    char * newData;
    newData = (char *) parameters->GetAsBytes(GM_DATA, &dataLen);
    */
    DELETE(parameters);

    /*
    if (strncmp(DEFAULT_DATA, newData, dataLen - 1) != 0) {
      WriteString(llWarning, __FILE__ ":%d:Didn't got right data, but should.", 
		  __LINE__);
    }

    WriteString(llDebug, __FILE__ ":%d:Received data: %s", __LINE__, newData);
    DELETE(newData);
    */

    fprintf(stderr, "%lu O.K.\n", i);
  }

  exit(0);
  return NULL;
}

/**
 * TestZ.
 *
 * Z-test of Client.
 *
 *
 * @author  Venca, Pechy
 * @see     Client
 *
 */
void * Majordomo::TestZ(void * )
{
  Size defaultLoops;
  Size defaultOnionLen;
  Size defaultAddHops;

  string tmpStr;
  tmpStr = ((Client *) six)->configFile->GetValue(CFG_DEBUG_LOOPS);
  defaultLoops = ((tmpStr != "") ? atol(tmpStr.c_str()) : DEFAULT_LOOPS);
  tmpStr = ((Client *) six)->configFile->GetValue(CFG_DEBUG_ONION_LENGTH);
  defaultOnionLen = (tmpStr != "") ? atol(tmpStr.c_str()) : DEFAULT_ONION_LEN;
  tmpStr = ((Client *) six)->configFile->GetValue(CFG_DEBUG_ADD_HOPS_TO_SEND_DATA);
  defaultAddHops = (tmpStr != "") ? atol(tmpStr.c_str()) : DEFAULT_ADD_HOPS;

  int useRandom;
  tmpStr = ((Client *) six)->configFile->GetValue("debug_use_random");
  useRandom = (tmpStr != "") ? 1 : 0;

  if (useRandom)
    defaultOnionLen = (random() & 7) + 1;
  WriteString(llDebug, __FILE__ ":%d:Request for onion with %lu hops.", 
	      __LINE__, defaultOnionLen); 

  //fprintf(stderr, "Resource before GetOnion: %ld\n", ResourceUsage());
  //GMessage *onion;
  //onion = GetOnion(defaultOnionLen, DEFAULT_REQUEST_ID); 
  //fprintf(stderr, "Resource after GetOnion: %ld\n", ResourceUsage());

  //for (Size i = 0; i < defaultLoops; ++i) {

    GMessage * message = new GMessage();
    message->SetAsString(GM_COMMAND, CMD_GET_CERTIFICATES);
    GMessage * parameters = NULL;
    parameters = new GMessage();
    parameters->SetAsString(GM_COUNT, "10");
    parameters->SetAsString(GM_CERTIFICATE_TYPE, GM_MIX);
    //parameters->SetAsGMessage(GM_ONION, onion);
    parameters->DebugHeaders(logFile, "parameters");
    message->SetAsGMessage(GM_PARAMETERS, parameters);
    message->DebugHeaders(logFile,"message");
    DELETE(parameters);

    GMessage * messageForSender = new GMessage();

    messageForSender->SetAsGMessage(GM_CHUNK, message);
    DELETE(message);
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
    messageForSender->SetAsGMessage("Recipient-1", recipient);
    DELETE(recipient);

    WriteString(llDebug, __FILE__ ":%d:What is in messageForSender?", __LINE__); 
    messageForSender->DebugHeaders(logFile);

    ((Client *) six)->majordomoToSender->Append(messageForSender);

      // Wait for reply and find out if it is OK.
    ((Client *) six)->receiverToMajordomo->GetMsg(&message);
    message->DebugHeaders(logFile, "message received back");

    char * command = NULL;
    command = message->GetAsString(GM_COMMAND);
    if (strcmp(CMD_CERTIFICATES_RECEIVED, command) != 0) {
      WriteString(llFatalError, __FILE__ ":%d:Didn't get data, but should.", 
		  __LINE__);
      exit(1);
    }
    DELETE(command);
    
    parameters = NULL;
    parameters = message->GetAsGMessage(GM_PARAMETERS); 
    DELETE(message);
    Size dataLen;
    char * newData;
    newData = (char *) parameters->GetAsBytes(GM_DATA, &dataLen);
    parameters->DebugHeaders(logFile, "parameters received back");
    DELETE(parameters);
    
    DELETE(newData);
    
    fprintf(stderr, "Oukej.\n");
  //  fprintf(stderr, "%lu O.K.\n", i);
  //}

  return NULL;

}


/**
 * TestY.
 *
 * Y-test of Client.
 *
 *
 * @author  Venca, Pechy
 * @see     Client
 *
 */
void * Majordomo::TestY(void * )
{
  Size defaultLoops;
  Size defaultOnionLen;
  Size defaultAddHops;

  string tmpStr;
  tmpStr = ((Client *) six)->configFile->GetValue(CFG_DEBUG_LOOPS);
  defaultLoops = ((tmpStr != "") ? atol(tmpStr.c_str()) : DEFAULT_LOOPS);

  tmpStr = ((Client *) six)->configFile->GetValue(CFG_DEBUG_ONION_LENGTH);
  defaultOnionLen = (tmpStr != "") ? atol(tmpStr.c_str()) : DEFAULT_ONION_LEN;

  tmpStr = ((Client *) six)->configFile->GetValue(CFG_DEBUG_ADD_HOPS_TO_SEND_DATA);
  defaultAddHops = (tmpStr != "") ? atol(tmpStr.c_str()) : DEFAULT_ADD_HOPS;

  int useRandom;
  tmpStr = ((Client *) six)->configFile->GetValue("debug_use_random");
  useRandom = (tmpStr != "") ? 1 : 0;

  if (useRandom)
    defaultOnionLen = (random() & 7) + 1;
  WriteString(llDebug, __FILE__ ":%d:Request for onion with %lu hops.", 
	      __LINE__, defaultOnionLen); 

  //fprintf(stderr, "Resource before GetOnion: %ld\n", ResourceUsage());
  GMessage *onion;
  onion = GetOnion(defaultOnionLen, DEFAULT_REQUEST_ID); 
  //fprintf(stderr, "Resource after GetOnion: %ld\n", ResourceUsage());


  GMessage * message = new GMessage();
  message->SetAsString(GM_COMMAND, CMD_GET_CERTIFICATES);
  GMessage * parameters = NULL;
  parameters = new GMessage();
  parameters->SetAsString(GM_COUNT, "10");
  parameters->SetAsString(GM_CERTIFICATE_TYPE, GM_MIX);
  parameters->SetAsString(GM_REQUEST_ID, CMD_GET_CERTIFICATES);
  parameters->SetAsGMessage(GM_ONION, onion);
  parameters->DebugHeaders(logFile, "parameters");
  message->SetAsGMessage(GM_PARAMETERS, parameters);
  message->DebugHeaders(logFile,"message");
  DELETE(parameters);
  MsgField *dataMsgField = NULL;
  dataMsgField = message->StoreToMsgField();
  DELETE(message);

  //fprintf(stderr, "Resource before SendData: %ld\n", ResourceUsage());
  if (useRandom)
    defaultAddHops = (random() & 7) + 1;
  WriteString(llDebug, __FILE__ ":%d:Send data with %lu hops.", __LINE__, 
	      defaultAddHops); 
  GMessage * acscert = NULL;
  acscert = ((Client *) six)->certificateRepository->GetCertificate(ACTYPE_ACS);
  GMessage * mix = acscert->GetAsGMessage(GM_MIX_CERTIFICATE); 
  char * name = acscert->GetAsString(GM_NAME); 
  mix->SetAsString(GM_DESTINATION, name);
  SendData(dataMsgField, mix, GM_MIX, defaultAddHops);
  DELETE(acscert);
  // DELETE(dataMsgField);

    // Wait for reply and find out if it is OK.
  ((Client *) six)->receiverToMajordomo->GetMsg(&message);
  message->DebugHeaders(logFile, "message received back");

  char * command = NULL;
  command = message->GetAsString(GM_COMMAND);
  if (strcmp(CMD_DATA_RECEIVED, command) != 0) {
    WriteString(llFatalError, __FILE__ ":%d:Didn't get data, but should.", 
		__LINE__);
    exit(1);
  }
  DELETE(command);
  
  parameters = NULL;
  parameters = message->GetAsGMessage(GM_PARAMETERS); 
  DELETE(message);
  Size dataLen;
  char * newData;
  newData = (char *) parameters->GetAsBytes(GM_DATA, &dataLen);
  parameters->DebugHeaders(logFile, "parameters received back");
  DELETE(parameters);
  
  DELETE(newData);
  
  fprintf(stderr, "Oukej.\n");

  return NULL;

}
