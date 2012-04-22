#include "./H/uidGenerator.h"

/**
 * Unique ID generator constructor.
 *
 * Dlouhy_komentar
 *
 *
 * @param   anIDTableName
 * @param   startAt
 * @author  Pavel
 * @see     RandomGenerator
 */
UniqueIDGenerator::UniqueIDGenerator(Char *anIDTableName, ID startAt)
{
  TableFieldQueue *definition;

  startID = startAt;

    // initialize mutex
  pthread_mutex_init(&mutex, NULL);
  
  if(anIDTableName == NULL)
    lastIDTable = NULL;
  else
  {
      // lastIDTable init
    definition = new TableFieldQueue();
    if(CreateDefinition(definition) == KO)
      lastIDTable = NULL;
    else
      lastIDTable = new Table(anIDTableName, definition);

      // file with lastIDTable initialization
    InitLastIDFile(anIDTableName);
  }
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
 * @see     RandomGenerator
 */
Err UniqueIDGenerator::CreateDefinition(TableFieldQueue *definition)
{
  IDField *field;

  if(definition == NULL)
    return KO;

  field = new IDField(UIDG_FLD_ID);
  if(field == NULL)
    return KO;

  if(definition->Insert(field) == KO)
  {
    DELETE(field);
    return KO;
  }

  return OK;
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
 * @see     RandomGenerator
 */
Err UniqueIDGenerator::InitLastIDFile(Char *anIDTableName)
{
  File   *anIDFile;
  Record *record;
  Err     result;

  if(anIDTableName == NULL)
    return KO;

  anIDFile = new File(anIDTableName, "wb");
  if(anIDFile == NULL)
    return KO;

    // if the file was initialized before no further
    // initialization is needed
  if(anIDFile->Exists())
  {
    DELETE(anIDFile);
    return OK;
  }

    // create new record and set it to the starting value
  record = lastIDTable->NewRecord();
  if(record == NULL)
  {
    DELETE(anIDFile);
    return KO;
  }
  if(record->SetField(UIDG_FLD_ID, &startID) == KO)
  {
    DELETE(anIDFile);
    DELETE(record);
    return KO;
  }

  pthread_mutex_lock(&mutex);

    // open table
  if(lastIDTable->Open() == KO)
  {
    DELETE(anIDFile);
    DELETE(record);
    return KO;
  }

    // insert the new record
  result = lastIDTable->InsertRecord(record);

    // close the table
  lastIDTable->Close();

  pthread_mutex_unlock(&mutex);

  DELETE(anIDFile);
  DELETE(record);

  return result;
}


/**
 * Get unique ID.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     RandomGenerator
 */
Err UniqueIDGenerator::GetID(ID *newID)
{
  Record  *record;
  IDField *field;
  Err      result;
  ID       anID;

  if(lastIDTable == NULL)
    return KO;

    // open table
  if(lastIDTable->Open() == KO)
    return KO;

    // get the first record (the only one) and close the table
  record = lastIDTable->First();
  if(record == NULL)
  {
    lastIDTable->Close();
    return KO;
  }

    // get the field from the record
  field = (IDField*)(record->GetField(UIDG_FLD_ID));
  if(field == NULL)
  {
    lastIDTable->Close();
    DELETE(record);
    return KO;
  }

    // get ID from the field
  anID = field->GetIDValue();

    // increase value
  anID++;

    // update table
  field->SetValue(&anID);
  result = lastIDTable->UpdateRecord(record);

    // finish
  memcpy(newID, &anID, sizeof(ID));
  lastIDTable->Close();
  DELETE(record);

  return result;
}
