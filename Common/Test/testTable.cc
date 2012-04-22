// *************************************************************************
//
// File: testTable.cc
// Name: Table test
// Author: Pavel
// Last Modified: 15.11.1998 M
//
// Description: This file illustrates usage of the Table class.
// It works as follows:
// 1) Initialization
//      - creates a FieldQueue to be used as a definition of the Table's
//        structure
//      - fills the FieldQueue with 2 fields
//      - creates a Table object using previously created FieldQueue
// 2) Opens the Table
//      - if no Table of such name exists, it creates a new one
// 3} Insertion of records
//      - creates new Record object using Table
//      - several times
//        - fills the Record with values
//        - inserts the modified Record into the Table
//      - disposes the Record object
// 4) Prints the contents of the Table
//      - uses First, Next of the Table
//      - be CAREFUL about deletes
//      - the Print method (Record) is used only for dubugging, do NOT use
//        it for other purposes
// 5) Updates all records with "1" in Integer Field
//      - uses FindFirst/FindNext and UpdateRecord of the Table
//      - CAREFUL about disposing off the Records (deletes) (see table.h)
// 6) Prints the contents of the Table
// 7) Double update of the first Record
//      - to show ability of multiple Updates of one Record being applied
//        one after another
// 8) Prints the contents of the Table
// 9) Deletes the second and the third Record
//      - to show that DeleteRecord may be followed by another DeleteRecord
//        with at least one First/Next in between
// 10) Prints the contents of the Table
// 11) Closes the Table
// *************************************************************************

#include <stdio.h>
#include "./../GMessage/H/table.h"
#include "./../GMessage/H/gMessage.h"

void PrintRecord(Record *r, LogFile *log)
{
 Char *s1, *s2;
 GMessage *gm;
  if(r){
    s1 = r->GetField("Integer")->Print();
    s2 = r->GetField("String")->Print();
    gm = ((BytesField *)r->GetField("Bytes"))->GetAsGMessage();
    log->WriteString(llDebug, "Integer: %s, String: %s", s1, s2);
    if(gm != NULL)
    {
      gm->DebugHeaders(log);
      DELETE(gm);
    }
    else
      log->WriteString(llDebug, "GMessage empty");
    DELETE(s1);
    DELETE(s2);
  }
  else
    log->WriteString(llDebug, "NULL");
}


void PrintTable(Table *t, LogFile *log)
{
  Record *r;
  log->WriteString(llDebug, "Retrieving records");
  r = t->First();
  while( r != NULL )
  {
    PrintRecord(r, log);
    DELETE(r);
    r = t->Next();
  }
//  getch();
}

int main()
{
  TableFieldQueue *fq;
  Record *r;
  Table *t;
  Int i, search;
  LogFile *log = new LogFile("testTable.log");
  GMessage *gm = new GMessage(log);

  printf("\nAll output goes to testTable.log\n");
  log->WriteString(llDebug, "Initializing");

  fq = new TableFieldQueue(log);
  fq->Insert( new IntField("Integer") );
  fq->Insert( new StringField("String") );
  fq->Insert( new BytesField("Bytes") );

  t = new Table("testTable.tab", fq, log);
  DELETE(fq);

  log->WriteString(llDebug, "Opening table");

  t->Open();

  log->WriteString(llDebug, "Inserting records");

  r = t->NewRecord();

  i = 1;
  gm->SetAsString("gmString", "ahoj");
  gm->SetAsInt("gmInt", 10);
  r->SetField("Integer", &i);
  r->SetField("String", "jedna");
  r->SetField("Bytes", NULL);
  t->InsertRecord( r );
 
  i = 2;
  r->SetField("Integer", &i);
  r->SetField("String", "dve");
  ((BytesField *)r->GetField("Bytes"))->SetAsGMessage(gm);
  t->InsertRecord( r );

  i = 3;
  r->SetField("Integer", &i);
  r->SetField("String", "tri");
  r->SetField("Bytes", NULL);
  t->InsertRecord( r );

  i = 4;
  r->SetField("Integer", &i);
  r->SetField("String", "ctyri");
  r->SetField("Bytes", NULL);
  t->InsertRecord( r );

  i = 1;
  r->SetField("Integer", &i);
  r->SetField("String", "jedna");
  r->SetField("Bytes", NULL);
  t->InsertRecord( r );

  DELETE(gm);
  DELETE(r);

//  PrintTable(t, log);

  log->WriteString(llDebug, "Updating fields with \"1\" in Integer field to <100, sto>");

    // !! POZOR "search" se nesmi behem vyhledavani zmenit, hledala by se
    // jeho nova hodnota.
  search = 1;
  i = 100;
  r = t->FindFirst("Integer", &search);
  while(r != NULL)
  {
    r->SetField("Integer", &i);
    r->SetField("String", "JEDNA");
    t->UpdateRecord(r);
    DELETE(r);
    r = t->FindNext();
  }

//  PrintTable(t, log);

  log->WriteString(llDebug, "Updating the second record");

  DELETE(t)->First();
  r = t->Next();
  i = 100;
  if(r)
  {
    r->SetField("Integer", &i);
    r->SetField("String", "STO");
    if(t->UpdateRecord(r) == OK)
      log->WriteString(llDebug, "OK");
    else
      log->WriteString(llDebug, "FAILED");
  }

  log->WriteString(llDebug, "Immediate update of the same record");

  i = 102;
  if(r)
  {
    r->SetField("Integer", &i);
    r->SetField("String", "STODVA");
    if(t->UpdateRecord(r) == OK)
      log->WriteString(llDebug, "OK");
    else
      log->WriteString(llDebug, "FAILED");
    DELETE(r);
  }

//  PrintTable(t, log);

  log->WriteString(llDebug, "Deleting the second & the third record");

  DELETE(t)->First();
  DELETE(t)->Next();
  if(t->DeleteRecord() == OK)
    log->WriteString(llDebug, "OK");
  else
    log->WriteString(llDebug, "KO");

  DELETE(t)->Next();
  if(t->DeleteRecord() == OK)
    log->WriteString(llDebug, "OK");
  else
    log->WriteString(llDebug, "KO");

//  PrintTable(t, log);

  log->WriteString(llDebug, "Closing table");

  t->Close();

  DELETE(t);
  DELETE(log);

  return 0;
}
