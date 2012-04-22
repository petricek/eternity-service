// Description: This class realizes transaction management.
// ATTENTION: synchronization needed !! at least in:
//                       BeginTransaction
//                       EndTransaction

#include "./H/transactionManager.h"

  // names of Fields in transactionLog Table 
#define TLFLD_TID   "TID"
#define TLFLD_STATE "State"

/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
TID TransactionManager::GenerateTID()
{
  TID newTID;
  if(uidGenerator->GetID(&newTID) != OK)
    return 0;
  else
    return newTID;
}

/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err TransactionManager::WriteToLog(TID tid, TransactionState state)
{
  Record *transLogRecord;
  Err result, res1, res2;

    // create new empty Record
  transLogRecord = transactionLog->NewRecord();
  if(transLogRecord == NULL)
    return KO;

    // fill the Record with given parameters
  res1 = transLogRecord->SetField(TLFLD_TID, &tid);
  res2 = transLogRecord->SetField(TLFLD_STATE, &state);
  if((res1 == KO) || (res2 == KO))
  {
    DELETE(transLogRecord);
    return KO;
  }

    // insert the Record to the transactionLog Table
  result = transactionLog->InsertRecord(transLogRecord);

  DELETE(transLogRecord);

  return result;
}

/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
TransactionManager::TransactionManager(Char *transLogName, LogFile *aLogFile)
: Debugable(aLogFile)
{
    // create Table "transactionLog"
  TableFieldQueue *logDefinition = new TableFieldQueue;
  if(logDefinition != NULL)
  {
    logDefinition->Insert(new IDField(TLFLD_TID));
    logDefinition->Insert(new IntField(TLFLD_STATE));
  }
  transactionLog = new Table(transLogName, logDefinition, aLogFile);
  if(logDefinition != NULL)
    DELETE(logDefinition);

    // create empty queue of rollbackable objects
  rollbackableObjects = new Queue();

    // open uidGenerator
  Char *uidgName = new Char[strlen(transLogName) + 6];
  sprintf(uidgName, "%s.uid", transLogName);
  uidGenerator = new UniqueIDGenerator(uidgName);

    // initialize mutex
  pthread_mutex_init(&mutex, NULL);
}

/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
TransactionManager::~TransactionManager()
{
  if(transactionLog != NULL)
    DELETE(transactionLog);
  if(rollbackableObjects != NULL)
    DELETE(rollbackableObjects);
  if(uidGenerator != NULL)
    DELETE(uidGenerator);
}

/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err TransactionManager::Rollback(TID tid)
{
  Rollbackable *rbObject;
  Err result;
  Err singleResult;

  if(rollbackableObjects == NULL)
    return KO;

  result = OK;

    // call Rollback for all
  rbObject = (Rollbackable *)rollbackableObjects->First();
  while(rbObject != NULL)
  {
    singleResult = rbObject->Rollback(tid);
    result = (singleResult == KO) || (result == KO) ? KO : OK;
    rbObject = (Rollbackable *)rollbackableObjects->First();
  }
 
  return result;   
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err TransactionManager::RegisterRollbackable(Rollbackable *rbObject)
{
  if(rollbackableObjects == NULL)
    return KO;

    // store the pointer to a rollbackable object
  return rollbackableObjects->Insert(rbObject);
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err TransactionManager::BeginTransaction(TID *tid)
{
  TID newTID;
  Err result;

  if((transactionLog == NULL) || (tid == NULL))
    return KO;

    // start of critical section
  pthread_mutex_lock(&mutex);

    // open transactionLog, generate new TID, write to transactionLog
  if(transactionLog->Open() == OK)
  {
    newTID = GenerateTID();
    result = WriteToLog(newTID, tsBegin);
    transactionLog->Close();
  }
  else
    result = KO;

    // end of critical section
  pthread_mutex_unlock(&mutex);

    // return newTID
  memcpy(tid, &newTID, sizeof(TID));

  return result;
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err TransactionManager::EndTransaction(TID tid)
{
  Err result;

  if(transactionLog == NULL)
    return KO;

    // start of critical section
  pthread_mutex_lock(&mutex);

    // open transactionLog, write to transactionLog
  if(transactionLog->Open() == OK)
  {
    result = WriteToLog(tid, tsEnd);
    transactionLog->Close();
  }
  else
    result = KO;

    // end of critical section
  pthread_mutex_unlock(&mutex);

  return result;
}
