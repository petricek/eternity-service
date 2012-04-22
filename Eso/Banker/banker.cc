#include "./H/banker.h"


/**
 * Banker Constructor.
 *
 * Banker Constructor creates bankers' persistent tables.
 *
 * @param   logFile pointer to log file
 * @param   eso pointer to main class eso
 * @author  Ivana
 * @see     Eso
 */
Banker::Banker(LogFile *logFile, Eso *eso)
  :Debugable(logFile), Rollbackable(), SixOffspring(eso)
{
  pPlans = clientPPlan = payments = currPays = notPaidPays = NULL;
  if( CreatePPlans() != OK  ||  CreateClientPPlan() != OK  ||
      CreatePayments() != OK  ||  CreateCurrPays() != OK ||
      CreateNotPaidPays() != OK )
    WriteString(llFatalError,
      "Banker: Cannot create some table - banker has not created correctly");
  else
    WriteString(llDebug,"Banker: Created");
}


/**
 * Banker Destructor.
 *
 * Banker Destructor deletes bankers' persistent tables.
 *
 * @author  Ivana
 * @see     Eso
 */
Banker::~Banker()
{
  if( pPlans ){ DELETE(pPlans); }
  if( clientPPlan ){ DELETE(clientPPlan); }
  if( payments ){ DELETE(payments); }
  if( currPays ){ DELETE(currPays); }
  if( notPaidPays ){ DELETE(notPaidPays); }
  WriteString(llDebug,"Banker: Deleted");
}


/**
 * Rollback transaction.
 *
 * We don't have to cancel bank transactions, because they have TimeOuts.
 * After some time they will be resent. We only must rollback generated
 * payment plan (if any). TID is used as FileID!.
 *
 * @param   tId ID transaction to be rollbacked
 * @return  error code
 * @author  Ivana
 * @see     
 */
Err Banker::Rollback(TID tId)
{
  (Void)DeleteRecsInTable(pPlans, TF_TID, tId);
  (Void)DeleteRecsInTable(payments, TF_FILE_ID, tId);
  WriteString(llWarning,"Banker: Rollback done TID: %ld", tId);
  return OK;
};


/**
 * Generates payment plan for a client.
 *
 * First it check system time. If it is OK, count price for storrage
 * and for each pay period chooses bank and generate payment record. After
 * generating all pay records returns whole payment plan as new msgField.
 *
 * @param   size size of file, which will be stored
 * @param   timeToStore period for which is file stored
 * @param   tId current transactoin ID
 * @return  payment plan as a new msgField
 * @author  Ivana
 * @see     Majordomo::DRCRequestForStorage()
 */
MsgField *Banker::MakePayPlan(Size size, RelTime timeToStore, TID tId)
{
  ESTime *ptime=NULL, time;
  ID payId;
  GMessage *bankAC=NULL;
  Char *amount=NULL;
  EsoPayPlanRec *planRec=NULL;
  BytesMsgField *msgField=NULL;
  
  //is "SYS Time" OK?
  if( (ptime = ((Eso *)six)->scheduler->GetTime()) == NULL ){
    WriteString(llError,
      "Banker: Bad System Time for making the payPlan TID: %ld", tId);
    return NULL;
  }
  time = *ptime; DELETE(ptime);

  //count price for such a big file:
  amount = CountPrice(size, timeToStore);

  //now in the "loop" we generate payments
  //in dependence of <size> and <timeToStore>
  //(in 1. version we consider only one pay at the end)
  {
    time = (time + timeToStore);
    bankAC = DecideWhichBank();
    planRec = ( GeneratePayId(&payId) == OK ) ?
              new EsoPayPlanRec(payId, tId, time, bankAC, amount) : NULL;
    if( bankAC ){ DELETE(bankAC); }
    if( amount ){ DELETE(amount); }

    if( planRec == NULL  ||  planRec->IsOK() != OK  ||
        planRec->WriteToTable(pPlans) != OK  ||
        planRec->WriteToTable(clientPPlan) != OK  )
    {
      if( planRec ){ DELETE(planRec); }
      WriteString(llWarning,
        "Banker: Error in generating PayPlan; TID: %ld", tId);
      (Void)DeleteRecsInTable(pPlans, TF_TID, tId);
      (Void)DeleteRecsInTable(clientPPlan);
      return NULL;
    }

    if( planRec ){ DELETE(planRec); }
  }

  //table "clientPPlan" will be sent as msgField "PaymentPlan" to client
  Char *tmp = ((Eso *) six)->GetPath(CLIENT_P_PLAN_TAB); 
  if( tmp ){
    msgField = new BytesMsgField();
    msgField->SetAsFile(tmp);
    DELETE(tmp);
  }
  else msgField = NULL;

  //clear table clientPPlan for next client
  (Void)DeleteRecsInTable(clientPPlan);

  if( msgField )
    WriteString(llWarning,"Banker: PayPlan generated TID: %ld, velikost:%lu",
      tId, msgField->SizeOf());
  else
    WriteString(llWarning,"Banker: PayPlan has not generated TID: %ld", tId);

  return msgField;
}


/**
 * Deletes Payment Plan with given TID.
 *
 * Deletes Payment Plan with given TID. The reason of it may be:
 * first, client didn't send data (timeout from Scheduller),
 * second, client sent "wrong" data (too big, ...),
 * third, eso couldn't store sent file.
 *
 *
 * @param   tId transaction ID corresponding to the payment plan
 * @return  void
 * @author  Ivana
 * @see     Majordomo::DRCDataToStore()
 */
Void Banker::DelPayPlan(TID tId)
{
  (Void)DeleteRecsInTable(pPlans, TF_TID, tId);
  WriteString(llWarning,"Banker: PayPlan deleted OldTID: %ld", tId);
}


/**
 * Schedule Payments with given TID.
 *
 * Client sent file and eso stored it. So the banker has to schedule
 * payments. For each payment record from table generatedPayPlans
 * with given TID: first, writes it to table payments; second, set time out
 * for it.
 * 
 *
 * @param   tId transaction ID corresponding to the payment plan
 * @param   fileId ID od file, which has been stored
 * @return  error code
 * @author  Ivana
 * @see     Majordomo::DRCDataToStore()
 */
Err Banker::SchedulePays(TID tId, ID fileId)
{
  Payment *p=NULL;
  Int i=0;

  if( (p = new Payment()) == NULL ) return KO;

  //while we have records with given TID
  for( ; p->FetchAndDeleteFromTable(pPlans, TF_TID, tId) == OK ; ){
    //try to schedule the payment
    if( p->SetID(TF_FILE_ID, fileId) != OK ||
        SetPaymentTimeOut(p->GetID(TF_PAY_ID), p->GetTime()) != OK ||
        p->WriteToTable(payments) != OK ){
      //if st. wrong, clear tables
      DELETE(p);
      DeleteRecsInTable(payments, TF_FILE_ID, fileId);
      DeleteRecsInTable(pPlans, TF_TID, tId);
      WriteString(llWarning,
        "Banker: Cannot schedule some payment OldTID: %ld",
        tId);
      return KO;
    }
    p->Clear();
    i++;
  }
  DELETE(p);

  if( i>0 ) WriteString(llWarning,"Banker: PayPlan scheduled OldTID: %ld", tId);
  else WriteString(llWarning,"Banker: Nothing to schedule OldTID: %ld", tId);

  return OK;
}


/**
 * Time For Some Payment is up.
 *
 * When time of some payment is up, finds payment record with given payId,
 * generates request for bank including payment ID, sets timeout of current
 * payment and fills in table currentPayments.
 *
 * @param   inMsg message from scheduler
 * @param   tId current transaction ID
 * @param   bankACID ID of bank's access certificate (return parametr)
 * @return  message (request) for bank
 * @author  Ivana
 * @see     Majordomo::DRCTimeForPayment()
 */
GMessage *Banker::TimeForPayment(GMessage *inMsg, TID tId, ACID *bankACID)
{
  GMessage *outMsg=NULL;
  ID payId;
  CurrPay *p=NULL;

  //check arguments first
  if( inMsg == NULL || bankACID == NULL ) return NULL;

  if( inMsg->GetAsID(GM_PAYMENT_ID, &payId) != OK ){
    WriteString(llWarning,"Banker: No PayID for creating request for bank");
    return NULL;
  }
  if( (p = new CurrPay()) == NULL ){
    WriteString(llError,"Banker: Cannot create auxiliary structure");
    return NULL;
  }

  //find payment with given ID
  if( p->FetchAndDeleteFromTable(payments, TF_PAY_ID, payId) != OK ||
      p->SetID(TF_TID, tId) != OK ){
    WriteString(llWarning,"Banker: Bad or missing payment record in Payments in creating request for bank payID: %ld", payId);
    (Void)p->WriteToTable(notPaidPays,"Bad or missing payment record in Payments in creating request for bank");
    DELETE(p);
    return NULL;
  }
  //generate request for bank
  if( (outMsg = GenerateRequest(p)) == NULL ){
    WriteString(llWarning,
      "Banker: Fault in generating request for bank payID: %ld", payId);
    (Void)p->WriteToTable(notPaidPays,"Fault in generating request for bank");
  }
  else{
    //writes the payment to table "currently transacted payments"
    (Void)p->WriteToTable(currPays);
    WriteString(llWarning,"Banker: Request for bank generated payID: %ld", payId);
    (*bankACID) = p->GetID(TF_BANK_ACID);
  }
  DELETE(p);
  
  return outMsg;
}


/**
 * Reacts to a challenge from bank.
 *
 * As an answer to eso's 'request for payment' bank sends challenge.
 * If something goes wrong (bank does not know the payment, eso
 * requests for payment too early,...) bank does not send the key
 * for computing MAC.
 * If everything is OK (bank knows the payment and time is OK),
 * an OAuthentification is in challenge. On this OAuth and stored
 * client's file banker computes MAC. Then eso send this MAC
 * along with SAuthentification as a 'response to challenge' to bank.
 *
 *
 * @param   inMsg message (challenge) from bank
 * @param   tId current transaction ID
 * @param   bankACID ID of bank's access certificate (return parametr)
 * @return  message (response to challenge) for bank
 * @author  Ivana
 * @see     Majordomo::DRCChallengeFromBank()
 */
GMessage *Banker::ChallengeFromBank(GMessage *inMsg, TID tId, ACID *bankACID)
{
  GMessage *outMsg=NULL;
  ID payId;
  Int success;
  CurrPay *p=NULL;
  Char strErr[50], *bankReason=NULL;

  //check arguments first
  if ( inMsg == NULL || bankACID == NULL ) return NULL;
  
  if ( inMsg->GetAsID(GM_PAYMENT_ID, &payId) != OK ||
       inMsg->GetAsInt(GM_SUCCESS, &success) != OK ){
    WriteString(llWarning,
      "Banker: No PayID or 'Success' field in challenge from bank");
    return NULL;
  }
  if( (p = new CurrPay()) == NULL ){
    WriteString(llError,"Banker: Cannot create auxiliary structure");
    return NULL;
  }

  //find payment with given ID
  if( p->FetchFromTable(currPays, TF_PAY_ID, payId) != OK ||
      p->SetID(TF_TID, tId) != OK ||
      ((*bankACID) = p->GetID(TF_BANK_ACID)) == BAD_ID ){
    DELETE(p);
    WriteString(llWarning,
      "Banker: Bad or missing CurrPay record; payId: %ld", payId);
    return NULL;
  }

  //check state of the payment
  if( p->GetState() != REQUEST ){
    //nothing to do, we have alredy sent an answer
    DELETE(p);
    return NULL;
  }

  switch (success){
  //---------------
    case OK:
      //generate answer for bank
      outMsg = ChallengeOK(p, inMsg);
      (*bankACID) = p->GetID(TF_BANK_ACID);
      DELETE(p);
      if( outMsg ) WriteString(llWarning,
          "Banker: Answer for bank generated; payId: %ld", payId);
      else WriteString(llWarning,
          "Banker: Fault in generating answer for bank; payId: %ld", payId);
      return outMsg;

  //---------------
    case BAD_TIME:
      //start synchronization and schedule pay (not implemented now):
      ChallengeWrongTime(p, inMsg);
      DELETE(p);
      WriteString(llWarning,
        "Banker: Bad time in challenge from bank; payId: %ld", payId);
      return NULL;

  //---------------
    case UNKNOWN_PAY_ID:
      strcpy(strErr, "Unknown PaymentId"); break;

  //---------------
    case OTHER:
      strcpy(strErr, "'");
      if( (bankReason = inMsg->GetAsString(GM_REASON_OF_FAIL)) != NULL ){
        strcat(strErr, bankReason);
        DELETE(bankReason);
      }
      strcat(strErr, "'"); break;

  //---------------
    default:
      strcpy(strErr, "Bad 'Success' field"); break;
  }

  //something wrong was happen ...
  ChallengeKO(p, strErr);
  DELETE(p);
  return NULL;
}


/**
 * Reacts to an answer from bank.
 *
 * If everything is OK, writes to log file and deletes records in temporary
 * table. Otherwise writes to Not Paid Pays table and warn administrator
 * of the eso.
 *
 * @param   inMsg message (answer) from bank
 * @return  void
 * @author  Ivana
 * @see     Majordomo::DRCAnswerFromBank()
 */
Void Banker::AnswerFromBank(GMessage *inMsg)
{
  ID payId;
  Int success;
  CurrPay *p=NULL;
  Char strErr[50], *bankReason=NULL;

  //check arguments first
  if ( inMsg == NULL ) return ;
  
  if ( inMsg->GetAsID(GM_PAYMENT_ID, &payId) != OK ||
       inMsg->GetAsInt(GM_SUCCESS, &success) != OK ){
    WriteString(llWarning,
      "Banker: No PayID or 'Success' field in answer from bank");
    return ;
  }
  if( (p = new CurrPay()) == NULL ){
    WriteString(llError,"Banker: Cannot create auxiliary structure");
    return ;
  }

  //find payment with given ID
  if( p->FetchFromTable(currPays, TF_PAY_ID, payId) != OK ){
    DELETE(p);
    WriteString(llWarning,
      "Banker: Bad or missing CurrPay record; payId: %ld", payId);
    return ;
  }

  //check state of the payment
  if( p->GetState() != ANSWER ){
    DELETE(p);
    return ;
  }

  switch (success){
  //---------------
    case OK:
      //do final operations (write to log, ...)
      AnswerOK(p, inMsg);
      DELETE(p);
      return ;

  //---------------
    case UNKNOWN_PAY_ID:
      strcpy(strErr, "Unknown PaymentId"); break;

  //---------------
    case BAD_MAC:
      strcpy(strErr, "Bad MAC"); break;

  //---------------
    case BAD_S_AUTH:
      strcpy(strErr, "Bad SAuth"); break;

  //---------------
    case OTHER:
      strcpy(strErr, "'");
      if( (bankReason = inMsg->GetAsString(GM_REASON_OF_FAIL)) != NULL ){
        strcat(strErr, bankReason);
        DELETE(bankReason);
      }
      strcat(strErr, "'");
 break;

  //---------------
    default:
      strcpy(strErr, "Bad 'Success' field"); break;
  }

  //something wrong was happen ...
  AnswerKO(p, strErr);
  DELETE(p);
}


/**
 * Reacts to a timeout of some payment.
 *
 * When time of some current payment is up, finds payment record
 * with given payId. If number of tries to get money from bank is
 * still under limit, generates request or answer (depend on 'state' field)
 * for bank and again sets timeout.
 *
 *
 * @param   inMsg message from scheduler
 * @param   tId current transaction ID
 * @param   bankACID ID of bank's access certificate (return parametr)
 * @param   request wheather state is request or answer;
 *          parametr can have values YES or NO (retrun parametr)
 * @return  message for bank
 * @author  Ivana
 * @see     Majordomo::DRCTimeOutPayment()
 */
GMessage *Banker::TimeOutPayment(GMessage *inMsg, TID tId,
  ACID *bankACID, Int *request)
{
  GMessage *outMsg=NULL;
  ID payId;
  Int state, tableState, tries;
  CurrPay *p=NULL;

  //check argument first
  if( inMsg == NULL || bankACID == NULL || request == NULL)
    return NULL;
  
  if( inMsg->GetAsID(GM_PAYMENT_ID, &payId) != OK  ||
      inMsg->GetAsInt(GM_STATE, &state) != OK ){
    WriteString(llWarning,"Banker: No PayID or 'StateOfPay' from scheduler for creating request/answer to bank");
    return NULL;
  }
  if( (p = new CurrPay()) == NULL ){
    WriteString(llError,"Banker: Cannot create auxiliary structure");
    return NULL;
  }

  //find payment with given ID
  if( p->FetchFromTable(currPays, TF_PAY_ID, payId) != OK ||
      p->SetID(TF_TID, tId) != OK ){
    WriteString(llWarning,"Banker: Bad or missing record in CurrentPays in creating request/answer to bank; payId: %ld", payId);
    (Void)DeleteRecsInTable(currPays, TF_PAY_ID, payId);
    (Void)p->WriteToTable(notPaidPays,"Bad or missing record in CurrentPays (in creating request/answer to bank)");
    DELETE(p);
    return NULL;
  }

  //if something will go wrong no destruction operations needed!
  tableState = p->GetState();
  tries = p->NumOfTries() + 1;//old numOfTries plus this try
  (*bankACID) = p->GetID(TF_BANK_ACID);

  //according to state of payment in table do ...

  if(state == REQUEST  &&  tableState == REQUEST){
    (*request) = YES;
    //try to send request again ...
    outMsg = TimeOutCurrPay(p);
    DELETE(p);
    if( outMsg == NULL ){
      WriteString(llWarning,
        "Banker: Cannot generate request for bank - %d. try; payId: %ld",
        tries, payId);
      return NULL;
    }
    WriteString(llWarning,
      "Banker: Request for bank generated - %d. try; payId: %ld", tries, payId);
    return outMsg;
  }

  if(state == ANSWER  &&  tableState == ANSWER){
    (*request) = NO;
    //try to send answer again ...
    outMsg = TimeOutCurrPay(p);
    DELETE(p);
    if( outMsg == NULL ){
      WriteString(llWarning,
        "Banker: Cannot generate answer for bank - %d. try; payId: %ld",
        tries, payId);
      return NULL;
    }
    WriteString(llWarning,
      "Banker: Answer for bank generated - %d. try; payId: %ld", tries, payId);
    return outMsg;
  }

  //"else" probably timeOut of REQUEST, when we are alredy in "ANSWER mode"
  
  DELETE(p);
  return NULL;
}


/**
 * Deletes all records with given file ID.
 *
 * If administrator wants to delete a file (the reason can be, that
 * there are no payments for it), this method is used. It deletes
 * all records (in all tables) with given file ID.
 *
 * @param   fileId ID of deleted file records
 * @return  void
 * @author  Ivana
 * @see     DeleteRecsPayID
 */
Void Banker::DeleteRecsFileID(ID fileId)
{
  (Void)DeleteRecsInTable(payments, TF_FILE_ID, fileId);
  (Void)DeleteRecsInTable(currPays, TF_FILE_ID, fileId);
  (Void)DeleteRecsInTable(notPaidPays, TF_FILE_ID, fileId);
  WriteString(llWarning,"Banker: Records with fileId: %ld deleted", fileId);
}


/**
 * Deletes all records with file ID which correspond to given payment ID.
 *
 * If administrator wants to delete a file, which correspond to given
 * payment ID (the reason can be, that there are no payments for it),
 * this method is used. It deletes all records (in all tables) with file ID
 * which correspond to given payment ID.
 *
 * @param   payId ID of payment that correspond to deleted file records
 * @return  void
 * @author  Ivana
 * @see     DeleteRecsFileID()
 */
Void Banker::DeleteRecsPayID(ID payId)
{
  ID fileId;

  Record *r = NULL;
  payments->Open();

  //try to find fileId ...

  //...in payments
  if ((r=payments->FindFirst(TF_PAY_ID, &payId)) == NULL){
    payments->Close();
    currPays->Open();
    //...in currPays
    if((r=currPays->FindFirst(TF_PAY_ID, &payId)) == NULL){
      currPays->Close();
      notPaidPays->Open();
      //...in notPaidPays
      if((r=notPaidPays->FindFirst(TF_PAY_ID, &payId)) == NULL){
        //no such a record - give it up
        notPaidPays->Close();
        return;
      }
      else notPaidPays->Close();
    }
    else currPays->Close();
  }
  else payments->Close();

  fileId = ((IDField *)r->GetField(TF_FILE_ID))->GetIDValue();
  DELETE(r);

  //delete records (in all tables) with given fileId
  DeleteRecsFileID(fileId);
}
