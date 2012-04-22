#include "./H/acManager.h"

/**
 * Access certificate manger constructor.
 *
 * Takes care of certificates of certain type. Keeps the number
 * between specified ranges and downloads new certificates if
 * necessary.
 *
 * @param   aType          Type of certificates to store.
 * @param   aTableName     Table where to store the certificates.
 * @param   minACCountP    Minimum number of certificates to keep.
 * @param   maxACCountP    Maximum number of certificates to keep.
 * @param   aLogFile       Log file for debug messages.
 * @param   countTestP     Time to wait before checking if we have enough certificates
 * @param   gotCertifTestP Time to wait before checking if certificates arrived
 * @author  marek
 * @see     Six
 */
ACManager::ACManager(const Char *aType, Char* aTableName,
                     const Int minACCountP, const Int maxACCountP,
		     LogFile *aLogFile,
                     const RelTime *countTestP, 
                     const RelTime *gotCertifTestP) 
: Debugable(aLogFile)
{
    
    TableFieldQueue *definition;   

    //set the properties
        //set the AC type
      if (aType == NULL){
        type = new Char[strlen(DEFAULT_ACMGR_TYPE) + 1];
        strcpy(type, DEFAULT_ACMGR_TYPE);
      }
      else {
        type = new Char[strlen(aType) + 1];
        strcpy(type, aType);
      };
        //set the min/maxACCount
      minACCount = minACCountP;
      maxACCount = maxACCountP;
 
        //set the test intervals
      if (countTestP == NULL)
        (&countTest)->SetTime(DEFAULT_TIMEOUT_INTERVAL);
      else
        countTest = *countTestP;
      
      if (gotCertifTestP == NULL)
        (&gotCertifTest)->SetTime(DEFAULT_TIMEOUT_INTERVAL);
      else
        gotCertifTest = *gotCertifTestP;

        //set the nextTimeout
      nextTimeOut = new ESTime();
     
 
    //initialize the AC Table
  definition = CreateDefinition();
  if(definition == NULL)
    WriteString(llError, __FILE__":%d:ACManager[%s]::ACManager(): cannot create table definition", __LINE__, type);
  WriteString(llDebug, __FILE__":%d:ACManager[%s]::ACManager(): creating table in file %s", __LINE__,
              type, aTableName);
  acTable = new Table(aTableName, definition);
  if(acTable == NULL)
    WriteString(llError, __FILE__":%d:ACManager[%s]::ACManager(): cannot create table ", __LINE__, type);
  if(definition != NULL)
    DELETE(definition);
}    

/**
 * Access certificates manager destructor.
 *
 * Deletes all certificates from memory.
 *
 * @author  marek
 * @see     ACManager(), Table
 */
ACManager::~ACManager()
{
  if (type)
    DELETE(type);
  if (acTable)
    DELETE(acTable);
}

/**
 * Creates a definition of Table.
 *
 * This table is then used for storage of access certificates of
 * type specified in constructor.
 *
 * @return  Array of fields that will be in the queue.
 * @author  marek
 * @see     ACManager(), Table
 */
TableFieldQueue *
ACManager::CreateDefinition()
{
TableFieldQueue *definition;
  TableField *field;

  definition = new TableFieldQueue;
  if(definition == NULL)
    return NULL;
   
    //create field for ACID
  field = new IDField(ACMGR_TBLFLD_ACID);
  if(field == NULL){
    WriteString(llError, __FILE__":%d:ACManager[%s]::CreateDefinition(): cannot create TableField", __LINE__, type);
    DELETE(definition);
    return NULL;
  }
  definition->Insert(field);

    //create field for AC
  field = new BytesField(ACMGR_TBLFLD_AC, NULL, logFile);
  if(field == NULL){
    WriteString(llError, __FILE__":%d:ACManager[%s]::CreateDefinition(): cannot create TableField", __LINE__, type);
    DELETE(definition);
    return NULL;
  }
  definition->Insert(field);

    //create field for Expiration
  field = new TimeField(ACMGR_TBLFLD_EXPIRATION);
  if(field == NULL){
    WriteString(llError, __FILE__":%d:ACManager[%s]::CreateDefinition(): cannot create TableField", __LINE__, type);
    DELETE(definition);
    return NULL;
  }
  definition->Insert(field);

  return definition;
}

/**
 * Get count of certificates.
 *
 * Returns the current count of certificates in the manager - used to
 * keep the number in the range specified on creation of AC manager.
 *
 * @param   count    out parameter that gets filled with the count of certificates currently in AC manager.
 * @return  Error code
 * @author  marek
 * @see     ACManager()
 */
Err 
ACManager::ACCount(Int *count)
{
  Int result;
  Record *acRecord;
  ESTime *esTime;
  TimeField *timeFld;

  *count = 0;

  if (acTable == NULL)  
    return KO;

    //get current time
  esTime = new ESTime();
  if (esTime == NULL)
    return KO;

    //open the table
  if(acTable->Open() != OK){
    WriteString(llError, __FILE__":%d:ACManager[%s]::ACTableInsert(): cannot open allocation table", __LINE__, type);
    return KO;
  }
 
  result = OK;

  acRecord = acTable->First();
  while(acRecord != NULL) {
    timeFld = (TimeField*)acRecord->GetField(ACMGR_TBLFLD_EXPIRATION);
    if (timeFld == NULL){
      DELETE(acRecord);
      result = KO;
      continue;
    }

    if (timeFld->GetESTimeValue() >= *esTime)
      *count += 1; 
    else 
      acTable->DeleteRecord();

    // !!WATCH OUT!! don't DELETE(timeFld) - you didn't get a new one, but
    //               only a pointer to an old one in the acRecord

    DELETE(acRecord);
    acRecord = acTable->Next();
  }  

  acTable->Close();
  DELETE(esTime);
  return result;
}

/**
 * Insert Access certificate.
 *
 * Inserts a GMessage into the table (or rewrites old record with same ACID)
 *
 * @param   ac         Access certificate that should be inserted.
 * @return  Error code
 * @author  marek
 */
Err ACManager::InsertAC(GMessage* ac)
{
  Record *record;
  Err result;
  ID acid;  
  ESTime acExpire;
  MsgField *acMsgFld;


  if (ac == NULL)
    return KO;

  if (acTable == NULL)
    return KO;
    //get the ACID and expiuration time of the ac
  if(ac->GetAsID(ACFLD_ACID, &acid) == KO)
   return KO;
  if(ac->GetAsBasicTime(ACFLD_NOT_VALID_AFTER, &acExpire) == KO)
   return KO;

    //open the table
  if(acTable->Open() != OK){
    WriteString(llError, __FILE__":%d:ACManager[%s]::ACTableInsert(): cannot open allocation table", __LINE__, type);
    return KO;
  }

    //get old record with the same ACID ...
  record = acTable->FindFirst(ACMGR_TBLFLD_ACID, (void*)&acid);
  acTable->Close();
  if(record != NULL){
     WriteString(llWarning, __FILE__":%d:ACManager[%s]::InsertAC():ACID already in use by other AC", __LINE__, type);
     DELETE(record);
     return KO;
  }

    //...or create a new one
  if(record == NULL){
    record = acTable->NewRecord();
    if(record == NULL){
      WriteString(llError, __FILE__":%d:ACManager[%s]::ACTableInsert(): cannot create new allocation record", __LINE__, type);
      return KO;
    }
  }


     //transform ac into acMsgField
  acMsgFld = ac->StoreToMsgField(); 
  if (acMsgFld == NULL){
    DELETE(record);
    return KO;
  }

    //fill the record   
  result = record->SetField(ACMGR_TBLFLD_ACID, (void*)&acid);
  if(record->SetField(ACMGR_TBLFLD_AC, (void*)acMsgFld) != OK)
    result = KO;
  if(record->SetField(ACMGR_TBLFLD_EXPIRATION, (void*)&acExpire) != OK)
    result = KO;
  DELETE(acMsgFld);
  

  if(result != OK){
    WriteString(llError, __FILE__":%d:ACManager[%s]::ACTableInsert(): cannot fill new allocation record", __LINE__, type);
    DELETE(record);
    return KO;
  }
   
    //open the table
  if(acTable->Open() != OK){
    WriteString(llError, __FILE__":%d:ACManager[%s]::ACTableInsert(): cannot open allocation table", __LINE__, type);
    return KO;
  }

     // insert new record into acTable
  result = acTable->InsertRecord(record);
  if(result != OK)
    WriteString(llError, __FILE__":%d:ACManager[%s]::ACTableInsert(): cannot insert to AC table", __LINE__, type);
  DELETE(record);
  acTable->Close();

  return result;
}


/**
 * Get access certificates.
 *
 * Gets count ACs, that are not in notIn Queue if there are less
 * then count of such of them, returns less.
 *
 * @param   count    Number of certificates to return.
 * @param   notIn    List of forbidden certificates.
 * @return  List of certificates that matchthe query.
 * @author  marek
 * @see     Six
 */
Queue *ACManager::GetACs(const Int count, Queue *notIn)
{
    //directing variables
  Int iCount;
  Int result;
  Queue *resultQ = NULL;
  Int isInQueue;
  Int resultInsert;


    //ac from table and its modifications
  Record *acRecord = NULL;
  BytesField *acFld = NULL;
  GMessage *acGM = NULL;

    //idFld from acRecord and its modifications
  IDField *idFld;
  ID acID;
  
    //AC from the notIn queue and its ACID
  GMessage *acFromQueue = NULL;
  ID acFromQueueACID;

DBG;
  if (acTable == NULL)
    return NULL;  

DBG;
  resultQ = new Queue();
  if (resultQ == NULL)
    return NULL;

  result = OK;
  iCount = count;

    //open the table
  if(acTable->Open() != OK){
    WriteString(llError, __FILE__":%d:ACManager[%s]::ACTableInsert(): cannot open allocation table", __LINE__, type);
    return NULL;
  }

DBG;
    //search [i] ACs in the table
  acRecord = acTable->First();
  while((iCount>0) && (acRecord != NULL) && (result == OK)){
      //get ID of the AC in the table
    idFld = (IDField*) acRecord->GetField(ACMGR_TBLFLD_ACID);
    if (idFld == NULL){
      WriteString(llError, __FILE__":%d:ACManager[%s]::GetACs(): Bad structure of AC Table", __LINE__, type);
      DELETE(acRecord);
      DBG;
      result = KO;
      continue;
    }
    acID = idFld->GetIDValue();
      //don't DELETE(IDField), it's only a ptr
    
      //try if it is in the queue
    isInQueue = 0;
    if(notIn != NULL){
      acFromQueue = (GMessage*)notIn->First();
      while(acFromQueue != NULL){
        if(acFromQueue->GetAsID(ACFLD_ACID, &acFromQueueACID) == OK)   
          isInQueue = (acFromQueueACID == acID);
        acFromQueue = (GMessage*)notIn->Next();
      }
    }
    
DBG;
       //if not, add it to result queue
    if (!isInQueue){
        //get the AC
      acFld = (BytesField*) acRecord->GetField(ACMGR_TBLFLD_AC);
      if (acFld == NULL){
DBG;
        WriteString(llError, __FILE__":%d:ACManager[%s]::GetACs(): Bad structure of AC Table", __LINE__, type);
        DELETE(acRecord);
        result = KO;
        continue;
      }

DBG;
      acGM = acFld->GetAsGMessage();
      if(acGM == NULL){
DBG;
        WriteString(llError, __FILE__":%d:ACManager[%s]::GetACs(): Can't convert acMsgFld into acGMsg", __LINE__, type);
        DELETE(acRecord);
        result = KO;
        continue;
      }
      
DBG;
      resultInsert = resultQ->Insert(acGM);
        // don't DELETE(acGM) !!!
      if (resultInsert != OK){
DBG;
        DELETE(acRecord);
        result = KO;
        continue;
      }
      iCount -= 1;
    }
DBG;
      //continue searching;
    DELETE(acRecord);
    acRecord = acTable->Next();
  }

  acTable->Close();

    //test the result     
  if (result == KO){
    //DELETE(queue) resultQ including all its members
     acGM = (GMessage*)resultQ->First();
     while (acGM != NULL){
       DELETE(acGM);
       acGM = (GMessage*)resultQ->Next();
     }
     DELETE(resultQ);
     return NULL;
  }
  else { 
    if (iCount>0)
      WriteString(llWarning, __FILE__":%d:ACManager[%s]::GetACs(): Not enough ACs found", __LINE__, type);
  } 
  return resultQ;
}
    

/**
 * Finds and returns the AC according to its ACID.
 *
 * Searches the internal table of certificates for an access
 * certificate with the specified access certificate ID.
 *
 * @param   acID
 * @return  Access certificate that matches the query.
 * @author  marek
 * @see     Six
 */
GMessage *ACManager::GetACByACID(ACID acID)
{

  Record *acRecord;
  BytesField *acFld;
  GMessage *acGM; 

  if (acTable == NULL)
    return NULL;  

    //open the table
  if(acTable->Open() != OK){
    WriteString(llError, __FILE__":%d:ACManager[%s]::ACTableInsert(): cannot open allocation table", __LINE__, type);
    return NULL;
  }

  acRecord = acTable->FindFirst(ACMGR_TBLFLD_ACID, &acID);
  
  if(acRecord == NULL)
    return NULL;

  WriteString(llDebug, __FILE__":%d:ACManager[%s]::GetACByACID(): got record", __LINE__,
              type);

  acFld = (BytesField*) acRecord->GetField(ACMGR_TBLFLD_AC);
  WriteString(llDebug, __FILE__":%d:ACManager[%s]::GetACByACID(): got acFld", __LINE__,
              type);
 
 
  if (acFld == NULL){
    WriteString(llError, __FILE__":%d:ACManager[%s]::GetACs(): Bad structure of AC Table", __LINE__, type);
    return NULL;
  }

  acGM = acFld->GetAsGMessage();
  WriteString(llDebug, __FILE__":%d:ACManager[%s]::GetACByACID(): got acGM", __LINE__,
              type);
   
  DELETE(acRecord);
  return acGM;
}

/**
 * Returns time of next timeout.
 *
 * Works around scheduler for now - finds out the time of next
 * timeout.
 *
 * @return  Time when the timeout should occur. 
 * @author  marek
 * @see     Scheduler
 */
ESTime 
ACManager::NextTimeOut()
{
  return *nextTimeOut;
}

/**
 * Finds out how many certificates are missing.
 *
 * Controlling the count of ACs at timeout returns a GMessage for
 * mix, if necessary to get certificates.
 *
 * @param   countOfMissingACs  out parametr taht is filled with number
 *          of necessary access certificates.
 * @return  Error code
 * @author  marek
 * @see     Tests if the access certificates count is in the range specified.
 */
Err ACManager::TestACCount(Int *countOfMissingACs)
{
  Int count;

  if(countOfMissingACs == NULL)
    return KO;

  if (ACCount(&count) == KO)
     return KO;
   
  if(count < minACCount){
    *countOfMissingACs = maxACCount - count;
    WriteString(llDebug, "TestACCount - previous timeout %ld", *nextTimeOut);
    *nextTimeOut = *nextTimeOut + gotCertifTest;
    WriteString(llDebug, "TestACCount - previous timeout after + %ld", *nextTimeOut);
    
  }
  else {
    *countOfMissingACs = 0;
    *nextTimeOut = *nextTimeOut +  countTest;
  }

  return OK;

}
