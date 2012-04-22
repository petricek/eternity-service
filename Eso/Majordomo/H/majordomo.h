#ifndef ESO_MAJORDOMO_H
#define ESO_MAJORDOMO_H

//#define NO_BANKER

#include "../../Common/H/constants.h"
#include "../../Common/H/labels.h"
#include "../../../Common/H/common.h"
#include "../../../Common/H/commands.h"
#include "../../../Common/H/labels.h"
#include "../../../Common/Runable/H/runable.h"
#include "../../../Common/GMessage/H/arrayGMessage.h"
#include "../../TCBWrapper/H/tcbWrapper.h"
#include "../../Finder/H/finder.h"
#include "../../../Six/SixMajordomo/H/sixMajordomo.h"

/**@name Labels for Eso's configuration file*/
/*@{*/
  /// Maximum search depth allowed by this Eso.
#define CFG_MAX_SEARCH_DEPTH "max_search_depth"
  /// Maximum search width allowed by this Eso.
#define CFG_MAX_SEARCH_WIDTH "max_search_width"
/*@}*/

/**
 * Class responsible for handling messages comming to Eso.
 *
 * This class encapsulates main Eso's functionality. Its 
 * behaviour is inherited from general SixManager. Majordomo 
 * is capable of receiving messages from Mix and other Sixes 
 * in the Eternity Service, taking appropriate actions (such as
 * to store or find a file) and creating replies.
 *
 * @author Ivanka, Marek, Pavel
 * @see    SixMajordomo, Six, Eso
 */
class Majordomo : public SixMajordomo
{
  protected:
  /**@name methods*/
  /*@{*/
    
    /** Functions for commands got from receiverToMajordomo queue 
     * (Mix cmds and timeouts). */
    /*@{*/
    virtual Err SwitchMixCommand(MsgField *commandFld, MsgField *dataFld);
    Err CommandCertificatesReceived(MsgField *dataFld);
    virtual Err GenerateThisSixAC(MsgField *onion, GMessage **generatedAC);
    virtual Err HandleGeneratedAC(GMessage *generatedAC);
    Err CommandDelFinderTbl(MsgField *dataFld);
    Err CommandControllACCount(MsgField *dataFld);
    Err CommandTimeoutAllocation(MsgField *dataFld);
    /*@}*/

    /** Functions for commands from Service.*/
    /*@{*/
    virtual Err SwitchDataReceivedCommand(const Char     *command, 
				  const MsgField *dataFld,
				  const ACID      acid);

      /** Saving files. */
      /*@{*/
    Err DRCRequestForStorage(const MsgField *dataFld);
    Err DRCDataToStore(const MsgField *dataFld, const ACID acid);
    Err DRCRFSGetFields(const MsgField *dataFld,
			GMessage      **replyToAC,
			TID            *replyToTID,
			Size           *fileSize,
			RelTime       **storageTime);
    Err DRCDTSGetFields(const MsgField *dataFld,
			GMessage      **replyToAC,
			TID            *replyToTID,
			TID            *inReplyToTID,
			MsgField      **ffid,
			MsgField      **keywords,
			MsgField      **dataFld);
      /*@}*/

      /** Finding files. */
      /*@{*/
    Err DRCRequestForHeaders(const MsgField *dataFld, const ACID acid);
    Err DRCRequestForFile(const MsgField *dataFld, const ACID acid);
    Err DRCAnswerHeader(const MsgField *dataFld);
    Err DRCAnswerFile(const MsgField *dataFld);
      /*@}*/
    
    /*@}*/
  
    /** Filling messages for Service. */
    /*@{*/
    Err FillRequestForHeaders(GMessage *outMsg, MsgField *aFAM, GMessage *transportKey, Int depth, Int width);
    Err FillRequestForFile(GMessage *outMsg, MsgField *aFFID, GMessage *transportKey, Int depth, Int width);
    Err FillReplyHeader(GMessage *outMsg, TID inReplyToTID, GMessage *aHeader);
    Err FillReplyFile(GMessage *outMsg, TID inReplyToTID, MsgField *aFile);
    Err FillReplyAllocation(GMessage *outMsg, TID inReplyToTID, MsgField *payPlan);
    Err FillReplyFileStored(GMessage *outMsg, TID inReplyToTID);
    /*@}*/
    
    /** 
     * Decrypting data from Service.
     * Overrides pure virtual SixMajordomo::DecryptByACPriv 
     */
    virtual MsgField *DecryptByACPriv(const ACID acid,
                                      const MsgField *dData);
    
    /** Sends messages to other Esos through Mix. */
    Err SendToEsos(Char *command, GMessage *msg, Int count);

  /*@}*/

  public:
      /// Eso's majordomo constructor.
    Majordomo(Eso * parent, LogFile * majordomoLogFile);

    /**Functions for ACManagers. */
    /*@{*/
    virtual ACManager *GetPtrToACManager(Char *type);
    Err AskForEsoCertificates();
    Err InsertToOutMsgQueue(GMessage *msg);
    /*@}*/
};

#endif
