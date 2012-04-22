#include "./H/payManager.h"
#include "./../../Common/Utils/H/createTables.h"


/**
 * Create Payments Table.
 *
 * Creates table Payments - a persistent PayManager's table.
 *
 *
 * @return  error code
 * @author  Ivana
 * @see     PayManager()
 */
Err PayManager::CreatePayments()
{
  TableFieldQueue *fq;

  if( (fq = new TableFieldQueue()) == NULL ) return KO;

  fq->Insert( new TimeField(TF_TIME) );
  fq->Insert( new IDField(TF_PAY_ID, BAD_ID) );
  fq->Insert( new StringField(TF_AMOUNT, NULL) );
  fq->Insert( new BytesField(TF_S_AUTH, NULL) );
  fq->Insert( new BytesField(TF_O_AUTH, NULL) );
  fq->Insert( new BytesField(TF_MAC, NULL) );
//  fq->Insert( new BytesField(TF_ACCOUNT, NULL) );

  Char *tmp = ((Bank *) six)->GetPath(PAYMENTS_TAB);
  payments = ( tmp == NULL ) ? NULL : new Table(tmp, fq);
  if( tmp ){ DELETE(tmp);}
  DELETE(fq);
  return ( payments == NULL ) ? KO : OK;
}


/**
 * Deletes all records in the table.
 *
 * Deletes all records in the table with given ID.
 * If whichId is not specified (NULL), clears given table.
 *
 *
 * @param   t table in which we will delete
 * @param   whichId specifies ID, that is used for deleting
 * @param   id a value of the ID
 * @return  error code
 * @author  Ivana
 * @see     AnswerFromEso()
 */
Err PayManager::DeleteRecsInTable(Table *t, Char *whichId, ID id)
{
  Record *r=NULL;

  if( t == NULL ) return KO;

  if( whichId == NULL ) return t->Remove();

  if( t->Open() != OK ) return KO;

  for( r=t->FindFirst(whichId, &id); r != NULL; ){
    (Void)t->DeleteRecord();
    DELETE(r);
    r=t->FindNext();
  }

  return t->Close();
}


/**
 * Finds record in the table.
 *
 * Finds record in the table with given ID.
 *
 *
 * @param   t table in which we will search
 * @param   whichId specifies ID, that is used for searching
 * @param   id a value of the ID
 * @return  found record or NULL
 * @author  Ivana
 * @see     RequestFromEso()
 */
Record *PayManager::FindInTable(Table *t, Char *whichId, ID id)
{
  Record *r;

  if( t == NULL || whichId == NULL || t->Open() != OK )
    return NULL;

  r = t->FindFirst(whichId, &id);

  (Void)t->Close();

  return r;
}


/**
 * Create Common Message.
 *
 * Creates part of message common to all outgoing messages.
 * If parametr payId is not specified, does not set it.
 *
 *
 * @param   succ value of succes field 
 * @param   reason value of reason field
 * @param   payId value of paymentId field 
 * @return  created message
 * @author  Ivana 
 * @see     MoneyFromClient(), RequestFromEso(), AnswerFromEso() 
 */
GMessage *PayManager::CreateCommonMsg(Err succ, Char *reason, ID *payId)
{
  GMessage *outMsg = NULL;

  if( (outMsg = new GMessage()) != NULL )
    //sets Succcess field:
    if( outMsg->SetAsInt(GM_SUCCESS, succ) == OK ){
      //sets Payment-ID field
      if( payId ) outMsg->SetAsID(GM_PAYMENT_ID, *payId);
      //sets Reason-Of-Fail field
      outMsg->SetAsString(GM_REASON_OF_FAIL, reason);
    }
    else{ DELETE(outMsg); }

  return outMsg;
}


/**
 * Stores Payments.
 *
 * Stores payments from given table.
 * For each record first unseal SAuth and then stores all useful
 * information to 'payments' table. 
 * If something goes wrong, returns -1 instead of sum.
 *
 *
 * @param   fromClient table, from which payments are taken
 * @param   myACID ID of bank's ac, which used for encrypting sAuth
 * @return  total sum of money from client
 * @author  Ivana
 * @see     MoneyFromClient()
 */
Long PayManager::StorePayments(Table *fromClient, ACID myACID)
{
  BankPayRec *pRec = NULL;
  MsgField *sAuth = NULL, *sealSA = NULL;
  Majordomo *majordomo = NULL;
  Char *charAm = NULL;
  Long amount = 0;
  Int i = 0;

  if( fromClient == NULL || (pRec = new BankPayRec()) == NULL )
    return -1;

  majordomo = (Majordomo *)((Bank *)six)->majordomo;

  //for each record from table fromClient ...
  for( pRec->FetchAndDeleteFromTable(fromClient) ;
       pRec->IsOK() == OK ;
       pRec->FetchAndDeleteFromTable(fromClient) ){

    //unseal SAuth and write to table payments
    if( (sealSA = pRec->GetSealSAuth()) == NULL ||
        (sAuth = majordomo->UnsealSAuth(myACID, sealSA)) == NULL )
      if( sealSA ){ DELETE(sealSA);}
    DELETE(sealSA);
    if(
	pRec->SetSAuth(sAuth) != OK ||
        pRec->WriteToTable(payments) != OK ||
        (charAm = pRec->GetAmount()) == NULL ){
      DELETE(sAuth);
      DELETE(pRec);
      return -1;
    }
    DELETE(sAuth);
    pRec->Clear();
    
    //increase total sum
    amount += atol(charAm);
    DELETE(charAm);
    i++;
  }

  DELETE(pRec);

  return (i>0) ? amount : -1;
}


/**
 * PayManager constructor.
 *
 * Creates PayManager's persistent table.
 *
 *
 * @param   logFile pointer to log file
 * @param   bank pointer to main class bank
 * @return  
 * @author  Ivana
 * @see     Bank
 */
PayManager::PayManager(LogFile *logFile, Bank *bank)
  :Debugable(logFile), SixOffspring(bank)
{
  payments = NULL;
  if( CreatePayments() != OK )
    WriteString(llFatalError,"PayManager: Cannot create table");
  else
    WriteString(llDebug,"PayManager: Created");
}


/**
 * PayManager destructor.
 *
 * Destructor deletes PayManager's persistent table.
 *
 *
 * @return  
 * @author  Ivana
 * @see     Bank
 */
PayManager::~PayManager()
{
  if( payments ) DELETE(payments);
  WriteString(llDebug,"PayManager: Deleted");
}


/**
 * Reacts to message from client.
 *
 * It first generate temporary table of sent PaymentPlan.
 * Then stores modified (with unsealed SAuths) table to PayManager's
 * payments table, transfer money from client account and generate
 * message for him.
 *
 *
 * @param   inMsg message from client
 * @return  message (answer) for client
 * @author  Ivana
 * @see     Majordomo::DRCMoneyFromClient()
 */
GMessage *PayManager::MoneyFromClient(GMessage *inMsg)
{
//  cAccount; //client account
  Char *cAmount=NULL, str[70]; //client amount
  Long ca, amount; //client amount as long, counted amount
  ACID myACID;
  MsgField *cPPlan=NULL; //client pay plan
  Table *cTmpTab=NULL; //tmp pay plan table (from client)

  //check argument and get usefull
  if( inMsg == NULL || 
      inMsg->GetAsID(GM_BANK_ACID, &myACID) != OK ||
      (cAmount = inMsg->GetAsString(GM_AMOUNT)) == NULL ||
      (cPPlan = inMsg->GetAsMsgField(GM_PAYMENT_PLAN)) == NULL ){
      //|| (cAccount = inMsg->GetAsBytes(GM_ACCOUNT)) == NULL )
    if( cAmount ){ DELETE(cAmount);}
    WriteString(llWarning,"PayMgr: Bad inMsg for creating answer to Client");
    return CreateCommonMsg( KO, "Bad inMsg for creating answer to Client");
  }
  ca = atol(cAmount);
  DELETE(cAmount);

  //create table from msgField
  Char *tmp = ((Bank *) six)->GetPath(CLIENT_TMP_TAB);
  if( tmp ){
    cTmpTab = ( cPPlan->GetAsFile(tmp) == OK ) ? 
      CreateClientToBankTable(tmp) : NULL;
    DELETE(tmp);
  }
  else cTmpTab = NULL;

  DELETE(cPPlan);
  if( cTmpTab == NULL ){
    WriteString(llError,
      "PayMgr: Cannot create tmp table in creating answer to Client");
    return CreateCommonMsg( KO, "Internal error");
  }

  //store temp payments table and get total sum of money to transfer
  amount = StorePayments(cTmpTab, myACID);
  cTmpTab->Remove(); 
  DELETE(cTmpTab);
  
  //check wheather everything is OK
  if( amount < 0 ){
    WriteString(llWarning,"PayMgr: Cannot store payments");
    return CreateCommonMsg( KO, "Cannot store payments");
  }
  if( amount > ca ){
    sprintf(str, "Too low amount due from client %ld / %ld", amount, ca);
    WriteString(llWarning, str);
    return CreateCommonMsg(KO, str);
  }

  //get money (amount) from client's account (if st. wrong, return KO msg)
  
  WriteString(llWarning,"PayMgr: Money transfer from client OK; amount: %ld", ca);
  return CreateCommonMsg( OK );
}


/**
 * Reacts to a request for payment from Eso.
 *
 * If it finds the payment record, the Eso ask for, and time of it is OK,
 * sends to Eso OAuth (to be able to compute MAC).
 *
 *
 * @param   inMsg message from eso
 * @return  message (challenge) for eso
 * @author  Ivana
 * @see     Majordomo::DRCRequestFromEso()
 */
GMessage *PayManager::RequestFromEso(GMessage *inMsg)
{
  ID payId;
  Record *r = NULL;
  TableField *tf = NULL;
  ESTime *time = NULL, payTime;
  Int payTimeOK = YES;
  MsgField *oAuth = NULL;
  GMessage *outMsg = NULL;

  //check argument and get usefull
  if( inMsg == NULL || inMsg->GetAsID(GM_PAYMENT_ID, &payId) != OK ){
    WriteString(llWarning,"PayMgr: Bad inMsg for creating challenge for Eso");
    return CreateCommonMsg( KO, "Bad inMsg for creating challenge for Eso");
  }

  //find the payment
  if( (r = FindInTable(payments, TF_PAY_ID, payId)) == NULL ){
    WriteString(llWarning,"PayMgr: Unknown payId: %ld", payId);
    return CreateCommonMsg(UNKNOWN_PAY_ID, NULL, &payId);
  }

  //get time and oAuth
  if( (tf = r->GetField(TF_TIME)) == NULL ) payTimeOK = NO;
  else payTime = ((TimeField *)tf)->GetESTimeValue();
  if( (tf = r->GetField(TF_O_AUTH)) != NULL )
    oAuth = ((BytesField *)tf)->GetAsMsgField();
  DELETE(r);

  //check time
  if( (time = ((Majordomo *)(six->majordomo))->GetTime()) == NULL ){
    WriteString(llFatalError,"PayMgr: Bad system time");
    if( oAuth ){ DELETE(oAuth);}
    return CreateCommonMsg(OTHER, "Bad bank's time", &payId);
  }
  if( payTimeOK && ((*time) < payTime) ){
    WriteString(llWarning,"PayMgr: Too early pay payId: %ld", payId);
    //fill "Time" field in outMsg
    DELETE(time);
    if( oAuth ){ DELETE(oAuth);}
    return CreateCommonMsg(BAD_TIME, NULL, &payId);
  }
  DELETE(time);

  //send paymentId, OAuth
  if( (outMsg = CreateCommonMsg(OK, NULL, &payId)) != NULL )
    outMsg->SetAsMsgField(GM_O_AUTHENTIFICATION, oAuth);
  if( oAuth ){ DELETE(oAuth);}

  return outMsg;
}


/**
 * Reacts to an answer from Eso.
 *
 * If it finds the payment record and MAC and SAuth are OK,
 * transfers money to Eso's account.
 *
 *
 * @param   inMsg message (response to challenge) from eso
 * @return  message (answer) for eso
 * @author  Ivana
 * @see     Majordomo::DRCAnswerFromEso()
 */
GMessage *PayManager::AnswerFromEso(GMessage *inMsg)
{
  ID payId;
  MsgField *esoSAuth = NULL, *esoMAC = NULL;
  BankPayRec *pRec = NULL;
  GMessage *outMsg = NULL;
  Char *amount = NULL;

  //check argument and get usefull
  if( inMsg == NULL || inMsg->GetAsID(GM_PAYMENT_ID, &payId) != OK ){
    WriteString(llWarning,"PayMgr: Bad inMsg for creating answer to Eso");
    return CreateCommonMsg(KO,"Bad inMsg for creating answer to Eso");
  }      
  if( (esoSAuth = inMsg->GetAsMsgField(GM_S_AUTHENTIFICATION)) == NULL ||
      (esoMAC = inMsg->GetAsMsgField(GM_MAC)) == NULL ||
//      (esoAccount = inMsg->GetAs...(GM_ACCOUNT)) == NULL ||
      (pRec = new BankPayRec()) == NULL ){
    if( esoMAC ){ DELETE(esoMAC);}
    if( esoSAuth ){ DELETE(esoSAuth);}
    WriteString(llWarning,
      "PayMgr: No SAuth or MAC in answer from Eso; payId: %ld", payId);
    return CreateCommonMsg(OTHER,
      "No SAuth or MAC in answer from Eso", &payId);
  }

  //find the payment
  if( pRec->FetchFromTable(payments, TF_PAY_ID, payId) != OK ){
    DELETE(esoMAC);
    DELETE(esoSAuth);
    DELETE(pRec);
    WriteString(llWarning,"PayMgr: Unknown payId: %ld", payId);
    return CreateCommonMsg(UNKNOWN_PAY_ID, NULL, &payId);
  }

  //compare MAC and SAuth
  if( pRec->MACEqualTo(esoMAC) )
    if( pRec->SAuthEqualTo(esoSAuth) ){
      //everything is OK, generate msg for Eso
      outMsg = CreateCommonMsg(OK, NULL, &payId);
      amount = pRec->GetAmount();
      //transfer money (amount) to Eso's account
      if( outMsg ) outMsg->SetAsString(GM_AMOUNT, amount);
      WriteString(llWarning,
        "PayMgr: Money transfered to Eso payId: %ld, amount: %s", payId, amount);
      if( amount ){ DELETE(amount);}
      (Void)DeleteRecsInTable(payments, TF_PAY_ID, payId);//!!!!!
    }
    else{
      //bad SAuth; generate msg for Eso
      outMsg = CreateCommonMsg(BAD_S_AUTH, NULL, &payId);
      WriteString(llWarning,"PayMgr: Bad SAuth payId: %ld", payId);
    }
  else{
    //bad MAC; generate msg for Eso
    outMsg = CreateCommonMsg(BAD_MAC, NULL, &payId);
    WriteString(llWarning,"PayMgr: Bad MAC payId: %ld", payId);
  }

  DELETE(esoMAC);
  DELETE(esoSAuth);
  DELETE(pRec);

  return outMsg;
}

