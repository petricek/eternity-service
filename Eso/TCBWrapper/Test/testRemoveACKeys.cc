#include "./../H/tcbWrapper.h"
#include "./test.h"
#include "./../../../Common/H/labels.h"

  
int main()
{
  LogFile *log;
  TCBWrapper *tcb;
  
  log = new LogFile("testRemoveACKeys.log");
  tcb = new SWTCBWrapper(log);
  
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

    // call RemoveACKeys
  if(tcb->RemoveACKeys(acid) == OK)
    log->WriteString(llDebug, "RemoveACKeys OK");
  else
  {
    log->WriteString(llError, "RemoveACKeys failed");
    DELETE(tcb);
    DELETE(log);
    return -1;
  }

  DELETE(tcb);
  DELETE(log);
  
  return 0;
}
