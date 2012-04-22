#include "H/timesynch.h"
#include "H/squeue.h"

TimeSynchronizer::TimeSynchronizer(LogFile *aLogFile, Eso *eso): 
SixOffspring(eso), Debugable(aLogFile)
{
  char *tmp;

  numberItem = 0;
  ts_id = 0;
  tbl = NULL;
//-  enableSynch = false;
  enableSynch = true;
  defTbl = new TableFieldQueue();
  defTbl->Insert( new TimeField(TS_TIMECOLUMN) );
  idGen = new UniqueIDGenerator(tmp = eso->GetPath(TS_PATH""TS_GENERATOR));
  DELETE(tmp);
}

TimeSynchronizer::~TimeSynchronizer() {
  DELETE(defTbl);
  if (tbl != NULL) DELETE(tbl);
  DELETE(idGen);
}

Err TimeSynchronizer::Init() {
  RelTime rt;
  File *f;
  char *tmp;

  WriteString(TS_LOGLEVEL, "Init starting ...");
  if (!TS_SYNCHRONIZE_AT_START) rt.SetTime(TS_TIME_TO_SYNCHRONIZE);
//-  ((Eso *) six)->scheduler->SetTimeOut(TO_TIME_TO_SYNCHRONIZE, NULL, rt);

  //physical delete of table
  f = new File(tmp = ((Eso *) six)->GetPath(TS_PATH""TS_TABLE));
  DELETE(tmp);
  if (f == NULL) return KO;
  f->Remove();
  DELETE(f);

  WriteString(TS_LOGLEVEL, "Init ending ...");
  return OK;
} 

Err TimeSynchronizer::TO_TimeToSynchronize(){
  Queue *q = NULL, *myq = NULL;
  GMessage *gm = NULL, *msg = NULL, *myAC = NULL;
  int type = TS_TYPE_OF_SYNCHRONIZATION;
  RelTime rt;
  Majordomo *hMsg = (Majordomo *)((Eso *) six)->majordomo;
  Scheduler *sch = ((Eso *) six)->scheduler;

  WriteString(TS_LOGLEVEL, "TO_TimeToSynchronize starting ...");
  if (sch == NULL) return KO;
  if (hMsg == NULL) return KO;
  if (!sch->IsSetTimeOut(TO_TIME_TO_SYNCHRONIZE)) {
    WriteString(TS_LOGLEVEL, "TO_Tim: starting new synchronization");
    q = hMsg->GetACs(ACTYPE_ESO, TS_NUMBER_TO_SYNCHRONIZE);

    //geting of my AC
    myq = hMsg->GetACs(ACTYPE_THIS_SIX);
    if (myq == NULL) return KO;
    myAC = (GMessage*) myq->First();
    myAC = new GMessage(); //for testing
    if (myAC == NULL) return KO;
    DELETE(myq);

    // new synchronization has own unique ID
    idGen->GetID(&ts_id);  
    WriteString(TS_LOGLEVEL, "TO_Tim: ts_id = %d", ts_id);
    ESTime *auxTime = new ESTime();
    ((BasicTime*)&beginSynch)->SetTime( auxTime->GetTime() );
    DELETE(auxTime);
    char *str;
    WriteString(TS_LOGLEVEL, "TO_Tim: begTime = %s", str = beginSynch.Print());
    DELETE(str);
    enableSynch = true;
    
    msg = new GMessage();
    if (msg == NULL) return KO;
    msg->SetAsBytes(TS_ID, &ts_id, sizeof(ID));
    msg->SetAsInt(TS_TYPE, type);
    if (myAC == NULL) return KO;
    msg->SetAsGMessage(TS_REPLY, myAC);

    DELETE(myAC);

    if (q == NULL) return KO;
    gm = (GMessage*) q->First();
    while (gm != NULL) {
      hMsg->InsertToOutMsgQueue( hMsg->CreateSendDataMsg(CMD_SYNCHRONIZER_REQUEST, msg, gm) );
      //delete gm;
      gm = (GMessage*) q->Next();
    }
    
    DELETE(q);
    DELETE(msg);

    msg = new GMessage();
    if (msg == NULL) return KO;
    msg->SetAsBytes(TS_ID, &ts_id, sizeof(ID));

    rt.SetTime(TS_TIME_TO_RESPONSE);
    sch->SetTimeOut(TO_RESPONSE_TO_SYNCHRONIZE, msg, rt);

    DELETE(msg);
    
    rt.SetTime(TS_TIME_TO_SYNCHRONIZE);
    sch->SetTimeOut(TO_TIME_TO_SYNCHRONIZE, NULL, rt);
  }
  else
    WriteString(TS_LOGLEVEL, "TO_Tim: no new synchronization");

  WriteString(TS_LOGLEVEL, "TO_TimeToSynchronize ending ...");
  return OK;
}

Err TimeSynchronizer::TS_Request(MsgField *dataField) {
  GMessage *inMsg, *myAC;
  int type;
  long t = 0L;
  ESTime *pTime;
  Majordomo *hMsg = (Majordomo *)((Eso *) six)->majordomo;
  Scheduler *sch = ((Eso *) six)->scheduler;

  WriteString(TS_LOGLEVEL, "TS_Request starting ...");
  if (dataField == NULL) return KO;
  if (hMsg == NULL) return KO;
  if (sch == NULL) return KO;
  if ((inMsg = dataField->GetAsGMessage()) == NULL) return KO;

  inMsg->GetAsInt(TS_TYPE, &type);
  myAC = inMsg->GetAsGMessage(TS_REPLY);
  if (myAC == NULL) return KO;
  if (inMsg->Delete(TS_REPLY) == KO) return KO; 

  switch (type) {
    case TS_WITH_OTHER_ESOS:
      WriteString(TS_LOGLEVEL, "TS_Req: type of synch.: with other Esos");
      pTime = sch->GetTime();
      if (pTime != NULL) {
        char *str;
        WriteString(TS_LOGLEVEL, "TS_Req: Time: %s", str = pTime->Print());
	DELETE(str);
        t = pTime->GetTime();
        DELETE(pTime);
      }
    break;
    default:
      WriteString(TS_LOGLEVEL, "TS_Req: bad type of synchronization");
    break;
  }

  inMsg->SetAsBytes(TS_TIME, &t, sizeof(long));
  hMsg->InsertToOutMsgQueue( hMsg->CreateSendDataMsg(CMD_SYNCHRONIZER_RESPONSE, inMsg, myAC));
  //delete inMsg;

  DELETE(myAC);
  WriteString(TS_LOGLEVEL, "TS_Request ending ...");
  return OK;
}

Err TimeSynchronizer::TO_Response(MsgField *dataField){
  GMessage *inMsg;
  ID *pID;

  WriteString(TS_LOGLEVEL, "TO_Response starting ...");
  if (dataField == NULL) return KO;
  if ((inMsg = dataField->GetAsGMessage()) == NULL) return KO;

  pID = (ID *) inMsg->GetAsBytes(TS_ID);
  if (pID == NULL) return KO;
  if (*pID == ts_id && enableSynch) {
    if (tbl != NULL) {
      tbl->Remove();
      DELETE(tbl);
    }
    WriteString(TS_LOGLEVEL, "TO_Res: Synchro. was not ended! (ts_id: %ld)", ts_id);
    enableSynch = false;
    numberItem = 0;
  }

  DELETE(pID);
  DELETE(inMsg);
  WriteString(TS_LOGLEVEL, "TO_Response ending ...");
  return OK;
}

Err TimeSynchronizer::TS_Response(MsgField *dataField){
  GMessage *inMsg;
  ID *pID;
  ESTime *pTime;
  long *plong;
  TimeSortedQueue tsqueue;
  int type;
  Record *r;
  Scheduler *sch = ((Eso *) six)->scheduler;
  char *tmp;

  WriteString(TS_LOGLEVEL, "TS_Response starting ...");
  if (dataField == NULL) return KO;
  if (sch == NULL) return KO;
  if ((inMsg = dataField->GetAsGMessage()) == NULL) return KO;
  //inMsg->DebugHeaders(log);

  pID = (ID *) inMsg->GetAsBytes(TS_ID);
  if (pID == NULL) return KO;
  printf("ts_id = %ld, pID = %ld \n", ts_id, *pID); 
  if (*pID == ts_id && enableSynch) {
    inMsg->GetAsInt(TS_TYPE, &type);
    switch (type) {
      case TS_WITH_OTHER_ESOS:
        WriteString(TS_LOGLEVEL, "TS_Res: type of synch.: with_other_Esos");
        if (tbl == NULL) tbl = new Table(tmp = ((Six *) six)->GetPath(TS_PATH""TS_TABLE), defTbl);
        DELETE(tmp);
        numberItem++;

        plong = (long *) inMsg->GetAsBytes(TS_TIME);
        pTime = new ESTime();
        ((BasicTime*)pTime)->SetTime(*plong);
	char *str;
        WriteString(TS_LOGLEVEL, "TS_Res: other Eso: %s", str = pTime->Print());
	DELETE(str);
        DELETE(plong);

        r = tbl->NewRecord();
        r->SetField(TS_TIMECOLUMN, pTime);
        tbl->Open();
        tbl->InsertRecord( r );
        tbl->Close();
        DELETE(r);
        DELETE(pTime);

        if (numberItem == TS_NUMBER_TO_COMPUTE) {
          // computing of comparing time
          WriteString(TS_LOGLEVEL, "TS_Res: I start to compute divergence.");
          tbl->Open();
          r = tbl->First();
          while (r != NULL) {
	    pTime = new ESTime;
            (*pTime) = ((TimeField*)r->GetField(TS_TIMECOLUMN))->GetESTimeValue();
            tsqueue.Insert( pTime );   
            DELETE(r);
            r = tbl->Next();
          }   
          tbl->Close();
          tbl->Remove();
          DELETE(tbl);

          //printing of sorted queue
	  printf("Pred mazanim:\n");
	  tsqueue.Print(); 
          
          int i;
          for(i=0; i < TS_NUMBER_TO_DELETE; i++) {
            pTime = (ESTime*) tsqueue.DeleteAt(1);
            DELETE(pTime);
          }

          long average = 0; 
          RelTime rt; 
          for(i=0; i < (TS_NUMBER_TO_COMPUTE - 2*TS_NUMBER_TO_DELETE); i++) {
            if (i==0) pTime = (ESTime*) tsqueue.First();
            else pTime = (ESTime*) tsqueue.Next();
            if (*pTime > beginSynch) {
              rt = *pTime - beginSynch;
              average += rt.GetTime();
            }  
            else {
              rt = beginSynch - *pTime;
              average -= rt.GetTime();
	    }
	    //printf("rozdil: %ld\n", rt.GetTime());
	    ////char *str;
	    //printf("zac. synch.: %s\n", str = beginSynch.Print());
	    //DELETE(str);
	    //printf("jiny cas: %s\n", str = pTime->Print());
	    //DELETE(str);
          }

          //printing of sorted queue
	  printf("Po mazani:\n");
	  tsqueue.Print(); 
          
          for(i=0; i < TS_NUMBER_TO_COMPUTE - TS_NUMBER_TO_DELETE; i++) {
            pTime = (ESTime*) tsqueue.DeleteAt(1);
            DELETE(pTime);
          }
         
          WriteString(TS_LOGLEVEL, "TS_Res: Sum divergence is %ld sec", average);
          average /= (TS_NUMBER_TO_COMPUTE - 2*TS_NUMBER_TO_DELETE);
          WriteString(TS_LOGLEVEL, "TS_Res: Divergence is %ld sec", average);
      
          if ( (average > TS_TIME_TOLERANCE) || (average < -TS_TIME_TOLERANCE) ) {
            WriteString(TS_LOGLEVEL, "\n\n!!! TS_Res: Time of machine is bad, from this time I will not be delete files!!!\n");
            SetTimeOk(sch, false);        //nastavit spatny cas!!!
	  }
	  else {
            WriteString(TS_LOGLEVEL, "TS_Res: Time is OK!");
            SetTimeOk(sch, true);        //nastavit dobry cas!!!
	  }
          enableSynch = false;
          numberItem = 0;
        }
      break;
      default:
        WriteString(TS_LOGLEVEL, "TS_Res: type of synch.: undefined");
      break;
    }
  }

  DELETE(pID);
  DELETE(inMsg);
  WriteString(TS_LOGLEVEL, "TS_Response ending ...");
  return OK;
}

void TimeSynchronizer::SetTimeOk(Scheduler *sch, bool state) {
  sch->SetTimeOk(state);
}
