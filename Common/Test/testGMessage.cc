#include "../GMessage/H/gMessage.h"

void PrintResult(LogFile *log, Err result)
{
  log->WriteString(llDebug, "%s", (result == OK) ? "OK" : "KO");
}

void SetThreeFields(GMessage *gm, LogFile *log)
{
  Err result;

  log->WriteString(llDebug, "Setting fields as bytes...");
  result = gm->SetAsString("Prvni","Prvni");
  PrintResult(log, result);
/*  
  Int i = 2;
  result = gm->SetAsBytes("Druhy",&i,sizeof(i));
  PrintResult(log, result);
*/
  result = gm->SetAsString("Druhy",NULL);
  PrintResult(log, result);
  
  result = gm->SetAsString("Treti","");
  PrintResult(log, result);
    
  gm->DebugHeaders(log, "after 3 inserts:");
}


void main()
{
  GMessage *gm, *outerGM, *oneMoreGM;
  Int i = 2;
  Err result;
  Void *buff;
  LogFile *log;
  MsgField *aMsgField;

  log = new LogFile("testGMessage.log");

  log->WriteString(llDebug, "Creating new GMessage...");
  gm = new GMessage(log);

  SetThreeFields(gm, log);
  
  log->WriteString(llDebug, "Clearing...");
  gm->Clear();

  gm->DebugHeaders(log, "gm after Clear():");

  SetThreeFields(gm, log);
  
		// putting gm into outerGM as a nested message
  log->WriteString(llDebug, "Nesting the created GMessage...");
  outerGM = new GMessage(log);
  result = outerGM->SetAsGMessage("Body", gm);
  PrintResult(log, result);
//  DELETE(aBuffer);

  outerGM->DebugHeaders(log, "outerGM after nesting:");
/*
  log->WriteString(llDebug, "Storing to a file...");
  result = outerGM->StoreToFile("TestMsg.txt");
*/
  log->WriteString(llDebug, "Storing to a byte stream...");
  Size aSize;
  buff = outerGM->StoreToBytes(&aSize);
  result = buff == NULL ? KO : OK;
  PrintResult(log, result);
  
  oneMoreGM = new GMessage(gm);
  oneMoreGM->DebugHeaders(log, "gm copied to oneMoreGM:");
  DELETE(oneMoreGM);

  DELETE(outerGM);
  DELETE(gm);
  
//  outerGM = new GMessage("TestMsg.txt");
  outerGM = new GMessage(buff, aSize);
  DELETE(buff);
  
  gm = outerGM->GetAsGMessage("Body");

  outerGM->DebugHeaders(log, "outerGM read from a file:");
  gm->DebugHeaders(log, "gm after unnesting:");

  log->WriteString(llDebug, "Storing gm to a MsgField...");
  aMsgField = gm->StoreToMsgField();
  log->WriteString(llDebug, "%s", (aMsgField == NULL) ? "KO" : "OK");
  if(aMsgField != NULL)
    DELETE(aMsgField);

    // renaming
  log->WriteString(llDebug, "Renaming Druhy to Second...");
  result = gm->Rename("Druhy", "Second");
  PrintResult(log, result);

  gm->DebugHeaders(log, "gm after renaming:");

    // deleting
  log->WriteString(llDebug, "Deleting Prvni...");
  result = gm->Delete("Prvni");
  PrintResult(log, result);

  gm->DebugHeaders(log, "gm after DELETE(of) \"Prvni\":");

  char randPointer[512];
  gm->LoadFromBytes(randPointer, 1024);
  gm->DebugHeaders(log, "gm after load from nonsense bytes stream");

  DELETE(outerGM);

  DELETE(gm);
  DELETE(log);
}
