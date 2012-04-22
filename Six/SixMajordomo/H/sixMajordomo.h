#ifndef SIX_MAJORDOMO_H
#define SIX_MAJORDOMO_H

#include "../../Common/H/constants.h"
#include "../../../Common/H/common.h"
#include "../../../Common/H/commands.h"
#include "../../../Common/H/labels.h"
#include "../../../Common/Runable/H/runable.h"
#include "../../SixOffspring/H/sixOffspring.h"
#include "../../../Common/GMessage/H/gMessage.h"
#include "../../../Common/ACManager/H/acManager.h"

/**
 * Object processing various requests and commands.
 *
 * Processes various requests comming from network and from Mix. 
 * Also distributes messages to apropriate objects.
 *
 *
 * @author  Pavel 
 * @see     Runable, SixOffSpring, Mix 
 */
class SixMajordomo : public Runable, public SixOffspring
{
  protected:
  /**@name attributes */
  /*@{*/
      ///transaction ID.
    TID tID;
      ///Queue with messages prepared to send to Mix.
    Queue *outMsgQueue; 
      ///Object for managing transactions.
    TransactionManager *transactionManager;
  /*@}*/
  

  protected:
  /**@name methods */
  /*@{*/
    /**Help functions
      *for putting the messages to the majordomoToSender queue.*/
    /*@{*/
      ///Puts the messages from outMsgQueue to majordomoToSender queue. 
    Err FeedSenderWithOutMsgQueue();
      ///Puts one message to majordomoToSender queue.
    Err FeedSenderWithMsgForMix(GMessage *mixMsg); 
    
    /*@}*/
 
    
    /**Functions for commands got from IIQ (Mix cmds)
      **/
    /*@{*/
      ///gets the Command and Data fields from inMsg and calls SwitchMixCommand.
    virtual Void HandleMixMessage(GMessage *inMsg);
      ///calls appropriate method for Mix command.
    virtual Err SwitchMixCommand(MsgField *commandFld, MsgField *dataFld);
      ///Serves Test command from MIX.
    virtual Err CommandTest(MsgField *dataFld);
      ///Serves DataReceived command from MIX.
    virtual Err CommandDataReceived(MsgField *dataFld);
      ///Serves OnionGenerated command from MIX.
    virtual Err CommandOnionGenerated(MsgField *dataFld);
    /*@}*/
  

    /**Functions needed 
      *for serving OnionGenerated command from Mix.*/
    /*@{*/
      ///Generates AC of this six having the Onion (Mix certificate).
    virtual Err GenerateThisSixAC(MsgField *onion, GMessage **generatedAC) = 0;
      ///Handles the generated AC according to the type of Six.
    virtual Err HandleGeneratedAC(GMessage *generatedAC);
    /*@}*/

    
    /**Functions needed for serving
      *DataReceived command from Mix.*/
    /*@{*/
      ///Decrypting data by privateKey associated with ACID.
    virtual MsgField *DecryptByACPriv(const ACID acid, 
                                      const MsgField *dData) = 0;
      ///Decrypting data that came from Service.
    Err DecryptDataMsg(const GMessage *dataMsg,
                       MsgField      **decryptedBodyFld,
		       ACID           *acid);
      ///Handles the body of the message (the decrypted request from Service).
    Err HandleDataReceivedBody(const MsgField *bodyFld, const ACID acid);
      ///Chooses the way to continue according to the protocol version of the message.
    virtual Err SwitchDataReceivedVersion(const Char      *version, 
                                          const GMessage  *body,
		        		  const ACID       acid);
    /*@}*/
    
    
    /**Functions
      *that handle various commands from Service.*/
    /*@{*/
      ///Initializing switch.
    virtual Err SwitchDataReceivedCommand(const Char     *command, 
                                          const MsgField *dataFld,
         		                  const ACID      acid);
      ///Serving Test command.					  
    Err DRCTest(const MsgField *dataFld);
    /*@}*/

    
    /**Filling the fields into the messages
      *to Service.*/
    /*@{*/
      ///Filling fields of TestMessage.
    Err FillTest(GMessage *gMsg, MsgField *msgFld);
      ///Filling Reply-to-AC and Reply-To-TID fields of a GMessage.
    Err FillReplyToInfo(GMessage *gMsg);
    /*@}*/
    
    
    /**Functions that create
      *messages for Mix.*/
    /*@{*/
      ///Creates testing message.
    GMessage *CreateTestMsg(MsgField *dataFld);
      ///Creates "get_certificates" message.
    GMessage *CreateGetACsMsg(Char *type, Int count);
      ///Creates "generate_onion" message.
    GMessage *CreateGenerateOnionMsg(Size hopCount = CFG_HOPS_IN_AC);
      //Creates "publish_certificates" message.
    GMessage *CreatePublishCertificateMsg(GMessage *ac);
    /*@}*/
  /*@}*/

  public:
  /**@name methods */
  /*@{*/
      ///Constructor.
    SixMajordomo(Six * parent, LogFile * majordomoLogFile);
      ///Destructor.
    virtual ~SixMajordomo();
      ///Method that runs the whole Eso - processes messages from receiverToMajordomo.
    virtual void * Run(void *);

      ///Creates "send_data" message.
    GMessage *CreateSendDataMsg(Char *commandSer, 
                 GMessage *dataSer, GMessage *ac, 
                 Size hops = CFG_SEND_DATA_HOPS);

      /// Starts new transaction and returns its ID.
    Err BeginTransaction(ID *newTID);
      /// Ends transaction with ID tid.
    Err EndTransaction(ID tid);
      /// Returns ID of the most recently started transaction.
    ID GetTID();

    /**Functions
      *for getting information from ACManagers.*/
    /*@{*/
        ///Gets pointer to ACManager.
      virtual ACManager *GetPtrToACManager(Char *type) = 0;
        ///Inserts AC into ACManager of given type.
      virtual Err InsertAC(Char *type, GMessage *ac);
        ///Gets given count of ACs of given type not included in notIn queue.
      virtual Queue *GetACs(Char *type, const int count = 1, Queue *notIn = NULL);
        ///Gets AC of given type by ACID.
      virtual GMessage *GetACByACID(Char *type, ACID acID);
        ///Returns next timeout of ACManager of given type.
      virtual Err NextACTimeOut(Char *type, ESTime *t);
        ///Creates messages that will be returned in next TimeOut.
      virtual GMessage *CreateNextACTimeOutMsg(Char *type);
        ///Tests AC count in ACMgr of given type.
      virtual Err TestACCount(Char *type, Int *missing);
        ///Generates request for missing ACs.
      virtual Err GenerateRequestForMissingACs(Char *acType);
        ///Asks Mix for onion.
      virtual Err AskForOnion();
    /*@}*/
  /*@}*/
};
#endif
