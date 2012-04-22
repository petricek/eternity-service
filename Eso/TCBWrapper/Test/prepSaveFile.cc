#include "./../H/tcbWrapper.h"
#include "./test.h"
#include "./../../../Common/H/labels.h"

// this program prepares some data for testSaveFile and should be run always before it
// to ensure correctness of the test.

// this program creates AC key pair and encrypts some test data with its tcbPubKey,
// which are then stored in TEST_SAVE_FILE_FILE_NAME

int main()
{
  Err result;
  LogFile *log;
  TCBWrapper *tcb;
  Cipherer *cipherer;
  ACID acid;
  GMessage *acPubKey,  *tcbPubKey,
           *acPubKey2, *tcbPubKey2;
  Keywords *keywords;
  MsgField *dFFID, *dKeywords, *dData,
           *eFFID, *eKeywords, *eData;

  printf("This takes a while... (generating 2 asymetric keys)\n");
  
  log = new LogFile("prepSaveFile.log");
  tcb = new SWTCBWrapper(log);
  cipherer = new Cipherer(log);

    // generate AC keys
  log->WriteString(llDebug, "Starting TCBW::GenerateACKeys()");
  if(tcb->GenerateACKeys(&acid, &acPubKey, &tcbPubKey) == OK)
  {
    log->WriteString(llDebug, "Keys generated");
/*
    log->WriteString(llDebug, " ACID = %d", acid);
    acPubKey->DebugHeaders(log, " acPubKey");
    tcbPubKey->DebugHeaders(log, " tcbPubKey");
*/
    DELETE(acPubKey);
    DELETE(tcbPubKey);
  }
  else
  {
    log->WriteString(llError, "Keys cannot be generated");
    return -1;
  }

    // test, if they can be found
  if(tcb->GetACPubKeys(acid, &acPubKey2, &tcbPubKey2) == OK)
  {
    log->WriteString(llDebug, "Keys found");
/*
    tcbPubKey->DebugHeaders(log, " tcbPubKey");
    tcbPubKey2->DebugHeaders(log, " tcbPubKey2");
*/
    DELETE(acPubKey2);
    DELETE(tcbPubKey2);
  }
  else
  {
    log->WriteString(llError, "Keys cannot be found");
    return -1;
  }

    // create msgFields for test data
  dFFID = new BytesMsgField("", TEST_FFID, strlen(TEST_FFID) + 1, log);
  dData = new BytesMsgField("", TEST_DATA, strlen(TEST_DATA) + 1, log);
  keywords = new Keywords(log);
  keywords->AddKeyword(TEST_KEYWORD1);
  keywords->AddKeyword(TEST_KEYWORD2);
  keywords->AddKeyword(TEST_KEYWORD3);
  keywords->AddKeyword(TEST_KEYWORD4);
  dKeywords = new BytesMsgField("", NULL, 0, log);
  dKeywords = keywords->StoreToMsgField();
  DELETE(keywords);
  
    // encrypt test data
  cipherer->EncryptWithPublicKey(tcbPubKey, dFFID, &eFFID);
  cipherer->EncryptWithPublicKey(tcbPubKey, dKeywords, &eKeywords);
  cipherer->EncryptWithPublicKey(tcbPubKey, dData, &eData);

  if(eFFID && eKeywords && eData)
    log->WriteString(llDebug, "Encryption OK");
  else
  {
    log->WriteString(llError, "Encryption failed");
    return -1;
  }

    // create GMessage with test data
  GMessage *gmSaveFile = new GMessage(log);
  result  = gmSaveFile->SetAsID(GM_ACID, acid);
  result  = gmSaveFile->SetAsID(GM_FID,  1);
  result += gmSaveFile->SetAsMsgField(GM_FFID, eFFID);
  result += gmSaveFile->SetAsMsgField(GM_KEYWORDS, eKeywords);
  result += gmSaveFile->SetAsMsgField(GM_DATA, eData);

  if(result == 5*OK)
    log->WriteString(llDebug, "gmSaveFile created");
  else
  {
    log->WriteString(llError, "gmSaveFile cannot be created");
    return -1;
  }
  
   // save it to a file
  if(gmSaveFile->StoreToFile(TEST_SAVE_FILE_FILE_NAME) == OK)
    log->WriteString(llDebug, "gmSaveFile stored");
  else
    log->WriteString(llError, "gmSaveFile cannot be stored");
  
  DELETE(gmSaveFile);
  DELETE(dFFID);
  DELETE(eFFID);
  DELETE(dKeywords);
  DELETE(eKeywords);
  DELETE(dData);
  DELETE(eData);
  DELETE(cipherer);
  DELETE(tcb);
  DELETE(log);
  
  return 0;
}
