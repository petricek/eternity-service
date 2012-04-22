
#include <stdio.h>
#include "../H/stable.h"

void PrintRecord(Record *r)
{
 Char *s1, *s2;
  if(r){
    s1 = r->GetField("Time")->Print();
    s2 = r->GetField("String")->Print();   
    if (s2 == NULL) s2 = "NULL";
    printf("Time: %s, String: %s\n", s1, s2);
    delete s1;
    delete s2;
  }
  else
    printf("NULL\n");
}


void PrintTable(Table *t)
{
  Record *r;
  printf("Retrieving records...\n");
  r = t->First();
  while( r != NULL )
  {
    PrintRecord(r);
    delete r;
    r = t->Next();
  }
}

int main()
{
  TableFieldQueue *fq;
  Record *r;
  TimeSortedTable *t;
  ESTime i, search, ipuv;
  RelTime rt;

  printf("\n\nInitializing...\n");


  fq = new TableFieldQueue();
  fq->Insert( new TimeField("Time") );
  fq->Insert( new StringField("String") );

  t = new TimeSortedTable("stable.tx", fq);

  printf("Opening table...\n");

  t->Open();

  r = t->NewRecord();

  printf("Inserting records...\n");

  rt.SetTime(0, 0, 3);
  i.SetTime(rt);
  ipuv = i;
  r->SetField("Time", &i);
  r->SetField("String", "jedna");
  t->InsertRecord( r );

  rt.SetTime(0, 0, 1);
  i.SetTime(rt);
  r->SetField("Time", &i);
  r->SetField("String", "dva");
  t->InsertRecord( r );

  rt.SetTime(0, 0, 2);
  i.SetTime(rt);
  r->SetField("Time", &i);
  r->SetField("String", "tri");
  t->InsertRecord( r );

  delete r;
  
  PrintTable(t);

  printf("Closing table...\n");

  t->Close();

  delete fq;
  delete t;

  return 0;
}
