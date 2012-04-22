#ifndef ESO_BANKER_H
#define ESO_BANKER_H

#include "./bankDefines.h"
#include "./../../../Common/Debugable/H/debugable.h"
#include "./../../../Common/Rollbackable/H/rollbackable.h"
#include "./../../../Common/GMessage/H/gMessage.h"
#include "./../../../Common/GMessage/H/table.h"
#include "./../../../Common/Payments/H/payRecord.h"
#include "./../../../Six/SixOffspring/H/sixOffspring.h"
#include "./../../Eso/H/eso.h"

/**
 * Eso Payment Plan Record.
 *
 * An auxiliary class for one Payment Plan record in Eso.
 *
 * @author Ivana
 * @see    Banker
 */
class EsoPayPlanRec: public PayRecord
{
  protected:

  /**@name attributes */
  /*@{*/
    TID tId;
    ACID bankACID;
    GMessage *bankAC;
    MsgField *sAuth;
  /*@}*/

  /**@name methods */
  /*@{*/
    Err GenerateSAuth(Size lengthOfSA);
    Err SealSAuth();
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
    EsoPayPlanRec(ID payId, TID tId, ESTime time, GMessage *bankAC,
                  Char *amount, Size lenOfSA = 0);
    virtual ~EsoPayPlanRec();

    virtual Err FetchFromRecord(Record *r);
    virtual Err FillRecord(Record *r);
  /*@}*/
};


/**
 * Payment.
 *
 * An auxiliary class for one Payment record in Eso.
 *
 * @author Ivana
 * @see
 */
class Payment: public PayRecord
{
  protected:

  /**@name attributes */
  /*@{*/
    ID fileId;
    ACID bankACID;
    MsgField *sAuth;
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
    Payment();
    virtual ~Payment();

    virtual Err FetchFromRecord(Record *r);
    virtual Err FillRecord(Record *r);

    virtual ID GetID(Char *whichId);
    ESTime GetTime();
    Char *GetAmount();

    virtual Err SetID(Char *whichId, ID value);
    Void Clear();
  /*@}*/
};


/**
 * Current Payment.
 *
 * An auxiliary class for one currently taken payment in Eso.
 *
 * @author Ivana
 * @see
 */
class CurrPay: public Payment
{
  protected:

  /**@name attributes */
  /*@{*/
    TID tId;
    MsgField *oAuth;
    Int state; //REQUEST or ANSWER
    MsgField *mac;
    Int numOfTries;
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
    CurrPay();
    virtual ~CurrPay();

    virtual Err FetchFromRecord(Record *r);
    virtual Err FillRecord(Record *r);
    Err WriteToTable(Table *t, Char *reason=NULL);

    virtual ID GetID(Char *whichId);
    MsgField *GetAuth(Char *whichAuth);
    MsgField *GetMAC();
    Int GetState();
    Int NumOfTries(Char operation = '0');

    virtual Err SetID(Char *whichId, ID value);
    Err SetAuth(Char *whichAuth, MsgField *oAuth);
    Err SetMAC(MsgField *mac);
    Err SetState(Int state);
  /*@}*/
};


/**
 * Banker.
 *
 * A class which manages everything about payments in Eso:
 * 
 * Makes Payment Plans for Clients
 * Has control over all payment transactions between Bank and Eso
 *     (sends request for payment and reacts to a challenge from Bank)
 *
 * @author Ivana
 */
class Banker: public Debugable, public Rollbackable, SixOffspring
{
  protected:

  /**@name attributes*/
  /*@{*/
    ///bankers' persistent tables:
    Table *pPlans; 
    ///generated payment plans
    Table *clientPPlan; 
    ///pay plan that will be sent to client
    Table *payments; 
    ///all payments that should be done
    Table *currPays; 
    ///all payments that are carried out right now
    Table *notPaidPays; 
    ///payments that haven't been paid
  /*@}*/

  //*************** AUXILIARY TABLES FNCS ***************
  /**@name methods */
  /*@{*/
    Err CreatePPlans();
    Err CreateClientPPlan();
    Err CreatePayments();
    Err CreateCurrPays();
    Err CreateNotPaidPays();

    Err PartialUpdateCurrPays(CurrPay *p);

    Err DeleteRecsInTable(Table *t, Char *whichId=NULL, ID id=0);

  //*************** AUXILIARY FNCS **********************
    Err GeneratePayId(ID *payId);

    //counts price for stored data (returns it in a string)
    Char *CountPrice(Size size, RelTime timeToStore);

    //chooses apropriate bank (returns it's AC)
    GMessage *DecideWhichBank();

    //sets time outs
    Err SetPaymentTimeOut(ID payId, ESTime time);
    Err SetCurrPayTimeOut(ID payId, Int state);

    //generates request/answer for bank
    GMessage *GenerateRequest(CurrPay *p);
    GMessage *GenerateAnswer(CurrPay *p);

    //reactions to a challenge from bank
    GMessage *ChallengeOK(CurrPay *p, GMessage *inMsg);
//  GMessage *ChallengeOKMorePays(CurrPay *p, GMessage *inMsg);
    Void ChallengeWrongTime(CurrPay *p, GMessage *inMsg);
    Void ChallengeKO(CurrPay *p, Char *reason);

    //reactions to an answer from bank
    Err AnswerOK(CurrPay *p, GMessage *inMsg);
    Void AnswerKO(CurrPay *p, Char *reason);

    //reaction to timeOut of currentPay from scheduler
    GMessage *TimeOutCurrPay(CurrPay *pay);
  /*@}*/

  //*************** PUBLIC FNCS *************************
  public:

  /**@name methods */
  /*@{*/
    Banker(LogFile *logFile, Eso *eso);
    virtual ~Banker();

    //rollbacks transaction
    virtual Err Rollback(TID tId);

    //making and scheduling payment plan
    MsgField *MakePayPlan(Size size, RelTime timeToStore, TID tId);
    Void DelPayPlan(TID tId);
    Err SchedulePays(TID tId, ID fileId);

    //creates a request for bank
    GMessage *TimeForPayment(GMessage *inMsg, TID tId, ACID *bankACID);

    //reacts to a challenge from bank
    GMessage *ChallengeFromBank(GMessage *inMsg, TID tId, ACID *bankACID);

    //reacts to an answer from bank
    Void AnswerFromBank(GMessage *inMsg);

    //reacts to a timeout of some payment
    GMessage *TimeOutPayment(GMessage *inMsg, TID tId, ACID *bACID, Int *req);

    //reacts to a request from Eso's administrator
    Void DeleteRecsFileID(ID fileId);
    Void DeleteRecsPayID(ID payId);
  /*@}*/
};
#endif
