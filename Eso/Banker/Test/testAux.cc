// *************************************************************************
//
// File: testAux.cc
// Name: Test Auxiliary File
// Author: Ivana
// Last Modified: 11.2.1999
//
// Description: 
// *************************************************************************

#include "./../../../Common/GMessage/H/estime.h"
#include "./../../../Common/GMessage/H/gMessage.h"
#include "./../../UidGenerator/H/uidGenerator.h"
#include "./../H/bankDefines.h"

#define ACID_ "ACID"
#define PUB_KEY "PubKey"
#define PAY_ID_GEN_TAB "payIdGen.tab"

//--------------------------------------------------------------------------
// pro testy
//--------------------------------------------------------------------------
GMessage *Prepare_TOCurrPay_Input(LogFile *log=NULL)
{
  GMessage *gm;

  if( (gm = new GMessage()) == NULL ) return NULL;
/*
  if( gm->SetAsID(GM_PAYMENT_ID, 2) != OK ||
      gm->SetAsInt(GM_STATE, REQUEST) != OK ){
    if( log ) gm->DebugHeaders(log, "TOCurrPay GM (FAIL)");
    delete gm;
    return NULL;
  }
*/  
  gm->SetAsID(GM_PAYMENT_ID, 2);
  gm->SetAsInt(GM_STATE, REQUEST);
  if( log ) gm->DebugHeaders(log, "TOCurrPay GM");
  
  return gm;
}

//--------------------------------------------------------------------------
GMessage *Prepare_TOPayment_Input(LogFile *log=NULL)
{ 
  GMessage *gm;
  
  if( (gm = new GMessage()) == NULL ) return NULL;
/*
  if( gm->SetAsID(GM_PAYMENT_ID, 2) != OK ){
    if( log ) gm->DebugHeaders(log, "TOPayment GM (FAIL)");
    delete gm;
    return NULL;
  }
*/
  gm->SetAsID(GM_PAYMENT_ID, 2);
  if( log ) gm->DebugHeaders(log, "TOPayment GM");
  
  return gm;
}

//--------------------------------------------------------------------------
GMessage *Prepare_Chall_Input(LogFile *log=NULL)
{
  GMessage *gm;
//  BytesMsgField *bmf;

/*
  if( (gm = new GMessage()) == NULL ) return NULL;
  if( (bmf = new BytesMsgField(NULL, "OAUTHENT", 9)) == NULL ){
    delete gm;
    return NULL;
  }
  if( gm->SetAsID(GM_PAYMENT_ID, 2) != OK ||
      gm->SetAsInt(GM_SUCCESS, OK) != OK || //OK UNKNOWN_PAY_ID WRONG_TIME OTHER
      gm->SetAsMsgField(GM_O_AUTHENTIFICATION, bmf) != OK ||
      gm->SetAsString(GM_REASON_OF_FAIL, "Neco je spatne") != OK ){
    delete bmf; bmf = NULL;
    if( log ) gm->DebugHeaders(log, "Chall GM (FAIL)");
    delete gm;
    return NULL;
  }
  delete bmf;
*/
/*
  if( (gm = new GMessage()) == NULL ) return NULL;
  gm->SetAsID(GM_PAYMENT_ID, 2);
  gm->SetAsInt(GM_SUCCESS, OK); //OK UNKNOWN_PAY_ID WRONG_TIME OTHER
  bmf = new BytesMsgField(NULL, "OAUTHENT", 9);
  gm->SetAsMsgField(GM_O_AUTHENTIFICATION, bmf);
  if( bmf ) delete bmf;
  gm->SetAsString(GM_REASON_OF_FAIL, "Neco je spatne");
  if( log ) gm->DebugHeaders(log, "Chall GM");
*/  
  if( (gm = new GMessage("fromBank1.gm")) == NULL ) return NULL;
  if( log ) gm->DebugHeaders(log, "Answ GM");
  
  return gm;
}

//--------------------------------------------------------------------------
GMessage *Prepare_Answ_Input(LogFile *log=NULL)
{
  GMessage *gm;
  
/*
  if( (gm = new GMessage()) == NULL ) return NULL;
  if( gm->SetAsID(GM_PAYMENT_ID, 2) != OK ||
      gm->SetAsInt(GM_SUCCESS, OK) != OK || //OK UNKNOWN_PAY_ID BAD_MAC BAD_S_AUTH OTHER
      gm->SetAsString(GM_AMOUNT, "999") != OK || //i jine hodnoty!!
      gm->SetAsString(GM_REASON_OF_FAIL, "Neco je spatne") != OK ){
    if( log ) gm->DebugHeaders(log, "Answ GM (FAIL)");  
    delete gm;
    return NULL;
  }
*/

  if( (gm = new GMessage()) == NULL ) return NULL;
  gm->SetAsID(GM_PAYMENT_ID, 2);
  gm->SetAsInt(GM_SUCCESS, OK); //OK UNKNOWN_PAY_ID BAD_MAC BAD_S_AUTH OTHER
  gm->SetAsString(GM_AMOUNT, "100"); //i jine hodnoty!!
  gm->SetAsString(GM_REASON_OF_FAIL, "Neco je spatne");
  if( log ) gm->DebugHeaders(log, "Answ GM");
  
  return gm;
}



GMessage *GetACByACID(Char *, ID);
//--------------------------------------------------------------------------
// simulace metod esa (a jeho potomku)
//--------------------------------------------------------------------------
Queue *GetACs(Char *)
{
  Queue *q=NULL;

  if( (q = new Queue()) == NULL ) return NULL;
  (Void)q->Insert( GetACByACID("BlaBla", 0) );
  return q;
}

//--------------------------------------------------------------------------
GMessage *GetACByACID(Char *, ID)
{
  GMessage *ac = NULL, *pom = NULL;

  if( (ac = new GMessage()) != NULL ){
    if( (pom = new GMessage()) != NULL ){
      if( pom->SetAsBytes(PUB_KEY, "PubKey", 7) ||
          ac->SetAsID(ACID_, 123) != OK ||
          ac->SetAsGMessage(PUB_KEY, pom) != OK ){
        delete ac;
        ac = NULL;
      }
      delete pom;
    }
    else { delete ac; ac = NULL; }
  }

  return ac;
}

//--------------------------------------------------------------------------
void SetTimeOut(char *, GMessage *, ESTime )
{
}

//--------------------------------------------------------------------------
void SetTimeOut(char *, GMessage *, RelTime )
{
}

//--------------------------------------------------------------------------
ESTime *GetTime()
{
  ESTime *est;
  
  if( (est = new ESTime()) )
    est->SetTime(99, 2, 11);
  return est;
}

//--------------------------------------------------------------------------
TID GetTID()
{
  return 987;
}

//--------------------------------------------------------------------------
GMessage *CreateSendDataMsg(char *, GMessage *gm, GMessage *, int)
{
  return new GMessage(gm);
}

//--------------------------------------------------------------------------
Err ComputeMAC(ID, MsgField *oAuth, MsgField **mac)
{
  if( oAuth == NULL || mac == NULL ) return KO;
  (* mac) = (MsgField *)oAuth->CreateCopy();
  if( mac ) return OK;
  else return KO;
}

