// *************************************************************************
// 
// File: schedule.cc
// Name: Scheduler
// Author: Jirka
// Last Modified: 11.1.1999
// 
// Description: This file implements class Schedeler. 
//
// *************************************************************************

#include "./H/scheduler.h"
//#include "./H/squeue.h"
#include <unistd.h>
#include <stdio.h>

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
Scheduler::Scheduler(LogFile *aLogFile,
                     Eso *eso)
  : Runable(aLogFile), SixOffspring(eso), Shutdownable(eso->killer)
{
  Record *r;
  int ok;
  char *tmp;

  shutdown = false;
  
  defTbl = new TableFieldQueue();
  defTbl->Insert( new TimeField(SCH_TIME));  
  defTbl->Insert( new StringField(SCH_COMMAND));  
  defTbl->Insert( new BytesField(SCH_INFO));  
  tbl = new TimeSortedTable(tmp = eso->GetPath(SCH_PATH""SCH_TABLE), defTbl);
  DELETE(tmp);

  defTblTimeOk = new TableFieldQueue();
  defTblTimeOk->Insert( new IntField(SCH_TIMEOK));  
  tblTimeOk = new Table(tmp = eso->GetPath(SCH_PATH""SCH_TABLE_TIMEOK), defTblTimeOk);
  DELETE(tmp);

  tblTimeOk->Open();
  r = tblTimeOk->First();
  if (r == NULL) {
    ok = 1;
    r = tblTimeOk->NewRecord();
    r->SetField(SCH_TIMEOK, &ok);
    tblTimeOk->InsertRecord( r );
    TimeOk = true;
  }
  else {
    ok = ((IntField*)r->GetField(SCH_TIMEOK))->GetIntValue();
    if (ok == 1) TimeOk = true;
    else TimeOk = false;
  }
  tblTimeOk->Close();
  
  if (pthread_mutex_init(&mutex, pthread_mutexattr_default) == -1)
    WriteString(llError, __FILE__ "%d: Error init mutex", __LINE__);

  if (pthread_mutex_init(&mutexTbl, pthread_mutexattr_default) == -1)
    WriteString(llError, __FILE__ "%d: Error init mutexTbl", __LINE__);
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
Scheduler::~Scheduler() {
  DELETE(defTbl);
  DELETE(tbl);
  DELETE(defTblTimeOk);
  DELETE(tblTimeOk);
  if (pthread_mutex_destroy(&mutex) == -1)
    WriteString(llError, __FILE__ "%d: Error destroy mutex", __LINE__);
  if (pthread_mutex_destroy(&mutexTbl) == -1)
    WriteString(llError, __FILE__ "%d: Error destroy mutexTbl", __LINE__);
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
bool Scheduler::GetTimeOk() {
  bool ret;
  if (pthread_mutex_lock(&mutex) == -1) {
    WriteString(llError, __FILE__ "%d: Error lock mutex", __LINE__);
    return false;
  }
  ret = TimeOk;    
  if (pthread_mutex_unlock(&mutex) == -1) {
    WriteString(llError, __FILE__ "%d: Error unlock mutex", __LINE__);
    return false;
  }
  return ret;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
Err Scheduler::SetTimeOk(bool state) {
  Record *r;
  int ok;
  
  if (pthread_mutex_lock(&mutex) == -1) {
    WriteString(llError, __FILE__ "%d: Error lock mutex", __LINE__);
    return KO;
  }

  if (state != TimeOk) {
    if (tblTimeOk->Open() != OK) return KO;
      r = tblTimeOk->First();
      if (r == NULL) { // throw runtime_error("blank table TimeOk");
        tblTimeOk->Close();
	return KO;
      }
      if (state) ok = 1;
      else ok = 0;
      r->SetField(SCH_TIMEOK, &ok);
      if (tblTimeOk->UpdateRecord( r ) != OK) { 
        DELETE(r);
        tblTimeOk->Close();
	return KO;
      }
      TimeOk = state;
    DELETE(r);
    tblTimeOk->Close();
  }
  
  if (pthread_mutex_unlock(&mutex) == -1) {
    WriteString(llError, __FILE__ "%d: Error unlock mutex", __LINE__);
    return KO;
  }
  return OK;
}


//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
ESTime* Scheduler::GetTime() {
  if (GetTimeOk() == true) return new ESTime();
  else return NULL;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
bool Scheduler::IsSetTimeOut(char *command) {
  Record *r;
  
  if (tbl == NULL) {
    WriteString(llError, __FILE__ "table null pointer", __LINE__);
    return false;
  }

  tbl->Open();
  if (pthread_mutex_lock(&mutexTbl) == -1) {
    WriteString(llError, __FILE__ "%d: Error lock mutexTbl", __LINE__);
    return false;
  }

  r = tbl->FindFirst(SCH_COMMAND, command);

  if (pthread_mutex_unlock(&mutexTbl) == -1) {
    WriteString(llError, __FILE__ "%d: Error unlock mutexTbl", __LINE__);
    return false;
  }
  tbl->Close();

  if (r != NULL) {
    DELETE(r);
    return true;
  }
  else return false;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
Err Scheduler::CommonSetTimeOut(char *command, GMessage *info, ESTime *pTime) {
  Record *r;
  char *auxStr;
  
  if (tbl == NULL) return KO;
  r = tbl->NewRecord();
  if (r == NULL) return KO;
  r->SetField(SCH_TIME, pTime);
  r->SetField(SCH_COMMAND, command);
  ((BytesField*)r->GetField(SCH_INFO))->SetAsGMessage(info);
  tbl->Open();
  if (pthread_mutex_lock(&mutexTbl) == -1) {
    WriteString(llError, __FILE__ "%d: Error lock mutexTbl", __LINE__);
    return KO;
  }

  tbl->InsertRecord( r );

  if (pthread_mutex_unlock(&mutexTbl) == -1) {
    WriteString(llError, __FILE__ "%d: Error unlock mutexTbl", __LINE__);
    return KO;
  }
  tbl->Close();

  WriteString(llDebug, "Ins c: %s, t: %s", command, auxStr = pTime->Print());
  DELETE(auxStr);

  DELETE(r);
  return OK;
}
  
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
Err Scheduler::SetTimeOut(char *command, GMessage *info, RelTime time) {
  ESTime *pTime = new ESTime( time );
  if (pTime == NULL) return KO;
  if (CommonSetTimeOut(command, info, pTime) != OK) {
    DELETE(pTime);
    return KO;
  }
  DELETE(pTime);
  return OK;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
Err Scheduler::SetTimeOut(char *command, GMessage *info, ESTime time) {
  ESTime *pTime = &time;
  if (CommonSetTimeOut(command, info, pTime) != OK) {
    return KO;
  }
  return OK;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void* Scheduler::Run(void*) {
  //int i = 0;
  for(;;) {
    //if (!shutdown && (i < 3)) 
    if (!shutdown) {
      ProcessTimeOut();
      sleep(SCH_TIMEOUT);
      //i++;
    }
    else {
      //if (i == 3) return NULL;
      exit(0);
    }
  }
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void Scheduler::ProcessTimeOut() {
  Record *r;
  ESTime *pTime1, aTime2;
  GMessage *gm, *info;
  char *command;
  char *auxStr;
  
  WriteString(llDebug, "Sch: ProcessTimeOut start"); 
  pTime1 = GetTime();
  if (pTime1 == NULL) {
    WriteString(llError, __FILE__ "%d: WARNING: TIME IS BAD", __LINE__);
    pTime1 = new ESTime();
  }
  //some control of null pointer
  if (tbl == NULL) {
    WriteString(llError, __FILE__ "%d: null pointer", __LINE__);
    return;
  }
  tbl->Open();
  r = tbl->First();
  while (r != NULL) {
  
    WriteString(llDebug, "processTimeOut in while cycle");
  
    //WriteString(llDebug, "second %ld", ((TimeField*)r->GetField(SCH_TIME))->GetESTimeValue().GetTime()); 
    aTime2 = ((TimeField*)r->GetField(SCH_TIME))->GetESTimeValue();
    WriteString(llDebug, "first %ld second %ld", pTime1->GetTime(), aTime2.GetTime());
    if ((*pTime1) < aTime2) {
      WriteString(llDebug, "Sch: now is no timeout");
      break;
    }

    //creating message about timeout
    command = ((StringField*)r->GetField(SCH_COMMAND))->AsString();
    info = ((BytesField*)r->GetField(SCH_INFO))->GetAsGMessage();
    
    gm = new GMessage();
    gm->SetAsString(GM_COMMAND, command);
/*    GMessage *gmData = new GMessage();
    gmData->SetAsGMessage(GM_DATA, info);
*/    gm->SetAsGMessage(GM_PARAMETERS, info);
    
    //HARD don't check if messageQueue is full
    //SOFT return QUEUE_FULL if messageQueue is full
    WriteString(llDebug, "vloz do fronty");
    six->receiverToMajordomo->Append(gm, HARD);
    WriteString(llDebug, "vlozeno");
    
    if (pthread_mutex_lock(&mutexTbl) == -1) {
      WriteString(llError, __FILE__ "%d: Error lock mutexTbl", __LINE__);
    }

    tbl->DeleteRecord();
  
    if (pthread_mutex_unlock(&mutexTbl) == -1) {
      WriteString(llError, __FILE__ "%d: Error unlock mutexTbl", __LINE__);
    }

    ESTime* pNow = new ESTime;
    WriteString(llDebug, "Del command: %s, time: %s", command, auxStr = pNow->Print());
    DELETE(auxStr);
    DELETE(pNow);
    
    DELETE(command);
    if (info != NULL) DELETE(info);
    //delete gm;

    DELETE(r);
    r = tbl->Next();
  }
  tbl->Close();
  if (r != NULL) {
    DELETE(r);
  }
  DELETE(pTime1);
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void Scheduler::Shutdown() {
  shutdown = true;
}
