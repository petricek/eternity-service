#include <unistd.h>

#include "./../../Common/RandomGenerator/H/randomGenerator.h"
#include "./H/majordomo.h"

extern char * fileName;
extern char * fileKeys;
extern char * storeTime;
extern int    esoCount;


/**
 * Uploads file.
 *
 * Tries to upload file on number of Esos specified by user. Upload is
 * made in two phases. The first phase is request for allocation. 
 * The file is then sent to the Esos, whose reply is positive.
 *
 *
 * @return  void 
 * @author  Pavel
 * @see     
 */
void 
Majordomo::UploadFile()
{
DBG;
  if((fileName == NULL) || (strlen(fileName) < 1))
  {
    WriteString(llError, __FILE__":%d:No file name given", __LINE__);
    return;
  }
fprintf(stderr, "<CLIENT>: Running Upload with these parameters:\n");
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
fprintf(stderr, "<CLIENT>: Name of file to upload:     %s\n", fileName);
fprintf(stderr, "<CLIENT>: Keywords of the file:       %s\n", fileKeys);
fprintf(stderr, "<CLIENT>: Count of Esos to upload to: %d\n", esoCount);
fprintf(stderr, "<CLIENT>: Storetime:                  %s\n", storeTime);
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");

DBG;
    // does this file exist?
  File *fileToStore = new File(fileName);
  if(! fileToStore->Exists())
  {
    WriteString(llError, __FILE__":%d:No such file", __LINE__);
    return;
  }
  
DBG;
    // get number of Esos that should store the file
  string strRedundancy;
  if(esoCount == 0)
  {
    strRedundancy = ((Client *) six)->configFile->GetValue(CFG_DEFAULT_REDUNDANCY);
    esoCount = atoi(strRedundancy.c_str());
  }
   
DBG;
    // Get Esos' certificates
fprintf(stderr, "<CLIENT>: Asking for Eso certificates...\n");
  ArrayGMessage *esoCertificates = (ArrayGMessage *)GetCertificates(ACTYPE_ESO, esoCount);
  if(esoCertificates == NULL)
  {
    WriteString(llError, __FILE__":%d:No eso certificates given", __LINE__);
    DELETE(fileToStore);
    return;
  }
  esoCertificates->DebugHeaders(logFile, "Eso certificates:");
fprintf(stderr, "<CLIENT>: Got Eso certificates.\n");
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
  
DBG;
    // Contact all these Eso's and ask if it is possible to store file
  RequestStorage(esoCertificates, fileToStore);
  DELETE(esoCertificates);
  DELETE(fileToStore);
  yesEsoCertificates->DebugHeaders(logFile, "YES Eso certificates:");
  
DBG;
    // Store the file on those who agree
  StoreFile(yesEsoCertificates);
  DELETE(yesEsoCertificates);
  WriteString(llDebug, __FILE__":%d:Finished storing file", __LINE__);
}


/**
 * Asks Esos to store the file.
 *
 * Contact all these Eso's and ask if it is possible to store file
 *
 *
 * @param   certificates Certificates of Esos to ask.
 * @param   fileToStore file to try to store
 * @return  void 
 * @author  Pavel
 * @see     
 */
void 
Majordomo::RequestStorage(ArrayGMessage *certificates, File *fileToStore)
{
  GMessage *outMsg, *certificate;
  MsgField *certFld;
  
    // create simple request to store
fprintf(stderr, "<CLIENT>: Creating requests for storage...\n");
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
  GMessage *simple = CreateSimpleReqForStor(fileToStore);
  
    // for each certificate
  int i = 1;
fprintf(stderr, "<CLIENT>: Sending requests for storage...\n");
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
  certFld = certificates->GetField(GM_CERTIFICATE, i);
  while(certFld != NULL)
  {
      // convert MsgField to GMessage
    certificate = certFld->GetAsGMessage();
    DELETE(certFld);
    if(certificate == NULL)
      continue;
      
      // create message
    outMsg = CreateSendDataMsg(CE_REQ_FOR_STOR, simple, certificate);
    DELETE(certificate);
    if(outMsg == NULL)
    {
      WriteString(llError, __FILE__":%d:Cannot create outMsg", __LINE__);
      continue;
    }
    
      // send message
    if(FeedSenderWithMsgForMix(outMsg) != OK)
      WriteString(llError, __FILE__":%d:Cannot push outMsg to Sender", __LINE__);
    DELETE(outMsg);
    
      // next certificate
    certFld = certificates->GetField(GM_CERTIFICATE, ++i);
  }
  DELETE(simple);

fprintf(stderr, "<CLIENT>: Waiting for answers from Esos...\n");
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
    // wait for answers
  WaitForAnswersToReqForStor(--i);
fprintf(stderr, "<CLIENT> \n");
fprintf(stderr, "<CLIENT>: Allocation successful at %d Esos.\n", receivedAnswers);
fprintf(stderr, "<CLIENT> \n");
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
}


/**
 * Stores file on Esos.
 *
 * Creates messages to store file on Esos with given certificates and
 * waits for answers to them.
 *
 *
 * @param   certificates Certificates of Esos to store the file on.
 * @return  void
 * @author  Pavel
 * @see     Banker 
 */
void 
Majordomo::StoreFile(ArrayGMessage *certificates)
{
  GMessage *outMsg, *simple, *certificate;
  MsgField *certFld;
  MsgField *fileIDFld;
  
    // for each certificate
fprintf(stderr, "<CLIENT>: Preparing file to be stored...\n");
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");

    // get random FFID
  int ffidLen;
  string ffidLenStr = six->configFile->GetValue(CFG_FFID_LENGTH);
  if( ffidLenStr.empty() || ((ffidLen = atoi(ffidLenStr.c_str())) == 0)) 
    ffidLen = CFG_DEFAULT_FFID_LENGTH;
  RandomGenerator randGen(logFile);
  fileIDFld = randGen.RandomBytes(ffidLen);
  if(fileIDFld == NULL) {
    WriteString(llError, __FILE__":%d:Cannot generate FFID");
    return;
  }

    // and save it to fileName.ffid
  Char *ffidFileName = new Char[strlen(fileName) + 10];
  strcpy(ffidFileName, fileName);
  strcat(ffidFileName, ".ffid");
  if(fileIDFld->GetAsFile(ffidFileName) != OK) {
    WriteString(llError, __FILE__":%d:Cannot save FFID");
    return;
  }
  else{
    fprintf(stderr, "<CLIENT>: File was assigned a ffid.\n");
    fprintf(stderr, "<CLIENT>: The ffid was saved to %s.\n", ffidFileName);
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
  }
  DELETE(ffidFileName);

fprintf(stderr, "<CLIENT>: Sending file to Esos.\n");
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
  int i = 1;
  certFld = certificates->GetField(GM_CERTIFICATE, i);
  while(certFld != NULL)
  {
      // convert MsgField to GMessage
    certificate = certFld->GetAsGMessage();
    DELETE(certFld);
    if(certificate == NULL)
      continue;
      
    WriteString(llDebug, "CIPHERED DATA...0");
#ifndef NO_BANKER
    MsgField *payPlan = certificate->GetAsMsgField(GM_PAYMENT_PLAN);
    WriteString(llDebug, "CIPHERED DATA...1");
    if(((Client *)six)->banker->DecideAcceptPPlan(payPlan) == OK)
    {

      WriteString(llDebug, "CIPHERED DATA...2");
      if(CreateCipheredData() != OK)
        continue;

      if(((Client *)six)->banker->MakePPlanForBanks(payPlan) != OK)
        continue;
#endif
        // create message
      simple = CreateSimpleFileToStor(certificate, fileIDFld);

       //Marek added outMsg = 
      outMsg =  CreateSendDataMsg(CE_DATA_TO_STOR, simple, certificate);
    
      DELETE(certificate);
      DELETE(simple);
      if(outMsg == NULL)
      {
        WriteString(llError, __FILE__":%d:Cannot create outMsg", __LINE__);
        continue;
      }
    
        // send message
      if(FeedSenderWithMsgForMix(outMsg) != OK)
        WriteString(llError, __FILE__":%d:Cannot push outMsg to Sender");
      DELETE(outMsg);

#ifndef NO_BANKER
    }
#endif
    
      // next certificate
    certFld = certificates->GetField(GM_CERTIFICATE, ++i);
  }
 DELETE (fileIDFld);
 fprintf(stderr, "<CLIENT>: Waiting for answer from Esos...\n"); 
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
    // wait for answers
  WaitForAnswersToFileToStor(--i);
  fprintf(stderr, "<CLIENT>\n");
  fprintf(stderr, "<CLIENT>: Storage successful at %d Esos.\n", receivedAnswers);
  fprintf(stderr, "<CLIENT>\n");
}


/**
 * Creates simple req for storage.
 *
 * Creates simple request for storage, that is the message sent to Eso
 * as a CE_REQUEST_FOIR_STOR.
 *
 *
 * @param   fileToStore File that is wanted to be stored.
 * @return  created messsage. 
 * @author  Pavel
 * @see     GMessage
 */
GMessage *
Majordomo::CreateSimpleReqForStor(File *fileToStore)
{
    // get information that is needed in the request
  RelTime storageTime;
  storageTime.SetTime("00:01:00:00:00:00");
  DBG;
  WriteString(llDebug, "CreateSimpleRequestForStorage!!!!");
    // put info to GM
  GMessage *request = new GMessage();
  request->SetAsID(GM_FILE_SIZE, fileToStore->GetSize());
  request->SetAsBasicTime(GM_STORAGE_TIME, storageTime);
DBG;
    // add standard info
  FillReplyToInfo(request);
DBG;
  request->DebugHeaders(logFile, "simple REQUEST FOR STORAGE");
  return request;
}


/**
 * Waits for answers to CE_REQUEST_FOR_STOR.
 *
 * Waits for answers to CE_REQUEST_FOR_STOR in a loop until a timeout
 * is over.
 *
 *
 * @param   requestSent Number of messages for wait for.
 * @return  void
 * @author  Pavel
 * @see     
 */
void 
Majordomo::WaitForAnswersToReqForStor(int requestsSent)
{
  int secondsToWait;
  GMessage *inMsg;
  
    // get default wait time from config file
  string strSecondsToWait = six->configFile->GetValue(CFG_SECONDS);
  if(strSecondsToWait.empty())
    secondsToWait = CFG_SECONDS_TO_WAIT;
  else
    secondsToWait = atoi(strSecondsToWait.c_str());
    
    // wait until all requests are answerd or time is out
  receivedAnswers = 0;
  if(yesEsoCertificates != NULL) DELETE(yesEsoCertificates);
  yesEsoCertificates = new ArrayGMessage();
  for(int sec=0; (sec < secondsToWait) && (receivedAnswers < requestsSent); sec ++)
  {
      // check input queue
    if(six->receiverToMajordomo->MsgCount() > 0)
    {
      six->receiverToMajordomo->GetMsg(&inMsg);
      HandleMixMessage(inMsg);
    }
    
      // sleep for a second
    sleep(1);
  }
  
    // stop client
  exit;
}


/**
 * Creates simple message with a file for storage.
 *
 * Creates a message with file for storage, that is going to be sent to Eso
 * as a CE_FILE_FOR_STOR including encryption of the file with
 * TCBPubKey of the Eso.
 *
 *
 * @param   certificate Certificate of the Eso with the TCBPubKey.
 * @return  created messsage. 
 * @author  Pavel
 * @see     GMessage, Cipherer
 */
GMessage *
Majordomo::CreateSimpleFileToStor(GMessage *certificate, MsgField *fileIDFld)
{
  MsgField * eFileFld;
  MsgField * eKwdsFld;
  MsgField * eFileIDFld;
    // prepare other info
  ArrayGMessage *kwdsMsg = new ArrayGMessage();
  kwdsMsg->LoadFromLines(GM_KEYWORD, fileKeys);
  MsgField *fileFld = new FileMsgField("", fileName);
  MsgField *kwdsFld = kwdsMsg->StoreToMsgField();
  DELETE(kwdsMsg);

    // encrypt info with TCB public key
  GMessage *tcbPubKey = certificate->GetAsGMessage(GM_TCB_PUBLIC_KEY);
  GMessage *ciphData = cipheredData->GetAsGMessage();
  ciphData->DebugHeaders(logFile, __FILE__": CipheredData before encryption");
  DELETE(ciphData);
  six->cipherer->EncryptWithPublicKey(tcbPubKey, cipheredData,   &eFileFld);
  six->cipherer->EncryptWithPublicKey(tcbPubKey, fileIDFld, &eFileIDFld);
  six->cipherer->EncryptWithPublicKey(tcbPubKey, kwdsFld,   &eKwdsFld);
  DELETE(fileFld);
  DELETE(kwdsFld);
  DELETE(tcbPubKey);

    // put info to GM
  GMessage *request = new GMessage();
  request->SetAsMsgField(GM_DATA, eFileFld);
  request->SetAsMsgField(GM_FFID, eFileIDFld);
  request->SetAsMsgField(GM_KEYWORDS, eKwdsFld);
  DELETE(eFileFld);
  DELETE(eFileIDFld);
  DELETE(eKwdsFld);

   //get reply-to-tid from the certificate 
   //it was added there before inserting to yesCertificates
  ID inReplyToTID;
  certificate->GetAsID(GM_REPLY_TO_TID, &inReplyToTID);
  request->SetAsID(GM_IN_REPLY_TO_TID, inReplyToTID);
  
    // add standard info
  FillReplyToInfo(request);

  return request;
}


/**
 * Waits for answers to CE_FILE_TO_STOR.
 *
 * Waits for answers to CE_FILE_TO_STOR in a loop until a timeout
 * is over.
 *
 *
 * @param   requestSent Number of messages for wait for.
 * @return  void
 * @author  Pavel
 * @see     
 */
void 
Majordomo::WaitForAnswersToFileToStor(int requestsSent)
{
  int secondsToWait;
  GMessage *inMsg;
  
    // get default wait time from config file
  string strSecondsToWait = six->configFile->GetValue(CFG_SECONDS);
  if(strSecondsToWait.empty())
    secondsToWait = CFG_SECONDS_TO_WAIT;
  else
    secondsToWait = atoi(strSecondsToWait.c_str());
    
    // wait until all requests are answerd or time is out
  receivedAnswers = 0;
  yesEsoCertificates = new ArrayGMessage();
  for(int sec=0; (sec < secondsToWait) && (receivedAnswers < requestsSent); sec ++)
  {
      // check input queue
    if(six->receiverToMajordomo->MsgCount() > 0)
    {
      six->receiverToMajordomo->GetMsg(&inMsg);
      HandleMixMessage(inMsg);
    }
    
      // sleep for a second
    sleep(1);
  }
  
    // stop client
  exit;
}


#ifndef NO_BANKER
/**
 * Creates ciphered data.
 *
 * Modifies file for storage. Adds random string and encrypts with
 * private key.
 * <pre>
 * The result looks like this:
 * GM_PUBLIC_KEY:
 * GM_DATA: (encrypted with Private Key)
 *   GM_RANDOM: (random string)
 *   GM_FILE: (decrypted file)
 * </pre>
 *
 * @return  error code 
 * @author  pavel
 * @see     Cipherer, RandomGenerator
 */
Err 
Majordomo::CreateCipheredData()
{
   
  DBG;
  WriteString(llDebug, "CIPHERED DATA CREATING !!!");
    // create GM to store decrypted file with random string
  GMessage *dMsg = new GMessage();
  DBG;
  
    // put decrypted file in it
  MsgField *dData = new FileMsgField();
  dData->SetAsFile(fileName);
  dMsg->SetAsMsgField(GM_FILE, dData);
  DELETE(dData);
  
  DBG;
    // put there random string as well
  RandomGenerator randGen(logFile);
  MsgField *randFld = randGen.RandomBytes(CFG_DEFAULT_RANDOM_STRING_LENGTH);
  dMsg->SetAsMsgField(GM_RANDOM, randFld);
  DELETE(randFld);
  MsgField *dFld = dMsg->StoreToMsgField();
  DELETE(dMsg);
  
  DBG;
    // encrypt it with private key
    //: get client's ACPrivKey
  GMessage *acPrivKey = LoadPrivateKey();
  if(acPrivKey == NULL)
    return KO;
  
  DBG;
    //: encrypt
  MsgField *eFld;
  six->cipherer->EncryptWithPrivateKey(acPrivKey, dFld, &eFld);
  DELETE(dFld);
  DELETE(acPrivKey);
  
  DBG;
    // get client's public key
  GMessage *acPublicKey = LoadPublicKey();
  if(acPublicKey == NULL) {
    DELETE(eFld);
    return KO;
  }
  DBG;

    // create file message (encrypted file + public key) and store it to MsgField
  GMessage *eFileMsg = new GMessage();
  eFileMsg->SetAsMsgField(GM_DATA, eFld);
  eFileMsg->SetAsGMessage(GM_PUBLIC_KEY, acPublicKey);
  eFileMsg->DebugHeaders(logFile, "CIPHERED DATA...6");
  cipheredData = eFileMsg->StoreToMsgField();
  DELETE(eFld); DELETE(acPublicKey); DELETE(eFileMsg);

  DBG;
  return OK;
}

/**
 * Parses ciphered data.
 *
 * Parses encrypted data to get the stored file. Decrypts them with
 * added publicKey. 
 * <pre>
 * The incoming ciphered data looks like this:
 * GM_PUBLIC_KEY:
 * GM_DATA: (encrypted with Private Key)
 *   GM_RANDOM: (random string)
 *   GM_FILE: (decrypted file)
 * </pre>
 *
 * @return  msgField decrypted file  
 * @author  Pavel
 * @see     Cipherer
 */
MsgField *
Majordomo::ParseCipheredData()
{
DBG;
  if(cipheredData == NULL)
    return NULL;

    DBG;
  GMessage *eFileMsg = cipheredData->GetAsGMessage();
  if(eFileMsg == NULL) return NULL;
 
  DBG;
  eFileMsg->DebugHeaders(logFile, "Ciphered data GM");
  GMessage *pubKey = eFileMsg->GetAsGMessage(GM_PUBLIC_KEY);
  if(pubKey == NULL) {
    DELETE(eFileMsg);
    return NULL;
  }
 
  DBG;
  MsgField *eData = eFileMsg->GetAsMsgField(GM_DATA);
  DELETE(eFileMsg);
  if(eData == NULL) {
    DELETE(pubKey);
    return NULL;
  }
 
  DBG;
  MsgField *dData;
  Err result = six->cipherer->DecryptWithPublicKey(pubKey, eData, &dData);
  DELETE(eData); DELETE(pubKey);
  if(result != OK) return NULL;
 
  DBG;
  GMessage *dDataMsg = dData->GetAsGMessage();
  DELETE(dData);
  if(dDataMsg == NULL) return NULL;
 
  DBG;
  MsgField *dFileFld = dDataMsg->GetAsMsgField(GM_FILE);
  DELETE(dDataMsg);

DBG;
  return dFileFld;
}
#endif

/**
 * Computes MAC.
 *
 * Computes MAC from OAuths.
 *
 *
 * @param   oAuth oAuth to compute MAC.
 * @return  MsgField computed MAC.
 * @author  Pavel
 * @see     
 */
MsgField *
Majordomo::ComputeMAC(MsgField *oAuth)
{
    // get (file + random string) encrypted with private key 
  GMessage *cipheredDataMsg = cipheredData->GetAsGMessage();
  if(cipheredDataMsg == NULL)
    return NULL;
  MsgField *eData = cipheredDataMsg->GetAsMsgField(GM_DATA);
  DELETE(cipheredDataMsg);
  if(eData == NULL)
    return NULL;

    // join oAuth with eData
  GMessage *compoundMsg = new GMessage();
  if(compoundMsg == NULL) {
    DELETE(eData);
    return NULL;
  }
  Err result = compoundMsg->SetAsMsgField(GM_DATA, eData);
  DELETE(eData);
  if(result != OK) {
    DELETE(compoundMsg);
    return NULL;
  }
  if((result = compoundMsg->SetAsMsgField(GM_OAUTH, oAuth)) != OK) {
    DELETE(compoundMsg);
    return NULL;
  }
  MsgField *compoundFld = compoundMsg->StoreToMsgField();
  DELETE(compoundMsg);
  if(compoundFld == NULL)
    return NULL;

    // count a hash from compoundFld (oAuth + eData)
  MsgField *mac;
  result = six->cipherer->MessageDigest(compoundFld, &mac);
  DELETE(compoundFld);

  return mac;
}

