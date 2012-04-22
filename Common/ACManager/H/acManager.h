#ifndef ACMANAGER_H
#define ACMANAGER_H

#include "./../../Rollbackable/H/rollbackable.h"
#include "./../../Debugable/H/debugable.h"
#include "./../../GMessage/H/table.h"
#include "./../../GMessage/H/gMessage.h"
#include "./../../H/common.h"
#include "./../../H/labels.h"
#include "./../../GMessage/H/estime.h"


#define ACMGR_TBLFLD_ACID     "ACID"
#define ACMGR_TBLFLD_AC        "AC"
#define ACMGR_TBLFLD_EXPIRATION "Expirate"

#define DEFAULT_ACMGR_TYPE      "notTyped"
#define DEFAULT_ACID 0L

#define DEFAULT_TIMEOUT_INTERVAL "00:00:01:00:00:00"

/**
 * ACManager.
 *
 * encapsulates the table of Access certificates
 * @author Marek
 * @see
**/
class ACManager: public Debugable
{
  protected:
  /**@name attributes */
  /*@{*/
    Char *type;
    Table *acTable;
    Int minACCount, maxACCount;
    RelTime countTest, gotCertifTest;
    ESTime *nextTimeOut;
  /*@}*/
 
  /**@name methods */
  /*@{*/
      /// creates definition of acTable - used only in constructor
    TableFieldQueue *CreateDefinition(); 
  
      /// returns count of not expired ACs, deletes expired ACs
    Err ACCount(Int *count);
  /*@}*/

  public:  
          
  /**@name methods */
  /*@{*/
    /// Constructor
    ACManager(const Char *aType, Char* aTableName, 
              const Int minACCountP = 1, const Int maxACCountP = 1, 
	      LogFile *aLogFile = NULL,
              const RelTime *countTestP = NULL, 
              const RelTime *gotCertifTestP = NULL); 
              
    /// destructor
    virtual ~ACManager();

      ///inserts AC into the table
    Err InsertAC(GMessage* ac);

      /** returns queue of [count] GMessages (ACs),
       * that are not in [notIn] queue
       * if there are less then count */
    Queue *GetACs(const Int count = 1, Queue *notIn = NULL);
    
      /// finds the AC according to its ACID
    GMessage *GetACByACID(ACID acID);
  
      ///tells the Scheduler to set the timeout
    ESTime NextTimeOut();

      /** controlling of count of ACs at timeout
       * returns a GMessage for MIX,
       * if it is necessary to get new ACs
       * or NULL if number of ACs is between minACCount and maxACCount */
    Err TestACCount(Int *countOfMissingACs);
  /*@}*/
};

#endif

