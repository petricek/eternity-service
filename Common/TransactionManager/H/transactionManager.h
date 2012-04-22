#ifndef TRANSACTMGR_H
#define TRANSACTMGR_H

  // has to be the FIRST include !
#include "./../../H/common.h"

#include <pthread.h>

#include "./../../Rollbackable/H/rollbackable.h"
#include "./../../GMessage/H/table.h"
#include "./../../Debugable/H/debugable.h"
#include "./../../UidGenerator/H/uidGenerator.h"


/**
 * Transaction manager class.
 *
 * makes a log of transactions and enables some classes to be able to Roll 
 * back.
 *
 * @author  Pavel
 * @see     Rollbackable
 */
class TransactionManager: public Debugable
{
protected:
  /**@name attributes */
  /*@{*/
  Table *transactionLog;
  Queue *rollbackableObjects;
  UniqueIDGenerator *uidGenerator;
  enum  TransactionState { tsBegin, tsEnd };

  pthread_mutex_t mutex;
  /*@}*/

  /**@name methods */
  /*@{*/
    /// generates new transaction ID
  TID GenerateTID();
    /// writes to transactionLog - must be open
  Err WriteToLog(TID tid, TransactionState state);
    /// reads from transactionLog - must be open
  Err ReadFromLog(TID *tid, TransactionState *state);
  /*@}*/

public:
  /**@name methods */
  /*@{*/
  TransactionManager(Char *transLogName, LogFile *aLogFile = NULL);
    /// goes through the transaction log and  rolls back all not commited transactions
  CheckTransactionLog();
    /// calls Rollback(tid) for all rollbackable objects 
  Rollback(TID tid);
    /// registers an rollbackable object
  Err RegisterRollbackable(Rollbackable *rbObject);
    /// returns new TID and  writes to Transaction Log
  Err BeginTransaction(TID *tid);
    /// writes to Transaction Log
  Err EndTransaction(TID tid);
  /*@}*/
};

#endif
