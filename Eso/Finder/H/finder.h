#ifndef FINDER_H
#define FINDER_H

#include "./../../../Common/H/common.h"
#include "./../../../Common/GMessage/H/table.h"
#include "./../../../Common/GMessage/H/gMessage.h"
#include "./../../Common/H/labels.h"
#include "./../../Common/H/constants.h"
#include "./../../../Six/SixOffspring/H/sixOffspring.h"
#include "./../../Eso/H/eso.h"

/**General Finders' constants*/
/*@{*/
#define FINDER_PATH             "Finder/"

  /// Name of forwarding table
#define FINDER_FWD_TABLE_NAME   FINDER_PATH"forwards.tbl"
  /// Name of replying table
#define FINDER_REPLY_TABLE_NAME FINDER_PATH"reply.tbl"
/*@}*/

/// Names of Finder's tables' fields
/*@{*/
#define FI_TID "TID"
#define FI_EXP "Expiration"
#define FI_RAC "ReplyToAC"
#define FI_RID "ReplyToTID"
#define FI_FFID "FFID"
/*@}*/

/**
 * Manager of searching and downloading.
 * 
 * Encapsulates the TransactionForwardTable and makes
 * easier the operations associated with forwarding 
 * found files and headers.
**/
class Finder: public Debugable, public SixOffspring
{
  protected: 
  /**@name attributes*/
  /*@{*/
      /// Tables for forwarding transactions.
    Table *fwdTable, *replyTable;
      /// Method that creates fwdTable description.
    TableFieldQueue *CreateFwdTableDefinition();
      /// Method that creates replyTable description.
    TableFieldQueue *CreateReplyTableDefinition();
  /*@}*/
    
  public:
  /**@name methods*/
  /*@{*/
      /// Constructor.
    Finder(LogFile *aLogFile, Eso *eso);
      /// Destructor.
    ~Finder();

      /// Save info about client.
    Err SaveForwardInfo(const TID aTID, GMessage *inMsg,
                        const Int depth);
 
      /// Save info about sent file.
    Err SaveReplyInfo(const TID aTID, GMessage *aHeader,
                      const Int depth);
         
      /// Get forward info.
    Err GetForwardInfo(const TID inReplyToTID, GMessage **anAC, TID *replyToTID);

      /// Return 0/1 if such a record isn't/is in replyTable.
    Int AlreadySent(const TID aTID, GMessage *aHeader);

      /// Delete fwd info according a TID.
    Err DeleteForwardInfo(TID aTID);

      /// Delete records with by expiration.
    Err DeleteRecordsByExpiration(ESTime currentTime);
  /*@}*/
};


#endif



