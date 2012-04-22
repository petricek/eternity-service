#include "./../../Common/H/labels.h"
#include "./../../Common/RandomGenerator/H/randomGenerator.h"
#include "./../../Common/Cipherer/H/cipherer.h"
#include "./H/banker.h"

/**
 * Generate SAuthentification.
 *
 * Generates SAuthentification - a random string used as a secret
 * key between Eso and Bank.
 *
 *
 * @param   lengthOfSA length of generated SAuth
 * @return  error code
 * @author  Ivana
 * @see     EsoPayPlanRec()
 */
Err EsoPayPlanRec::GenerateSAuth(Size lengthOfSA)
{
  RandomGenerator *authGenerator=NULL;
  Size count;

  sAuth = NULL;
  count = ( lengthOfSA>0 ) ? lengthOfSA : MAX_LENGTH_OF_AUTH;
  authGenerator = new RandomGenerator();
  if( authGenerator == NULL ) return eCode = KO;
  sAuth = authGenerator->RandomBytes(count);
  DELETE(authGenerator);

  if ( sAuth ) return OK;
  else return eCode = KO;
}

/**
 * Seals SAuthentification.
 *
 * Encrypts SAuthentification with bank's public key.
 *
 * @param   void
 * @return  error code
 * @author  Ivana
 * @see     EsoPayPlanRec()
 */
Err EsoPayPlanRec::SealSAuth()
{
  Cipherer *ciph=NULL;
  GMessage *bankPK=NULL; //public key

//  if( sAuth ) sealSAuth = (MsgField *)sAuth->CreateCopy();
//  else sealSAuth = NULL;

  if( bankAC == NULL ) return eCode = KO;
  if( (ciph = new Cipherer(NULL)) == NULL ) return eCode = KO;

  if( (bankPK = bankAC->GetAsGMessage(GM_PUBLIC_KEY)) == NULL ||
      ciph->EncryptWithPublicKey(bankPK, sAuth, &sealSAuth) != OK )
    if( sealSAuth ){ DELETE(sealSAuth); }

  if( bankPK ){ DELETE(bankPK); }
  DELETE(ciph);

  return ( sealSAuth == NULL ) ? eCode = KO : OK;
}

/**
 * Eso Payment Plan Record Constructor.
 *
 * Eso Payment Plan Record Constructor sets class variables.
 * Generates SAuthentification and 'seals' it.
 *
 * @param   payId payment ID
 * @param   tId transaction ID 
 * @param   time time of storrage
 * @param   bankAC bank access certificate
 * @param   amount amount of money
 * @param   lenOfSA length of SAuthentification
 * @author  Ivana
 * @see     Banker::MakePayPlan()
 */
EsoPayPlanRec::EsoPayPlanRec(ID payId, TID tId, ESTime time, GMessage *bankAC,
  Char *amount, Size lenOfSA)
{
  this->bankACID = BAD_ID;
  this->bankAC = NULL;  
  this->sAuth = NULL;

  this->payId = payId;
  this->tId = tId;
  this->time = time;
  if( bankAC==NULL || bankAC->GetAsID(GM_ACID, &bankACID)!=OK ||
      (this->bankAC=new GMessage(bankAC))==NULL )
    eCode = KO;
  if (amount==NULL  || (this->amount=new Char[strlen(amount)+1])==NULL )
    eCode = KO;
  else
    this->amount = strcpy(this->amount, amount);
  (Void)GenerateSAuth(lenOfSA);
  (Void)SealSAuth();
}

/**
 * Eso Payment Plan Record destructor.
 *
 * Eso Payment Plan Record destructor deletes class variables.
 *
 * @author  Ivana
 * @see     Banker::MakePayPlan()
 */
EsoPayPlanRec::~EsoPayPlanRec()
{
  if( bankAC != NULL ){ DELETE(bankAC); }
  if( sAuth != NULL ){ DELETE(sAuth); }
}

/**
 * Fetches from given record.
 *
 * Fetches data from given record.
 * A virtual method, that should be overwritten, to be able
 * to read from table. (We will not use this method.)
 *
 *
 * @param   r record from which we take data
 * @return  error code
 * @author  Ivana
 * @see     PayRecord
 */
Err EsoPayPlanRec::FetchFromRecord(Record *)
{
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
Err EsoPayPlanRec::FillRecord(Record *r)
{
  MsgField *mf=NULL;

  if( r == NULL ) return KO;

  r->SetField(TF_TID, &tId);
  r->SetField(TF_TIME, &time);
  r->SetField(TF_PAY_ID, &payId);
  r->SetField(TF_BANK_ACID, &bankACID);
  if( bankAC ){
    mf = bankAC->StoreToMsgField(TF_BANK_AC);
    r->SetField(TF_BANK_AC, mf);
    if( mf ){ DELETE(mf); }
  }
  r->SetField(TF_AMOUNT, amount);
  r->SetField(TF_S_AUTH, sAuth);
  r->SetField(TF_SEAL_S_AUTH, sealSAuth);

  return OK;
}

/**
 * Payment constructor.
 *
 * Payment constructor initializes class variables.
 *
 * @author  Ivana
 * @see     Banker::SchedulePays()
 */
Payment::Payment()
{
  fileId = bankACID = BAD_ID;
  sAuth = NULL;
}

/**
 * Payment destructor.
 *
 * Payment destructor deletes class variables.
 *
 * @author  Ivana
 * @see     Banker::SchedulePays()
 */
Payment::~Payment()
{
  if( sAuth != NULL ){ DELETE(sAuth) };
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
Err Payment::FetchFromRecord(Record *r)
{
  TableField *tf=NULL;

  if( r == NULL ) return KO;

  if( (tf = r->GetField(TF_TIME)) != NULL )
    time = ((TimeField *)tf)->GetESTimeValue();
  if( (tf = r->GetField(TF_PAY_ID)) != NULL )
    payId = ((IDField *)tf)->GetIDValue();
  if( (tf = r->GetField(TF_FILE_ID)) != NULL )
    fileId = ((IDField *)tf)->GetIDValue();
  if( (tf = r->GetField(TF_BANK_ACID)) != NULL )
    bankACID = ((IDField *)tf)->GetIDValue();
  if( (tf = r->GetField(TF_AMOUNT)) != NULL ){
    if( amount ){ DELETE(amount); }
    amount = ((StringField *)tf)->AsString();
  }
  if( (tf = r->GetField(TF_S_AUTH)) != NULL ){
    if( sAuth ){ DELETE(sAuth); }
    sAuth = ((BytesField *)tf)->GetAsMsgField();
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
Err Payment::FillRecord(Record *r)
{
  if( r == NULL ) return KO;

  r->SetField(TF_TIME, &time);
  r->SetField(TF_PAY_ID, &payId);
  r->SetField(TF_FILE_ID, &fileId);
  r->SetField(TF_BANK_ACID, &bankACID);
  r->SetField(TF_AMOUNT, amount);
  r->SetField(TF_S_AUTH, sAuth);

  return OK;
}

/**
 * Returns requested ID.
 *
 * Returns 'whichId' ID value or BAD_ID.
 *
 *
 * @param   whichId name of ID we want to get
 * @author  Ivana
 * @see     Banker::SchedulePays()
 */
ID Payment::GetID(Char *whichId)
{
  if( whichId == NULL ) return BAD_ID;

  if( strcmp(whichId, TF_PAY_ID) == 0 )
    return payId;
  if( strcmp(whichId, TF_FILE_ID) == 0 )
    return fileId;
  if( strcmp(whichId, TF_BANK_ACID) == 0 )
    return bankACID;
  return BAD_ID;
}


/**
 * Returns time field.
 *
 * Returns value of time field.
 *
 *
 * @author  Ivana
 * @see     Banker::SchedulePays()
 */
ESTime Payment::GetTime()
{
  return time;
}


/**
 * Get Amount.
 *
 * Returns our amount field in new string.
 *
 *
 * @return  copy of amount
 * @author  Ivana
 * @see     Banker::SchedulePays()
 */
Char *Payment::GetAmount()
{
  Char *c=NULL;

  if( amount==NULL ) return NULL;
  c = new Char[strlen(amount)+1];
  strcpy(c, amount);
  return c;
}


/**
 * Sets given ID.
 *
 * Sets 'whichId' ID value.
 *
 *
 * @param   whichId name of ID we want to set
 * @return  error code 
 * @author  Ivana
 * @see     Banker::SchedulePays()
 */
Err Payment::SetID(Char *whichId, ID value)
{
  if( whichId == NULL ) return KO;

  if( strcmp(whichId, TF_PAY_ID) == 0 )
    payId = value;
  else if( strcmp(whichId, TF_FILE_ID) == 0 )
    fileId = value;
  else if( strcmp(whichId, TF_BANK_ACID) == 0 )
    bankACID = value;
  else return KO;
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
 * @see     Banker::SchedulePays()
 */
Void Payment::Clear()
{
  PayRecord::Clear();
  this->fileId = BAD_ID;
  this->bankACID = BAD_ID;
  if( sAuth != NULL ){ DELETE(sAuth); }
}


/**
 * Current Payment Costructor.
 *
 * Current Payment constructor initializes class variables.
 *
 * @author  Ivana
 * @see     Banker::TimeForPayment(), Banker::ChallengeFromBank(), 
 *          Banker::AnswerFromBank(), Banker::TimeOutPayment(),
 */
CurrPay::CurrPay()
{
  tId = BAD_ID;
  oAuth = NULL;
  mac = NULL;
  state = REQUEST;
  numOfTries = 1;
}

/**
 * Current Payment Destructor.
 *
 * Current Payment destructor deletes class variables.
 *
 * @author  Ivana
 * @see     Banker::TimeForPayment(), Banker::ChallengeFromBank(), 
 *          Banker::AnswerFromBank(), Banker::TimeOutPayment(),
 */
CurrPay::~CurrPay()
{
  if( oAuth != NULL ){ DELETE(oAuth); }
}

/**
 * Fetches from given record.
 *
 * Fetches data from given record.
 * A virtual method, that should be overwritten, to be able
 * to read from table. (We will not use this method.)
 *
 *
 * @param   r record from which we take data
 * @return  error code
 * @author  Ivana
 * @see     PayRecord
 */
Err CurrPay::FetchFromRecord(Record *r)
{
  TableField *tf=NULL;

  if( r == NULL ) return KO;

  if( (tf = r->GetField(TF_TID)) != NULL )
    tId = ((IDField *)tf)->GetIDValue();
  if( (tf = r->GetField(TF_TIME)) != NULL )
    time = ((TimeField *)tf)->GetESTimeValue();
  if( (tf = r->GetField(TF_PAY_ID)) != NULL )
    payId = ((IDField *)tf)->GetIDValue();
  if( (tf = r->GetField(TF_FILE_ID)) != NULL )
    fileId = ((IDField *)tf)->GetIDValue();
  if( (tf = r->GetField(TF_BANK_ACID)) != NULL )
    bankACID = ((IDField *)tf)->GetIDValue();
  if( (tf = r->GetField(TF_AMOUNT)) != NULL ){
    if( amount ){ DELETE(amount); }
    amount = ((StringField *)tf)->AsString();
  }
  if( (tf = r->GetField(TF_S_AUTH)) != NULL ){
    if( sAuth ){ DELETE(sAuth); }
    sAuth = ((BytesField *)tf)->GetAsMsgField();
  }
  if( (tf = r->GetField(TF_O_AUTH)) != NULL ){
    if( oAuth ){ DELETE(oAuth); }
    oAuth = ((BytesField *)tf)->GetAsMsgField();
  }
  if( (tf = r->GetField(TF_STATE)) != NULL )
    state = ((IntField *)tf)->GetIntValue();
  if( (tf = r->GetField(TF_MAC)) != NULL ){
    if( mac ){ DELETE(mac); }
    mac = ((BytesField *)tf)->GetAsMsgField();
  }
  if( (tf = r->GetField(TF_NUM_OF_TRIES)) != NULL )
    numOfTries = ((IntField *)tf)->GetIntValue();

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
Err CurrPay::FillRecord(Record *r)
{
  if( r == NULL ) return KO;

  r->SetField(TF_TID, &tId);
  r->SetField(TF_TIME, &time);
  r->SetField(TF_PAY_ID, &payId);
  r->SetField(TF_FILE_ID, &fileId);
  r->SetField(TF_BANK_ACID, &bankACID);
  r->SetField(TF_AMOUNT, amount);
  r->SetField(TF_S_AUTH, sAuth);
  r->SetField(TF_O_AUTH, oAuth);
  r->SetField(TF_STATE, &state);
  r->SetField(TF_MAC, mac);
  r->SetField(TF_NUM_OF_TRIES, &numOfTries);

  return OK;
}

/**
 * Writes record to given table.
 * 
 * Writes (inserts) a record to given table (it does not change eCode
 * variable).
 *
 *
 * @param   t table to which we will write new record
 * @param   reason reason of fail - an additional parametr
 *          for filling 'Reason-Of-Fail' field in some tables
 * @author  Ivana
 * @see     Banker::TimeForPayment(), Banker::TimeOutPayment(),
 *          Banker::ChallengeOK(), Banker::ChallengeKO(), 
 *          Banker::AnswerKO(), Banker::TimeOutCurrPay(),
 */
Err CurrPay::WriteToTable(Table *t, Char *reason)
{
  Record *r=NULL;
  Err e;

  if( t==NULL || t->Open()!=OK ) return KO;

  if( (r = t->NewRecord()) == NULL ){
    t->Close();
    return KO;
  }

  (Void)FillRecord(r);
  if( reason ) r->SetField(TF_REASON, reason);
  e = t->InsertRecord(r);
  e = (t->Close() == OK) ? e : KO;
  DELETE(r);

  return e;
}

/**
 * Returns requested ID.
 *
 * Returns 'whichId' ID value or BAD_ID.
 *
 *
 * @param   whichId name of ID we want to get
 * @author  Ivana
 * @see     Banker
 */
ID CurrPay::GetID(Char *whichId)
{
  if( whichId == NULL ) return BAD_ID;

  if( strcmp(whichId, TF_TID) == 0 )
    return tId;
  if( strcmp(whichId, TF_PAY_ID) == 0 )
    return payId;
  if( strcmp(whichId, TF_FILE_ID) == 0 )
    return fileId;
  if( strcmp(whichId, TF_BANK_ACID) == 0 )
    return bankACID;
  return BAD_ID;
}

/**
 * Returns requested authentification.
 *
 * Returns 'whichAuth' value or NULL.
 *
 *
 * @param   whichId name of auhtentification we want to get
 * @author  Ivana
 * @see     Banker::GenerateAnswer()
 */
MsgField *CurrPay::GetAuth(Char *whichAuth)
{
  if( whichAuth == NULL ) return NULL;

  if( strcmp(whichAuth, TF_O_AUTH) == 0 )
    if( oAuth ) return (MsgField *)oAuth->CreateCopy();
    else return NULL;
  if( strcmp(whichAuth, TF_S_AUTH) == 0 )
    if( sAuth ) return (MsgField *)sAuth->CreateCopy();
    else return NULL;
  return NULL;
}

/**
 * Get MAC.
 *
 * Returns our mac field in new MsgField.
 *
 *
 * @return  copy of mac
 * @author  Ivana
 * @see     Banker::GenerateAnswer()
 */
MsgField *CurrPay::GetMAC()
{
  if( mac == NULL ) return NULL;
  return (MsgField *)mac->CreateCopy();
}

/**
 * Get State Of Current Payment.
 *
 * Returns a value of state field.
 *
 *
 * @return  state value
 * @author  Ivana
 * @see     Banker
 */
Int CurrPay::GetState()
{
  return state;
}

/**
 * Get number of tries.
 *
 * If the operation is 'plus', first increase numOfTries and then return it.
 * If the operation is 'one', sets numOfTries to 1, else return numOfTries.
 *
 * @param   operation which operation should be done before return numOfTries
 * @author  Ivana
 * @see     Banker::PartialUpdateCurrPays(), Banker::TimeOutCurrPay(),
 *          Banker::TimeOutPayment()
 */
Int CurrPay::NumOfTries(Char operation)
{
  if( operation == '+' ) return ++numOfTries;
  if( operation == '1' ) return numOfTries=1;
  return numOfTries;
}


/**
 * Sets given ID.
 *
 * Sets 'whichId' ID value.
 *
 *
 * @param   whichId name of ID we want to set
 * @return  error code 
 * @author  Ivana
 * @see     Banker
 */
Err CurrPay::SetID(Char *whichId, ID value)
{
  if( whichId == NULL ) return KO;

  if( strcmp(whichId, TF_TID) == 0 )
    tId = value;
  else if( strcmp(whichId, TF_PAY_ID) == 0 )
    payId = value;
  else if( strcmp(whichId, TF_FILE_ID) == 0 )
    fileId = value;
  else if( strcmp(whichId, TF_BANK_ACID) == 0 )
    bankACID = value;
  else return KO;
  return OK;
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
 * @see     Banker::ChallengeOK()
 */
Err CurrPay::SetMAC(MsgField *mac)
{
  if( mac==NULL ) return KO;

  if( this->mac ){ DELETE(this->mac); }
  if( (this->mac=(MsgField *)mac->CreateCopy())==NULL ) return eCode = KO;
  return OK;
}


/**
 * Sets authentification.
 *
 * Sets 'whichAuth' authentification with new value.
 *
 *
 * @param   whichId name of auhtentification we want to set
 * @param   auth a new value for the auhtentification
 * @return  error code
 * @author  Ivana
 * @see     Banker::ChallengeOK()
 */
Err CurrPay::SetAuth(Char *whichAuth, MsgField *auth)
{
  if( whichAuth == NULL || auth == NULL ) return KO;

  if( strcmp(whichAuth, TF_O_AUTH) == 0 ){
    if( this->oAuth ){ DELETE(this->oAuth); }
    if( (this->oAuth = (MsgField *)auth->CreateCopy()) == NULL )
      return eCode = KO;
    else
      return OK;
  }
  if( strcmp(whichAuth, TF_S_AUTH) == 0 ){
    if( this->sAuth ){ DELETE(this->sAuth); }
    if( (this->sAuth = (MsgField *)auth->CreateCopy()) == NULL )
      return eCode = KO;
    else
      return OK;
  }
  return KO;
}

/**
 * Set State Of Current Payment.
 *
 * Set state of current payment field with given value.
 *
 *
 * @param   st a new value for the state
 * @return  OK if st is ANSWER or REQUEST, KO otherwise
 * @author  Ivana
 * @see     Banker::ChallengeOK()
 */
Err CurrPay::SetState(Int st)
{
  if( !(st==ANSWER || st==REQUEST) ) return KO;
  state = st;
  return OK;
}
