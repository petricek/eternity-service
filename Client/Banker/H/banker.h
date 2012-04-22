#ifndef CLIENT_BANKER_H
#define CLIENT_BANKER_H


#include "./bankDefines.h"
#include "./../../../Common/Debugable/H/debugable.h"
//#include "./../../../Common/Rollbackable/H/rollbackable.h"
#include "./../../../Common/GMessage/H/gMessage.h"
#include "./../../../Common/GMessage/H/table.h"
#include "./../../../Common/Payments/H/payRecord.h"
#include "./../../../Six/SixOffspring/H/sixOffspring.h"
#include "./../../Client/H/client.h"

/**
 * Client Payment Plan Record.
 *
 * An auxiliary class for one Payment record in Client.
 *
 *
 * @author Ivana
 * @see    Banker
 */
class ClientPayPlanRec: public PayRecord
{
  protected:

  /**@name attributes */
  /*@{*/
    TID tId;
    ACID bankACID;
    GMessage *bankAC;
    MsgField *oAuth;
    MsgField *mac;
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
    ClientPayPlanRec();
    virtual ~ClientPayPlanRec();

    virtual Err FetchFromRecord(Record *r);
    virtual Err FillRecord(Record *r);

    MsgField *GenerateOAuth(Size lengthOfOA = 0);

    Char *GetAmount();
    GMessage *GetBankAC();
    ACID GetBankACID();

    Void SetTID(TID tId);
    Err SetMAC(MsgField *mac);
    Void Clear();
  /*@}*/

};


/**
 * Banker.
 *
 * A class which manages everything about payments in Client:
 * 1) For each payment (in payment plan sent by Eso) generates OAuth 
 * and counts MAC.
 * 2) New payment plan sends to given banks (along with money).
 *
 * @author Ivana
 * @see    Client
 */
class Banker: public Debugable, SixOffspring
{
  protected:

  /**@name attributes */
  /*@{*/
    ///Bankers' persistent tables
    //payment plans from Esos with TID, OA and MAC
    Table *payPlan;
    //payment plans to Banks <TID, PPlan, BankAC, Amount[, Account]>
    Table *bankPPlan;
  /*@}*/

  //*************** AUXILIARY FUNCTIONS *****************
  /**@name methods */
  /*@{*/
    Err CreatePayPlan();
    Err CreateBankPPlan();

    Err DeleteRecsInTable(Table *t, Char *whichId=NULL, ID id=0);
    Record *FindAndDelInTable(Table *t, Char *whichId, ID id);

    //
    Err StorePayPlan(Table *esoPPlan, TID tId);
    
   Char *FillTableForBank(Record **r, Table *bankTmpTab);
   GMessage *GenerateMsgForBank(GMessage *bankAC, Char *sum, Table *bankTab);
/*
    //
    Err WriteToBankPPlan(TID tId, Char *fileName, GMessage *bankAC,
                         Long sum);//[,account]);

    //generating msgs for banks
    GMessage *PrepareMsg(Char *tabFileName, Long sum, GMessage *bankAC);
    GMessage *GenerateMsgForBank(ACID bankACID, Table *esoTmpTab,
                                 Table *bankTmpTab, File *aFile);
    Err FillOutMsgQForBanks(Table *t,File *f, Queue *outMsgQ);

*/
  /*@}*/

  //*************** PUBLIC FNCS *************************
  public:

  /**@name methods */
  /*@{*/
    Banker(LogFile *logFile, Client *client);
    virtual ~Banker();

     /**
      * decides wheather client accepts sent payment plan
      */
    Err DecideAcceptPPlan(MsgField *pPlanFld);
    
     /**
      * finishes payment plan (generates oAuths and count MACs) for bank
      */
    Err MakePPlanForBanks(MsgField *pPlanFld);

     /**
      * prepares msgs for banks
      */
    Queue *PrepareMsgsForBanks(TID oldTID);

/*    
    //stores sent payment plan
    Err StorePPlanFromEso(MsgField *pPlanFld);

    //deletes payment plan table
    Void DeletePPlanFromEso(TID oldtId);

    //finishes payment plan (generates oAuths and count MACs) for bank
    Err MakePPlanForBanks(TID oldtId, File *aFile, Queue *outMsgQ);

    //reacts to bank answer
    Err AnswerFromBank(GMessage *inMsg);
*/
  /*@}*/

};

#endif
