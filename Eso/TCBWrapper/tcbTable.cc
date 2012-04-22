#include "./H/tcbTable.h"
#include "./H/const.h"

/**
 * TCBTable constructor.
 *
 * Creates name for decrypted table and a file to access it.
 *
 *
 * @param   tableName name of encrypted table
 * @param   definition definition of table structure
 * @param   tableRingKey symetric key used to keep table encrypted
 * @param   aLogFile log file for debugging messages
 * @author  Pavel
 * @see     Table, TableFieldQueue
 */
TCBTable::TCBTable(const Char *tableName, TableFieldQueue *definition, 
                                          const GMessage *tableRingKey,
				          LogFile *aLogFile)
: Table(tableName, definition, aLogFile)
{
  ringKey = new GMessage(tableRingKey, aLogFile);
  ringKey->DebugHeaders(logFile, "TCBTable::ringKey:");

  cipherer = new Cipherer(aLogFile);

  fileName = new Char[strlen(tableName) + 1];
  strcpy(fileName, tableName);
  WriteString(llDebug, "TCBTable::tableName = %s", tableName);
 
  Char *decryptedTableName = new Char[strlen(tableName) + 20];
  sprintf(decryptedTableName, "%s.decrypted", tableName);
  WriteString(llDebug, "TCBTable::decryptedTableName = %s", decryptedTableName);

  if(file != NULL)
    DELETE(file);
  file = new File(decryptedTableName, "a+b", aLogFile);
  
  DELETE(decryptedTableName);
}


/**
 * TCBTable destructor.
 *
 * Destroyes everything created.
 *
 *
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
TCBTable::~TCBTable()
{
  if(ringKey != NULL)
    DELETE(ringKey);
  if(cipherer != NULL)
    DELETE(cipherer);
  DELETE(fileName);
}


/**
 * Opens TCBTable.
 *
 * Uses ringKey to decrypt the table to allow all common
 * table operations.
 *
 *
 * @return  error code
 * @author  Pavel
 * @see     Table, Cipherer, TCBWrapper
 */
Err 
TCBTable::Open()
{
  return Table::Open();
/*
 MsgField *encryptedData, *decryptedData;
  File *encryptedFile;
  Char *tmp;

  WriteString(llDebug, "TCBTable::Open()");

  if(IsOpen())
    return OK;

    // if file does not exist, skip the decrypting phase
  encryptedFile = new File(fileName, "rb", logFile);
  Int exists = encryptedFile->Exists();
  DELETE(encryptedFile);
  if(exists)
  {
      // decrypt the file
    encryptedData = new BytesMsgField();
    if(encryptedData->SetAsFile(fileName) != OK)
    {
      WriteString(llError, __FILE__":%d:Cannot load from file %s", __LINE__, fileName);
    }
    cipherer->DecryptWithSymetricKey(ringKey, encryptedData, &decryptedData);
    DELETE(encryptedData);
    if(decryptedData == NULL)
    {
      WriteString(llError, "TCBTable %s cannot be decrypted", fileName);
      DBG;
      return KO;
    }

    decryptedData->SetLogFile(logFile);
    decryptedData->GetAsFile(tmp = file->GetName());

    DELETE(decryptedData);
    DELETE(tmp);
  }

    // open it
  DBG;
  return file->Open();
  */
}


/**
 * Closes TCBTable.
 *
 * Uses ringKey to encrypt previously decrypted table.
 *
 *
 * @return  error code
 * @author  Pavel
 * @see     Table, Cipherer, TCBWrapper
 */
Err 
TCBTable::Close()
{
  return Table::Close();
/*
  MsgField *encryptedData, *decryptedData;
  Char *tmp;

  WriteString(llDebug, "TCBTable::Close()");

  if(!IsOpen())
    return OK;

    // close the file
  file->Close();

    // encrypt it
  decryptedData = new FileMsgField("", tmp= file->GetName(), logFile);
  
  Err result = cipherer->EncryptWithSymetricKey(ringKey, decryptedData, &encryptedData);
  
  DELETE(decryptedData);
  DELETE(tmp);

    // remove the decrypted copy
  file->Remove();

  if((result != OK) || (encryptedData == NULL))
  {
    WriteString(llError, "TCBTable %s cannot be encrypted", fileName);
    return KO;
  }

  encryptedData->GetAsFile(fileName);
  DELETE(encryptedData);

  return OK;
  */
}
