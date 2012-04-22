#include <string.h>

#include "./H/fields.h"
#include "./H/myString.h"

/**
 * Constructor of IntField.
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
IntField::IntField(Char *aName, Int aValue, LogFile *aLogFile)
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
Field *IntField::CreateCopy()
{
  return new IntField(name, value, logFile);
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
Int IntField::EqualTo(void *aValue)
{
  return (value == *(Int*)aValue);
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
Size IntField::SizeOf()
{
  return sizeof(Int);
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
Err IntField::ReadFromFile(File *aFile, Size *bytesToEORec)
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
 * Writes the value to a file.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Err IntField::WriteToFile(File *aFile)
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
Err IntField::SetValue(Void *aValue)
{
  if(aValue == NULL)
    return KO;
  value = *(Int*)aValue;
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
Char *IntField::AsString()
{
  Char *c;
  c = new Char[this->SizeOf()];
  MyStrCpy(c, (Char*) &value, this->SizeOf());
  return c;
}
