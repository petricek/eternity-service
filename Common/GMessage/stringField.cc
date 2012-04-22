#include <string.h>

#include "./H/fields.h"
#include "./H/myString.h"


/**
 * Constructor of StringField.
 *
 * Description: This file contains the Field, IntField, StringField 
 * classes which are used for inserting and retrieving
 * information into/from a Record.
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
StringField::StringField(Char *aName, Char *aValue, LogFile *aLogFile) 
: TableField(aName, aLogFile)
{
  value = NULL;
  SetValue(aValue);
}


/**
 * Destructor of StringField.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
StringField::~StringField()
{
  if(value != NULL)
    DELETE(value);
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
Field *StringField::CreateCopy()
{
  return new StringField(name, value, logFile);
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
Int StringField::EqualTo(void *aValue)
{
  if (value == NULL) 
    return (aValue ==NULL);
  else
    return (strcmp((Char*) aValue, value) == 0);
}


/**
 * Returns size of string representation of value.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Size StringField::SizeOf()
{
  if (value == NULL)
    return 1;
  else
    return MyStrLen(value) + 1;
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
Err StringField::SetValue(Void *aValue)
{
  Char *newValue;

  if(aValue == NULL)
  {
    if(value != NULL)
      DELETE(value);
    value = NULL;
    return OK;
  }

  newValue = new Char[MyStrLen((char*)aValue) + 1];
  if(newValue == NULL)
    return KO;

  if(value != NULL)
    DELETE(value);
  value = newValue;
  strcpy(value, (char*)aValue);

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
Err StringField::ReadFromFile(File *aFile, Size *bytesToEORec)
{
  Size readBytes;
  Long i;
  Int readC;
  Char c;
  Char *newValue;
  Long position; 

  if (aFile == NULL)
    return KO;
  position = aFile->Tell();

    //find the length of the string
  i = 1;
  readC = aFile->ReadBuffer(&c, sizeof(Char));
  if(readC != sizeof(Char))
    return KO;  

    //find the /0 at the end of the string
  while(c != 0){

    readC = aFile->ReadBuffer(&c, sizeof(Char));
    if (readC != sizeof(Char))
      return KO;

    i++;

      //if it is longer then the record
    if ((Size)i > *bytesToEORec)
      return KO;
  }
    
    //return back
  aFile->SeekCurrent(-i);

    // create space for new string
  newValue = new Char[i];
  if (newValue == NULL)
    return KO;

  readBytes =  aFile->ReadLongBuffer(newValue, i);

  if (readBytes != (Size)i){
    DELETE(newValue);
    return KO;
  }
  
    //fill the value with a new value
  if (value)
    DELETE(value);
  value = newValue;

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
Err StringField::WriteToFile(File *aFile)
{
  Size writtenBytes;
  Size shouldWrite;
  Char emptyStr[2] = "";
  Char *strToWrite;
 
  if (aFile == NULL)
    return KO;

  if(value != NULL)
    strToWrite  = value;
  else
    strToWrite  = emptyStr;

  shouldWrite = SizeOf();
  writtenBytes = aFile->WriteLongBuffer(strToWrite, shouldWrite);

  return ((writtenBytes != shouldWrite) ? KO : OK);
   
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
Char *StringField::AsString()
{
  Char *c;
  Size i;

  if (value == NULL) return NULL;
  i = this->SizeOf();
  c = new Char[i];
  MyStrCpy(c, value, i);
  return c;
}

