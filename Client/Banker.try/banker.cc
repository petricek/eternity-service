//#include <stdlib.h>

#include "./H/banker.h"
#include "./../../Common/Utils/H/createTables.h"
#include "./../Majordomo/H/majordomo.h"

//----vyhodit--------
//extern TID GetTID();
//extern MsgField *ComputeMAC(MsgField *oAuth);
//extern GMessage *CreateSendDataMsg(char *, GMessage *, GMessage *, int);
//extern Err EndTransaction();
//extern Err BeginTransaction(TID *);
//-------------------


/**
 * Create PayPlan Table.
 *
 * Creates table PayPlan - a persistent Banker's table.
 *
 *
 * @return  error code
 * @author  Ivana
 * @see     Banker()
 */
Err Banker::CreatePayPlan()
{
  TableFieldQueue *fq;

  if( (fq = new TableFieldQueue()) == NULL ) return KO;
  fq->Insert( new IDField(TF_TID, BAD_ID) );
  fq->Insert( new TimeField(TF_TIME) );
  fq->Insert( new IDField(TF_PAY_ID, BAD_ID) );
  fq->Insert( new IDField(TF_BANK_ACID, BAD_ID) );
  fq->Insert( new BytesField(TF_BANK_AC, NULL) );
  fq->Insert( new StringField(TF_AMOUNT, NULL) );
  fq->Insert( new BytesField(TF_SEAL_S_AUTH, NULL) );
  fq->Insert( new BytesField(TF_O_AUTH, NULL) );
  fq->Insert( new BytesField(TF_MAC, NULL) );

  Char *tmp = ((Client *) six)->GetPath(PAY_PLAN_TAB);
  payPlan = ( tmp == NULL ) ? NULL : new Table(tmp, fq);
  if( tmp ){ DELETE(tmp);}
  DELETE(fq);
  return ( payPlan == NULL ) ? KO : OK;
}


/**
 * Create BankPPlan Table.
 *
 * Creates table BankPPlan - a persistent Banker's table.
 *
 *
 * @return  error code
 * @author  Ivana
 * @see     Banker()
 */
Err Banker::CreateBankPPlan()
{
  TableFieldQueue *fq;

  if( (fq = new TableFieldQueue()) == NULL ) return KO;
  fq->Insert( new IDField(TF_TID) );
  fq->Insert( new BytesField(TF_BANK_P_PLAN) );
  fq->Insert( new BytesField(TF_BANK_AC) );
  fq->Insert( new StringField(TF_AMOUNT) );
//  fq->Insert( new BytesField(TF_ACCOUNT) );

  Char *tmp = ((Client *) six)->GetPath(BANK_PPLAN_TAB);
  bankPPlan = ( tmp == NULL ) ? NULL : new Table(tmp, fq);
  if( tmp ){ DELETE(tmp);}
  DELETE(fq);
  return ( bankPPlan == NULL ) ? KO : OK;
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
 * @see     
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
 * Finds and deletes record in the table.
 *
 * Finds and deletes record in the table with given ID.
 *
 *
 * @param   t table in which we will search
 * @param   whichId specifies ID, that is used for searching
 * @param   id a value of the ID
 * @return  found record or NULL
 * @author  Ivana
 * @see     PrepareMsgsForBanks()
 */
Record *Banker::FindAndDelInTable(Table *t, Char *whichId, ID id)
{
  Record *r=NULL;

  if( t == NULL || whichId == NULL || t->Open() != OK )
    return NULL;

  if( (r = t->FindFirst(whichId, &id)) != NULL )
    t->DeleteRecord();

  (Void)t->Close();

  return r;
}


/**
 * Stores payments from given table.
 *
 * For each record from table esoPPlan generates OAuth and compute MAC.
 * Then writes the record to table payPlan.
 *
 *
 * @param   esoPPlan table of payment plan sent by Eso
 * @param   tId current transaction ID
 * @return  error code
 * @author  Ivana
 * @see     MakePPlanForBanks()
 */
Err Banker::StorePayPlan(Table *esoPPlan, TID tId)
{
  ClientPayPlanRec *pPlanRec = NULL;
  Err e;
  MsgField *mac = NULL, *oAuth = NULL;
  Majordomo *majordomo;

  if( esoPPlan == NULL ) return KO;

  if( (pPlanRec = new ClientPayPlanRec()) == NULL ){
      WriteString(llError, "Banker: Cannot generate auxiliary structure");
    return KO;
  }

  pPlanRec->SetTID(tId);

  majordomo = (Majordomo *)((Client *)six)->majordomo;
  
  //for each record from table esoPPlan ...
  for( ; pPlanRec->FetchAndDeleteFromTable(esoPPlan) == OK ;  ){
    //generate OAuth, compute MAC
    if( (oAuth = pPlanRec->GenerateOAuth()) == NULL ) mac = NULL;
    else mac = majordomo->ComputeMAC(oAuth);
    
    e = pPlanRec->SetMAC(mac);
    if( oAuth ){ DELETE(oAuth); }
    if( mac ){ DELETE(mac); }
    
    //write to table payPlan
    if( e != OK || pPlanRec->WriteToTable(payPlan) != OK ){
      DELETE(pPlanRec);
      WriteString(llError,
        "Banker: Cannot generate OAuth or Compute MAC");
      return KO;
    }
    pPlanRec->Clear();
  }
  DELETE(pPlanRec);

  return OK;
}


/**
 * Fills given table with information about payments.
 *
 * From each record from table PayPlan with given bankACID and
 * transaction ID strips TID and writes it to the table for bank.
 *
 *
 * @param   r initialize record, in which we find useful information
 * @param   bankTable table for bank
 * @return  total sum of money to be paid to the bank
 * @author  Ivana
 * @see     PrepareMsgsForBanks()
 */
Char *Banker::FillTableForBank(Record **rec, Table *bankTmpTab)
{
  Record *r = NULL;
  ClientPayPlanRec *payRec = NULL;
  TID tId;
  ACID bankACID;
  Char *sum = NULL;
  Long sumL = 0;

  if( rec == NULL || (r = (*rec)) == NULL || bankTmpTab == NULL || 
      payPlan == NULL || (payRec = new ClientPayPlanRec()) == NULL )
    return NULL;

  if( payRec->FetchFromRecord(r) != OK ||
      payPlan->Open() != OK ||
      payRec->WriteToTable(bankTmpTab) != OK ||
      (sum = payRec->GetAmount()) == NULL ){
    DELETE(payRec);                                                                     
    WriteString(llError,                                                                    
      "Banker: PayPlan for bank %ld has not generated", bankACID);                              
    return NULL;
  }
  sumL = atol(sum);                          
  DELETE(sum);
				
  tId = ((IDField *)r->GetField(TF_TID))->GetIDValue();
  bankACID = ((IDField *)r->GetField(TF_BANK_ACID))->GetIDValue();

  for( payPlan->FindFirst(TF_TID, &tId) ; r != NULL ; r = payPlan->FindNext() ){
    payRec->Clear();
    if( ((IDField *)r->GetField(TF_BANK_ACID))->GetIDValue() != bankACID ||
        payRec->FetchFromRecord(r) != OK ){
      DELETE(r);
      continue;
    }
    payPlan->DeleteRecord();
    DELETE(r);
    if( payRec->WriteToTable(bankTmpTab) != OK ||
        (sum = payRec->GetAmount()) == NULL ){
      //something wrong happen
      payPlan->Close();
      DELETE(payRec);
      WriteString(llError,
        "Banker: PayPlan for bank %ld has not generated", bankACID);
      return NULL;
    }
    sumL += atol(sum);                          
    DELETE(sum);
  }
  payPlan->Close();
  DELETE(payRec);
  
  if( (sum = new Char[30]) != NULL ) sprintf(sum, "%ld", sumL);
  return sum;
}
    
/**
 * Generate message for bank.
 *
 * Generate message for bank including bank access certificate,
 * total sum of money to be paid and payment plan.
 *
 *
 * @return  message for bank or NULL
 * @author  Ivana
 * @see     PrepareMsgsForBanks()
 */
GMessage *Banker::GenerateMsgForBank(GMessage *bankAC, Char *sum,
  Table *bankTab)
{
  GMessage *outMsg = NULL;
  MsgField *msgField = NULL, *bAC = NULL;
  ACID bankACID = BAD_ID;

  if( bankAC == NULL || sum == NULL || bankTab == NULL ||
      bankAC->GetAsID(GM_ACID, &bankACID) != OK || 
      (bAC = bankAC->StoreToMsgField()) == NULL ||
      (outMsg = new GMessage()) == NULL ){
    if( bAC ){ DELETE(bAC); }
    return NULL;
  }

  //table bankTab will be sent as msgField "PaymentPlan" to bank                                      
  Char *tmp = bankTab ->GetFileName();                                                      
  if( tmp == NULL || (msgField = new BytesMsgField()) == NULL || 
      msgField->SetAsFile(tmp) != OK ){                                                                                                  
    if( tmp ){ DELETE(tmp);}
    if( msgField ){ DELETE(msgField);}
    DELETE(outMsg);
    DELETE(bAC);
    return NULL;
  }
  DELETE(tmp);                                                                                              

  if( outMsg->SetAsMsgField(GM_BANK_AC, bAC) != OK ||
      outMsg->SetAsID(GM_BANK_ACID, bankACID) != OK ||
      outMsg->SetAsMsgField(GM_PAYMENT_PLAN, msgField) != OK ||
      outMsg->SetAsString(GM_AMOUNT, sum) != OK ){ 
     //|| outMsg->SetAsBytes(GM_ACCOUNT)) == NULL )                                               
    DELETE(outMsg);
  }
  DELETE(msgField);
  DELETE(bAC);
  
  return outMsg;
}


/**
 * Banker constructor.
 *
 * Banker constructor creates (and clears) bankers' persistent tables.
 *
 *
 * @param   logFile pointer to log file
 * @param   client pointer to main class client
 * @return  
 * @author  Ivana
 * @see     Client
 */
Banker::Banker(LogFile *logFile, Client *client)
  :Debugable(logFile), SixOffspring(client)
{
  payPlan = bankPPlan = NULL;
  if( CreatePayPlan() != OK || CreateBankPPlan() != OK )
    WriteString(llFatalError,"Banker: Cannot create some table");
  else
    WriteString(llDebug,"Banker: Created");
  if( payPlan ) payPlan->Remove();
  if( bankPPlan ) bankPPlan->Remove();
}

/**
 * Banker destructor.
 *
 * Banker destructor deletes (and clears) bankers' persistent tables.
 *
 *
 * @return  
 * @author  Ivana
 * @see     Client
 */
Banker::~Banker()
{
  if( payPlan ){
    payPlan->Remove(); 
    DELETE(payPlan);
  }
  if( bankPPlan ){
    bankPPlan->Remove();
    DELETE(bankPPlan);
  }
  WriteString(llDebug,"Banker: Deleted");
}


/**
 * Decide Wheather To Accept Payment Plan.
 *
 * Decides wheather client accepts sent payment plan or not.
 * (Now agrees with everything, later it should accept client's decision.)
 * If sent payment plan is NULL, Eso store client's file for free.
 *
 *
 * @param   pPlanFld msgField of payment plan
 * @return  OK/KO
 * @author  Ivana
 * @see     Majordomo::StoreFile()
 */
Err Banker::DecideAcceptPPlan(MsgField * /*pPlanFld*/)
{
  //check sent banks' access certificates
  //write payment plan table from eso somewhere
  //wait for client decision???
  WriteString(llDebug,"Banker: Accepting sent PayPlan TID: %ld",
    six->majordomo->GetTID());
  return OK;
}


/**
 * Makes Payment Plans For Banks.
 *
 * Receiving Eso's payment plan, finishes it (by generating oAuths
 * and computing MAC for each record) and stores it with current 
 * transaction ID.
 *
 *
 * @param   pPlanFld msgField of payment plan
 * @return  error code
 * @author  Ivana
 * @see     Majordomo::StoreFile()
 */
Err Banker::MakePPlanForBanks(MsgField *pPlanFld)
{
  TID tId;
  Table *esoTmpTab = NULL;
  Err e;

  if( pPlanFld == NULL ) return KO;

  tId = six->majordomo->GetTID();
  
  //create table esoTmpTab from msgField
  Char *tmp = ((Client *) six)->GetPath(ESO_TMP_TAB);
  if( tmp ){
    esoTmpTab = ( pPlanFld->GetAsFile(tmp) == OK ) ?
      CreateEsoToClientTable(tmp) : NULL;
    DELETE(tmp);
  }
  else esoTmpTab = NULL;
  if( esoTmpTab == NULL ){
    WriteString(llError, "Banker: Cannot create tmp table");
    return KO;
  }

  //store sent payPlan (generate OAuth for each payment and compute MAC)
  e = StorePayPlan(esoTmpTab, tId);

  esoTmpTab->Remove();
  DELETE(esoTmpTab);

  if( e == OK )
    WriteString(llDebug,"Banker: PayPlans for banks stored; TID: %ld", tId);
  WriteString(llError, "Banker: PayPlans for banks have not stored");
  return e;
}


/**
 * Prepares queue of mesages for banks.
 *
 * Finds all records with given TID. For each bank found in these
 * records generate message including payment plan.
 *
 *
 * @param   oldTID transaction ID with which the payments are connected
 * @return  queue of messages for banks
 * @author  Ivana
 * @see     Majordomo
 */
Queue *Banker::PrepareMsgsForBanks(TID oldTID)
{
  Char *tmp = NULL;
  Table *bankTmpTab = NULL;
  MsgField *mf = NULL;
  Queue *q = NULL;
  Record *r = NULL;
  GMessage *bankAC = NULL, *outMsg = NULL;
  Char *sum = NULL;
  
  //prepare temp table
  tmp = ((Client *) six)->GetPath(BANK_TMP_TAB);
  if( tmp ){
    bankTmpTab = CreateClientToBankTable(tmp);
    DELETE(tmp);
  }
  else bankTmpTab = NULL;
  if( bankTmpTab == NULL ){
    WriteString(llError, "Banker: Cannot create tmp table");
    return NULL;
  }

  if( (q = new Queue()) == NULL ){
      WriteString(llError, "Banker: Cannot create auxiliary structure");
    DELETE(bankTmpTab);
    return NULL;
  }
  
  //for each record from table payPlan with given TID ...
  for( ; (r = FindAndDelInTable(payPlan, TF_TID, oldTID)) != NULL ;  ){
    //get bank AC
    mf = ((BytesField *)r->GetField(TF_BANK_AC))->GetAsMsgField();
    if( mf == NULL ) bankAC = NULL;
    else{
      bankAC = mf->GetAsGMessage();
      DELETE(mf);
    }
    
    //lets create table of payment plan for bank
    bankTmpTab->Remove();
    if( bankAC == NULL || (sum = FillTableForBank(&r, bankTmpTab)) == NULL ){
      if( bankAC ){ DELETE(bankAC);}
      continue;
    }
    
    //try to generate msg for bank and insert it to outQueue
    if( (outMsg = GenerateMsgForBank(bankAC, sum, bankTmpTab)) )
      if( q->Insert(outMsg) != OK ){ DELETE(outMsg); }
      
    DELETE(sum);
    DELETE(bankAC);
  }

  bankTmpTab->Remove();
  DELETE(bankTmpTab);
  
  return q;
}
