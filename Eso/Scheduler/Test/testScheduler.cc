
#include <stdio.h>
#include <unistd.h>
#include "../H/scheduler.h"

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


int main() {
  LogFile *log = NULL;
  MessageQueue *msgQ = NULL;
  Killer *killer = NULL;
  Scheduler *sched = NULL;
  ESTime est;
  RelTime rt;
  ESTime *pEST;
  GMessage *info = NULL;
  TimeSynchronizer *ts = NULL;
  
  printf("Creating...\n");
  log = new LogFile(SCH_LOGFILE);
  msgQ = new MessageQueue(log);
  killer = new Killer(log);
  sched = new Scheduler(log, msgQ, killer);

  pEST = sched->GetTime();
  if (pEST != NULL)
    printf("nyni je tento cas: %ld\n", pEST->GetTime());
  else printf("cas je NULL\n");
  delete pEST;
  
  rt.SetTime( 20 );
  sched->SetTimeOut("test1", NULL, rt);
  info = new GMessage();
  info->SetAsString("info", "testovaci info");
  rt.SetTime( 30 );
  sched->SetTimeOut("test3", info, rt);
  sched->SetTimeOut("test2", info, est);
  delete info;
  info = NULL;
  
  if (sched->IsSetTimeOut("test2")) 
    printf("Je nastaven command test2\n");
  else printf("Neni nastaven command test2\n");

  TableFieldQueue *fq;
  Table *t;

  fq = new TableFieldQueue();
  fq->Insert( new TimeField(SCH_TIME) );
  fq->Insert( new StringField(SCH_COMMAND) );
  fq->Insert( new BytesField(SCH_INFO) );

  t = new Table(SCH_TABLE, fq, log);

  t->Open();
  PrintTable(t);
  t->Close();

  delete fq;
  delete t;

  sleep(2);
  
  printf("zacina run\n");
  sched->Run(NULL);
  printf("konci run\n");

  printf("vypis msgQueue\n");
  msgQ->GetMsg(&info);
  
  char * str;
  printf("command %s\n", str = info->GetAsString(SCH_COMMAND));
  delete str;

  info->DebugHeaders(log);
  
  delete info;

  if (sched->IsSetTimeOut("test2")) 
    printf("Je nastaven command test2\n");
  else printf("Neni nastaven command test2\n");

  pEST = sched->GetTime();
  if (pEST != NULL)
    printf("nyni je tento cas: %ld\n", pEST->GetTime());
  else printf("cas je NULL\n");
  delete pEST;
  
  ts = new TimeSynchronizer();
  ts->SetTimeOk(sched, false);
  
  pEST = sched->GetTime();
  if (pEST != NULL)
    printf("nyni je tento cas: %ld\n", pEST->GetTime());
  else printf("cas je NULL\n");
  delete pEST;

  ts->SetTimeOk(sched, true);
  delete ts;

  delete sched;
  delete killer;
  delete msgQ;
  delete log;
  
  return 0;
}

