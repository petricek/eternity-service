// *************************************************************************
//
// File: stable.cc 
// Name: TimeSortedTable 
// Author: Jirka
// Last Modified: 10.1.1998 
// 
// Description: This file contains the TimeSortedTable class which expands
//  class Table about sorted InsertRecord by ESTime
// Condition: The first field must be TimeField
// *************************************************************************

#include "./H/stable.h"
#include <stdio.h>

//#define TST_TIME_FIELD 1
#define TST_TIME_FIELD "Time"

// ------------------------------------------------------------------------
// TimeSortedTable::InsertRecord
// 
// insert the specified record to the table sorted by ESTime
// ------------------------------------------------------------------------
Err TimeSortedTable::InsertRecord(Record *aRecord)
{
  Table *tempTable;
  File *tempFile;
  Char *tempName;
  Size pos, oldRecLength;
  Record *r;  
  ESTime aTime1, aTime2;

  ((BasicTime*) &aTime1)->SetTime( ((TimeField*)aRecord->GetField(TST_TIME_FIELD))->GetESTimeValue().GetTime() );
  r = First();
  while (r != NULL) {
    ((BasicTime*)&aTime2)->SetTime( ((TimeField*)r->GetField(TST_TIME_FIELD))->GetESTimeValue().GetTime() );
    if ( (aTime1) >= (aTime2) ) {
      DELETE(r);
      r = Next();
    }
    else {
      DELETE(r);
      break;
    }
  }
  
  // 1) insert at the ending of table
  if (r == NULL) return Table::InsertRecord(aRecord);
  
  // 2) others possibilities
  
  // remember the beginning of the old record
  oldRecLength = lastRecordLength + SIZE_OF_HEADER;

  // remember the beginning of the old record
  oldRecLength = lastRecordLength + SIZE_OF_HEADER;
  pos = file->Tell() - oldRecLength;

  // copy the current file to temp file (from start to "pos")
  tempName = CreateTempFileName();
  if(tempName == NULL)
  return KO;

  tempFile = new File(tempName, "w+b");
  tempFile->Open();
  file->CopyStartToPos(tempFile, pos);
  DELETE(tempFile);

  // to the place of the old record write the new one
  tempTable = new Table(tempName, definition);
  tempTable->Open();
  tempTable->InsertRecord(aRecord);
  DELETE(tempTable);

  // copy the rest of "file" skipping the old record
  file->SeekCurrent();
  tempFile = new File(tempName, "a+b");
  tempFile->Open();
  file->AppendTheRestTo(tempFile);
  file->Close();
  tempFile->Close();

  // rename TempFile
  tempFile->Rename(file->GetName());
  DELETE(tempFile);
  DELETE(tempName);
  file->Open();

  // seek the end of the newly created record
  // (size of the header is already included in a call to SizeOf()
  file->SeekStart(pos + aRecord->SizeOf());

  return OK;
}

