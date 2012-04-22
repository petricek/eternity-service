#include <string.h>
#include "./H/record.h"
#include "./H/fields.h"
#include "./H/myString.h"

/**
 * Record constructor.
 *
 * This file contains the Record class which is used for
 * storing and  retrieving information to/from a Table.
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
Record::Record(TableFieldQueue *modelQueue)
{
  fields = new TableFieldQueue(*modelQueue);
}

/**
 * Destructor for Record.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
Record::~Record()
{
  DELETE(fields);
}


/**
 * Composes values of fields into a string.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
Char *Record::AsString()
{
  Char *c;
  Size len, pos;
  TableField* aField;

    // get length of the array
  len = 0;
  aField = fields->First();
  while(aField != NULL) {
    len += aField->SizeOf();
    aField = fields->Next();
  }

    // allocate the size for the array
  c = new Char[SizeOfHeader() + len];
    // copy the header to the beginning
  MyStrCpy(c, (Char*) &len, sizeof(Size));

    // copy the different fields
  pos = 0;
  aField = fields->First();
  while(aField != NULL) {
    MyStrCpy(c, aField->AsString(), aField->SizeOf(),
             pos + SizeOfHeader());
    pos += aField->SizeOf();
    aField = fields->Next();
  }

  return c;
}

/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Parses "count" bytes to TableFields according to "fields"
 * reading it from the current position in an open "tableFile".
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
Err Record::ReadFromFile(File *tableFile, Size *count)
{
  Size fileSize;
  Size position;
  Size bytesToEORec;
  Err  result;
  TableField *aTableField;

  if(tableFile == NULL)
    return KO;

    // check for room to read the record length
  fileSize = tableFile->GetSize();
  position = tableFile->Tell();
  if(position + sizeof(Size) > fileSize)
    return KO;

    // read the record length
  if(sizeof(Size) != tableFile->ReadBuffer(count, sizeof(Size)))
    return KO;

    // KO, if number of bytes to be read is larger than the rest
    // of tableFile
  position += sizeof(Size);
  if(position + (*count) > fileSize)
    return KO;

    // let all TableFields in "definition" parse its values
  result = OK;
  bytesToEORec = *count;
  aTableField = fields->First();
  while((aTableField != NULL) && (result == OK))
  {
    result = aTableField->ReadFromFile(tableFile, &bytesToEORec);
    aTableField = fields->Next();
  }

  result = (result == OK) && (bytesToEORec == 0) ? OK : KO;

  return result;
}


/**
 * Writes all TableFields from "fields" stored to an open "tableFile".
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
Err Record::WriteToFile(File *tableFile)
{
  TableField *aTableField;
  Err result = OK;
  Size recordLength;

  if(tableFile == NULL)
    return KO;

    // check the record length and 
  recordLength = SizeOf() - SizeOfHeader();

    // write the "recordLength" into the "tableFile"
  if(sizeof(recordLength) != tableFile->WriteBuffer(&recordLength, sizeof(recordLength)))
    return KO;

    // tell all TableFields from fields to write themselves into
    // a "tableFile"
  aTableField = fields->First();
  while((aTableField != NULL) && (result == OK))
  {
    result = aTableField->WriteToFile(tableFile);
    aTableField = fields->Next();
  }

  result = result == OK ? OK : KO;

  return result;
}


/**
 * Returns the size of AsString().
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
Size Record::SizeOf()
{
  return fields->SizeOf() + SizeOfHeader();
}


/**
 * Returns the size of the header in a string returned by AsString().
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
Int Record::SizeOfHeader()
{
  return(sizeof(Size));
}


/**
 * Returns 1 if the length of the record is fixed; 0 otherwise.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
Int Record::IsFixed()
{
  return fields->FieldsFixed();
}


/**
 * Returns a pointer to a field with given name (or a NULL pointer).
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
TableField *Record::GetField(Char *aFieldName)
{
  return (TableField*)fields->GetField(aFieldName);
}


/**
 * Returns a pointer to a field with given index (or a NULL pointer).
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
TableField *Record::GetFieldByIndex(Int anIndex)
{
  return (TableField*)fields->GetFieldByIndex(anIndex);
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Returns 1 if the value of the field at index "anIndex" equals
 * to "aFieldValue". 0 otherwise.
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
Int Record::FieldByIndexEqualTo(Int anIndex, Void *aFieldValue)
{
  return fields->FieldByIndexEqualTo(anIndex, aFieldValue);
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Finds a field of name "aFieldName" (if such is not found returns
 * KO) and  sets its value to "aFieldValue".
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
Err Record::SetField(const Char *aFieldName, const Void *aFieldValue)
{
  return fields->SetField((Char *)aFieldName, (Void *)aFieldValue);
}
