#include <string.h>

#include "./H/fields.h"
#include "./H/myString.h"

/**
 * Constructor of IDField.
 *
 * This file contains the Field, IntField, StringField and
 * FixedStringField classes which are used for inserting and retrieving
 * information into/from a Record.
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
IDField::IDField(Char *aName, ID aValue, LogFile *aLogFile)
: TableField(aName, aLogFile)
{
  value = aValue;
}


/**
 * Returns identical object.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Field *IDField::CreateCopy()
{
  return new IDField(name, value, logFile);
}


/**
 * Decides if the value is equal to the parameter.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Int IDField::EqualTo(void *aValue)
{
 return (value == *(ID *)aValue);
}


/**
 * Returns size of string represening the value.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Size IDField::SizeOf()
{
  return sizeof(ID);
}


/**
 * Returns value as an ID.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Err IDField::GetID(ID *anID)
{
  if(anID == NULL)
    return KO;

  memcpy(anID, &value, sizeof(ID));
  return OK;
}

/**
 * Reads the value from a file.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Err IDField::ReadFromFile(File *aFile, Size *bytesToEORec)
{
 Size readBytes;

 if (aFile == NULL)
   return KO;

 if (SizeOf() > *bytesToEORec)
   return KO;

 readBytes = aFile->ReadLongBuffer(&value, SizeOf());

 if (readBytes < SizeOf())
   return KO;

 *bytesToEORec -= SizeOf();

 return OK;
}

/**
 * Writes value to a File.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Err IDField::WriteToFile(File *aFile)
{
  Size writtenBytes;

  if (aFile == NULL)
    return KO;

  writtenBytes = aFile->WriteLongBuffer(&value, SizeOf());

  return ((writtenBytes < SizeOf()) ? KO : OK);


}

/**
 * Sets the "value" to "*aValue".
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Err IDField::SetValue(Void *aValue)
{
  if(aValue == NULL)
    return KO;
  else
    value = *(ID*)aValue;
  return OK;

}

/**
 * Returns string representation of the value.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Char *IDField::AsString()
{
  Char *c;
  c = new Char[this->SizeOf()];
  MyStrCpy(c, (Char*) &value, this->SizeOf());
  return c;
}

/**
 * Constructor of SizeField.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
SizeField::SizeField(Char *aFieldName, Size aValue, LogFile *aLogFile)
  :IDField(aFieldName, aValue, aLogFile) 
{}
