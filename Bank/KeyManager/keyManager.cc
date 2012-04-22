#include "./H/keyManager.h"

/**
 * Creates definition of acTable - used only in constructor.
 *
 * The records from the table have 3 fields: ACID, PUB_KEY and
 * PRIVATE_KEY.
 *
 *
 * @return  tableFieldQueue - model queue 
 * @author  Marek
 * @see     
 */
TableFieldQueue *
KeyManager::CreateDefinition()
{
  TableFieldQueue *definition;
  TableField *field;

  definition = new TableFieldQueue;
  if(definition == NULL)
    return NULL;
   
    //create field for ACID
  field = new IDField(KEYMGR_TBLFLD_ACID);
  if(field == NULL){
    WriteString(llError, "KeyManager::CreateDefinition(): cannot create TableField");
    DELETE(definition);
    return NULL;
  }
  definition->Insert(field);

    //create field for AC
  field = new BytesField(KEYMGR_TBLFLD_PUB_KEY, NULL, logFile);
  if(field == NULL){
    WriteString(llError, "ACManager::CreateDefinition(): cannot create TableField");
    DELETE(definition);
    return NULL;
  }
  definition->Insert(field);

  field = new BytesField(KEYMGR_TBLFLD_PRIV_KEY, NULL, logFile);
  if(field == NULL){
    WriteString(llError, "ACManager::CreateDefinition(): cannot create TableField");
    DELETE(definition);
    return NULL;
  }
  definition->Insert(field);

  return definition;
}

/**
 * Constructor of keyManager.
 *
 * Creates the keyManager table.
 *
 *
 * @param   logFile keyManager logFile
 * @param   aTableName Name of file to create keyManager table in.
 * @author  Marek
 * @see     
 */
KeyManager::KeyManager(LogFile *aLogFile, Char* aTableName)
:Debugable(aLogFile)
{
  TableFieldQueue *definition;   

 
  definition = CreateDefinition();
  if(definition == NULL)
    WriteString(llError, "KeyManager::KeyManager(): cannot create table definition");
  DBG;
  WriteString(llDebug, "KeyManager::KeyManager(): creating table in file %s",
               aTableName);
  keyTable = new Table(aTableName, definition);
  if(keyTable == NULL)
    WriteString(llError, "KeyManager::KeyManager(): cannot create table ");
  if(definition != NULL)
    DELETE(definition);
}
              
/**
 * Destructor of keyManager.
 *
 * Deletes the keyManager's table.
 *
 *
 * @author  Marek
 * @see     
 */
KeyManager::~KeyManager()
{
  if(keyTable)
   DELETE(keyTable);
}

/**
 * Inserts keys into the table.
 *
 * Saves public and private key together with the associated ACID.
 *
 *
 * @param   acid Acid to save.
 * @param   pubKey PubKey to save.
 * @param   privKey PrivKey to save.
 * @return  error code
 * @author  Marek
 * @see     
 */
Err 
KeyManager::SaveKeys(ACID acid, GMessage *pubKey, GMessage *privKey)
{
  Record *record;
  Err result;
  MsgField *pubKeyMsgFld, *privKeyMsgFld;

DBG;
  if (keyTable == NULL)
    return KO;

DBG;
    //open the table
  if(keyTable->Open() != OK){
    WriteString(llError, "keyManager::keyTableInsert(): cannot open allocation table");
    return KO;
  }

DBG;
    //get old record with the same keyID ...
  record = keyTable->FindFirst(KEYMGR_TBLFLD_ACID, (void*)&acid);
  if(record != NULL)
     keyTable->DeleteRecord();
  keyTable->Close();  

DBG;
    //...or create a new one
  if(record == NULL){
    record = keyTable->NewRecord();
    if(record == NULL){
      WriteString(llError, "keyManager::keyTableInsert(): cannot create new allocation record");
      return KO;
    }
  }


DBG;
     //transform keys into keyMsgField
  pubKeyMsgFld = pubKey->StoreToMsgField(); 
  if (pubKeyMsgFld == NULL){
    DELETE(record);
    return KO;
  }
DBG;
  privKeyMsgFld = privKey->StoreToMsgField(); 
  if (privKeyMsgFld == NULL){
    DELETE(record);
    return KO;
  }

DBG;
    //fill the record   
  result = record->SetField(KEYMGR_TBLFLD_ACID, (void*)&acid);
DBG;
  if(record->SetField(KEYMGR_TBLFLD_PUB_KEY, (void*)pubKeyMsgFld) != OK)
    result = KO;
DBG;
  if(record->SetField(KEYMGR_TBLFLD_PRIV_KEY, (void*)privKeyMsgFld) != OK)
    result = KO;
DBG;
  DELETE(pubKeyMsgFld);
  DELETE(privKeyMsgFld);
  

DBG;
  if(result != OK){
    WriteString(llError, "keyManager::keyTableInsert(): cannot fill new allocation record");
    DELETE(record);
    return KO;
  }
   
DBG;
    //open the table
  if(keyTable->Open() != OK){
    WriteString(llError, "keyManager::keyTableInsert(): cannot open allocation table");
    return KO;
  }

DBG;
     // insert new record into keyTable
  result = keyTable->InsertRecord(record);
  if(result == KO)
    WriteString(llError, "keyManager::keyTableInsert(): cannot insert to key table");
  DELETE(record);
  keyTable->Close();

DBG;
  return result;

}

/**
 * Finds the key according to its keyID.
 *
 * Returns private key associated with ACID from the keyManager's
 * table.
 *
 *
 * @param   acid ACID to find the key with.
 * @return  privKey (GMessage)
 * @author  Marek
 * @see     
 */
GMessage *
KeyManager::GetPrivKeyByACID(ACID acID)
{
  Record *keyRecord;
  BytesField *keyFld;
  GMessage *keyGM; 

  if (keyTable == NULL)
    return NULL;  

    //open the table
  if(keyTable->Open() != OK){
    return NULL;
  }

  keyRecord = keyTable->FindFirst(KEYMGR_TBLFLD_ACID, &acID);
  
  if(keyRecord == NULL)
    return NULL;

  keyFld = (BytesField*) keyRecord->GetField(KEYMGR_TBLFLD_PRIV_KEY);
 
 
  if (keyFld == NULL){
    return NULL;
  }

  keyGM = keyFld->GetAsGMessage();
   
  DELETE(keyRecord);
  return keyGM;
}
