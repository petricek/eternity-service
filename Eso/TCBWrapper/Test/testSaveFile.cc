#include "./../H/tcbWrapper.h"
#include "./test.h"
#include "./../../../Common/H/labels.h"

  
int main()
{
  Err result, result2;
  LogFile *log;
  TCBWrapper *tcb;
  
  log = new LogFile("testSaveFile.log");
  tcb = new SWTCBWrapper(log);
  
    // load GMessage prepared by ./prepSaveFile
  GMessage *gmSaveFile = new GMessage(TEST_SAVE_FILE_FILE_NAME, log);
  gmSaveFile->DebugHeaders(log, "gmSaveFile");
  
    // get fields of that GM
  ACID acid;
  ID   fid;
  result = gmSaveFile->GetAsID(GM_ACID, &acid);
  result2 = gmSaveFile->GetAsID(GM_FID, &fid);
  MsgField *eFFID = gmSaveFile->GetAsMsgField(GM_FFID);
  MsgField *eKeywords = gmSaveFile->GetAsMsgField(GM_KEYWORDS);
  MsgField *eData = gmSaveFile->GetAsMsgField(GM_DATA);
  if((result == OK) && (result2 == OK) && eFFID && eKeywords && eData)
    log->WriteString(llDebug, "gmSaveFile parsed");
  else
  {
    log->WriteString(llError, "gmSaveFile cannot be parsed");
    return -1;
  }
  
    // call SaveFile
  if(tcb->SaveFile(acid, fid, eFFID, eKeywords, eData) == OK)
    log->WriteString(llDebug, "SaveFile OK");
  else
    log->WriteString(llError, "SaveFile failed");
  
  DELETE(gmSaveFile);
  DELETE(eFFID);
  DELETE(eKeywords);
  DELETE(eData);
  DELETE(tcb);
  DELETE(log);
  
  return 0;
}
