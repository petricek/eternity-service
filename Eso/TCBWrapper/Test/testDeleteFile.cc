#include "./../H/tcbWrapper.h"
#include "./test.h"
#include "./../../../Common/H/labels.h"

  
int main()
{
  Err result2;
  LogFile *log;
  TCBWrapper *tcb;
  
  log = new LogFile("testDeleteFile.log");
  tcb = new SWTCBWrapper(log);
  
    // load GMessage prepared by ./prepSaveFile
  GMessage *gmSaveFile = new GMessage(TEST_SAVE_FILE_FILE_NAME, log);
  gmSaveFile->DebugHeaders(log, "gmSaveFile");
  
    // get fields of that GM
  ID   fid;
  result2 = gmSaveFile->GetAsID(GM_FID, &fid);
  if((result2 == OK))
    log->WriteString(llDebug, "gmSaveFile parsed");
  else
  {
    log->WriteString(llError, "gmSaveFile cannot be parsed");
    return -1;
  }
  
    // call DeleteFileByFID
  if(tcb->DeleteFileByFileID(fid) == OK)
    log->WriteString(llDebug, "DeleteFile OK");
  else
    log->WriteString(llError, "DeleteFile failed");
  
  DELETE(gmSaveFile);
  DELETE(tcb);
  DELETE(log);
  
  return 0;
}
