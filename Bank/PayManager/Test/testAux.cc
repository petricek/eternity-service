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

ESTime *GetTime()
{
  return new ESTime();
}

GMessage *Prepare_ReqFromEso_Input()
{
  GMessage *gm;

  if( (gm = new GMessage()) == NULL ) return NULL;
  gm->SetAsID("PaymentId", 2);
  return gm;
}
