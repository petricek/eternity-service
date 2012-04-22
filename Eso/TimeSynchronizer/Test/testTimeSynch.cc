
#include <stdio.h>
#include <unistd.h>
#include "../H/timesynch.h"

/*
void PrintRecord(Record *r)
{
 Char *s1, *s2;
  if(r){
    s1 = r->GetField(SCH_TIME)->Print();
    s2 = r->GetField(SCH_COMMAND)->Print();   
    if (s2 == NULL) s2 = "NULL";
    printf("Time: %s, Command: %s\n", s1, s2);
    delete s1;
    delete s2;
  }
  else
    printf("NULL\n");
}


void PrintTable(Table *t)
{
  Record *r;
  printf("Vypis...\n");
  r = t->First();
  while( r != NULL )
  {
    PrintRecord(r);
    delete r;
    r = t->Next();
  }
}
*/

int main() {
  LogFile *log = NULL;
  MessageQueue *msgQ = NULL;
  Killer *killer = NULL;
  Scheduler *sched = NULL;
  Majordomo *majDom = NULL;
  ACManager *acMgr = NULL;
  
/*/
  ESTime est;
  RelTime rt;
  ESTime *pEST;
*/

  GMessage *test = NULL;
  GMessage *testAC = NULL;
  MsgField *testMF = NULL;

  TimeSynchronizer *ts = NULL;
  
  printf("Creating...\n");

  log = new LogFile(SCH_LOGFILE);
  msgQ = new MessageQueue(log);
  killer = new Killer(log);
  sched = new Scheduler(log, msgQ, killer);
  majDom = new Majordomo(NULL, log);

  test = new GMessage();
  test->SetAsInt(TS_TYPE, TS_TYPE_OF_SYNCHRONIZATION);
  ID id = 1;
  test->SetAsBytes(TS_ID, &id, sizeof(ID));

  testAC = new GMessage();
  testAC->SetAsString("Test","TestText");
  test->SetAsGMessage(TS_REPLY, testAC);
  delete testAC;

  testMF = test->StoreToMsgField();
  delete test;

  ts = new TimeSynchronizer();
  if (ts->Init(sched) == OK) printf("Init OK\n");
  else printf("Init KO\n");

//  ts->SetTimeOk(sched, false);
  if (ts->TS_Request(testMF, msgQ, majDom, sched) == OK) printf("TS_Request OK\n");
  else printf("TS_Request KO\n");
  if (ts->TS_Request(testMF, msgQ, majDom, sched) == OK) printf("TS_Request OK\n");
  else printf("TS_Request KO\n");
  if (ts->TS_Request(testMF, msgQ, majDom, sched) == OK) printf("TS_Request OK\n");
  else printf("TS_Request KO\n");
  delete testMF;

  test = new GMessage();
  id = 0;
  test->SetAsBytes(TS_ID, &id, sizeof(ID));

  testMF = test->StoreToMsgField();
  delete test;

  if (ts->TO_Response(testMF) == OK) printf("TO_Response OK\n");
  else printf("TO_Response KO\n");

  sleep(1);

  if (ts->TO_TimeToSynchronize(msgQ, majDom, acMgr, sched) == OK) printf("TimeToSynchronize OK\n");
  else printf("TimeToSynchronize KO\n");

  while (msgQ->MsgCount() > 0) {
    msgQ->GetMsg(&test);
    test->DebugHeaders(log);
    testMF = test->StoreToMsgField();
    
    if (ts->TS_Response(testMF, sched) == OK) printf("TS_Response OK\n");
    else printf("TS_Response KO\n");
    delete testMF;

    delete test;
  }
  

  printf("Deleting ...\n");

  delete ts;


  delete majDom;
  delete sched;
  delete killer;
  delete msgQ;
  delete log;

  printf("End\n");   
  return 0;
}

