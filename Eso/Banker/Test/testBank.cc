#include "./../../../Common/LogFile/H/logFile.h"
#include "./../H/banker.h"

#define BANKER_LOG "banker.log"

extern GMessage *Prepare_TOCurrPay_Input(LogFile *lf=NULL);
extern GMessage *Prepare_TOPayment_Input(LogFile *lf=NULL);
extern GMessage *Prepare_Chall_Input(LogFile *lf=NULL);
extern GMessage *Prepare_Answ_Input(LogFile *lf=NULL);

int main()
{ 
  LogFile *lf = NULL;
  Banker *banker = NULL;
  RelTime time;
  MsgField *msgF = NULL;
  GMessage *gmIn = NULL, *gm = NULL;
  
  lf = new LogFile(BASIC_PATH""BANKER_FILES_PATH""BANKER_LOG);
  if( (banker = new Banker(lf)) == NULL ) return KO;

  time.SetTime(3,0);
  
  msgF = banker->MakePayPlan(1024, time, 11);
  printf("0) msgF filled\n");
  if( msgF != NULL ){
    //pro ladeni klienta:
    (void)msgF->GetAsFile("proKlienta.msgF");
    delete msgF;
  }
  banker->SchedulePays(11, 333);
/*  
  gmIn = Prepare_TOCurrPay_Input(lf);
  gm = banker->TimeOutPayment(gmIn);
  if( gmIn ) delete gmIn;
  printf("TimeOutPayment\n");
  if( gm != NULL ){ gm->DebugHeaders(lf); delete gm;}
*/
  gmIn = Prepare_TOPayment_Input(lf);
  gm = banker->TimeForPayment(gmIn);
  if( gmIn ) delete gmIn;
  printf("TimeForPayment\n");
  if( gm != NULL ){ gm->DebugHeaders(lf); delete gm;}
/*
  gmIn = Prepare_TOCurrPay_Input(lf);
  gm = banker->TimeOutPayment(gmIn);
  if( gmIn ) delete gmIn;
  printf("TimeOutPayment\n");
  if( gm != NULL ){ gm->DebugHeaders(lf); delete gm;}
*/
  gmIn = Prepare_Chall_Input(lf);
  gm = banker->ChallengeFromBank(gmIn);
  if( gmIn ) delete gmIn;
  printf("ChallengeFromBank\n");
  if( gm != NULL ){ 
    gm->DebugHeaders(lf); 
    gm->StoreToFile("proBanku.gm");
    delete gm;
  }
/*
  gmIn = Prepare_TOCurrPay_Input(lf);
  gm = banker->TimeOutPayment(gmIn);
  if( gmIn ) delete gmIn;
  printf("TimeOutPayment\n");
  if( gm != NULL ){ gm->DebugHeaders(lf); delete gm;}
*/
  gmIn = Prepare_Answ_Input(lf);
  banker->AnswerFromBank(gmIn);
  if( gmIn ) delete gmIn;
  printf("AnswerFromBank\n");
/*
  gmIn = Prepare_TOCurrPay_Input(lf);
  gm = banker->TimeOutPayment(gmIn);
  if( gmIn ) delete gmIn;
  printf("TimeOutPayment\n");
  if( gm != NULL ){ gm->DebugHeaders(lf); delete gm;}
*/
/*
  msgF = banker->MakePayPlan(1024, time, 13);
  printf("1) msgF filled\n");
  if( msgF != NULL ) delete msgF;
  banker->Rollback(13);

  msgF = banker->MakePayPlan(1024, time, 15);
  printf("2) msgF filled\n");
  if( msgF != NULL ) delete msgF;
  banker->DelPayPlan(15);

  msgF = banker->MakePayPlan(1024, time, 17);
  printf("3) msgF filled\n");
  if( msgF != NULL ) delete msgF;
  banker->SchedulePays(17, 117);
  
  msgF = banker->MakePayPlan(1024, time, 19);
  printf("4) msgF filled\n");
  if( msgF != NULL ) delete msgF;
  banker->SchedulePays(19, 119);
  
  banker->DeleteRecsFileID(119);
*/  
  delete banker;
  delete lf;
  return OK;
}
