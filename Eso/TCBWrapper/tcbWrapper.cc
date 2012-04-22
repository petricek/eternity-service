#include "./H/tcbWrapper.h"
#include "./../../Common/RandomGenerator/H/randomGenerator.h"
#include "./../../Common/H/labels.h"

/**
 * TCBWrapper constructor.
 *
 * Initializes all ancestors.
 *
 *
 * @param   anEso pointer to owner of this class
 * @param   aLogFile log file for debugging messages
 * @author  Pavel
 * @see     SWTCBWrapper
 */
TCBWrapper::TCBWrapper(Eso *anEso, LogFile *aLogFile)
: Rollbackable(),
  Debugable(aLogFile),
  SixOffspring(anEso)
{}

/**
 * TCBWrapper destructor.
 *
 * Does nothing special.
 *
 *
 * @author  Pavel
 * @see     SWTCBWrapper
 */
TCBWrapper::~TCBWrapper()
{}


/**
 * SWTCBWrapper constructor.
 *
 * Restores the MasterKey, decrypts ringKeys, instantiates TCBTables.
 *
 *
 * @param   anEso pointer to owner of this class
 * @param   aLogFile log file for debugging messages
 * @author  Pavel
 * @see     Eso
 */
SWTCBWrapper::SWTCBWrapper(Eso *anEso, LogFile *aLogFile)
: TCBWrapper(anEso, aLogFile),
  cipherer(NULL),
  masterKey(NULL), 
  storageTable(NULL), 
  certifTable(NULL)
{
  GMessage *ringKeys = NULL;
  
  cipherer = new Cipherer(aLogFile);
  
  if(LoadMasterKey() != OK)

   WriteString(llError, "SWTCBWrapper: cannot load master key");
  else 
  {
    masterKey->DebugHeaders(logFile, "Master Key");
    if(GetRingKeys(&ringKeys) != OK)

     WriteString(llError, "SWTCBWrapper: cannot load ring keys");
    else 
    {
      ringKeys->DebugHeaders(logFile, "Ring keys");
      if(InstantiateTCBTables(ringKeys) != OK)

       WriteString(llError, "SWTCBWrapper: cannot instantiate TCBTables");
    }
  }

  if(ringKeys != NULL)
    DELETE(ringKeys);
}


/**
 * SWTCBWrapper destructor.
 *
 * Destroyes all created objects.
 *
 *
 * @author  Pavel
 * @see     
 */
SWTCBWrapper::~SWTCBWrapper()
{
  if(cipherer != NULL)
    DELETE(cipherer);
  if(masterKey != NULL)
    DELETE(masterKey);
  if(storageTable != NULL)
    DELETE(storageTable);
  if(certifTable != NULL)
    DELETE(certifTable);
}


/**
 * Loads master key.
 *
 * Initializes masterKey of key hierarchy loading it from a file.
 *
 *
 * @return  error code
 * @author  Pavel
 * @see      
 */
Err
SWTCBWrapper::LoadMasterKey()
{
  masterKey = new GMessage(logFile);
  if(masterKey == NULL)
    return KO;
    
  Char *tmp;
  Err result = masterKey->LoadFromFile(tmp = ((Eso *) six)->GetPath(TCBW_MASTER_KEY_FILE));
  DELETE(tmp);

  return result;
}


/**
 * Loads ring keys.
 *
 * Initializes ring keys for tables loading it from files, that are
 * encrypted by master key (according to rules of hierarchical key
 * management).
 *
 *
 * @param   ringKeys out parameter returning ring keys if successfull
 * @return  error code
 * @author  Pavel
 * @see     Cipherer, MsgField, GMessage
 */
Err
SWTCBWrapper::GetRingKeys(GMessage ** ringKeys)
{
  MsgField *eRingKeys, *dRingKeys;
  Char *tmp;
  Err result;

  eRingKeys = new BytesMsgField("", logFile);
  if(eRingKeys == NULL)
    return KO;
    
  result = eRingKeys->SetAsFile(tmp = ((Eso *) six)->GetPath(TCBW_RING_KEYS_FILE));
  DELETE(tmp);
  if(result != OK)
  {

   WriteString(llError, "SWTCBWrapper: cannot find ring keys file");
    DELETE(eRingKeys);
    return KO;
  }
  
  cipherer->DecryptWithSymetricKey(masterKey, eRingKeys, &dRingKeys);
  if(dRingKeys == NULL)
  {

   WriteString(llError, "SWTCBWrapper: cannot decrypt ring keys");
    DELETE(eRingKeys);
    return KO;
  }
  
  *ringKeys = dRingKeys->GetAsGMessage();
  if(ringKeys == NULL)

   WriteString(llError, "SWTCBWrapper: cannot convert ring keys");
  
  DELETE(eRingKeys);
  DELETE(dRingKeys);

  return (*ringKeys != NULL) ? OK : KO;
}


/**
 * Initializes all TCBTables.
 *
 * Creates instances for both storageTable and certifTable generating
 * its definitions and using appropriate ring keys.
 *
 *
 * @param   ringKeys ring keys for TCBTable constructors
 * @return  error code
 * @author  Pavel
 * @see     TCBTable, TableFieldQueue
 */
Err
SWTCBWrapper::InstantiateTCBTables(const GMessage *ringKeys)
{

#define STORAGE_CHECK_NULL if(tmp == NULL) { DELETE(storageDef); return KO; }
#define AC_CHECK_NULL if(tmp == NULL) { DELETE(storageDef); DELETE(certifDef); return KO; }
  
  Char *tmpFName;
  TableField *tmp;
  TableFieldQueue *storageDef, *certifDef;
  GMessage *storageRingKey, *certifRingKey;
  
  if(ringKeys == NULL)
    return KO;

    // create storageTable definition
  storageDef = new TableFieldQueue(logFile);
  if(storageDef == NULL)
    return KO;
    
  storageDef->Insert( tmp = new IDField    (TCBW_FLD_FID, 0, logFile));            STORAGE_CHECK_NULL
  storageDef->Insert( tmp = new BytesField (TCBW_FLD_FFID, NULL, logFile));        STORAGE_CHECK_NULL
  storageDef->Insert( tmp = new StringField(TCBW_FLD_FILE_NAME, NULL, logFile));   STORAGE_CHECK_NULL
  storageDef->Insert( tmp = new BytesField (TCBW_FLD_STORAGE_KEY, NULL, logFile)); STORAGE_CHECK_NULL
  storageDef->Insert( tmp = new BytesField (TCBW_FLD_KEYWORDS, NULL, logFile));    STORAGE_CHECK_NULL
  storageDef->Insert( tmp = new SizeField  (TCBW_FLD_FILE_SIZE, NULL, logFile));   STORAGE_CHECK_NULL

    // create certifTable definition
  certifDef = new TableFieldQueue(logFile);
  if(certifDef == NULL)
  {
    DELETE(storageDef);
    return KO;
  }

  certifDef->Insert( tmp = new IDField   (TCBW_FLD_ACID,         0,    logFile)); AC_CHECK_NULL
  certifDef->Insert( tmp = new BytesField(TCBW_FLD_AC_PUB_KEY,   NULL, logFile)); AC_CHECK_NULL
  certifDef->Insert( tmp = new BytesField(TCBW_FLD_AC_PRIV_KEY,  NULL, logFile)); AC_CHECK_NULL
  certifDef->Insert( tmp = new BytesField(TCBW_FLD_TCB_PUB_KEY,  NULL, logFile)); AC_CHECK_NULL
  certifDef->Insert( tmp = new BytesField(TCBW_FLD_TCB_PRIV_KEY, NULL, logFile)); AC_CHECK_NULL

    // create storageTable
  storageRingKey = ringKeys->GetAsGMessage(TCBW_RING_KEY_STORAGE);
  if(storageRingKey == NULL)
  {

   WriteString(llError, "SWTCBWrapper: Ring key for storage table not found");
    DELETE(storageDef);
    DELETE(certifDef);
    return KO;
  }
  storageTable = new TCBTable(tmpFName = ((Eso *) six)->GetPath(TCBW_STORAGE_TABLE_FILE), 
                              storageDef, 
			      storageRingKey,
			      logFile);
  DELETE(tmpFName);
  DELETE(storageDef);
  DELETE(storageRingKey);
  if(storageTable == NULL)
  {
    DELETE(certifDef);
    return KO;
  }

    // create certifTable
  certifRingKey = ringKeys->GetAsGMessage(TCBW_RING_KEY_CERTIFICATES);
  if(certifRingKey == NULL)
  {

   WriteString(llError, "SWTCBWrapper: Ring key for certificates table not found");
    DELETE(certifDef);
    return KO;
  }
  certifTable = new TCBTable(tmpFName = ((Eso *) six)->GetPath(TCBW_CERTIFICATES_TABLE_FILE),
                             certifDef,
      			     certifRingKey,
			     logFile);
  DELETE(tmpFName);
  DELETE(certifDef);
  DELETE(certifRingKey);
  if(certifTable == NULL)
  {
    DELETE(storageTable);
    storageTable = NULL;
    return KO;
  }
  
  return OK;

#undef STORAGE_CHECK_NULL
#undef AC_CHECK_NULL

}


/**
 * Inserts record to storageTable.
 *
 * Creates new Record for storageTable, fills it with given arguments
 * and inserts it to storageTable.
 *
 *
 * @param   fid file ID of saved file
 * @param   ffid search file ID of saved file
 * @param   fileName name of save file
 * @param   storageKey symetric key used to encrypt saved file
 * @param   keywords keywords for saved file
 * @param   fileSize size of saved file
 * @return  error code
 * @author  Pavel
 * @see     TCBTable
 */
Err
SWTCBWrapper::StorTableInsert(  const ID fid, 
		                  const MsgField *ffid,
		                  const Char     *fileName,
		                  const GMessage *storageKey,
		                  const MsgField *keywords,
		                  const Size      fileSize)
{
  if(storageTable == NULL)
    return KO;

  MsgField *tmp;
  Record *storRec = storageTable->NewRecord();

  storRec->SetField(TCBW_FLD_FID,        &fid);
  storRec->SetField(TCBW_FLD_FFID,        ffid);
  storRec->SetField(TCBW_FLD_FILE_NAME,   fileName);
  storRec->SetField(TCBW_FLD_STORAGE_KEY, 
                    tmp = storageKey->StoreToMsgField());
  storRec->SetField(TCBW_FLD_KEYWORDS,    keywords);
  storRec->SetField(TCBW_FLD_FILE_SIZE,  &fileSize);
  
  if(tmp != NULL) DELETE(tmp);

    // open storage table
  if(storageTable->Open() != OK)
  { 
    DELETE(storRec);
    return KO;
  }

    // check for record with same ID (fid)
  Err result; 
  Record *findRec = storageTable->FindFirst(TCBW_FLD_FID, &fid);
  if(findRec != NULL) {
    WriteString(llError, __FILE__":%d:Already have file with same File ID (%ld)", 
                         __LINE__, fid);
    DELETE(findRec);
    result = KO;
  }
  else 
      // insert into storage table
    result = storageTable->InsertRecord(storRec);
  
  DELETE(storRec);
  
    // close storage table
  storageTable->Close();

  return result;
}


/**
 * Parses record got from storageTable.
 *
 * Parses record that was got from storageTable filling
 * out parameters.
 *
 *
 * @param   storRec record read from storTable
 * @param   fid file ID of saved file (out parameter)
 * @param   ffid search file ID of saved file (out parameter)
 * @param   fileName name of save file (out parameter)
 * @param   storageKey symetric key used to encrypt saved file (out parameter)
 * @param   keywords keywords for saved file (out parameter)
 * @param   fileSize size of saved file (out parameter)
 * @return  error code
 * @author  Pavel
 * @see     Record, TableField
 */
Err
SWTCBWrapper::StorTableParseRecord(const Record *storRec, 
                                     ID        *fid,
				     MsgField **ffid,
		                     Char     **fileName,
		                     GMessage **storageKey,
		                     Keywords **keywords,
		                     Size      *fileSize)
{
  Err result;
  
  if(storRec == NULL)
    return KO;

  if(fid != NULL)
  {
    result = ((IDField *)((Record *)storRec)->GetField(TCBW_FLD_FID))->GetID(fid);
    if(result != OK)
      return KO;
  }

  if(ffid != NULL)
    *ffid = ((BytesField *)((Record *)storRec)->GetField(TCBW_FLD_FFID))->GetAsMsgField();
  
  if(fileName != NULL)
    *fileName = ((StringField *)((Record *)storRec)->GetField(TCBW_FLD_FILE_NAME))->AsString();

  if(storageKey != NULL)
    *storageKey = ((BytesField *)((Record *)storRec)->GetField(TCBW_FLD_STORAGE_KEY))->GetAsGMessage();
  
  if(keywords != NULL)
    *keywords = (Keywords *) (((BytesField *)((Record *)storRec)->GetField(TCBW_FLD_KEYWORDS))->GetAsGMessage());
  
  if(fileSize != NULL)
  {
    *fileSize = ((SizeField *)((Record *)storRec)->GetField(TCBW_FLD_FILE_SIZE))->GetSizeValue();
    WriteString(llDebug, "size = %ld", *fileSize);
  }

  return OK;
}


/**
 * Finds record in storageTable.
 *
 * Searches open table for appearance of record with
 * appropriate file ID. Enables to close storageTable
 * after successfull search.
 *
 *
 * @param   fid file ID to search by
 * @param   shouldClose Should I close the storageTable, after I'm
 *                      finished?
 * @param   fileName name of save file (out parameter)
 * @param   storageKey symetric key used to encrypt saved file (out parameter)
 * @return  error code
 * @author  Pavel
 * @see     
 */
Err
SWTCBWrapper::StorTableFindByFID(  const ID   fid, 
			             const Int  shouldClose, 
			             Char     **fileName,
				     GMessage **storageKey)
{
  if(storageTable == NULL)
    return KO;

  if(storageTable->Open() != OK)
    return KO;

  Record *storRec = storageTable->FindFirst(TCBW_FLD_FID, &fid);
  
  if(shouldClose != 0)
    storageTable->Close();
  
  if(storRec == NULL)
    return KO;

  Err result = StorTableParseRecord(storRec, NULL, NULL, fileName, storageKey); 

  DELETE(storRec);

  return result;
}


/**
 * Finds record in storageTable.
 *
 * Searches open table for appearance of record with
 * appropriate search file ID.
 *
 *
 * @param   ffid search file ID to search by
 * @param   fileName name of save file (out parameter)
 * @param   storageKey symetric key used to encrypt saved file (out parameter)
 * @param   keywords keywords for saved file (out parameter)
 * @param   size size of saved file (out parameter)
 * @return  error code
 * @author  Pavel
 * @see     
 */
Err
SWTCBWrapper::StorTableFindByFFID(  const MsgField *ffid, 
			              Char          **fileName, 
			              GMessage      **storageKey,
			              Keywords      **keywords,
			              Size           *size)
{
  if(storageTable == NULL)
    return KO;

  if(storageTable->Open() != OK)
    return KO;

  Record *storRec = storageTable->FindFirst(TCBW_FLD_FFID, ffid);
  storageTable->Close();
  if(storRec == NULL)
    return KO;

  Err result = StorTableParseRecord(storRec, NULL, 
                                             NULL, 
			              	     fileName, 
				             storageKey,
				             keywords,
				             size);
  DELETE(storRec);

  return result;
}


/**
 * Inserts record to certifTabel.
 *
 * Creates new Record for certifTable, fills it with given arguments
 * and inserts it to certifTable.
 *
 *
 * @param   acid Access certificate ID
 * @param   acPubKey Access certificate public key    
 * @param   acPrivKey Access certificate private key
 * @param   tcbPubKey TCB public key
 * @param   tcbPrivKey TCB private key
 * @return  error code
 * @author  Pavel
 * @see     
 */
Err
SWTCBWrapper::CertTableInsert(  const ACID      acid, 
		                  const GMessage *acPubKey,
		                  const GMessage *acPrivKey,
		                  const GMessage *tcbPubKey,
		                  const GMessage *tcbPrivKey)
{
  DBG;
  if(certifTable == NULL)
    return KO;

  ((GMessage *)tcbPrivKey)->DebugHeaders(logFile, "TCBW::CertTableInsert - tcbPrivKey");


    // new record for certifTable
  Record *certifRec = certifTable->NewRecord();
  
    // set fields (GMessages need to be converted to MsgFields)
  MsgField *tmp;
  certifRec->SetField(TCBW_FLD_ACID,        &acid);
  certifRec->SetField(TCBW_FLD_AC_PUB_KEY,   tmp = acPubKey->StoreToMsgField());
    if(tmp != NULL) DELETE(tmp);
  certifRec->SetField(TCBW_FLD_AC_PRIV_KEY,  tmp = acPrivKey->StoreToMsgField());
    if(tmp != NULL) DELETE(tmp);
  certifRec->SetField(TCBW_FLD_TCB_PUB_KEY,  tmp = tcbPubKey->StoreToMsgField());
    if(tmp != NULL) DELETE(tmp);
  certifRec->SetField(TCBW_FLD_TCB_PRIV_KEY, tmp = tcbPrivKey->StoreToMsgField());
    if(tmp != NULL) DELETE(tmp);
  
  DBG;
    // open the table
  if(certifTable->Open() != OK)
  { 
    DELETE(certifRec);
    DBG;
    return KO;
  }
  
    // check for existence of same ACID
  DBG;
  Err result;
  Record *findRec = certifTable->FindFirst(TCBW_FLD_ACID, &acid);
  if(findRec != NULL) {
    WriteString(llError, __FILE__":%d:ACID %ld already assigned.", __LINE__, acid);
    DELETE(findRec);
    result = KO;
  }
  else 
      // insert new record
    result = certifTable->InsertRecord(certifRec);
  
  DELETE(certifRec);
  
  certifTable->Close();

  return result;
}


/**
 * Finds record in certifTable.
 *
 * Searches open table for appearance of record with
 * appropriate search file ID. Enables to close 
 * certifTable after successfull search.
 *
 *
 * @param   acid Access certificate ID
 * @param   shouldClose Should I close the certifTable, after I'm
 *                      finished?
 * @param   acPubKey Access certificate public key (out parameter)
 * @param   acPrivKey Access certificate private key (out parameter)
 * @param   tcbPubKey TCB public key (out parameter)
 * @param   tcbPrivKey TCB private key (out parameter)
 * @return  error code
 * @author  Pavel
 * @see     
 */

Err
SWTCBWrapper::CertTableFindByACID(  const ACID acid,
			              const shouldClose,
			              GMessage **acPubKey,
			              GMessage **acPrivKey,
			              GMessage **tcbPubKey,
			              GMessage **tcbPrivKey)
{
DBG;
  if(certifTable == NULL)
    return KO;

DBG;
  if(certifTable->Open() != OK)
    return KO;

DBG;
  Record *certifRec = certifTable->FindFirst(TCBW_FLD_ACID, &acid);
  
DBG;
  if(shouldClose != 0)
    certifTable->Close();

DBG;
  if(certifRec == NULL)
    return KO;

DBG;
  if(acPubKey != NULL)
    *acPubKey = ((BytesField *)certifRec->GetField(TCBW_FLD_AC_PUB_KEY))->GetAsGMessage();

DBG;
  if(acPrivKey != NULL)
    *acPrivKey = ((BytesField *)certifRec->GetField(TCBW_FLD_AC_PRIV_KEY))->GetAsGMessage();

DBG;
  if(tcbPubKey != NULL)
    *tcbPubKey = ((BytesField *)certifRec->GetField(TCBW_FLD_TCB_PUB_KEY))->GetAsGMessage();

DBG;
  if(tcbPrivKey != NULL)
    *tcbPrivKey = ((BytesField *)certifRec->GetField(TCBW_FLD_TCB_PRIV_KEY))->GetAsGMessage();

  DBG;
  if(tcbPrivKey != NULL){
    WriteString(llDebug, "Melo by byt: *tcbPrivKey != NULL:%d", *tcbPrivKey != NULL);
    (*tcbPrivKey)->DebugHeaders(logFile, "TCBW::CertTableFindByACID - tcbPrivkey");
  }
  else{ 
    WriteString(llDebug,"TCBW::CertTableFindByACID - didn't want tcbPrivkey");
  }

  DELETE(certifRec);

  return OK;
}


/**
 * Decrypts data with private key.
 *
 * Decrypts data with TCB private key identified by access certificate
 * ID. Proper key is found in certifTable by acid.
 *
 *
 * @param   acid Access certificate ID
 * @param   data Data to be encrypted by TCB private key
 * @return  Decrypted data
 * @author  Pavel
 * @see     
 */
MsgField *
SWTCBWrapper::DecryptByTCBPriv(const ACID acid, const MsgField *data)
{
  GMessage *tcbPrivKey;
  MsgField *eData;

    // find TCBPriv to acid
  if(CertTableFindByACID(acid, 1, NULL, NULL, NULL, &tcbPrivKey) != OK)
  {
    WriteString(llWarning, "TCBW::DecryptByTCBPriv - acid not found %d", acid);
    return NULL;
  }

  tcbPrivKey->DebugHeaders(logFile, "TCBW::DecryptByTCBPriv - tcbPrivKey");
    // decrypt
  Err result = cipherer->DecryptWithPrivateKey(tcbPrivKey, (MsgField*)data, &eData);
  DELETE(tcbPrivKey);

  return result == OK ? eData : NULL;
}


/**
 * Creates a header to reply.
 *
 * This method creates header by encrypting given file size and search
 * file ID with client's public key and giving them into GMessage.
 *
 *
 * @param   clientPubKey Public key to encrypt header with
 * @param   ffid         Search file ID to include in header
 * @param   size         File size to include in header
 * @return  Header encrypted by clientPubKey
 * @author  Pavel
 * @see     
 */
GMessage *
SWTCBWrapper::CreateReplyFileHeader(const GMessage *clientPubKey,
                                              const MsgField *ffid,
					      const Size      size)
{
  Err       result;
  MsgField *dSize, *eSize, *eFFID;
  GMessage *answerMsg;
  
  if((dSize = new BytesMsgField("", (Size *)&size, sizeof(Size))) == NULL)
    return NULL;
    
  if(cipherer->EncryptWithPublicKey((GMessage *)clientPubKey, dSize, &eSize) != OK)
  {
    DELETE(dSize);
    return NULL;
  }
  
  if(cipherer->EncryptWithPublicKey((GMessage *)clientPubKey, (MsgField *)ffid, &eFFID) != OK)
  {
    DELETE(eSize);
    DELETE(dSize);
    return NULL;
  }
  
  if((answerMsg = new GMessage()) == NULL)
  {
    DELETE(eFFID);
    DELETE(eSize);
    DELETE(dSize);
    return NULL;
  }
  
  result = answerMsg->SetAsMsgField(GM_FFID, eFFID);
  result += answerMsg->SetAsMsgField(GM_FILE_SIZE, eSize);

  DELETE(eFFID);
  DELETE(eSize);
  DELETE(dSize);

  if(result == 2*OK)
    return answerMsg;
  else
  {
    DELETE(answerMsg);
    return NULL;
  }
}


/**
 * Rolls back a transaction.
 *
 * Rollback of a transaction means deletion of file
 * stored with same ID as is ID of that transaction.
 *
 *
 * @param   tid ID of a transaction to rollback
 * @return  error code
 * @author  Pavel
 * @see     Rollbackable, DeleteFileByFileID()
 */
Err
SWTCBWrapper::Rollback(const TID tid)
{
  return DeleteFileByFileID(tid);
}


/**
 * Saves file.
 *
 * Prepares file for saving: decrypting it by TCB private key
 * and encrypting with a newly generated symetric storage key.
 * Then it puts all information (prepared the same way) together
 * to storageTable.
 *
 *
 * @param   acid Access certificate ID
 * @param   fid file ID of file to save
 * @param   ffid search file ID of file to save
 * @param   keywords keywords for file to save
 * @param   data to save
 * @return  error code
 * @author  Pavel
 * @see     Cipherer, StorTableInsert()
 */
Err
SWTCBWrapper::SaveFile(const ACID acid, const ID fid, 
                                          const MsgField *ffid, 
                                          const MsgField *keywords,
                                          const MsgField *data)
{
  GMessage *tcbPrivKey;
  GMessage *storageKey;
  Char     *fname;
  Char     *tmp;
  MsgField *dFFID, *dKeywords, *dData, *eData;
  
  if((ffid == NULL) || (keywords == NULL) || (data == NULL))
    return KO;

    // find TCBPrivKey to ACID
  if(CertTableFindByACID(acid, 1, NULL, NULL, NULL, &tcbPrivKey) != OK)
  {
    WriteString(llWarning, "TCBW::SaveFile cannot find ACID %d", acid);
    return KO;
  }

    // decrypt ffid, keywords and data
  cipherer->DecryptWithPrivateKey(tcbPrivKey, (MsgField *)ffid,     &dFFID);
  if(dFFID == NULL)
    return KO;
  cipherer->DecryptWithPrivateKey(tcbPrivKey, (MsgField *)keywords, &dKeywords);
  if(dKeywords == NULL)
  {
    DELETE(dFFID);
    return KO;
  }
DBG;
  cipherer->DecryptWithPrivateKey(tcbPrivKey, (MsgField *)data,     &dData);
DBG;
  if(dData == NULL)
  {
DBG;
    DELETE(dFFID);
    DELETE(dKeywords);
    return KO;
  }
DBG;

    // generate storage key and encrypt eData with it
  storageKey = cipherer->GenerateSymetricKey();
  cipherer->EncryptWithSymetricKey(storageKey, dData, &eData);
  
    // store encrypted data to a file
  fname = CreateTempFileName(tmp = ((Eso *) six)->GetPath(TCBW_DIR_STORAGE));
  DELETE(tmp);
  eData->GetAsFile(fname);
  File *aFile = new File(fname);
  Size fsize = aFile->GetSize();

Char *ffidName = six->GetPath("upload.ffid");
dFFID->GetAsFile(ffidName);
DELETE(ffidName);
  
    // insert record to storageTable
  Err result = StorTableInsert(fid, dFFID, fname, storageKey, dKeywords, fsize);
  
    // dispose off
  DELETE(tcbPrivKey);
  DELETE(storageKey);
  DELETE(fname);
  DELETE(dFFID);
  DELETE(dKeywords);
  DELETE(dData);
  DELETE(eData);
  DELETE(aFile);

  return result;
}


/**
 * Deletes file by file ID.
 *
 * What more should be said?
 *
 *
 * @param   fid ID of a file to be deleted
 * @return  error code
 * @author  Pavel
 * @see     
 */
Err
SWTCBWrapper::DeleteFileByFileID(const ID fid)
{
  Char *fname;
  
    // find file w/ ID tid, don't close storageTable, get filename
  if(StorTableFindByFID(fid, 0, &fname) != OK)
  {
    storageTable->Close();
    WriteString(llWarning, "TCBW::DeleteFileByFileID - searched file not found");
    return OK;
  }

    // DELETE(record);
  storageTable->DeleteRecord();
  storageTable->Close();

  File *file = new File(fname);
  file->Remove();

  DELETE(file);
  DELETE(fname);

  return OK;
}


/**
 * Computes MAC.
 *
 * Computes hash from stored file and given oAuth.
 *
 *
 * @param   fid ID of a file MAC should be counted from
 * @param   oAuth String sent by bank to verify file owner
 * @param   mac Counted hash (out parameter)
 * @return  error code
 * @author  Pavel
 * @see     Cipherer, CreateTempFileName()
 */
Err
SWTCBWrapper::ComputeMAC(const ID fid, const MsgField *oAuth, 
                                         MsgField **mac)
{
  Err   result;
  Char *tmp;
  File *file;
  Size  size;
  Void *buff;
  Char *fName, *tempName;
  GMessage *storageKey;
  MsgField *dData, *eData;
  MsgField *filePlusOAuth; 

  if((oAuth == NULL) || (((MsgField *)oAuth)->SizeOf() == 0))
    return KO;
  
    // find file with FID "fid"
  if(StorTableFindByFID(fid, 1, &fName, &storageKey) != OK)
  {
    WriteString(llWarning, "TCBW::ComputeMAC - cannot find file with FID %d", fid);
    return KO;
  }
  
    // encrypt the file
  eData = new FileMsgField("", fName);
  result = cipherer->DecryptWithSymetricKey(storageKey, eData, &dData);
  DELETE(eData);
  if(result != OK)
  {
    WriteString(llWarning, "TCBW::ComputeMAC - cannot find file with FID %d", fid);
    return KO;
  }
  
    // append "oAuth" to encrypted file
  tempName = CreateTempFileName(tmp = ((Eso *) six)->GetPath(TCBW_DIR_TEMP));
  if(tempName == NULL)
  {
    WriteString(llError, "TCBW::ComputeMAC - cannot create temp file name");
    return KO;
  }
  
  result = dData->GetAsFile(tempName);
  DELETE(dData);
  if(result != OK)
  {
    WriteString(llError, "TCBW::ComputeMAC - cannot save decrypted file");
    DELETE(tempName);
    return KO;
  }
  
  file = new File(tempName, "ab");
  if(file == NULL)
  {
    WriteString(llError, "TCBW::ComputeMAC - cannot create File object");
    DELETE(tempName);
    return KO;
  }
  
  
  if(file->Open() != OK)
  {
    WriteString(llError, "TCBW::ComputeMAC - cannot open decrypted file");
    file->Remove();
    DELETE(file);
    DELETE(tempName);
    return KO;
  }
  
  buff = ((MsgField *)oAuth)->GetAsBytes(&size);
  if(buff == NULL)
  {
    WriteString(llError, "TCBW::ComputeMAC - cannot get oAuth in bytes");
    file->Remove();
    DELETE(file);
    DELETE(tempName);
    return KO;
  }
  
  Long bytesWritten = file->WriteLongBuffer(buff, size);
  DELETE(buff);
  if((Size)bytesWritten != size)
  {
    WriteString(llError, "TCBW::ComputeMAC - cannot append oAuth to decrypted file");
    file->Remove();
    DELETE(file);
    DELETE(tempName);
    return KO;
  }
  
  file->Close();

    // open it to MsgField and remove it
  filePlusOAuth = new FileMsgField("", tempName);
  file->Remove();
  DELETE(file);
  DELETE(tempName);

    // compute MAC
  WriteString(llDebug, "TCBW::ComputeMAC - calling Cipherer::MessageDigest");
  WriteString(llDebug, "  filePlusOAuth = %s", filePlusOAuth->GetAsBytes(&size));
  result = cipherer->MessageDigest(filePlusOAuth, mac);
  if(result != OK)
    WriteString(llError, "TCBW::ComputeMAC - cannot count MAC");
  DELETE(filePlusOAuth);
  
  return result;
}


/**
 * Searches for file by file attribute mask.
 *
 * Searches through storageTable for files that are matching all criteria 
 * expressed in FAM - File Attribute Mask. This was designed to create simple
 * extensions even for complex masks. Returns file descripting headers
 * encrypted by clientPubKey.
 *
 *
 * @param   acid Access certificate ID identifying TCB private key
 *               used to encrypt File Attribute Mask
 * @param   fam Encrypted File Attribute Mask to search by
 * @param   clientPubKey Client's public key to encrypt replies with
 * @return  Queue with headers encrypted by clientPubKey, NULL
 *          otherwise
 * @author  Pavel
 * @see     Keywords, DecryptByTCBPriv()
 */
Queue *
SWTCBWrapper::GetFileHeadersByFAM(const ACID acid,
                                       const MsgField *fam, 
                                       const GMessage *clientPubKey)
{
  Err result;
  Size size;
  Keywords *storKwds;
  GMessage *eAnswer;
  MsgField *ffid, *dFAM;
  Queue *answerQueue;
  
  if(fam == NULL)
    return NULL;

DBG;
    // decrypt FAM
  dFAM = DecryptByTCBPriv(acid, fam);
  if(dFAM == NULL)
  {
    WriteString(llWarning, "cannot decrypt FAM");
    return NULL;
  }
    
DBG;
    // convert FAM to GMessage
  GMessage *famMsg = ((MsgField *)dFAM)->GetAsGMessage();
  DELETE(dFAM);
  if(famMsg == NULL)
  {
    WriteString(llWarning, "cannot parse decrypted FAM");
    return NULL;
  }
  famMsg->DebugHeaders(logFile, "Got FAM:");
DBG;
    // prepare queue for encrypted answers
  answerQueue = new Queue();
  if(answerQueue == NULL)
  {
    WriteString(llWarning, "cannot create answer queue");
    return NULL;
  }

DBG;
    // parse famMsg
  Size minSize, maxSize;
  if(famMsg->GetAsID(GM_MIN_SIZE, &minSize) != OK)
    minSize = 0;
  if(famMsg->GetAsID(GM_MAX_SIZE, &maxSize) != OK)
    maxSize = 0xFFFFFFFF;
  Keywords *allKwds = (Keywords *)famMsg->GetAsGMessage(GM_KEYWORDS);
  if(allKwds == NULL)
  {
    WriteString(llWarning, __FILE__":%d:No keywords to search by given.", __LINE__);
    return NULL;
  }
  allKwds->DebugHeaders(logFile, "Keywords received:");
  Char *keywordsPattern = allKwds->Keyword(1);
  DELETE(allKwds);
DBG;
    // check for files that fit the requests
  storageTable->Open();
  Record *storRec = storageTable->First();
  while(storRec != NULL)
  {
DBG;
      // pares storRec
    result = StorTableParseRecord(storRec, NULL, &ffid, NULL, NULL, &storKwds, &size);
    WriteString(llDebug, "parse result = %d", result);
    WriteString(llDebug, "size = %ld", size);
    storKwds->DebugHeaders(logFile, "storKwds:");
DBG;
  WriteString(llWarning, "GetFileHeadersByFAM: pattern: %s", keywordsPattern);
      // check keywords
    if((result == OK) && (keywordsPattern != NULL))
    {
    DBG;
      if(storKwds != NULL) {
    DBG;
        result = (storKwds->MatchesPattern(keywordsPattern)) ? OK : KO;
    DBG;
      }
      else 
      {
    DBG;
	DELETE(ffid);
	DELETE(storKwds);
        result = KO;
      }
    }
      
DBG;
      // check size
    if(result == OK){
     DBG;
      result = ((minSize <= size) && (size <= maxSize)) ? OK : KO;
    }
DBG;
      // add to answer
    if(result == OK)
    {

     WriteString(llDebug, "TCBWrapper::Creating answer");
      if((eAnswer = CreateReplyFileHeader(clientPubKey, ffid, size)) != NULL){
        DBG;

Char *ffidName = six->GetPath("download.ffid");
ffid->GetAsFile(ffidName);
DELETE(ffidName);

eAnswer->DebugHeaders(logFile, "eAnswer");
  
        answerQueue->Insert(eAnswer);
      }
      else 

       WriteString(llError, "TCBWrapper::GetFileHeadersByFAM():"
	                     "cannot create answer");
      
DBG;
      DELETE(ffid);
      DELETE(storKwds);
    }

DBG;
    DELETE(storRec);
    storRec = storageTable->Next();
  }

DBG;
  return answerQueue;
}


/**
 * Pops file with ffid.
 *
 * Searches storageTable for files that have search file ID equal to ffid.
 *
 *
 * @param   acid Access certificate ID identifying TCB private key
 *               used to encrypt search file ID
 * @param   ffid Encrypted search file ID
 * @param   clientPubKey Client's public key to encrypt replies with
 * @return  File encrypted with clientPubKey, NULL otherwise
 * @author  Pavel
 * @see     Cipherer
 */
MsgField *
SWTCBWrapper::GetFileByFinderFID(const ACID      acid,
                                         const MsgField *ffid, 
                                         const GMessage *clientPubKey)
{
  Char *fname;
  GMessage *storageKey;
  MsgField *eData, *dFFID, *dData, *reData;
DBG;
    // decrypt ffid 
  dFFID = DecryptByTCBPriv(acid, ffid);
  if(dFFID == NULL)
  {
    WriteString(llWarning, "TCBW::GetFileByFinderFID - cannot decrypt ffid");
    return NULL;
  }
  
DBG;
    // find file with ffid (its storageKey, file name)
  if(StorTableFindByFFID(dFFID, &fname, &storageKey) != OK)
  {
    WriteString(llWarning, "TCBW::GetFileByFinderFID - ffid not found");
    DELETE(dFFID);
    return NULL;
  }

DBG;
    // decrypt the file with storageKey
  eData = new FileMsgField("whatever", fname, logFile);
  if(cipherer->DecryptWithSymetricKey(storageKey, eData, &dData) != OK)
  {
    WriteString(llError, "TCBW::GetFileByFinderFID - cannot decrypt file");
    DELETE(dFFID);
    DELETE(eData);
    return NULL;
  }
    
DBG;
    // encrypt the file with clientPubKey
  if(cipherer->EncryptWithPublicKey((GMessage *)clientPubKey, dData, &reData) != OK)
  {
    WriteString(llError, "TCBW::GetFileByFinderFID - cannot decrypt file");
    DELETE(dFFID);
    DELETE(eData);
    DELETE(dData);
    return NULL;
  }

DBG;
    // dispose off
  DELETE(fname);
  DELETE(storageKey);
  DELETE(dFFID);
  DELETE(eData);
  DELETE(dData);

DBG;
  return reData;
}


/**
 * Decrypts by private key.
 *
 * Decrypts data with private key identified by access certificate
 * ID. Private key is found in certifTable.
 *
 *
 * @param   acid Access certificate ID identifying access public key
 *          that was used to encrypt data
 * @param   data Data whished to decrypt
 * @return  Decrypted data if successfull, NULL otherwise
 * @author  Pavel
 * @see     Cipherer, CertTableFindByACID()
 */
MsgField *
SWTCBWrapper::DecryptByACPriv(const ACID acid, const MsgField *data)
{
  GMessage *acPrivKey;
  MsgField *dData;
 
DBG;
    // find ACPrivKey
  if((CertTableFindByACID(acid, 1, NULL, &acPrivKey) != OK) || (acPrivKey == NULL))
  {
    WriteString(llWarning, __FILE__":%d:ACPrivKey not found by ACID.", __LINE__);
    return NULL;
  }

DBG;
    // decrypt
  if(cipherer->DecryptWithPrivateKey(acPrivKey, (MsgField *)data, &dData) != OK)
    WriteString(llWarning, __FILE__":%d:Cannot decrypt by ACPrivKey.", __LINE__);

  return dData;
}


/**
 * Generates keys for access certificate.
 *
 * Generates asymetric key pairs for access certificate and stores
 * them in certifTable to be able to use them automatically later
 * given only access certificate ID, that is generated as well.
 *
 *
 * @param   acid Generated access certificate ID  (out parameter)
 * @param   acPubKey Generated access public key (out parameter)
 * @param   tcbPubKey Generated access public key (out parameter)
 * @return  error code
 * @author  Pavel
 * @see     Cipherer
 */
Err
SWTCBWrapper::GenerateACKeys(ACID *acid, GMessage **acPubKey, 
                                           GMessage **tcbPubKey)
{
    // generate ACID
  Void *tmp;
  Size aSize;
  RandomGenerator *randGen = new RandomGenerator(logFile);
  MsgField *acidMsg = randGen->RandomBytes(sizeof(ACID));
  *acid = *(ACID*)(tmp = acidMsg->GetAsBytes(&aSize));
  DELETE(tmp);
  DELETE(randGen);
  DELETE(acidMsg);
  
    // generate keys
  DBG;
  GMessage *keyPair = cipherer->GenerateAsymetricKey();
  if(keyPair == NULL)
  {
    return KO;
  }

  DBG;
  *acPubKey = keyPair->GetAsGMessage(GM_PUBLIC_KEY_INFO);
  GMessage *acPrivKey = keyPair->GetAsGMessage(GM_PRIVATE_KEY_INFO);
  DELETE(keyPair);

  DBG;
  (*acPubKey)->DebugHeaders(logFile, "TCBW::GenACKeys - acPubKey");
  acPrivKey->DebugHeaders(logFile, "TCBW::GenACKeys - acPrivKey");
  
  DBG;
  keyPair = cipherer->GenerateAsymetricKey();
  if(keyPair == NULL)
  {
    delete *acPubKey;
    DELETE(acPrivKey);
    return KO;
  }
  *tcbPubKey = keyPair->GetAsGMessage(GM_PUBLIC_KEY_INFO);
  GMessage *tcbPrivKey = keyPair->GetAsGMessage(GM_PRIVATE_KEY_INFO);

   //!!!!!!
  tcbPrivKey->DebugHeaders(logFile, "TCBW::GenACKeys - tcbPrivKey");
  
  DELETE(keyPair);

  DBG;
    // store them in certificates table
  Err result = CertTableInsert(*acid, *acPubKey, acPrivKey, *tcbPubKey, tcbPrivKey);
  if(result != OK)
  {
    DELETE(*acPubKey);
    DELETE(*tcbPubKey);
  }
    // dispose off
  DELETE(acPrivKey);
  DELETE(tcbPrivKey);

  return result;
}


/**
 * Returns public keys.
 *
 * Returns both TCB and access certificate public keys identified by
 * access certificate ID. The keys are searched for in certifTable.
 *
 *
 * @param   acid Access certificate ID to search by
 * @param   acPubKey Access public key (out parameter)
 * @param   tcbPubKey TCB public key (out parameter)
 * @return  error code
 * @author  Pavel
 * @see     CertTableFindByACID()
 */
Err
SWTCBWrapper::GetACPubKeys(const ACID acid, GMessage **acPubKey, GMessage **tcbPubKey)
{
  return CertTableFindByACID(acid, 1, acPubKey, NULL, tcbPubKey, NULL);
}


/**
 * Removes access certificate keys.
 *
 * Removes both TCB and access certificate key pairs from certifTable.
 *
 *
 * @param   acid Access certificate ID identifying keys to remove
 * @return  error code
 * @author  Pavel
 * @see     CertTableFindByACID()
 */
Err
SWTCBWrapper::RemoveACKeys(const ACID acid)
{
    // find record with ACID
  if(CertTableFindByACID(acid, 0) != OK)
    return OK;
    
    // DELETE(it)
  Err result = certifTable->DeleteRecord();
  certifTable->Close();

  return result;
}


/**
 * Create new master key and reencrypt the file that stores ring keys.
 *
 * Create new master key and reencrypt the file that stores ring keys.
 *
 *
 * @return  error code
 * @author  Pavel
 * @see     Cipherer
 */
Err
SWTCBWrapper::RefreshMasterKey()
{
  return KO;
}


/**
 * Create new ring keys and reencrypt certifTable and certifTable.
 *
 * Create new ring keys and reencrypt certifTable and certifTable.
 *
 *
 * @return  error code
 * @author  Pavel
 * @see     Cipherer
 */
Err
SWTCBWrapper::RefreshRingKeys()
{
  return KO;
}


Void
SWTCBWrapper::Test()
{
#define RESULT (result == OK ? "OK" : "KO")
  ID fid = 1;
  MsgField *ffid = new BytesMsgField("", "fffffffffid", 12, logFile);
  Char *fname = new Char[10]; strcpy(fname, "file.dat");
  GMessage *stKey = new GMessage(logFile); stKey->SetAsString("neco", "cosi");
  MsgField *keywords = new BytesMsgField("", "klicova slova", 14, logFile);
  Size size = 1023;
  Err result;
  
  WriteString(llDebug, "TCBW::Test()");
  
  result = StorTableInsert(fid, ffid, fname, stKey, keywords, size);
  WriteString(llDebug, "TCBW::StorTableInsert(%d) %s", fid, RESULT);
  DELETE(fname);
  
  result = StorTableFindByFID(fid, 1, &fname);
  WriteString(llDebug, "TCBW::StorTableFindByFID(%d) %s, %s", fid, RESULT, fname);
#undef RESULT
}
