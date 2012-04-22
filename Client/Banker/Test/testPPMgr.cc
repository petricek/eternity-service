#include "./../H/pPlanMgr.h"

extern Err EndTransaction();
extern Err BeginTransaction(TID *);
extern TID GetTID();
extern GMessage *Prepare_AnswFromBank_Input();

#define PPLAN_LOG "pPlan.log"

int main()
{ 
  LogFile *lf = NULL;
  PayPlanMgr *pPlanMgr = NULL;
  TID tId;
  MsgField *mf = NULL;
  GMessage *gm = NULL;
  File *file = NULL;
  Queue *q = NULL;
  
  lf = new LogFile(BASIC_PATH""PPLAN_FILES_PATH""PPLAN_LOG);
  if( (pPlanMgr = new PayPlanMgr(lf)) == NULL ){
    delete lf;
    return KO;
  }

  BeginTransaction(&tId);
  mf = new FileMsgField(NULL, "proKlienta.msgF");
  if( pPlanMgr->DecideAcceptPPlan(mf) == OK )
    if( pPlanMgr->StorePPlanFromEso(mf) == OK );
  if( mf ) delete mf; mf = NULL;
  EndTransaction();
//  pPlanMgr->Rollback(GetTID());
//  pPlanMgr->DeletePPlanFromEso(GetTID());

  BeginTransaction(&tId);
  file = new File("ukladanySoubor");
  q = new Queue();
  pPlanMgr->MakePPlanForBanks(GetTID()-1, file, q);
  if( file ) delete file;
  EndTransaction();

  if( q ){
    for( ; (gm = (GMessage *)q->DeleteAt(1)) != NULL ; ){
//      gm->DebugHeaders(lf);
      gm->StoreToFile("odKlienta.gm");
      delete gm;
    }
    delete q;
  }
  
  gm = Prepare_AnswFromBank_Input();
  pPlanMgr->AnswerFromBank(gm);
  if( gm ) delete gm; gm = NULL;
  
//  delete pPlanMgr;
  delete lf;
  
  return OK;
}
