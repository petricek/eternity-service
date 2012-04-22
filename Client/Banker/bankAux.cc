#include "./H/banker.h"
#include "./../../Common/RandomGenerator/H/randomGenerator.h"


/**
 * Client Paymen Plan Record constructor.
 *
 * Client Paymen Plan Record constructor initializes variables.
 *
 *
 * @return  
 * @author  Ivana
 * @see     Banker::StorePayPlan()
 */
ClientPayPlanRec::ClientPayPlanRec()
{
  tId = BAD_ID;
  bankACID = BAD_ID;
  bankAC = NULL;
  oAuth = NULL;
  mac = NULL;
}


/**
 * Client Paymen Plan Record destructor.
 *
 * Client Paymen Plan Record destructor deletes variables.
 *
 *
 * @return  
 * @author  Ivana
 * @see     Banker::StorePayPlan()
 */
ClientPayPlanRec::~ClientPayPlanRec()
{
  if( bankAC != NULL ){ DELETE(bankAC); }
  if( oAuth != NULL ){ DELETE(oAuth); }
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
Err ClientPayPlanRec::FetchFromRecord(Record *r)
{
  TableField *tf=NULL;
  MsgField *mf=NULL;

  if( r == NULL ) return KO;

  if( (tf = r->GetField(TF_TID)) != NULL )
    tId = ((IDField *)tf)->GetIDValue();
  if( (tf = r->GetField(TF_TIME)) != NULL )
    time = ((TimeField *)tf)->GetESTimeValue();
  if( (tf = r->GetField(TF_PAY_ID)) != NULL )
    payId = ((IDField *)tf)->GetIDValue();
  if( (tf = r->GetField(TF_BANK_ACID)) != NULL )
    bankACID = ((IDField *)tf)->GetIDValue();
  if( (tf = r->GetField(TF_BANK_AC)) != NULL &&
      (mf = ((BytesField *)tf)->GetAsMsgField()) != NULL ){
     if( bankAC ){ DELETE(bankAC); }
     bankAC = mf->GetAsGMessage();
     DELETE(mf); 
  }
  if( (tf = r->GetField(TF_AMOUNT)) != NULL ){
    if( amount ){ DELETE(amount); }
    amount = ((StringField *)tf)->AsString();
  }
  if( (tf = r->GetField(TF_SEAL_S_AUTH)) != NULL ){
    if( sealSAuth ){ DELETE(sealSAuth);}
    sealSAuth = ((BytesField *)tf)->GetAsMsgField();
  }
  if( (tf = r->GetField(TF_O_AUTH)) != NULL ){
    if( oAuth ){ DELETE(oAuth); }
    oAuth = ((BytesField *)tf)->GetAsMsgField();
  }
  if( (tf = r->GetField(TF_MAC)) != NULL ){
    if( mac ){ DELETE(mac); }
    mac = ((BytesField *)tf)->GetAsMsgField();
  }

  if( payId == BAD_ID ) return eCode = KO;
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
Err ClientPayPlanRec::FillRecord(Record *r)
{
  MsgField *mf=NULL;

  if( r == NULL ) return KO;

  r->SetField(TF_TID, &tId);
  r->SetField(TF_TIME, &time);
  r->SetField(TF_PAY_ID, &payId);
  r->SetField(TF_BANK_ACID, &bankACID);
  r->SetField(TF_AMOUNT, amount);
  r->SetField(TF_SEAL_S_AUTH, sealSAuth);
  if( bankAC ){
    mf = bankAC->StoreToMsgField(TF_BANK_AC);
    r->SetField(TF_BANK_AC, mf);
    if( mf ){ DELETE(mf);}
  }
  r->SetField(TF_O_AUTH, oAuth);
  r->SetField(TF_MAC, mac);

  return OK;
}


/**
 * Generate OAuthentification.
 *
 * Generates OAuthentification - a random string used as a secret
 * key between Client and Bank.
 *
 *
 * @param   lengthOfAuth length of generated OAuth
 * @return  generated oAuth
 * @author  Ivana
 * @see     Banker::StorePayPlan()
 */
MsgField *ClientPayPlanRec::GenerateOAuth(Size lengthOfAuth)
{
  RandomGenerator *authGenerator=NULL;
  Size count;

  if( oAuth ){ DELETE(oAuth); }
  count = ( lengthOfAuth>0 ) ? lengthOfAuth : MAX_LENGTH_OF_AUTH;
  authGenerator = new RandomGenerator();
  if( authGenerator == NULL ){ eCode = KO; return NULL; }
  oAuth = authGenerator->RandomBytes(count);
  DELETE(authGenerator);

  if( oAuth ) return (MsgField *)oAuth->CreateCopy();
  else{ eCode = KO; return NULL; }
}


/**
 * Get Amount.
 *
 * Returns our amount field in new string.
 *
 *
 * @return  copy of amount
 * @author  Ivana
 * @see     
 */
Char *ClientPayPlanRec::GetAmount()
{
  Char *c;

  if( amount==NULL ) return NULL;
  c = new Char[strlen(amount)+1];
  strcpy(c, amount);
  return c;
}


/**
 * Get bank's access certificate.
 *
 * Returns bank's access certificate field in new gMessage.
 *
 *
 * @return  copy of bankAC
 * @author  Ivana
 * @see     
 */
GMessage *ClientPayPlanRec::GetBankAC()
{
  return new GMessage(bankAC);
}


/**
 * Get bank's access certificate ID.
 *
 * Returns bank's access certificate ID field.
 *
 *
 * @return  bankACID
 * @author  Ivana
 * @see     
 */
ACID ClientPayPlanRec::GetBankACID()
{
  return bankACID;
}


/**
 * Set transaction ID.
 *
 * Sets transaction ID to given TID
 *
 *
 * @param   tId given transaction ID
 * @return  void
 * @author  Ivana
 * @see     Banker::StorePayPlan()
 */
Void ClientPayPlanRec::SetTID(TID tId)
{
  this->tId = tId;
}


/**
 * Set MAC.
 *
 * Sets our MAC to given one.
 *
 *
 * @param   mac new mac for setting our data
 * @return  error code
 * @author  Ivana
 * @see     Banker::StorePayPlan()
 */
Err ClientPayPlanRec::SetMAC(MsgField *aMac)
{
  if( aMac == NULL ) return KO;
  if( this->mac ){ DELETE(this->mac);}
  if( (this->mac=(MsgField *)aMac->CreateCopy())==NULL ) return KO;
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
 * @see     Banker::StorePayPlan()
 */
Void ClientPayPlanRec::Clear()
{
  PayRecord::Clear();
  tId = BAD_ID;
  bankACID = BAD_ID;
  if( bankAC != NULL ){ DELETE(bankAC); }
  if( oAuth != NULL ){ DELETE(oAuth); }
  if( mac != NULL ){ DELETE(mac); }
}
