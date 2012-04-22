#include "./H/majordomo.h"

extern char * fileName;
extern char * ffidFileName;

/**
 * Chooses the method to handle the command.
 *
 * Decidec which method to use according to got command.
 *
 *
 * @param   command got command
 * @param   dataFld got data
 * @param   acid ID of certificate to which the message was sent
 * @return  error code 
 * @author  Pavel
 * @see     
 */
Err 
Majordomo::SwitchDataReceivedCommand(const Char     *command,
                                         const MsgField *dataFld,
                                         const ACID      acid)
{
  Err result;
  DBG;
  WriteString(llDebug, "SwitchDataReceivedCommand - command:%s", command);
  
  if(strcmp(command, EC_REPLY_ALLOC) == 0)
    result = DRCReplyAllocation(dataFld);
  else if(strcmp(command, EC_REPLY_STOR) == 0)
    result = DRCReplyStorage(dataFld);
  else if(strcmp(command, CMD_ANSWER_HEADER) == 0)
    result = DRCAnswerHeader(dataFld);
  else if(strcmp(command, CMD_ANSWER_FILE) == 0)
    result = DRCAnswerFile(dataFld);
  else if(SixMajordomo::SwitchDataReceivedCommand(command, dataFld, acid) == OK)
    result = OK;
  else
  {
    WriteString(llWarning, __FILE__":%d: unknown data_received command", __LINE__);
    result = KO;
  }

  return result;
}


/**
 * Serves the CE_REPLY_ALLOCATION message.
 *
 * Looks into the messages, whether or not the allocation was
 * succesful. If yes, adds the Eso certificate to yesCertificates
 * ArrayGMessage. 
 * 
 *
 * @param   dataFld data from the incoming message
 * @return  error code
 * @author  Pavel
 * @see     Banker 
 */
Err 
Majordomo::DRCReplyAllocation(const MsgField *dataFld)
{
  if(dataFld == NULL)
  {
    WriteString(llError, __FILE__":%d:Reply to CE_REQ_FOR_FILE empty.", __LINE__);
    return KO;
  }

DBG;
    // convert to GM
  GMessage *dataMsg = ((MsgField *)dataFld)->GetAsGMessage();
  if(dataMsg == NULL)
  {
    WriteString(llError, __FILE__":%d:Reply to CE_REQ_FOR_FILE cannot be converted to GM.", __LINE__);
    return KO;
  }
  dataMsg->DebugHeaders(logFile, "EC_REPLY_ALLOC message:");

    // add Replyy-To-AC to yesEsoCertificates
      //Marek added: needed not only reply-to AC, but also reply-to-TID
  ID replyToTID;
  if(dataMsg->GetAsID(GM_REPLY_TO_TID, &replyToTID) != OK){
    WriteString(llError, __FILE__":%d:Reply to CE_REQ_FOR_FILE cannot get Reply-To-TID.", __LINE__);
    DELETE(dataMsg);
    return KO;
  }
 
  DBG;
  GMessage *yesCertificate = dataMsg->GetAsGMessage(GM_REPLY_TO_AC);
  DBG;
  if(yesCertificate == NULL)
  {
    WriteString(llError, __FILE__":%d:Reply to CE_REQ_FOR_FILE cannot get Reply-To-AC.", __LINE__);
    return KO;
  }
DBG;
#ifndef NO_BANKER
  DBG;
  MsgField *payPlanFld = NULL;
  DBG;
  payPlanFld = dataMsg->GetAsMsgField(GM_PAY_PLAN);
  DBG;
  if(payPlanFld == NULL) {
    DELETE(yesCertificate);
    DELETE(dataMsg);
    DBG;
    return KO;
  }

DBG;
    //needed Payment-Plan for bank operations
  if(yesCertificate->SetAsMsgField(GM_PAY_PLAN, payPlanFld) != OK) {
    DELETE(yesCertificate);
    DELETE(dataMsg);
    DELETE(payPlanFld);
    return KO;
  }
  DELETE(payPlanFld);
#endif
  DELETE(dataMsg);
DBG;
    //needed reply-to-tid is added to the AC
  if(yesCertificate->SetAsID(GM_REPLY_TO_TID, replyToTID) != OK){
    DELETE(yesCertificate);
    return KO;
  }
  MsgField *yesField;
  if((yesField= yesCertificate->StoreToMsgField()) == NULL){
    DELETE(yesCertificate);
    return KO;
  }
  Err result = yesEsoCertificates->AddField(GM_CERTIFICATE, yesField);
  receivedAnswers++;
  DELETE(yesCertificate);

  return result;
}


/**
 * Serves the reply to CE_DATA_TO_STOR
 *
 * Looks into the message, whether the storage was successful and
 * prints the result to the stdout.
 *
 * @param   dataFld Data from the incoming message.
 * @return  error code
 * @author  Pavel
 * @see     Banker 
 */
Err 
Majordomo::DRCReplyStorage(const MsgField *dataFld)
{
  if(dataFld == NULL)
  {
    WriteString(llError, __FILE__":%d:Reply to CE_DATA_TO_STOR empty.", __LINE__);
    return KO;
  }

    // convert to GM
  GMessage *dataMsg = ((MsgField *)dataFld)->GetAsGMessage();
  if(dataMsg == NULL)
  {
    WriteString(llError, __FILE__":%d:Reply to CE_DATA_TO_STOR cannot be converted to GM.", __LINE__);
    return KO;
  }
  dataMsg->DebugHeaders(logFile, "EC_REPLY_STOR message:");

    // add Replyy-To-AC to yesEsoCertificates
  MsgField *yesCertificate = dataMsg->GetAsMsgField(GM_REPLY_TO_AC);
  DELETE(dataMsg);
  if(yesCertificate == NULL)
  {
    WriteString(llError, __FILE__":%d:Reply to CE_DATA_TO_STOR cannot get Reply-To-AC.", __LINE__);
    return KO;
  }
  yesEsoCertificates->AddField(GM_CERTIFICATE, yesCertificate);
  receivedAnswers++;
  DELETE(yesCertificate);

  return OK;
}


/**
 * Serves CE_REPLY_HEADER message.
 *
 * Gets the header out of the GMessage and prints info about it to the
 * stdout.
 *
 *
 * @param   dataFld Data from the GMessage.
 * @return  error code 
 * @author  Marek
 * @see     
 */
Err 
Majordomo::DRCAnswerHeader(const MsgField *dataFld)
{

  DBG;
  if(dataFld == NULL)
  {
    WriteString(llError, __FILE__":%d:Reply to ANSWER_HEADER empty.", __LINE__);
    return KO;
  }
DBG;
    // convert to GM
  GMessage *dataMsg = ((MsgField *)dataFld)->GetAsGMessage();
  if(dataMsg == NULL)
  {
    WriteString(llError, __FILE__":%d:Reply to ANSWER_HEADER cannot be converted to GM.", __LINE__);
    return KO;
  }
  dataMsg->DebugHeaders(logFile, "ANSWER_HEADER message:");
DBG;
  GMessage *header;
  header = dataMsg->GetAsGMessage(GM_HEADER);
DBG;
  header->DebugHeaders(logFile, "HEADER inside:");

  MsgField *encryptedFFID = header->GetAsMsgField(GM_FFID);
  if(encryptedFFID == NULL){
    return KO;
  }
DBG;  
  MsgField *ffid = DecryptByACPriv(0, encryptedFFID);
  DELETE(encryptedFFID);
  if(ffid == NULL){
    return KO;
  }
DBG;  
  MsgField *encryptedFileSize = header->GetAsMsgField(GM_FILE_SIZE);
  DELETE(header);
  if(encryptedFileSize == NULL){
    DELETE(ffid);
    return KO;
  }
DBG;  
  MsgField *fileSize = DecryptByACPriv(0, encryptedFileSize);
  if(fileSize == NULL){
    DELETE(ffid);
    return KO;
  }
DBG;  

  ID tmp;
  fileSize->GetAsID(&tmp);

    // create file for ffid
  receivedAnswers++;
  Char recAns[7];
  sprintf(recAns, "%d", receivedAnswers);
  Char *ffidTmp = new Char[strlen(ffidFileName) + 15];
  strcpy(ffidTmp, ffidFileName);
  strcat(ffidTmp, ".");
  strcat(ffidTmp, recAns);
  strcat(ffidTmp, ".ffid");
  ffid->SetLogFile(logFile);
  if(ffid->GetAsFile(ffidTmp) != OK) {
    WriteString(llError, __FILE__":%d:Cannot save file with FFID to %s", 
                         __LINE__, ffidTmp);
    return KO;
  }
  
  WriteString(llWarning, "<CLIENT>: Received header: FFID file: %s, FileSize: %ld\n", ffidTmp, tmp);
  if(strcmp(fileName, "") != 0){
    FILE *f = fopen(fileName, "w");
    fprintf(f,  "Received header: FFID file: %s, FileSize: %ld\n", ffidTmp, tmp);
    fclose(f);
  }
  else
    fprintf(stderr, "<CLIENT>: Received header: FFID file: %s, FileSize: %ld\n", ffidTmp, tmp);
    
 

DBG;  
  DELETE(ffid);
  DELETE(ffidTmp);
  DELETE(fileSize);
  
  return OK;
}

/**
 * Serves CE_REPLY_FILE message.
 *
 * Gets the file out of the GMessage and saves it into a file
 * specified by user.
 *
 *
 * @param   dataFld Data from the GMessage.
 * @return  error code 
 * @author  Marek
 * @see     
 */
Err 
Majordomo::DRCAnswerFile(const MsgField *dataFld)
{
  DBG;
  if(dataFld == NULL)
  {
    WriteString(llError, __FILE__":%d:Reply to CE_REPLY_FILE empty.", __LINE__);
    return KO;
  }

  DBG;
    // convert to GM
  GMessage *dataMsg = ((MsgField *)dataFld)->GetAsGMessage();
  if(dataMsg == NULL)
  {
    WriteString(llError, __FILE__":%d:Reply to CE_REPLY_FILE cannot be converted to GM.", __LINE__);
    return KO;
  }
  dataMsg->DebugHeaders(logFile, "EC_REPLY_FILE message:");

  DBG;
  MsgField *encryptedFile;
  encryptedFile = dataMsg->GetAsMsgField(GM_FILE);
  if(encryptedFile == NULL){
    WriteString(llError, __FILE__":%d:Didn't get file :-(", __LINE__);
    return KO;
  }
  
  MsgField *fileMsgField;
#ifndef NO_BANKER
  if((cipheredData = DecryptByACPriv(0, encryptedFile)) == NULL) {
    WriteString(llError, __FILE__":%d:Cannot decrypt file :-(", __LINE__);
    return KO;
  }
  fileMsgField = ParseCipheredData();
  if(fileMsgField == NULL) {
    WriteString(llError, __FILE__":%d:Cannot parse decrypted file :-(", __LINE__);
    return KO;
  }
  DELETE(cipheredData);
#else
  fileMsgField = DecryptByACPriv(0, encryptedFile);
  if(fileMsgField == NULL){
    WriteString(llError, __FILE__":%d:Cannot decrypt file :-(", __LINE__);
    return KO;
  }
#endif

  DBG;
  if(fileMsgField->GetAsFile(fileName) != OK){
  DBG;
    WriteString(llError, __FILE__":%d:Cannot save file :-(", __LINE__);
    DELETE(fileMsgField);
    return KO;
  }
  DELETE(fileMsgField);

  fprintf(stderr, "<CLIENT>\n");
  fprintf(stderr,"<CLIENT>: File downloaded successfully.\n");
  receivedAnswers++;
  DBG;
  fprintf(stderr, "<CLIENT>\n");
  fprintf(stderr, "<CLIENT>: File stored as %s\n", fileName);

//  DELETE(fileName);
  
  return OK;
}

