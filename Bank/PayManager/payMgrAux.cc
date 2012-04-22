#include "./H/payManager.h"


/**
 * Bank Payment Record constructor.
 *
 * Bank Payment Record constructor initializes variables.
 *
 *
 * @return  
 * @author  Ivana
 * @see     PayManager::StorePayments(), PayManager::AnswerFromEso()
 */
BankPayRec::BankPayRec()
{
  oAuth = NULL;
  sAuth = NULL;
  mac = NULL;
}


/**
 * Bank Payment Record destructor.
 *
 * Bank Payment Record destructor deletes variables.
 *
 *
 * @return  
 * @author  Ivana
 * @see     PayManager::StorePayments(), PayManager::AnswerFromEso()
 */
BankPayRec::~BankPayRec()
{
  if( oAuth != NULL ){ DELETE(oAuth); }
  if( sAuth != NULL ){ DELETE(sAuth); }
  if( mac != NULL ){ DELETE(mac); }
}


/**
 * Fetches from given record.
 *
 * Fetches data from given record.
 * A virtual method, that should be overwritten, to be able
 * to read from table.
 *
 *
 * @param   r record from which we take data
 * @return  error code
 * @author  Ivana
 * @see     PayRecord
 */
Err BankPayRec::FetchFromRecord(Record *r)
{
  TableField *tf=NULL;

  if( r == NULL ) return KO;

  if( (tf = r->GetField(TF_TIME)) != NULL )
    time = ((TimeField *)tf)->GetESTimeValue();
  if( (tf = r->GetField(TF_PAY_ID)) != NULL )
    payId = ((IDField *)tf)->GetIDValue();
  if( (tf = r->GetField(TF_AMOUNT)) != NULL ){
    if( amount ){ DELETE(amount);}
    amount = ((StringField *)tf)->AsString();
  }
  if( (tf = r->GetField(TF_SEAL_S_AUTH)) != NULL ){
    if( sealSAuth ){ DELETE(sealSAuth);}
    sealSAuth = ((BytesField *)tf)->GetAsMsgField();
  }
  if( (tf = r->GetField(TF_S_AUTH)) != NULL ){
    if( sAuth ){ DELETE(sAuth);}
    sAuth = ((BytesField *)tf)->GetAsMsgField();
  }
  if( (tf = r->GetField(TF_O_AUTH)) != NULL ){
    if( oAuth ){ DELETE(oAuth);}
    oAuth = ((BytesField *)tf)->GetAsMsgField();
  }
  if( (tf = r->GetField(TF_MAC)) != NULL ){
    if( mac ){ DELETE(mac);}
    mac = ((BytesField *)tf)->GetAsMsgField();
  }

  return OK;
}


/**
 * Fills given record.
 *
 * Fills given record with data.
 * A virtual method, that should be overwritten, to be able
 * to write to or update table.
 *
 *
 * @param   r record into which we store data
 * @return  error code
 * @author  Ivana
 * @see     PayRecord
 */
Err BankPayRec::FillRecord(Record *r)
{
  if( r == NULL ) return KO;

  r->SetField(TF_TIME, &time);
  r->SetField(TF_PAY_ID, &payId);
  r->SetField(TF_AMOUNT, amount);
  r->SetField(TF_SEAL_S_AUTH, sealSAuth);
  r->SetField(TF_S_AUTH, sAuth);
  r->SetField(TF_O_AUTH, oAuth);
  r->SetField(TF_MAC, mac);

  return OK;
}


/**
 * MAC is equal to given MAC.
 *
 * Finds wheather given mac is equal to this->mac.
 *
 *
 * @param   otherMAC a mac that is compared to ours
 * @return  YES/NO
 * @author  Ivana
 * @see     PayManager::AnswerFromEso()
 */
Int BankPayRec::MACEqualTo(MsgField *otherMAC)
{
  if( otherMAC == NULL )
    if( mac == NULL ) return YES;
    else return NO;
  else
    if( mac == NULL ) return NO;
    else return (mac->EqualTo(otherMAC)) ? YES : NO;
}


/**
 * SAuth is equal to given sAuth.
 *
 * Finds wheather given sAuth is equal to this->sAuth.
 *
 *
 * @param   otherSA an sAuth that is compared to ours
 * @return  YES/NO
 * @author  Ivana
 * @see     PayManager::AnswerFromEso()
 */
Int BankPayRec::SAuthEqualTo(MsgField *otherSA)
{
  if( otherSA == NULL )
    if( sAuth == NULL ) return YES;
    else return NO;
  else
    if( sAuth == NULL ) return NO;
    else return (sAuth->EqualTo(otherSA)) ? YES : NO;
}


/**
 * Get Amount.
 *
 * Returns our amount field in new string.
 *
 *
 * @return  copy of amount
 * @author  Ivana
 * @see     PayManager::StorePayments(), PayManager::AnswerFromEso()
 */
Char *BankPayRec::GetAmount()
{
  Char *c;

  if( amount==NULL ) return NULL;
  c = new Char[strlen(amount)+1];
  strcpy(c, amount);
  return c;
}


/**
 * Get SealSAuthentification.
 *
 * Returns our sealSAuth field in new MsgField.
 *
 *
 * @return  copy of sealSAuth
 * @author  Ivana
 * @see     PayManager::StorePayments()
 */
MsgField *BankPayRec::GetSealSAuth()
{
  if( sealSAuth == NULL ) return NULL;
  return (MsgField *)sealSAuth->CreateCopy();
}


/**
 * Set SAuthentification.
 *
 * Sets our sAuth to given one.
 *
 *
 * @param   otherSA sAuth for setting our data
 * @return  error code
 * @author  Ivana
 * @see     PayManager::StorePayments()
 */
Err BankPayRec::SetSAuth(MsgField *otherSA)
{
  if( otherSA == NULL ) return KO;
  if( sAuth ){ DELETE(sAuth); } 
  if( (sAuth = (MsgField *)otherSA->CreateCopy()) == NULL ) return KO;
  return OK;
}

/**
 * Clear data.
 *
 * Clears data - including eCode.
 *
 *
 * @return  void
 * @author  Ivana
 * @see     PayManager::StorePayments()
 */
Void BankPayRec::Clear()
{
  PayRecord::Clear();
  if( sAuth != NULL ){ DELETE(sAuth); }
  if( oAuth != NULL ){ DELETE(oAuth); }
  if( mac != NULL ){ DELETE(mac); }
}
