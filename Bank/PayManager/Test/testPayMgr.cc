#include "./../H/payManager.h"

extern GMessage *Prepare_ReqFromEso_Input();

#define PAYMGR_LOG "payMgr.log"

int main()
{ 
  LogFile *lf = NULL;
  PayManager *payMgr = NULL;
  GMessage *inGM = NULL, *gm = NULL;
  
  lf = new LogFile(BASIC_PATH""PAYMGR_FILES_PATH""PAYMGR_LOG);
  if( (payMgr = new PayManager(lf)) == NULL ){
    delete lf;
    return KO;
  }
  
  inGM = new GMessage("odKlienta.gm");
  gm = payMgr->MoneyFromClient(inGM);
  if( inGM ) delete inGM;
  if( gm ){ gm->DebugHeaders(lf, "Odpoved klientovi"); delete gm; }
  
  inGM = Prepare_ReqFromEso_Input();
  gm = payMgr->RequestFromEso(inGM);
  if( inGM ) delete inGM;
  if( gm ){ 
    gm->StoreToFile("toEso.gm");
    gm->DebugHeaders(lf, "Challenge Esu"); 
    delete gm;
  }
  
  inGM = new GMessage("fromEso.gm");
  gm = payMgr->AnswerFromEso(inGM);
  if( inGM ) delete inGM;
  if( gm ){ gm->DebugHeaders(lf, "Odpoved Esu"); delete gm; }
  
  delete payMgr;
  delete lf;
  
  return OK;
}
