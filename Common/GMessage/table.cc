#include "./H/table.h"
#include "./H/myString.h"

/**
 * Table constructor.
 *
 * Description: This file contains the Table class which enables
 * persistent "table-like" storage of information with sequential
 * access only.
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Table::Table(const Char *fileName, TableFieldQueue *aDefinition,
                                   LogFile *aLogFile)
:Debugable(aLogFile)
{
    // to access the file on a disk
  file = new File(fileName, "a+b", aLogFile);

    // copy the definiton queue
  definition = new TableFieldQueue(*aDefinition);

    // find out if the records' size will be fixed
  Record *record = NewRecord();
  if(record->IsFixed())
    recordLength = record->SizeOf();
  else
    recordLength = 0;
  DELETE(record);

    // initialize other attributes
	searchField = 0;
  searchValue = NULL;
  lastRecordLength = 0;
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
Table::~Table()
{
  if(file != NULL)
    DELETE(file);
  if(definition != NULL)
    DELETE(definition);
}


/**
 * Writes the specified record at the current file position.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err Table::WriteRecord(Record *aRecord)
{
  return aRecord->WriteToFile(file);
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Creates new instance of Record, reads the data from the current file
 * pointer position, fills the newly created record wiht it.
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Record *Table::ReadRecord()
{
    // create new empty record (according to the "definition")
  Record *record = NewRecord();
  if(record == NULL)
    return NULL;

    // read the rest of the record
  if(record->ReadFromFile(file, &lastRecordLength) == KO)
  {
    DELETE(record);
    return NULL;
  }

    // return "record"
  return record;
}


/**
 * Reads the header part of the record from the file to find out
 * its length.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Size Table::ReadRecordLength()
{
  Size length;

  if(! file->ReadBuffer(&length, sizeof(length)))
    return 0;

  return(length);
}


/**
 * Checks if the Delete operation can be done.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int Table::CanDelete()
{
    // The DELETE(operation) can be executed only after call to First() or
    // Next() (after FindFirst(), FindNext() as well, but they use the
    // methods mentioned above).
    // It's impossible to call Delete() twice without a call to First() or
    // Next() in between.
	return(lastRecordLength);
}


/**
 * Checks if the Update operation can be done.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int Table::CanUpdate()
{
    // The same conditions as by Delete with exception that Update() can be
    // called anytimes one after another (but the First() or Next() method
    // must preceed the first of those calls).
	return(lastRecordLength);
}


/**
 * Returns the name of a temporary file for this table.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Char *Table::CreateTmpFileName()
{
  Char *tempName;

  if((file == NULL) || (GetFileName() == NULL))
    return NULL;

  tempName = new Char[MyStrLen(GetFileName()) + 2];
  strcpy(tempName, GetFileName());
  strcat(tempName, "~");

  return(tempName);
}


/**
 * Returns the name of the file in which this table is kept.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Char *Table::GetFileName()
{
  if(file == NULL)
    return NULL;
  else
    return(file->GetName());
}


/**
 * 1 if the table is open.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int Table::IsOpen()
{
  if(file == NULL)
    return 0;
  else
    return file->IsOpen();
}


/**
 * 1 if the stored records are of the same size.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int Table::IsFixed()
{
  return((recordLength > 0) ? 1 : 0);
}


/**
 * Opens the file in which the table is stored.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err Table::Open()
{
  return(file->Open());
}


/**
 * Closes the file in which the table is stored.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err Table::Close()
{
  lastRecordLength = 0;
  searchField = 0;
  searchValue = NULL;
  return(file->Close());
}


/**
 * Removes the file in which the table is stored.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err Table::Remove()
{
  if(file == NULL)
    return KO;

  return file->Remove();
}


/**
 * Returns the first record of the table.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Record *Table::First()
{
  file->SeekStart();
	return(ReadRecord());
}


/**
 * Returns the succeeding record of the table.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Record *Table::Next()
{
	return(ReadRecord());
}


/**
 * Creates an empty record according to the "definition" TableFieldQueue.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Record *Table::NewRecord()
{
  return(new Record(definition));
}


/**
 * Appends the specified record to the table.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err Table::InsertRecord(Record *aRecord)
{
  file->SeekEnd();
  return(WriteRecord(aRecord));
}

/**
 * Deletes the last read record (see conditions described in CanDelete).
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err Table::DeleteRecord()
{
  if(CanDelete())
  {
    File *tempFile;
    Char *tempName;
    Size  oldRecLength;
    Long pos;

      // remember the beginning of the old record
    oldRecLength = lastRecordLength + SIZE_OF_HEADER;
    pos = file->Tell() - oldRecLength;

      // copy the current file to temp file (from start to "pos")
    tempName = CreateTmpFileName();
    if(tempName == NULL)
      return KO;

    tempFile = new File(tempName, "w+b", logFile);
    tempFile->Open();
    file->CopyStartToPos(tempFile, pos);
    DELETE(tempName);

      // copy the rest of "file" skipping the record to delete
    file->SeekCurrent(oldRecLength);
    file->AppendTheRestTo(tempFile);
    file->Close();
    tempFile->Close();

      // rename TempFile
    tempFile->Rename(file->GetName());
    DELETE(tempFile);
    file->Open();

      // seek the position of the record next to the deleted one
    file->SeekStart(pos);

    	// zakazu dalsi Delete
    lastRecordLength = 0;

    return OK;
	}
  else
    return KO;
}

/**
 * Updates the last accessed record (see conditions described in CanUpdate.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err Table::UpdateRecord(Record *aRecord)
{
  Table *tempTable;
  File *tempFile;
  Char *tempName;
  Size pos, oldRecLength;

	if(CanUpdate())
  {
      // remember the beginning of the old record
    oldRecLength = lastRecordLength + SIZE_OF_HEADER;

      // If the size of records stored in the table is fixed
      // or the length of the update record equals the length
      // of the old record than a simple algorithm can be used...
    if(0/*IsFixed() || (oldRecLength == aRecord->SizeOf())*/)
    {
        // write the new record over the old one
      file->SeekCurrent(-oldRecLength);
      WriteRecord(aRecord);
    }

      // ...otherwise a temporary table must be used.
    else
    {
        // remember the beginning of the old record
      oldRecLength = lastRecordLength + SIZE_OF_HEADER;
      pos = file->Tell() - oldRecLength;

        // copy the current file to temp file (from start to "pos")
      tempName = CreateTmpFileName();
      if(tempName == NULL)
        return KO;

      tempFile = new File(tempName, "w+b", logFile);
      tempFile->Open();
      file->CopyStartToPos(tempFile, pos);
      DELETE(tempFile);

        // to the place of the old record write the new one
      tempTable = new Table(tempName, definition, logFile);
      tempTable->Open();
      tempTable->InsertRecord(aRecord);
      DELETE(tempTable);

        // copy the rest of "file" skipping the old record
      file->SeekCurrent(oldRecLength);
      tempFile = new File(tempName, "a+b", logFile);
      tempFile->Open();
      file->AppendTheRestTo(tempFile);
      file->Close();
      tempFile->Close();

        // rename TempFile
      tempFile->Rename(file->GetName());
      DELETE(tempFile);
      DELETE(tempName);
      file->Open();

        // seek the end of the newly created record
        // (size of the header is already included in a call to SizeOf()
      file->SeekStart(pos + aRecord->SizeOf());
    }
    lastRecordLength = aRecord->SizeOf() - SIZE_OF_HEADER;

    return OK;
	}
  else
    return KO;
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Returns the first record from the beginning of the table where the field
 * of name "aFieldName" has a value matching "aFieldValue".
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Record *Table::FindFirst( const Char *aFieldName, const Void *aFieldValue)
{
  Record *r;

    // get index of the searched field
  searchField = definition->GetIndexOf((Char *)aFieldName);
  searchValue = (Void *)aFieldValue;

  if(searchField <= 0)
    return NULL;

  r = First();
  while(r != NULL)
  {
    if(r->FieldByIndexEqualTo(searchField, searchValue))
      break;
    DELETE(r);
    r = Next();
  }

  return r;
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Returns the next record matching criteria specified in FindFirst().
 * A call to FindFirst() must preceed the call to FindNext().
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Record *Table::FindNext()
{
  Record *r;

  if(searchField <= 0)
    return NULL;

  r = Next();
  while(r != NULL)
  {
    if(r->FieldByIndexEqualTo(searchField, searchValue))
      break;
    DELETE(r);
    r = Next();
  }

  return r;
}
