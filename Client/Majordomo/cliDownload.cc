#include <unistd.h>

#include "./H/majordomo.h"


extern char * fileName;
extern char * fileKeys;
extern int    esoCount;
extern char * ffidFileName;


/**
 * Downloads headers from Esos.
 *
 * Tries to find files according to FileAttributeMask given by user in
 * command line.
 *
 *
 * @return  void 
 * @author  Marek
 * @see     
 */
void 
Majordomo::DownloadHeaders()
{
  if((fileKeys == NULL) || (strcmp(fileKeys, "") == 0))
  {
    WriteString(llError, __FILE__":%d:No keywords file name given", __LINE__);
    return;
  }
  
fprintf(stderr, "<CLIENT>: Running Search with these parameters:\n");
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
fprintf(stderr, "<CLIENT>: Keywords of the search: %s\n", fileKeys);
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");


  
DBG;
    // get number of Esos that should search for the file
  string strEsosToAsk;
  if(esoCount == 0)
  {
    strEsosToAsk = six->configFile->GetValue(CFG_DEFAULT_SEARCH_WIDTH);
    esoCount = atoi(strEsosToAsk.c_str());
  }
   
DBG;
    // Get Esos' certificates
fprintf(stderr, "<CLIENT>: Asking for Eso certificates...\n");
  GMessage *esoCertificates = GetCertificates(ACTYPE_ESO,esoCount);
fprintf(stderr, "<CLIENT>: Got Eso certificates.\n");
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
  if(esoCertificates == NULL)
  {
    WriteString(llError, __FILE__":%d:No eso certificates given", __LINE__);
    return;
  }
  
DBG;
    // Find the headers on the given esos 
  FindHeaders((ArrayGMessage *)esoCertificates);
  DELETE(esoCertificates);
}


/**
 * Downloads file from Esos.
 *
 * Tries to find files according to FindFileID given by user in
 * command line.
 *
 *
 * @return  void 
 * @author  Marek
 * @see     
 */
void 
Majordomo::DownloadFile()
{
 DBG; 
  if((ffidFileName == NULL) || (strlen(ffidFileName) < 1))
  {
     WriteString(llError, __FILE__":%d:No ffid file name given", __LINE__);
     return;
   }
fprintf(stderr, "<CLIENT>: Running Download with these parameters:\n");
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
fprintf(stderr, "<CLIENT>: Ffid file name: %s\n", ffidFileName);
fprintf(stderr, "<CLIENT>: Download to:    %s\n", fileName);
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
	      
    // get number of Esos that should search for the file
  string strEsosToAsk;
  if(esoCount == 0)
  {
    strEsosToAsk = six->configFile->GetValue(CFG_DEFAULT_SEARCH_WIDTH);
    esoCount = atoi(strEsosToAsk.c_str());
  }
   DBG; 
    // Get Esos' certificates
fprintf(stderr, "<CLIENT>: Asking for Eso certificates...\n");
  GMessage *esoCertificates = GetCertificates(ACTYPE_ESO, esoCount);
fprintf(stderr, "<CLIENT>: Got Eso certificates.\n");
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
  if(esoCertificates == NULL)
  {
    WriteString(llError, __FILE__":%d:No eso certificates given", __LINE__);
    return;
  }
  DBG; 
    // Find the headers on the given esos 
  FindFile((ArrayGMessage*)esoCertificates);
  DBG; 
  DELETE(esoCertificates);
  DBG; 
  WriteString(llDebug, __FILE__":%d:File download done.", __LINE__);
}


/**
 * Tries to find headers on Esos.
 *
 * Creates request message, sends it to given number of Esos and waits
 * for reply.
 *
 *
 * @param   certificates Certificates of Esos to ask.
 * @return  void
 * @author  Marek
 * @see     
 */
void 
Majordomo::FindHeaders(ArrayGMessage *certificates)
{
  GMessage *outMsg;
  GMessage *certificate;  

    // create simple request to store
  GMessage *simple;
  
DBG;
    // for each certificate
  int i = 1;
  MsgField *certifMsgFld = certificates->GetField(GM_CERTIFICATE, i);
  while(certifMsgFld != NULL)
  {
DBG;
    if((certificate = certifMsgFld->GetAsGMessage()) == NULL){
      DELETE(certifMsgFld);
      continue;
    }

DBG;
    GMessage *tcbPubKey = certificate->GetAsGMessage(ACFLD_TCB_PUB_KEY);
    simple = CreateSimpleReqForHeaders(tcbPubKey);
    DELETE(tcbPubKey);

DBG;
   simple->DebugHeaders(logFile, "Download::FindHeaders - simpleMsg");
DBG;
      // create message
fprintf(stderr, "<CLIENT>: Sending requests for search. \n");
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
    outMsg = CreateSendDataMsg(CMD_REQ_FOR_HEADERS, simple, certificate);
    DELETE(certificate);
    if(outMsg == NULL)
    {
      WriteString(llWarning, __FILE__":%d:Cannot create outMsg", __LINE__);
      continue;
    }
    
DBG;
      // send message
    if(FeedSenderWithMsgForMix(outMsg) != OK)
      WriteString(llError, __FILE__":%d:Cannot push outMsg to Sender");
    DELETE(outMsg);
    
DBG;
      // next certificate
    certifMsgFld = certificates->GetField(GM_CERTIFICATE, ++i);
  }
  DELETE(simple);
  
DBG;
    // wait for answers
fprintf(stderr, "<CLIENT>: Waiting for headers. \n");
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
  WaitForAnswersToReqForHeaders();
  WriteString(llDebug, __FILE__":%d:Find headers done.", __LINE__);
}



/**
 * Tries to find file on Esos.
 *
 * Creates request message, sends it to given number of Esos and waits
 * for reply.
 *
 *
 * @param   certificates Certificates of Esos to ask.
 * @return  void
 * @author  Marek
 * @see     
 */
void 
Majordomo::FindFile(ArrayGMessage *certificates)
{
  GMessage *outMsg;
  GMessage *certificate;  

    // create simple request to store
  GMessage *simple;
  
  DBG; 
    // for each certificate
  int i = 1;
  MsgField *certifMsgFld = certificates->GetField(GM_CERTIFICATE, i);
  while(certifMsgFld != NULL)
  {
    if((certificate = certifMsgFld->GetAsGMessage()) == NULL){
      DELETE(certifMsgFld);
      continue;
    }

  DBG; 
    GMessage *tcbPubKey = certificate->GetAsGMessage(ACFLD_TCB_PUB_KEY);
  DBG;
    simple = CreateSimpleReqForFile(tcbPubKey);
    DELETE(tcbPubKey);
 
  DBG;
   simple->DebugHeaders(logFile, "simple ReqForFindFile");
      // create message
fprintf(stderr, "<CLIENT>: Sending requests for search. \n");
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
    outMsg = CreateSendDataMsg(CMD_REQ_FOR_FILE, simple, certificate);
    DELETE(certificate);
    if(outMsg == NULL)
    {
      WriteString(llWarning, __FILE__":%d:Cannot create outMsg", __LINE__);
      continue;
    }
    
  DBG; 
      // send message
    if(FeedSenderWithMsgForMix(outMsg) != OK)
      WriteString(llError, __FILE__":%d:Cannot push outMsg to Sender");
    DELETE(outMsg);
    
      // next certificate
  DBG; 
    certifMsgFld = certificates->GetField(GM_CERTIFICATE, ++i);
  }
  DELETE(simple);
  
  DBG; 
    // wait for answers
fprintf(stderr, "<CLIENT>: Waiting for answers. \n");
fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
  WaitForAnswersToReqForFile();
  DBG; 
  WriteString(llDebug, __FILE__":%d:Find file done.", __LINE__);
}


/**
 * Creates REQ_FOR_HEADERS.
 *
 * Creates REQ_FRO_HEADERS message, that is to be sent to Esos.
 *
 *
 * @param   pubKey PubKey for encrypting FAM.
 * @return  created GMessage 
 * @author  Marek
 * @see     Cipherer
 */
GMessage *
Majordomo::CreateSimpleReqForHeaders(GMessage *pubKey)
{
 DBG;
  WriteString(llDebug, "CreateSimpleRequestForHeaders - start");
    //get the keywords
  ArrayGMessage *keywords = new ArrayGMessage();
  if(keywords == NULL)
   return NULL;
  keywords->LoadFromLines(GM_KEYWORD, fileKeys);

  keywords->DebugHeaders(logFile, "keywords:");
    
  GMessage *fam = new GMessage();
  fam->SetAsGMessage(GM_KEYWORDS, keywords);
  DELETE(keywords);

  fam->DebugHeaders(logFile, "FAM:");

  MsgField *famMsgFld = fam->StoreToMsgField();
  DELETE(fam);

  MsgField *encryptedFAM;
  Err result = six->cipherer->EncryptWithPublicKey(pubKey,famMsgFld, &encryptedFAM);
  DELETE(famMsgFld);
  if(result != OK)
    return NULL;

    //get local private key
  GMessage *transportKey;
  if(GetAsymetricKey(&transportKey) != OK){
   DELETE(encryptedFAM);
   return NULL;
  }

    // put info to GM
  GMessage *request = new GMessage();
  if(request == NULL){
   DELETE(encryptedFAM);
   DELETE(transportKey);
   return NULL;
  }


  result = request->SetAsMsgField(GM_FAM, encryptedFAM);
  DELETE(encryptedFAM);
  string tmp = six->configFile->GetValue(CFG_DEFAULT_SEARCH_WIDTH);
  result += request->SetAsInt(GM_SEARCH_WIDTH, atoi(tmp.c_str()));
  tmp = six->configFile->GetValue(CFG_DEFAULT_SEARCH_DEPTH);
  result += request->SetAsInt(GM_SEARCH_DEPTH, atoi(tmp.c_str()));
  result += request->SetAsGMessage(GM_TRANSPORT_KEY, transportKey);
  DELETE(transportKey);
  
  if(result != 4*OK){
    DELETE(request);
    return NULL;
  }

    // add standard info
  FillReplyToInfo(request);
  return request;
}

/**
 * Creates REQ_FOR_FILE.
 *
 * Creates REQ_FRO_FILE message, that is to be sent to Esos.
 *
 *
 * @param   pubKey PubKey for encrypting FFID.
 * @return  created GMessage 
 * @author  Marek
 * @see     Cipherer
 */
GMessage *
Majordomo::CreateSimpleReqForFile(GMessage *pubKey)
{
   
DBG;
  MsgField *ffidMsgFld = new BytesMsgField();
  if(ffidMsgFld->SetAsFile(ffidFileName) != OK) {
    WriteString(llError, __FILE__":%d:Cannot load FFID from file %s", 
                         __LINE__, ffidFileName);
    return NULL;
  }

DBG;
  MsgField *encryptedFFID;
  Err result = six->cipherer->EncryptWithPublicKey(pubKey,ffidMsgFld, &encryptedFFID);
  DELETE(ffidMsgFld);
  if(result != OK)
    return NULL;

DBG;
    //get local pivate key
  GMessage *transportKey;
  if(GetAsymetricKey(&transportKey) != OK){
   DELETE(encryptedFFID);
   return NULL;
  }

DBG;
    // put info to GM
  GMessage *request = new GMessage();
  result = request->SetAsMsgField(GM_FFID, encryptedFFID);
  DELETE(encryptedFFID);
  string tmp = six->configFile->GetValue(CFG_DEFAULT_SEARCH_WIDTH);
  result += request->SetAsInt(GM_SEARCH_WIDTH, atoi(tmp.c_str()));
  tmp = six->configFile->GetValue(CFG_DEFAULT_SEARCH_DEPTH);
  request->SetAsInt(GM_SEARCH_DEPTH, atoi(tmp.c_str()));
  result += request->SetAsGMessage(GM_TRANSPORT_KEY, transportKey);
  DELETE(transportKey);
  result += request->SetAsID(GM_REPLY_TO_TID, tID);


DBG;
  if(result != 3*OK){
    DELETE(request);
    return NULL;
  }
  
DBG;
  // add standard info
  request->DebugHeaders(logFile, "Simple request before FillReplyInfo");
  FillReplyToInfo(request);
  request->DebugHeaders(logFile, "Simple request after FillReplyInfo");
DBG;
  return request;
}


/**
 * Waits for answers to REQ_FOR_HEADERS.
 *
 * Waits for answers for number of seconds given in ConfigFile.
 *
 *
 * @return  void 
 * @author  Marek
 * @see     
 */
void 
Majordomo::WaitForAnswersToReqForHeaders()
{
  int secondsToWait;
  GMessage *inMsg;

DBG;
    // get default wait time from config file
 string strSecondsToWait = six->configFile->GetValue(CFG_SECONDS);
  if(strSecondsToWait.empty())
    secondsToWait = CFG_SECONDS_TO_WAIT;
  else
    secondsToWait = atoi(strSecondsToWait.c_str());

 
DBG;
    // wait until all requests are answerd or time is out
  receivedAnswers = 0;
  for(int sec=0; (sec < secondsToWait); sec ++)
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
 * Waits for answers to REQ_FOR_FILE.
 *
 * Waits for answers for number of seconds given in ConfigFile.
 *
 *
 * @return  void 
 * @author  Marek
 * @see     
 */
void 
Majordomo::WaitForAnswersToReqForFile()
{
  int secondsToWait;
  GMessage *inMsg; 
 
  DBG; 
    // get default wait time from config file
 string strSecondsToWait = six->configFile->GetValue(CFG_SECONDS);
  if(strSecondsToWait.empty())
    secondsToWait = CFG_SECONDS_TO_WAIT;
  else
    secondsToWait = atoi(strSecondsToWait.c_str());
    
  DBG; 
    // wait until all requests are answerd or time is out
  receivedAnswers = 0;
  for(int sec=0; ((sec < secondsToWait) && (receivedAnswers <= 0)); sec ++)
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



