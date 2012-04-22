#include "./../H/acManager.h"

#define ACFLD_INFO "Info"
#define ACID1 129L
#define ACID2 139L



void PrintResult(ACManager *acMgr, char *str, Err result)
{
  if(acMgr == NULL)
    return;

  acMgr->WriteString(llDebug, "%s: %s", str, result == KO ? "KO" : "OK");
};



GMessage *CreateAC(ID acid, Char *text)
{
  GMessage *ac;
  ESTime *t;

  ac = new GMessage();
  ac->SetAsID(ACFLD_ACID, acid);
 
  t = new ESTime();
  ac->SetAsBasicTime(ACFLD_EXPIRATION, *t); 
  DELETE(t);

  ac->SetAsString(ACFLD_INFO, text);
  
  return ac;
}




void main()
{
  Err result;
  ACManager *acMgr;
  LogFile *logFile;
  GMessage *ac;


  logFile = new LogFile("acManagers.log");

  acMgr = new ACManager("bank", "banks.tbl", 50, 100, NULL, NULL, logFile);
  PrintResult(acMgr, "TestACManager: ACManager created", OK); 


  ac = CreateAC(ACID1, "ACID1 - 129L");
  PrintResult(acMgr, "TestACManager: ACCreated", OK);
  PrintResult(acMgr, "TestACManager: InsertAC result", acMgr->InsertAC(ac));
  DELETE(ac); 


  ac = CreateAC(ACID2, "ACID1 - 139L");
  PrintResult(acMgr, "TestACManager: ACCreated", OK);
  PrintResult(acMgr, "TestACManager: InsertAC result", acMgr->InsertAC(ac));

  ac = acMgr->GetACByACID(ACID1); 
  PrintResult(acMgr, "TestACManager: Got AC by ACID", OK );

  
  if (ac != NULL)
   ac->DebugHeaders(logFile, "Debugging got ac", llDebug);


 




DELETE(acMgr);
DELETE(logFile);
fprintf(stderr, "Ending program\n");

}
