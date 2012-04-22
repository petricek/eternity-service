#include "./../H/tcbWrapper.h"
#include "./test.h"
#include "./../../../Common/H/labels.h"

  
int main()
{
  LogFile *log;
  TCBWrapper *tcb;
  Cipherer *cipherer;
  
  log = new LogFile("testDecryptByACPriv.log");
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

    // get ACPubKey
  GMessage *acPubKey;
  if(tcb->GetACPubKeys(acid, &acPubKey) != OK)
  {
    log->WriteString(llError, "Cannot find ACPubKey");
    DELETE(tcb);
    DELETE(log);
    return -1;
  }
  else
    log->WriteString(llDebug, "ACPubKey found");
  
    // encrypt with it the test data
  MsgField *eData;
  MsgField *data = new BytesMsgField("", TEST_DATA, strlen(TEST_DATA) + 1);
  if(cipherer->EncryptWithPublicKey(acPubKey, data, &eData) != OK)
  {
    log->WriteString(llError, "Cannot encrypt test data");
    DELETE(tcb);
    DELETE(log);
    return -1;
  }
  else
    log->WriteString(llDebug, "Test data encrypted");
  
    // call DecryptByACPriv to decrypt the test data
  MsgField *dData = tcb->DecryptByACPriv(acid, eData);
  if(dData == NULL)
  {
    log->WriteString(llError, "DecryptByACPriv failed");
    DELETE(tcb);
    DELETE(log);
    return -1;
  }
  else
    log->WriteString(llDebug, "DecryptByACPriv OK");
  
    // compare the result with test data
  Char *tmp = dData->GetAsString();
  if(strcmp(TEST_DATA, tmp) == 0)
    log->WriteString(llDebug, "Data MATCH");
  else
    log->WriteString(llError, "Data DO NOT MATCH: %s", tmp);

  DELETE(tmp);
  DELETE(eData);
  DELETE(dData);
  DELETE(data);
  DELETE(acPubKey);
  DELETE(cipherer);
  DELETE(tcb);
  DELETE(log);
  
  return 0;
}
