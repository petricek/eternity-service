#ifndef PAY_MANAGER_H
#define PAY_MANAGER_H


#include "./payMgrDefs.h"
#include "./../../../Common/Payments/H/payRecord.h"
#include "./../../../Common/Debugable/H/debugable.h"
#include "./../../../Common/GMessage/H/gMessage.h"
#include "./../../../Common/GMessage/H/table.h"
#include "./../../../Six/SixOffspring/H/sixOffspring.h"
#include "./../../Bank/H/bank.h"


/**
 * Bank Payment Record.
 *
 * An auxiliary class for one Payment record in Bank.
 * Later it should have attribute account, which will be represent
 * a real 'noname account' information in bank, from which the bank
 * will pay to Eso.
 *
 *
 * @author Ivana
 * see     PayManager 
 */
class BankPayRec: public PayRecord
{
  protected:

  /**@name attributes */
  /*@{*/
    MsgField *sAuth;
    MsgField *oAuth;
    MsgField *mac;
//  ??? account;
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
    BankPayRec();
    virtual ~BankPayRec();

    virtual Err FetchFromRecord(Record *r);
    virtual Err FillRecord(Record *r);

    Int MACEqualTo(MsgField *otherMAC);
    Int SAuthEqualTo(MsgField *otherSA);
    Char *GetAmount();
    MsgField *GetSealSAuth();
    Err SetSAuth(MsgField *sAuth);
    Void Clear();
  /*@}*/

};


/**
 * Payment Manager.
 *
 * A class which manages everything about payments in Bank:
 * 1) stores payment plan sent by client (and transfer money)
 * 2) reacts to eso's request for payment - if everything is OK
 * transfer money to it's account
 *
 *
 * @author Ivana
 * see     Bank
 */
class PayManager: public Debugable, public SixOffspring
{
  protected:

  /**@name attributes */
  /*@{*/
    /// PayManager's persistent table of payments    
    Table *payments; 
  /*@}*/

  //*************** AUXILIARY FUNCTIONS *****************
  /**@name methods */
  /*@{*/
    Err CreatePayments();

    Err DeleteRecsInTable(Table *t, Char *whichId=NULL, ID id=0);
    Record *FindInTable(Table *t, Char *whichId, ID id);

      /**
       * stores PayPlan from given table to payments table
       */
    Long StorePayments(Table *fromClient, ACID myACID);

      /**
       * creates outMsg with fields Success, Reson-Of-Fail and Paymet-ID
       */
    GMessage *CreateCommonMsg(Err succ, Char *reason=NULL, ID *payId=NULL);
  /*@}*/

  //*************** PUBLIC FNCS *************************
  public:

  /**@name methods */
  /*@{*/
    PayManager(LogFile *logFile, Bank *bank);
    virtual ~PayManager();

      /**
       * reacts to message from client
       */
    GMessage *MoneyFromClient(GMessage *inMsg);

      /**
       * reacts to a request for payment from Eso (prepares challenge for it)
       */
    GMessage *RequestFromEso(GMessage *inMsg);

      /**
       * reacts to an answer from Eso
       */
    GMessage *AnswerFromEso(GMessage *inMsg);
  /*@}*/

};

#endif
