#include <string.h>

#include "./H/fields.h"
#include "./H/myString.h"
#include "./H/msgField.h"
#include "./H/gMessage.h"

/**
 * Constructor of StringField.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
BytesField::BytesField(Char *aName, MsgField *aValue, LogFile *aLogFile) 
: TableField(aName, aLogFile)
{
  value = NULL;
  SetAsMsgField(aValue);
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
BytesField::~BytesField()
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
Field *BytesField::CreateCopy()
{
  return new BytesField(name, value, logFile);
}

/**
 * This field does NOT SUPPORT SEARCHING YET - has to be able.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Int BytesField::EqualTo(void *aValue)
{
  if((value == NULL) || (aValue == NULL))
    return 0;
  else
    return value->EqualTo((MsgField *)aValue);
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
Size BytesField::SizeOf()
{
  if(value == NULL)
    return sizeof(Size);

    // size of stored field is equal to the size of contents
    // plus size of header
  return value->SizeOf() + sizeof(Size);
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
Char *BytesField::AsString()
{
//!!
  return NULL;
}


/**
 * Sets the "value" to "*aValue". Counts with MsgField!!
 * Accepts only NOT NULL value, MsgField can be empty.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Err BytesField::SetValue(Void *aValue)
{
  return SetAsMsgField((MsgField *)aValue);
}


/**
 * Gets contents of this BytesField as a GMessage.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
GMessage *BytesField::GetAsGMessage()
{
  if(value == NULL)
    return NULL;

  return value->GetAsGMessage();
}


/**
 * Gets contents of this BytesField as a MsgField.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
MsgField *BytesField::GetAsMsgField()
{
  if(value == NULL)
    return NULL;

  return (MsgField*)value->CreateCopy();
}


/**
 * Enables simple storage of MsgField.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Err BytesField::SetAsGMessage(GMessage *aGMessage)
{
  Err result;
  MsgField *field;

  if(aGMessage != NULL)
  {
    field = aGMessage->StoreToMsgField();
    if(field == NULL)
      return KO;
  }
  else
    field = NULL;

  result = SetAsMsgField(field);

  if(field)
    DELETE(field);

  return result;
}


/**
 * Enables simple storage of GMessage.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Err BytesField::SetAsMsgField(MsgField *aMsgField)
{
  MsgField *field;

  if(aMsgField == NULL)
    field = NULL;
  else
  {
    field = (MsgField*)aMsgField->CreateCopy();
    if(field == NULL)
      return KO;
  }

  if(value != NULL)
    DELETE(value);

  value = field;

  return OK;
}

/**
 * Reads the contents of a field stored in a file. ACCEPTS reading an
 * empty value.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Err BytesField::ReadFromFile(File *aFile, Size *bytesToEORec)
{
  Err result;
  Size fileSize;
  Size position;
  Size contentLength;
  MsgField *aMsgField;

  if(aFile == NULL)
    return KO;

    // check if there is enough room to read content length 
  position = aFile->Tell();
  fileSize = aFile->GetSize();
  if((sizeof(Size) > *bytesToEORec) || (position + sizeof(Size) > fileSize))
    return KO;

    // read length of field's contents
  if(sizeof(Size) != aFile->ReadBuffer(&contentLength, sizeof(Size)))
    return KO;
  position += sizeof(Size);
  *bytesToEORec -= sizeof(Size);

    // check if there is enough bytes we are to read
  if((contentLength > *bytesToEORec) || (position + contentLength > fileSize))
    return KO;

    // choose Bytes/FileMsgField
  aMsgField = MsgField::NewMsgField(contentLength);
  if(aMsgField == NULL)
    return KO;

    // call its method ReadFromFile
  if(aMsgField->ReadFromFile(aFile, contentLength) != OK)
  {
    DELETE(aMsgField);
    return KO;
  }
  *bytesToEORec -= contentLength;

    // set the newly read MsgField as a current value of this field
  if(aMsgField->SizeOf() > 0)
    result = SetAsMsgField(aMsgField);
  else
    result = SetAsMsgField(NULL);

  DELETE(aMsgField);

  return result;
}

/**
 * Writes the contents of a MsgField "value" to an open file.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Err BytesField::WriteToFile(File *aFile)
{
  Size aSize;
  Size bytesWritten;

  if(aFile == NULL)
    return KO;

    // write the size of the contents of the MsgField in "value" (even if it is NULL)
  aSize = (value == NULL) ? 0L : value->SizeOf();
  bytesWritten = aFile->WriteBuffer(&aSize, sizeof(Size));
  if(bytesWritten != sizeof(Size))
    return KO;

    // write the contents of the MsgField in "value", if it is NULL, write nothing
  if(value != NULL)
    return value->WriteToFile(aFile);

  return OK;
}
