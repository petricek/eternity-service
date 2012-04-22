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
  MsgField *eFFID = gmSaveFile->GetAsMsgField(GM_FFID);
  if(eFFID && (result == OK))
    log->WriteString(llDebug, "gmSaveFile parsed");
  else
  {
    log->WriteString(llError, "gmSaveFile cannot be parsed");
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
  
    // call GetFileByFinderFID
  MsgField *eData = tcb->GetFileByFinderFID(acid, eFFID, clientPubKey);
  if(eData != NULL)
    log->WriteString(llDebug, "GetFileByFinderFID OK (%d)", eData->SizeOf());
  else
  {
    log->WriteString(llError, "GetFileByFinderFID failed");
    DELETE(tcb);
    DELETE(log);
    return -1;
  }

    // decrypt eData v clientPrivKey
  MsgField *dData;
  if(cipherer->DecryptWithPrivateKey(clientPrivKey, eData, &dData) != OK)
  {
    log->WriteString(llError, "Cannot decrypt eData");
    DELETE(tcb);
    DELETE(log);
    return -1;
  }
    
    // compare it to TEST_DATA
  Char *data = dData->GetAsString();
  if(strcmp(data, TEST_DATA) == 0)
    log->WriteString(llDebug, "Tested data MATCH the data found");
  else
    log->WriteString(llError, "Tested data DO NOT match the data found: %s", data);
  
  DELETE(data);
  DELETE(clientPubKey);
  DELETE(clientPrivKey);
  DELETE(eFFID);
  DELETE(eData);
  DELETE(cipherer);
  DELETE(tcb);
  DELETE(log);
  
  return 0;
}
