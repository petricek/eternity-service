#include "./../H/tcbWrapper.h"
#include "./test.h"
#include "./../../../Common/H/labels.h"

  
int main()
{
  LogFile *log;
  TCBWrapper *tcb;
  Cipherer *cipherer;
  
  log = new LogFile("testFAM.log");
  tcb = new SWTCBWrapper(log);
  cipherer = new Cipherer(log);
  
    // load GMessage prepared by ./prepSaveFile
  GMessage *gmSaveFile = new GMessage(TEST_SAVE_FILE_FILE_NAME, log);
  gmSaveFile->DebugHeaders(log, "gmSaveFile");
  
    // get fields of that GM
  ACID acid;
  Err result = gmSaveFile->GetAsID(GM_ACID, &acid);
  if(result == OK)
    log->WriteString(llDebug, "gmSaveFile parsed");
  else
  {
    log->WriteString(llError, "gmSaveFile cannot be parsed");
    DELETE(tcb);
    DELETE(log);
    return -1;
  }

    // get tcbPubKey
  GMessage *tcbPubKey;
  if(tcb->GetACPubKeys(acid, NULL, &tcbPubKey) != OK)
  {
    log->WriteString(llError, "cannot get tcbPubKey");
    DELETE(tcb);
    DELETE(log);
    return -1;
  }

    // generate client's key pair
  printf("This will take a while... (generating asymetric key)\n");
  GMessage *clientPubKey, *clientPrivKey;
  GMessage *clientKeyPair = cipherer->GenerateAsymetricKey();
  clientPubKey  = clientKeyPair->GetAsGMessage(GM_PUBLIC_KEY_INFO);
  clientPrivKey = clientKeyPair->GetAsGMessage(GM_PRIVATE_KEY_INFO); 
  if(!clientPubKey || !clientPrivKey)
  {
    log->WriteString(llError, "Unsuccessful getting client key pair");
    DELETE(tcb);
    DELETE(log);
    return -1;
  }
  DELETE(clientKeyPair);

    // encrypt keyword to search by
  GMessage *fam = new GMessage();
  fam->SetAsString(GM_KEYWORDS, TEST_KEYWORD2);
  MsgField *dKwd = fam->StoreToMsgField();
  MsgField *eKwd;
  cipherer->EncryptWithPublicKey(tcbPubKey, dKwd, &eKwd);
  DELETE(dKwd);
  
    // call GetFileByFinderFID
  Queue *eAnsQueue = tcb->GetFileHeadersByFAM(acid, eKwd, clientPubKey);
  if(eAnsQueue != NULL)
    log->WriteString(llDebug, "GetFileHeadersByFAM OK");
  else
  {
    log->WriteString(llError, "GetFileHeadersByFAM failed");
    DELETE(tcb);
    DELETE(log);
    return -1;
  }
  DELETE(eKwd);

    // decrypt headers by clientPrivKey and print them
  Size size;
  MsgField *dFFID, *eFFID, *dSize, *eSize;
  GMessage *eHeader = (GMessage*)eAnsQueue->First();
  log->WriteString(llDebug, "** Answer headers **");
  while(eHeader != NULL)
  {
      // get encrypted fields
    eFFID = eHeader->GetAsMsgField(GM_FFID);
    eSize = eHeader->GetAsMsgField(GM_FILE_SIZE);
    
      // decrypt them
    if(cipherer->DecryptWithPrivateKey(clientPrivKey, eFFID, &dFFID) != OK)
    {
      log->WriteString(llError, "Cannot decrypt eFFID");
      DELETE(eFFID);
      DELETE(eSize);
      DELETE(eHeader);
      continue;
    }
    if(cipherer->DecryptWithPrivateKey(clientPrivKey, eSize, &dSize) != OK)
    {
      log->WriteString(llError, "Cannot decrypt eSize");
      DELETE(eFFID);
      DELETE(dFFID);
      DELETE(eSize);
      DELETE(eHeader);
      continue;
    }
    
      // print decrypted values to log
    Void *tmp;
    log->WriteString(llDebug, "Header:");
    log->WriteString(llDebug, "  1) ffid: %s", (char *)(tmp = dFFID->GetAsBytes(&size)));
    DELETE(tmp);
    log->WriteString(llDebug, "  2) size: %ld", *(Size *)(tmp = dSize->GetAsBytes(&size)));
    DELETE(tmp);
    DELETE(eFFID);
    DELETE(dFFID);
    DELETE(eSize);
    DELETE(dSize);
    DELETE(eHeader);

      // get next header
    eHeader = (GMessage *)eAnsQueue->Next();
  }
  log->WriteString(llDebug, "** End of answer headers **");
    
  DELETE(clientPubKey);
  DELETE(clientPrivKey);
  DELETE(cipherer);
  DELETE(tcb);
  DELETE(log);
  
  return 0;
}
