#include <string.h>

#include "./H/fields.h"
#include "./H/myString.h"

/**
 * Constructor of TimeField.
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
TimeField::TimeField(Char *aName, BasicTime *aValue, LogFile *aLogFile)
: TableField(aName, aLogFile)
{ 
  if (aValue == NULL) 
    value = DEFAULT_TIME;
  else
    value = aValue->GetTime();
}

/**
 * Special constructor of TimeField.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
TimeField::TimeField(Char *aName, TIME_REPREZ aValue, LogFile *aLogFile) 
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
Field *
TimeField::CreateCopy()
{
  return new TimeField(name, value);
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
Int 
TimeField::EqualTo(void *aValue)
{
 if (aValue == NULL) 
   return (value == DEFAULT_TIME);
 else
   return (value == ((BasicTime *)aValue)->GetTime());
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
Size 
TimeField::SizeOf()
{
  return sizeof(TIME_REPREZ);
}


/**
 * Returns the value as Time (usage: time = TimeField->GetESTimeValue().
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
ESTime 
TimeField::GetESTimeValue()
{
  ESTime t;

  ((BasicTime*)&t)->SetTime(value);

  return t;
}


/**
 * Returns the value as Time (usage: time = TimeField->GetRelTimeValue().
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
RelTime 
TimeField::GetRelTimeValue()
{
  RelTime t;

  ((BasicTime*)&t)->SetTime(value);

  return t;
}


/**
 * Reads the value froma file.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Err 
TimeField::ReadFromFile(File *aFile, Size *bytesToEORec)
{
  Size readBytes;

  if (aFile == NULL)
    return KO;

  if (SizeOf() > *bytesToEORec)
    return KO;
  readBytes = aFile->ReadLongBuffer(&value, SizeOf());

  if (readBytes < SizeOf())
    return KO;

  *bytesToEORec -=SizeOf();

  return OK;
}

/**
 * Writes the value to a File.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Err 
TimeField::WriteToFile(File *aFile)
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
Err 
TimeField::SetValue(Void *aValue)
{
  if(aValue == NULL)
    return KO;
  else
    value = ((BasicTime*)aValue)->GetTime();
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
Char *
TimeField::AsString()
{
  Char *c;
  
    c = new Char[this->SizeOf()];
    MyStrCpy(c, (Char*) &value, this->SizeOf());
    return c;
  
}
