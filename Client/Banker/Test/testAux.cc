// *************************************************************************
//
// File: testAux.cc
// Name: Test Auxiliary File
// Author: Ivana
// Last Modified: 31.1.1999
//
// Description: 
// *************************************************************************

#include "./../../../Common/H/common.h"
#include "./../../../Common/GMessage/H/estime.h"
#include "./../../../Common/GMessage/H/gMessage.h"
#include "./../H/pPlanDefs.h"

#define ACID_ "ACID"
#define PUB_KEY "PubKey"
#define GM_IN_REPLY_TO_TID "InReplyToTID"

TID tId=10;

GMessage *GetACByACID()
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


void SetTimeOut(char *, GMessage *, ESTime ){}
void SetTimeOut(char *, GMessage *, RelTime ){}


ESTime *GetTime()
{
  ESTime *est;
  
  if( (est = new ESTime()) )
    est->SetTime("99:02:03:00:00:00");
  return est;
}


Err BeginTransaction(TID *t)
{
  ++tId;
  if( t ) *t=tId;
  return OK;
}


Err EndTransaction()
{
  return OK;
}


TID GetTID()
{
  return tId;
}

GMessage *CreateSendDataMsg(char *, GMessage *gm, GMessage *, int)
{
  return new GMessage(gm);
}


Err ComputeMAC(ID, MsgField *oAuth, MsgField **mac)
{
  if( oAuth == NULL || mac == NULL ) return KO;
  (* mac) = (MsgField *)oAuth->CreateCopy();
  if( mac ) return OK;
  else return KO;
}

MsgField *ComputeMAC(File *, MsgField *oAuth)
{
  if( oAuth == NULL ) return NULL;
  return (MsgField *)oAuth->CreateCopy();
}


GMessage *Prepare_AnswFromBank_Input()
{
  GMessage *gm;

  if( (gm = new GMessage()) == NULL ) return NULL;
  gm->SetAsID(GM_IN_REPLY_TO_TID, GetTID()-1);
  
  return gm;
}
