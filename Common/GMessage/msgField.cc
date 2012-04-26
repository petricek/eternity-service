#include "./H/msgField.h"
#include "./H/gMessage.h"
#include "./H/myString.h"

/**
 * MsgField constructor.
 *
 * This file contains the MsgField class and its descendants
 * BytesMsgField and FileMsgField. The descendants are used to store
 * information kept in fields of GMessage. BytesMsgField stores the
 * information as a byte stream in memory, FileMsgField stores only a
 * name of a (temporary) file that keeps the information.
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
MsgField::MsgField(Char *aFieldName, LogFile *aLogFile)
:Field(aFieldName, aLogFile)
{
  value = NULL;
  size = 0;
}


/**
 * Destructor for MsgField.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
MsgField::~MsgField()
{
  if(value != NULL)
    DELETE(value);
}


/**
 * According to "aSize" creates Bytes/ or FileMsgField.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
MsgField *MsgField::NewMsgField(Size aSize, Char *aFieldName, 
                                            LogFile *aLogFile)
{
  if(aSize <= MAX_BYTE_MSG_FIELD_SIZE)
    return new BytesMsgField(aFieldName, NULL, 0L, aLogFile);
  else
    return new FileMsgField(aFieldName, NULL, aLogFile);
}


/**
 * Creates a copy of "size" bytes starting from the position "value"
 * points to.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Void *MsgField::CreateCopyOfValue()
{
  if(value == NULL)
    return NULL;

  Void *buffer = new Char[size];
  if(buffer == NULL)
    return NULL;

  memcpy(buffer, value, size);

  return buffer;
}


/**
 * Returns 1 if this field's content equals to the content of the given
 * field.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int MsgField::EqualTo(MsgField *aMsgField)
{
  Char *tmp1, *tmp2;
  Size size;
  Err  result;

  if(aMsgField == NULL)
    return 0;

  if(aMsgField->SizeOf() != SizeOf())
    return 0;

  tmp1 = (Char *)aMsgField->GetAsBytes(&size);
  if(tmp1 == NULL)
    return 0;

  tmp2 = (Char *)GetAsBytes(&size);
  if(tmp2 == NULL)
  {
    DELETE(tmp1);
    return 0;
  }

  result = MyStrCmp(tmp1, tmp2, size);

  DELETE(tmp1);
  DELETE(tmp2);

  return result;
}


/**
 * Sets "value" and "size" according to "aBuffer" and "aSize".
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err MsgField::SetValue(Void *aBuffer, Size aSize)
{
  Char *newValue = NULL;

  if((aBuffer != NULL) && (aSize > 0))
  {
    newValue = new Char[aSize];
    if(newValue == NULL)
      return KO;

    memcpy(newValue, aBuffer, aSize);
    size = aSize;
  }
  else
    size = 0;
  
  if(value != NULL)
    DELETE(value);
  value = newValue;

  return OK;
}


/**
 * Returns a copy of the stored information as a NULL-terminated string.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Char *MsgField::GetAsString()
{
  Size aSize;
  Char *tmp = (Char *) GetAsBytes(&aSize);
  Char *safeStr = new Char[aSize + 1];
  memcpy(safeStr, tmp, aSize);
  *(safeStr + aSize) = '\0';
  DELETE(tmp);
  return safeStr;
}

/**
 * Gets (and stores) the information from a NULL-terminated string.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err MsgField::SetAsString(Char *aString)
{
  return SetAsBytes(aString, MyStrLen(aString) + 1);
}


/**
 * BytesMsgField constructor.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
BytesMsgField::BytesMsgField(Char *aFieldName, Void *aBuffer, 
                                               Size aSize, 
                                               LogFile *aLogFile)
:MsgField(aFieldName, aLogFile)
{
  SetAsBytes(aBuffer, aSize);
}


/**
 * Returns copy of itself.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Field *BytesMsgField::CreateCopy()
{
  return new BytesMsgField(name, value, size);
}


/**
 * Returns the size of information stored (size of byte stream in this
 * case).
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Size BytesMsgField::SizeOf()
{
  return size;
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Writes "size" bytes from "value" to "aFile". This is used to store
 * the whole GMessage into a file or when storing a MsgField to a Table.
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err BytesMsgField::WriteToFile(File *aFile)
{
  if(aFile == NULL)
    return KO;

  Size count = aFile->WriteLongBuffer(value, size);
  return (count == size) ? OK : KO;
}


/**
 * Reads "aSize" bytes from "aFile" to "value" and "size".
 * This is used when retrieving the information from a file.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err BytesMsgField::ReadFromFile(File *aFile, Size aSize)
{
  Size count;
  Char *aBuffer;
  Err result;

    // alloc
  aBuffer = new Char[aSize];
  if(aBuffer == NULL)
    return KO;

    // read from file
  count = aFile->ReadLongBuffer(aBuffer, aSize);
  if(count == aSize)
      // if successful
    result = SetValue(aBuffer, aSize);
  else
      // if unsuccessful
    result = KO;

  DELETE(aBuffer);

  return result;
}


/**
 * Returns a copy of the stored information as a byte stream.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Void *BytesMsgField::GetAsBytes(Size *aSize)
{
    // create copy of "value"
  void *newBuffer = CreateCopyOfValue();

    // set "aSize"
  if(aSize != NULL)
  {
    if(newBuffer != NULL)
      *aSize = size;
    else
      *aSize = 0;
  }

  return newBuffer;
}


/**
 * This is used for unnesting a nested GMessage. There has to be a call
 * to this method when you want to acces the nested information.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
GMessage *BytesMsgField::GetAsGMessage()
{
  if(value == NULL)
    return NULL;
  else
    return new GMessage(value, size);
}


/**
 * Stores the information in a file "aFileName".
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err BytesMsgField::GetAsFile(Char *aFileName)
{
//  WriteString(llDebug, "BytesMsgField::GetAsFile() - start");

  Err result;
  Size count;
  File *aFile = new File(aFileName, "wb", logFile);
  if(aFile == NULL)
  {
    WriteString(llError, "BytesMsgField::GetAsFile(): cannot create File object");
    return KO;
  }

  if(aFile->Open() == KO)
  {
    WriteString(llError, "BytesMsgField::GetAsFile(): cannot open %s", aFileName);
    DELETE(aFile);
    return KO;
  }

  count = aFile->WriteLongBuffer(value, size);
  result = (count == size) ? OK : KO;
  if(result == KO)
    WriteString(llError, "BytesMsgField::GetAsFile(): cannot copy enough bytes");
  //else
    //WriteString(llDebug, "BytesMsgField::GetAsFile() - end successful");

  DELETE(aFile);
  return result;
}


/**
 * Gets (and stores) the information from a byte stream.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err BytesMsgField::SetAsBytes(Void *aBuffer, Size aSize)
{
  return SetValue(aBuffer, aSize);
}


/**
 * Gets (and stores) the information from a file.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err BytesMsgField::SetAsFile(Char *aFileName)
{
  File *aFile;
  Char *aBuffer;
  Size aSize;
  Err result;

  aFile = new File(aFileName, "rb", logFile);
  if(aFile == NULL)
    return KO;

    // check the length of the file
  aFile->Open();
  aSize = aFile->GetSize();
  if(aSize == 0)
  {
    DELETE(aFile);
    return KO;
  }

    // try to alloc aBuffer
  aBuffer = new Char[aSize];
  if(aBuffer == NULL)
  {
    DELETE(aFile);
    return KO;
  }

    // read the file
  result = aFile->ReadLongBuffer(aBuffer, aSize);
  DELETE(aFile);
  if(result == KO)
    return KO;

  result = SetValue(aBuffer, aSize);
  DELETE(aBuffer);

  return result;
}


/**
 * Constructor for FileMsgField.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
FileMsgField::FileMsgField(Char *aFieldName, Char *aFileName, 
                                             LogFile *aLogFile)
:MsgField(aFieldName, aLogFile)
{
  SetAsFile(aFileName);
}


/**
 * Destructor - deletes the file that was used to store the information.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
FileMsgField::~FileMsgField()
{
  File *f;
  if(value != NULL)
  {
    f = new File((Char*)value, "r", logFile);
    if(f != NULL)
    {
      f->Remove();
      DELETE(f);
    }
  }
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Nothing but a helpful routine shortening the code.
 * Opens a file that stores the information. :-) Wouldn't you say that?
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
File *FileMsgField::OpenFile(Char *aFileMode)
{
  File *aFile;
  Err result;

    // there's nothing to open yet
  if((value == NULL) && (aFileMode[0] == 'r'))
    return NULL;

    // there's still nothing to open but we are going to create it
  if(value == NULL)
  {
    value = CreateTempFileName();
    size = MyStrLen((Char*)value) + 1;
    if(value == NULL)
      return NULL;
  }

    // open it, finally ;-)
  aFile = new File((Char*)value, aFileMode, logFile);
  if(aFile == NULL)
    return NULL;

  result = aFile->Open();
  if(result == OK)
    return aFile;
  else
  {
    DELETE(aFile);
    return NULL;
  }
}


/**
 * returns copy of itself
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Field *FileMsgField::CreateCopy()
{
  FileMsgField *aMsgField;
	// create new FileMsgField
  aMsgField = new FileMsgField(name, NULL, logFile);
  if(aMsgField == NULL)
    return NULL;
    // copy current data into it
  aMsgField->SetAsFile((Char*)value);
  return aMsgField;
}


// returns the size of information stored (size of data in a file in this
// case).
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
Size FileMsgField::SizeOf()
{
  Size aSize;

  File *currFile = OpenFile("rb");
  aSize = currFile->GetSize();
  DELETE(currFile);

  return aSize;
}


// Writes all data from a file of name "value" to "aFile".
// This is used to store the whole GMessage into a file or Table.
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
Err FileMsgField::WriteToFile(File *aFile)
{
  File *fromFile;
  Err result;

  fromFile = OpenFile("rb");
  if(fromFile == NULL)
    return KO;

  result = fromFile->CopyTheRestTo(aFile);
  DELETE(fromFile);
  if(result == KO)
    return KO;

  return OK;
}


// Reads "aSize" bytes from "aFile" to "value" and "size".
// This is used when retrieving the information from a file.
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
Err FileMsgField::ReadFromFile(File *aFile, Size aSize)
{
  File *toFile;
  Err result;

  toFile = OpenFile("wb");
  if(toFile == NULL)
    return KO;

  result = aFile->CopyTo(toFile, aSize);
  DELETE(toFile);
  if(result == KO)
    return KO;

  return OK;
}


// Returns a copy of the stored information as a byte stream.
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
Void *FileMsgField::GetAsBytes(Size *aSize)
{
  File *aFile;
  Char *aBuffer;
  Size aTempSize;
  Err result;

    // open the apropriate file
  aFile = OpenFile("rb");
  if(aFile == NULL)
    return NULL;

    // check the length of the file
  aTempSize = aFile->GetSize();
  if(aTempSize == 0)
  {
    DELETE(aFile);
    return NULL;
  }

    // try to alloc aBuffer
  aBuffer = new Char[aTempSize];
  if(aBuffer == NULL)
  {
    DELETE(aFile);
    return NULL;
  }

    // read the file
  result = aFile->ReadLongBuffer(aBuffer, aTempSize);
  DELETE(aFile);
  if(result == KO)
  {
    DELETE(aBuffer);
    return NULL;
  }

    // assign aSize
  if(aSize != NULL)
    *aSize = aTempSize;

  return aBuffer;
}


// This is used for unnesting a nested GMessage. There has to be a call
// to this method when you want to acces the nested information.
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
GMessage *FileMsgField::GetAsGMessage()
{
  if(value == NULL)
    return NULL;
  else
    return new GMessage((Char*)value);
}


// Stores the information in a file "aFileName"
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
Err FileMsgField::GetAsFile(Char *aFileName)
{
  Err result;
  File *currFile;
  File *newFile;

  if(aFileName == NULL)
  {
    //WriteString(llDebug, "FileMsgField::GetAsFile() - start, NULL, returning");
    return KO;
  }
  else
    WriteString(llDebug, "FileMsgField::GetAsFile() - start, %s", aFileName);

  if((currFile = OpenFile("rb")) == NULL)
  {
    WriteString(llError, "FileMsgField::GetAsFile(): cannot create source File object");
    return KO;
  }
  if((newFile = new File(aFileName, "wb", logFile)) == NULL)
  {
    WriteString(llError, "FileMsgField::GetAsFile(): cannot create destination File object");
    DELETE(currFile);
    return KO;
  }
  if(newFile->Open() == KO)
  {
    WriteString(llError, "FileMsgField::GetAsFile(): cannot create destination file");
    DELETE(currFile);
    DELETE(newFile);
    return KO;
  }

  currFile->SeekStart();
  result = currFile->AppendTheRestTo(newFile);
  if(result == KO)
    WriteString(llError, "FileMsgField::GetAsFile(): cannot copy MsgField to %s",
    aFileName);
  //else
    //WriteString(llDebug, "FileMsgField::GetAsFile() - end");

  DELETE(currFile);
  DELETE(newFile);

  return result;
}


// Gets (and  stores) the information from a byte stream.
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
Err FileMsgField::SetAsBytes(Void *aBuffer, Size aSize)
{
  File *aFile;
  Size count;

  aFile = OpenFile("wb");
  if(aFile == NULL)
    return KO;

  count = aFile->WriteLongBuffer(aBuffer, aSize);
  DELETE(aFile);

  if(count == aSize)
    return OK;
  else
  {
    delete aBuffer;
    return KO;
  }
}


// Gets (and  stores) the information from a file.
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
Err FileMsgField::SetAsFile(Char *aFileName)
{
  File *toFile;
  File *fromFile;
  Err result;

  if(aFileName == NULL)
    return KO;

    // open the source file 
  fromFile = new File(aFileName, "rb", logFile);
  if(fromFile == NULL)
    return KO;
  if(fromFile->Open() == KO)
  {
    DELETE(fromFile);
    return KO;
  }

    // open the destination file 
  toFile = OpenFile("wb");
  if(toFile == NULL)
  {
    DELETE(fromFile);
    return KO;
  }

    // copy data from the destination file to the source file
  result = fromFile->AppendTheRestTo(toFile);

  DELETE(fromFile);
  DELETE(toFile);

  return result;
}

// returns the contents of a MsgField as an int
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
Err MsgField::GetAsID( ID *t) 
{ 
  Size s;
  Char *c;

  c = (Char *)GetAsBytes(&s);

  if (c!=NULL){
    if (s < sizeof(TID)){
      DELETE(c);
      return KO;
    }
    else{
      *t = *(TID*) c;
      DELETE(c);
      return OK;
    }
  }
  else return KO;
}
