#ifndef BANK_MAJORDOMO_H
#define BANK_MAJORDOMO_H

#include "../../../Common/H/common.h"
#include "../../../Common/H/commands.h"
#include "../../../Common/H/labels.h"
#include "../../../Common/Runable/H/runable.h"
#include "../../../Common/GMessage/H/gMessage.h"
#include "../../../Six/SixMajordomo/H/sixMajordomo.h"
#include "../../Bank/H/bank.h"

/**
 * Class responsible for handling messages comming to Bank.
 *
 * This descendant of SixMajordomo takes main functionality from its
 * ancestor extending it with functionality typical from Bank in
 * Eternity Service.
 *
 *
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
class Majordomo : public SixMajordomo
{
  protected:
  
  /**@name methods */
  /*@{*/
      /// Methods managing access certificates
      /*@{*/
    virtual Err GenerateThisSixAC(MsgField *onion, GMessage **generatedAC);
    virtual Err HandleGeneratedAC(GMessage *generatedAC);
    virtual Err GenerateACKeys(ACID *acid, GMessage **acPubKey);
      /*@}*/

      /// Decrypting data from Service
    virtual MsgField *DecryptByACPriv(const ACID acid,
                                      const MsgField *dData);
      /// Handle data_received command
    virtual Err SwitchDataReceivedCommand(const Char     *command, 
                                const MsgField *dataFld,
				const ACID      acid);
      /// Data_received commands handling methods
      /*@{*/
    Err DRCMoneyFromClient(MsgField *dataFld);
    Err DRCRequestFromEso(MsgField *dataFld);
    Err DRCAnswerFromEso(MsgField *dataFld);
      /*@}*/
       
  /*@}*/
  
  public:

  /**@name methods */
  /*@{*/
      /// Bank Majordomo constructor
    Majordomo(Bank * parent, LogFile * majordomoLogFile);
  
      /**
       * Overriden method that returns pointer to ACManager of
       * specified type 
       */
    virtual ACManager *GetPtrToACManager(Char *type);

      /// Some cool initialization
    Err Initialize();
    
      /// Returns current time
    ESTime *GetTime();
     
      /// Decrypts S_AUTH
    MsgField *UnsealSAuth(const ACID acid, const MsgField *sealSA); 
  /*@}*/
};

#endif
