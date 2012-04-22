#include "./H/banker.h"
#include "./../../Common/Utils/H/createTables.h"

/**
 * Creates Payment Plan table.
 *
 * Creates (or connects to exist.) Payment Plan table.
 *
 * @param   void
 * @return  error code
 * @author  Ivana
 * @see     Banker()
 */
Err Banker::CreatePPlans()
{
  TableFieldQueue *fq=NULL;

  if( (fq = new TableFieldQueue()) == NULL ) return KO;
  fq->Insert( new IDField(TF_TID, BAD_ID) );
  fq->Insert( new TimeField(TF_TIME) );
  fq->Insert( new IDField(TF_PAY_ID, BAD_ID) );
  fq->Insert( new IDField(TF_BANK_ACID, BAD_ID) );
  fq->Insert( new StringField(TF_AMOUNT, NULL) );
  fq->Insert( new BytesField(TF_S_AUTH, NULL) );

  Char *tmp = ((Eso *) six)->GetPath(P_PLANS_TAB);
  pPlans = ( tmp == NULL ) ? NULL : new Table(tmp, fq);
  if( tmp ){ DELETE(tmp);}
  DELETE(fq);
  return ( pPlans == NULL ) ?  KO : OK;
}

/**
 * Creates Client Payment Plan table.
 *
 * Creates (or connects to exist.) Client Payment Plan table.
 * This table is common to eso and client so it calls CreateEsoToClientTable()
 * function to create it.
 *
 *
 * @param   void
 * @return  error code
 * @author  Ivana
 * @see     Banker(), CreateEsoToClientTable()
 */
Err Banker::CreateClientPPlan()
{
  Char *tmp = ((Eso *) six)->GetPath(CLIENT_P_PLAN_TAB); 
  if( tmp == NULL ) return KO;
  
  //this one is common to eso and client
  //so the function, which creates it, is in COMMON things
  clientPPlan = CreateEsoToClientTable(tmp);
  DELETE(tmp);
  return ( clientPPlan == NULL ) ? KO : OK;
}

/**
 * Creates Payments table.
 *
 * Creates (or connects to exist.) Payments table.
 *
 * @param   void
 * @return  error code
 * @author  Ivana
 * @see     Banker()
 */
Err Banker::CreatePayments()
{
  TableFieldQueue *fq=NULL;

  if( (fq = new TableFieldQueue()) == NULL ) return KO;
  fq->Insert( new TimeField(TF_TIME) );
  fq->Insert( new IDField(TF_PAY_ID, BAD_ID) );
  fq->Insert( new IDField(TF_FILE_ID, BAD_ID) );
  fq->Insert( new IDField(TF_BANK_ACID, BAD_ID) );
  fq->Insert( new StringField(TF_AMOUNT, NULL) );
  fq->Insert( new BytesField(TF_S_AUTH, NULL) );

  Char *tmp = ((Eso *) six)->GetPath(PAYMENTS_TAB);
  payments = ( tmp == NULL ) ? NULL : new Table(tmp, fq);
  if( tmp ){ DELETE(tmp);}
  DELETE(fq);
  return ( payments == NULL ) ?  KO : OK;
}

/**
 * Creates Current Payments table.
 *
 * Creates (or connects to exist.) Current Payments table.
 *
 * @param   void
 * @return  error code
 * @author  Ivana
 * @see     Banker()
 */
Err Banker::CreateCurrPays()
{
  TableFieldQueue *fq=NULL;

  if( (fq = new TableFieldQueue()) == NULL ) return KO;
  fq->Insert( new IDField(TF_TID, BAD_ID) );
  fq->Insert( new TimeField(TF_TIME) );
  fq->Insert( new IDField(TF_PAY_ID, BAD_ID) );
  fq->Insert( new IDField(TF_FILE_ID, BAD_ID) );
  fq->Insert( new IDField(TF_BANK_ACID, BAD_ID) );
  fq->Insert( new StringField(TF_AMOUNT, NULL) );
  fq->Insert( new BytesField(TF_S_AUTH, NULL) );
  fq->Insert( new BytesField(TF_O_AUTH, NULL) );
  fq->Insert( new IntField(TF_STATE, 3) );
  fq->Insert( new BytesField(TF_MAC, NULL) );
  fq->Insert( new IntField(TF_NUM_OF_TRIES, 0) );

  Char *tmp = ((Eso *) six)->GetPath(CURR_PAYS_TAB);
  currPays = ( tmp == NULL ) ? NULL : new Table(tmp, fq);
  if( tmp ){ DELETE(tmp);}
  DELETE(fq);
  return ( currPays == NULL ) ?  KO : OK;
}

/**
 * Creates Not Paid Payments table.
 *
 * Creates (or connects to exist.) Not Paid Payments table.
 *
 * @param   void
 * @return  error code
 * @author  Ivana
 * @see     Banker()
 */
Err Banker::CreateNotPaidPays()
{
  TableFieldQueue *fq=NULL;

  if( (fq = new TableFieldQueue()) == NULL ) return KO;
  fq->Insert( new TimeField(TF_TIME) );
  fq->Insert( new IDField(TF_PAY_ID, BAD_ID) );
  fq->Insert( new IDField(TF_FILE_ID, BAD_ID) );
  fq->Insert( new IDField(TF_BANK_ACID, BAD_ID) );
  fq->Insert( new StringField(TF_AMOUNT, NULL) );
  fq->Insert( new IntField(TF_NUM_OF_TRIES, 0) );
  fq->Insert( new StringField(TF_REASON, NULL) );

  Char *tmp = ((Eso *) six)->GetPath(NOT_PAID_PAYS_TAB);
  notPaidPays = ( tmp == NULL ) ? NULL : new Table(tmp, fq);
  if( tmp ){ DELETE(tmp);}
  DELETE(fq);
  return ( notPaidPays == NULL ) ?  KO : OK;
}


/**
 * Updates table Current Payments.
 *
 * Updates a record in the table CurrentPayments with new TID and NumOfTries.
 *
 * @param   p current pay record
 * @return  error code
 * @author  Ivana
 * @see     TimeOutCurrPay()
 */
Err Banker::PartialUpdateCurrPays(CurrPay *p)
{
  ID payId;
  TID tId;
  Int num;
  Record *r=NULL;
  Err e;

  if( p==NULL || currPays==NULL || currPays->Open()!=OK ) return KO;

  payId = p->GetID(TF_PAY_ID);
  tId = p->GetID(TF_TID);
  num = p->NumOfTries();
  if( (r = currPays->FindFirst(TF_PAY_ID, &payId)) == NULL ){
    payments->Close();
    return KO;
  }

  r->SetField(TF_TID, &tId);
  r->SetField(TF_NUM_OF_TRIES, &num);
  e = currPays->UpdateRecord(r);
  DELETE(r);
  if( currPays->Close()==OK && e==OK ) return OK;
  else return KO;
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
 * @see     ChallengeOK, ChallengeKO, AnswerOK, AnswerKO, TimeOutCurrPay()
 */
Err Banker::DeleteRecsInTable(Table *t, Char *whichId, ID id)
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
 * Generates Pay ID.
 *
 * Generates unique payment ID.
 *
 * @param   payId return parametr
 * @return  error code
 * @author  Ivana
 * @see     MakePayPlan()
 */
Err Banker::GeneratePayId(ID *payId)
{
  UniqueIDGenerator *payIdGenerator = NULL;
  Err e = OK;
  if( payId == NULL ) return KO;
  (*payId) = BAD_ID;

  Char *tmp = ((Eso *) six)->GetPath(PAY_ID_GEN_TAB);
  if( tmp == NULL ) return KO;
  
  payIdGenerator = new UniqueIDGenerator(tmp, 1);
  DELETE(tmp);
  if( payIdGenerator == NULL ) return KO;
  e = payIdGenerator->GetID( payId );
  
  DELETE(payIdGenerator);
  return e;
}


/**
 * Counts price for stored data.
 *
 * From size and timeToStore counts price for stored data.
 * Returns it as a string.
 *
 * @param   size size of stored file
 * @param   timeToStore period of storrage of sent file
 * @return  amount of money
 * @author  Ivana
 * @see     MakePayPlan()
 */
Char *Banker::CountPrice(Size /*size*/, RelTime /*timeToStore*/)
{
  Char *c=NULL;

  if( (c = new Char[4]) == NULL ) return NULL;
  strcpy(c, "100");
  return c;
}


/**
 * Chooses a bank.
 *
 * Decide which bank we use when generating PayPlan.
 * Now randomly selects a bank AC (later it should be more sophisticated).
 *
 * @return  bank access certificate
 * @author  Ivana
 * @see     MakePayPlan()
 */
GMessage *Banker::DecideWhichBank()
{
  Queue *q=NULL;
  GMessage *bankAC=NULL, *gm=NULL;
  if( (q = ((Eso *)six)->majordomo->GetACs(ACTYPE_BANK, 1)) == NULL )
    return NULL;

  bankAC = (GMessage *)q->DeleteAt(1);

  //discard the queue
  for( gm = (GMessage *)q->DeleteAt(1) ; gm != NULL ;
       gm = (GMessage *)q->DeleteAt(1) ){
    DELETE(gm);
  }
  DELETE(q);

  return bankAC;
}


/**
 * Sets payment time out.
 *
 * Sets time out on the date the payment should be carried out.
 *
 * @param   payId payment ID
 * @param   time timeOut time
 * @return  error code
 * @author  Ivana
 * @see     SchedulePays()
 */
Err Banker::SetPaymentTimeOut(ID payId, ESTime time)
{
  GMessage *info=NULL;

  if( (info = new GMessage()) == NULL ) return KO;

  //generate gMessage for scheduler
  if( info->SetAsID(GM_PAYMENT_ID, payId) != OK ){
    DELETE(info);
    return KO;
  }

  //force scheduler to set timeOut
  ((Eso *)six)->scheduler->SetTimeOut(TO_PAYMENT, info, time);

  DELETE(info);
  return OK;
}

/**
 * Sets current payment time out.
 *
 * Sets time out of current payment to specified time.
 *
 * @param   payId current payment ID
 * @param   state state of payemnt (REQUEST/ANSWER)
 * @return  error code
 * @author  Ivana
 * @see     GenerateRequest(), GenerateAnswer()
 */
Err Banker::SetCurrPayTimeOut(ID payId, Int state)
{
  GMessage *info=NULL;
  RelTime relTime;

  if( (info = new GMessage())==NULL ) return KO;

  //generate gMessage for scheduler
  if( info->SetAsID(GM_PAYMENT_ID, payId) != OK ||
      info->SetAsInt(GM_STATE, state) != OK ) {
    DELETE(info);
    return KO;
  }

  //set timeOut to defined time
//  ((BasicTime *)&relTime).SetTime(TO_BANKER_REL_TIMEOUT);
  relTime.SetTime(TO_BANKER_REL_TIMEOUT);

  //force scheduler to set timeOut
  ((Eso *)six)->scheduler->SetTimeOut(TO_CURR_PAY, info, relTime);

  DELETE(info);
  return OK;
}

/**
 * Generates message for bank.
 *
 * Generates request for bank including payId.
 *
 * @param   p current payment structure
 * @return  message for bank
 * @author  Ivana
 * @see     TimeForPayment(), TimeOutCurrPay()
 */
GMessage *Banker::GenerateRequest(CurrPay *p)
{
  GMessage *dataMsg=NULL;
  ID payId;

  if( p == NULL || (dataMsg = new GMessage()) == NULL ) return NULL;

  payId = p->GetID(TF_PAY_ID);
  //generate gMessage with given payID and set timeout
  if( dataMsg->SetAsID(GM_PAYMENT_ID, payId) != OK ||
      SetCurrPayTimeOut(payId, REQUEST) != OK ){
    DELETE(dataMsg);
  }

  return dataMsg;
}

/**
 * Generates message for bank.
 *
 * Generates answer for bank, including payId, MAC, SAuthentification.
 *
 * @param   p current payment structure
 * @return  message for bank
 * @author  Ivana
 * @see     ChallengeOK(), TimeOutCurrPay()
 */
GMessage *Banker::GenerateAnswer(CurrPay *p)
{
  GMessage *dataMsg=NULL;
  MsgField *sAuth=NULL, *mac=NULL;
  ID payId;

  if( p==NULL || (sAuth=p->GetAuth(TF_S_AUTH))==NULL ||
      (mac=p->GetMAC())==NULL || (dataMsg=new GMessage())==NULL ){
    if( sAuth ){ DELETE(sAuth); }
    if( mac ){ DELETE(mac); }
    return NULL;
  }

  payId = p->GetID(TF_PAY_ID);
  //generate gMessage with given payID, sAuth and mac and set timeout
  if( dataMsg->SetAsID(GM_PAYMENT_ID, payId) != OK ||
      dataMsg->SetAsMsgField(GM_S_AUTHENTIFICATION, sAuth) != OK ||
      dataMsg->SetAsMsgField(GM_MAC, mac) != OK ||
      SetCurrPayTimeOut(payId, ANSWER) != OK ){
    DELETE(dataMsg);
  }
  DELETE(sAuth);
  DELETE(mac);

  return dataMsg;
}

/**
 * Reaction to a 'OK challenge' from bank.
 *
 * Reacts to a challenge from bank; success is OK - bank knows the
 * payment it counts MAC and writes it (with OAuth) to CurrPays.
 *
 * @param   p current payment structure
 * @param   inMsg message from bank
 * @return  message for bank
 * @author  Ivana
 * @see     ChallengeFromBank()
 */
GMessage *Banker::ChallengeOK(CurrPay *p, GMessage *inMsg)
{
  MsgField *oAuth=NULL, *mac=NULL;
  GMessage *outMsg=NULL;
  Err e;

  if( p==NULL || inMsg==NULL ) return NULL;

  (Void)p->SetState(ANSWER);

  //get oAuthentification
  oAuth = inMsg->GetAsMsgField(GM_O_AUTHENTIFICATION);
  if ( oAuth == NULL ){
    (Void)p->WriteToTable(notPaidPays, "No OAuth in challenge from bank");
    WriteString(llWarning,"Banker: No OAuth in challenge from bank");
    return NULL;
  }

  if( p->SetAuth(TF_O_AUTH, oAuth)!=OK ){
    DELETE(oAuth);
    (Void)p->WriteToTable(notPaidPays, "Cannot set OAuth");
    WriteString(llWarning,"Banker: Cannot set OAuth; payId: %ld",
      p->GetID(TF_PAY_ID));
    return NULL;
  }

  //compute MAC over oAuth and data
  e = ((Eso *)six)->tcbWrapper->ComputeMAC(p->GetID(TF_FILE_ID), oAuth, &mac);
  DELETE(oAuth);
  if( e!=OK || p->SetMAC(mac)!=OK ){
    DELETE(mac);
    (Void)p->WriteToTable(notPaidPays, "Cannot compute MAC");
    WriteString(llWarning,"Banker: Cannot compute MAC; payId: %ld",
      p->GetID(TF_PAY_ID));
    return NULL;
  }

  outMsg = GenerateAnswer(p);
  if( outMsg == NULL || p->UpdateTable(currPays) != OK){
    WriteString(llWarning,"Banker: Fault in generating answer for bank or updating CurrPays payID: %ld", p->GetID(TF_PAY_ID));
    (Void)DeleteRecsInTable(currPays, TF_PAY_ID, p->GetID(TF_PAY_ID));
    (Void)p->WriteToTable(notPaidPays,
      "Fault in generating answer for bank or updating CurrPays");
    return NULL;
  }

  return outMsg;
}

/**
 * Reaction to a 'BadTime challenge' from bank.
 *
 * Reacts to a challenge from bank. Bank has different time - warn 
 * synchronizer (not implemented now).
 *
 * @param   p current payment structure
 * @param   inMsg message from bank
 * @return  void
 * @author  Ivana
 * @see     ChallengeFromBank()
 */
Void Banker::ChallengeWrongTime(CurrPay * /*p*/, GMessage * /*inMsg*/)
{/*
  ESTime est;

  if( inMsg->GetAsBasicTime(GM_TIME, &est) == OK ){
    synchronizer->BadTime(est);
    return;
  }
  synchronizer->BadTime();

  SetPaymentTimeOut(p->GetID(TF_PAY_ID), p->GetTime());

  */
}

//-------------------------------------------------------------------------
// Reacts to a challenge from bank (3)
// bank has more payments with given ID (not implemented now)
//-------------------------------------------------------------------------


/**
 * Reaction to a 'KO challenge' from bank.
 *
 * Reacts to a challenge from bank. Something is wrong - write 
 * to notPaidPays and logFile.
 *
 * @param   p current payment structure
 * @param   reason reason of fail
 * @return  void
 * @author  Ivana
 * @see     ChallengeFromBank()
 */
Void Banker::ChallengeKO(CurrPay *p, Char *reason)
{
  if( p == NULL ) return;
  (Void)p->WriteToTable(notPaidPays, reason);
  WriteString(llWarning, "Banker: %s in challenge from bank; payId: %ld",
    reason, p->GetID(TF_PAY_ID));
  (Void)DeleteRecsInTable(currPays, TF_PAY_ID, p->GetID(TF_PAY_ID));
}

/**
 * Reaction to a 'OK answer' from bank.
 *
 * Reacts to an answer from bank. Success is OK - bank knows the 
 * payment, MAC and SAuth are correct.
 *
 * @param   p current payment structure
 * @param   inMsg message from bank
 * @return  error code
 * @author  Ivana
 * @see     AnswerFromBank()
 */
Err Banker::AnswerOK( CurrPay *p, GMessage *inMsg)
{
  ID payId;
  Char *amount=NULL, *bankAmount=NULL;
  Long a, ba;

  if( p == NULL || inMsg == NULL ) return KO;

  payId = p->GetID(TF_PAY_ID);

  //get amount, that should be paid
  amount = p->GetAmount();
  //get amount sent by bank
  bankAmount = inMsg->GetAsString(GM_AMOUNT);

  //delete the record in current payments table
  (Void)DeleteRecsInTable(currPays, TF_PAY_ID, payId);

  //check wheather any amount was paid
  if( bankAmount == NULL ){
    if( amount == NULL ){
      WriteString(llWarning,
       "Banker: No Amount in answer from bank; payId: %ld, should be paid: %s",
       payId, "???");
      return KO;
    }
    WriteString(llWarning,
      "Banker: No Amount in answer from bank; payId: %ld, should be paid: %s",
      payId, amount);
    DELETE(amount);
    return KO;
  }
  if( amount == NULL ){
    WriteString(llWarning,
      "Banker: Bank has paid %s; payId: %ld", bankAmount, payId);
    DELETE(bankAmount);
    return KO;
  }

  //check wheather right amount was paid
  a = atol(amount);
  ba = atol(bankAmount);
  DELETE(amount);
  DELETE(bankAmount);
  if( a > ba )
    WriteString(llWarning,"Banker: Bank has paid less -  payID: %ld, bank amount: %ld, right amount: %ld", payId, ba, a);
  else if( a < ba )
    WriteString(llWarning,"Banker: Bank has paid more - payID: %ld, bank amount: %ld, right amount: %ld", payId, ba, a);
  else{
    WriteString(llWarning,
      "Banker: Bank has paid correctly amount: %ld - payID: %ld", a, payId);
    return OK;
  }
  return KO;
}

/**
 * Reaction to a 'KO answer' from bank.
 *
 * Reacts to an answer from bank. Something is wrong - write to 
 * notPaidPays and logFile, deletes the record in current pays.
 *
 * @param   p current payment structure
 * @param   reason reason of fail
 * @return  void
 * @author  Ivana
 * @see     AnswerFromBank()
 */
Void Banker::AnswerKO( CurrPay *p, Char *reason)
{
  ID payId;

  if( p == NULL ) return;
  payId = p->GetID(TF_PAY_ID);
  (Void)p->WriteToTable(notPaidPays, reason);
  WriteString(llWarning, "Banker: %s in answer from bank; payId: %ld",
    reason, payId);
  (Void)DeleteRecsInTable(currPays, TF_PAY_ID, payId);
}

/**
 * TimeOut of some current payment.
 *
 * TimeOut of some current payment has gone.
 *
 * @param   p current payment structure
 * @return  message for bank or NULL
 * @author  Ivana
 * @see     TimeOutPayment()
 */
GMessage *Banker::TimeOutCurrPay(CurrPay *p)
{
  ID payId;
  GMessage *outMsg=NULL;
  Int maxTries=MAX_BANKER_TRIES;//rather get from config file

  if( p == NULL ) return NULL;

  //increase numOfTries and check if it is not too big
  if( p->NumOfTries('+') > maxTries ){
    //give it up
    WriteString(llWarning,
      "Banker: Too many tries of creating request/answer to bank; payId: %ld",
      payId);
    (Void)DeleteRecsInTable(currPays, TF_PAY_ID, payId);
    (Void)p->WriteToTable(notPaidPays,
      "Too many tries of creating request/answer to bank");
    return NULL;
  }

  //update TID and numOfTies in current payments table
  if( PartialUpdateCurrPays(p) != OK ){
    payId = p->GetID(TF_PAY_ID);
    //probably missing record
    WriteString(llError,
      "Banker: Cannot update record in table CurrPays; payId: %ld", payId);
    (Void)DeleteRecsInTable(currPays, TF_PAY_ID, payId);
    (Void)p->WriteToTable(notPaidPays,
      "Cannot update record in table CurrPays");
    return NULL;
  }

  payId = p->GetID(TF_PAY_ID);
  //let generate request/answer msg
  if( p->GetState() == REQUEST ) outMsg = GenerateRequest( p );
  else if( p->GetState() == ANSWER ) outMsg = GenerateAnswer( p );
  else outMsg = NULL;

  if( outMsg == NULL ){
    WriteString(llWarning,
      "Banker: Problems with generating request/answer for bank; payId: %ld",
      payId);
    (Void)DeleteRecsInTable(currPays, TF_PAY_ID, payId);
    (Void)p->WriteToTable(notPaidPays,
      "Problems with generating request/answer for bank");
    return NULL;
  }

  return outMsg;
}
