#include "./H/file.h"
#include "./H/gMessage.h"
#include "./H/myString.h"


/**
 * Constructor for GMessage.
 *
 * This file contains the GMessage class which is used
 * for parsing an incoming message or constructing an outcoming one.
 * It is also used in parameters of functions that serve messages coming
 * to ESo from MIX. Allows nested messages through SetAsMessage,
 * GetAsMessage.
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
GMessage::GMessage(LogFile *aLogFile): Debugable(aLogFile)
{
  fields = NULL;
  Initialize();
}


/**
 * Constructor for GMessage.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
GMessage::GMessage(const GMessage *aGMessage, LogFile *aLogFile)
: Debugable(aLogFile)
{
  if(aGMessage == NULL)
  {
    fields = NULL;
    Initialize();
    return;
  }
 
  if(aLogFile == NULL)
    SetLogFile(aGMessage->logFile);
  fields = new FieldQueue(*aGMessage->fields);
}


/**
 * Constructor - loading structure from a file.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
GMessage::GMessage(Char *aFileName, LogFile *aLogFile)
:Debugable(aLogFile)
{
  fields = NULL;
  Initialize();
  LoadFromFile(aFileName);
}


/**
 * Constructor - loading structure from a byte stream.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
GMessage::GMessage(Void *aBuffer, Size aSize, LogFile *aLogFile)
:Debugable(aLogFile)
{
  fields = NULL;
  Initialize();
  LoadFromBytes(aBuffer, aSize);
}


/**
 * Denstructor for GMessage.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
GMessage::~GMessage()
{
  if(fields)
    DELETE(fields);
}


/**
 * Initialization of attributes called from constructors.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Void GMessage::Initialize()
{
  if(fields != NULL)
    DELETE(fields);

  fields = new FieldQueue(logFile);
}


/**
 * Returns the size of the eso message in bytes.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Size GMessage::SizeOf() const
{
  MsgField *aMsgField = NULL;
  Char *aFieldName;
  Size aSize;

  aSize = 0;
  aMsgField = (MsgField*)fields->First();
  while (aMsgField != NULL)
  {
      // add the size of the header of the field
    aFieldName = aMsgField->GetName();
    if(aFieldName == NULL)
      return 0;
    aSize += MyStrLen(aFieldName);  // length of the name of the field
    aSize += 1;                   // length of the delimitor
    aSize += sizeof(Size);        // length of the length of the field's contents
    DELETE(aFieldName);

      // add the length of the contents of the field
    aSize += aMsgField->SizeOf();

      // next
// nemelo by se mazat - First a Next nevytvari nove    DELETE(aMsgField);
    aMsgField = (MsgField*)fields->Next();
  }

  return aSize;
}


/**
 * Debugging method.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::DebugHeaders(LogFile *aLogFile, Char *aLogLabel, 
                           LogLevel aLogLevel)
{
  MsgField *aMsgField = NULL;
  Char     *aName = NULL;
  Int      i;

  if(aLogFile == NULL)
    return KO;

  Size length = 0;
  char * whole = (char *)this->StoreToBytes(&length);
  Size checkWhole = CheckSum(whole, length);
  DELETE(whole);

    // write the initial string
  aLogFile->WriteString(aLogLevel, "%s: [check: %ld]", aLogLabel, checkWhole);
  i = 1;
  aMsgField = (MsgField*)fields->First();
  while(aMsgField != NULL)
  {
       // get the name and write it to the log
     aName = aMsgField->GetName();
     if(aName != NULL)
     {
       Size len;
       char * spy = (char*) aMsgField->GetAsBytes(&len);

       if(len == 0) {
         aLogFile->WriteString(aLogLevel, "  %d) %s :[%lu] >>EMPTY FIELD<<",
                               i, aName, len);
       }
       else {
         if( (len == sizeof(unsigned short)) && (spy[len-1]!='\0') ) {
	   aLogFile->WriteString(aLogLevel, "  %d) %s :[%lu] %u", i, aName, 
				 len, *((unsigned short *)spy));
	 }
	 else { 
	   if( (len == sizeof(Size))) {
	     if (spy[len - 1] != '\0') {
	       aLogFile->WriteString(aLogLevel, "  %d) %s :[%lu] %lu.", i, aName, 
				     len, *(Size *)spy);
	     } 
	     else {
	       aLogFile->WriteString(aLogLevel, "  %d) %s :[%lu] %lu >>OR<< %s.", i, aName, 
				   len, *(Size *)spy, spy);
	     }
	   }
	   else {
	     char *ptrView = NULL;
	     if(len > 0)
	     {
	       Size tmpLen = len;
	       ptrView = new char[tmpLen+1];
	       memcpy(ptrView, spy, tmpLen);

	       // Substitute zeros with dots
	       for (unsigned int j = 0; j < tmpLen; ++j) {
		 if (ptrView[j] == '\0')
		   ptrView[j] = '.';
		 if (ptrView[j] == '\n')
		   ptrView[j] = '#';
		 if (!isprint(ptrView[j]))
		   ptrView[j] = '_';
	       }

	       ptrView[tmpLen] = '\0';
	       Size checkSum = CheckSum(spy, len);
	       Size sum = Sum(spy, len);

	       aLogFile->WriteString(aLogLevel, "  %d) %s :[%lu] [check: %lu] "
				     "[sum: %lu] %s", i, aName, len, checkSum, 
				     sum, ptrView);
	       DELETE(ptrView);
	     }
	   }
	 }
       }
       DELETE(aName);
       DELETE(spy);
     }
     else
       aLogFile->WriteString(aLogLevel, "  %d) (no name)", i);

       // next field
     i++;
     aMsgField = (MsgField*)fields->Next();
  }

  return OK;
}


/**
 * Reads the field name + delimitor + size from the byte stream.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Char *GMessage::ReadBytesHeader(Void *aBuffer, Size aLength, Size *aSize, Size *anOffset)
{
#define EM_RBH_BUFFER (((Char*)aBuffer) + *anOffset)

  Char *aFieldName;
  Size pos;

    // find the delimitor
  pos = MyStrChar(EM_RBH_BUFFER, EM_DELIMITOR, aLength - *anOffset);
  if(pos == 0)  // delimitor not found or fieldname empty
    return NULL;

  aFieldName = new Char[pos+1];
  MyStrCpy(aFieldName, EM_RBH_BUFFER, pos);
  aFieldName[pos] = '\0';

    // update anOffset to point behind the delimitor
  *anOffset += pos + 1;
  if((*anOffset) + sizeof(Size) > aLength)
    return NULL;

    // read the size of the contents of the current field
  *aSize = *(Size*)EM_RBH_BUFFER;

    // update anOffset to point behind the size value
  *anOffset += sizeof(Size);
    // if it tells that contents of the field is longer than
    // a given buffer, message is corrupted
  if((*anOffset) + (*aSize) > aLength) 
    return NULL;

  return aFieldName;

#undef EM_RBH_BUFFER
}


/**
 * Reads the field name + delimitor + size from the file.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Char *GMessage::ReadFileHeader(File *aFile, Size *aSize)
{
#define EM_RH_ALLOC 100
#define EM_RH_CURRENT_POINTER (aFieldName + count)
#define EM_RH_LAST_CHAR (*(EM_RH_CURRENT_POINTER))

  Char aFieldName[EM_RH_ALLOC];
  Char *fieldName;
  Int count;
  Int countRead;

  count = 0;

    // read the first char
  countRead = aFile->ReadBuffer(EM_RH_CURRENT_POINTER, 1);
  if(countRead != 1)
    return NULL;

  while(EM_RH_LAST_CHAR != EM_DELIMITOR)
  {
      // move the pointer
    count++;
    if(count >= EM_RH_ALLOC)
      return NULL;

      // read the next char
    countRead = aFile->ReadBuffer(EM_RH_CURRENT_POINTER, 1);
    if(countRead != 1)
      return NULL;
  }
    // trailing zero
  EM_RH_LAST_CHAR = '\0';

    // read the size of the contents of the current field
  countRead = aFile->ReadBuffer(aSize, sizeof(Size));
  if(countRead != sizeof(Size))
    return NULL;

    // if everything went OK, alloc newly read fieldName
  fieldName = new Char[MyStrLen(aFieldName) + 1];
  if(fieldName == NULL)
    return NULL;
  strcpy(fieldName, aFieldName);

  return fieldName;

#undef EM_RH_ALLOC
#undef EM_RH_CURRENT_POINTER
#undef EM_RH_LAST_CHAR
}


/**
 * Writes the field name + delimitor + size into the byte stream.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::WriteBytesHeader(Void *aBuffer, MsgField *aMsgField, Size *anOffset) const
{
#define EM_WBH_BUFFER (((Char*)aBuffer) + *anOffset)

  Char *aFieldName;
  Size count;
  Size aSize;

    // get the name of the field
  aFieldName = aMsgField->GetName();
  if(aFieldName == NULL)
    return KO;

    // write the name
  count = MyStrLen(aFieldName);
  MyStrCpy(EM_WBH_BUFFER, aFieldName, count);
  DELETE(aFieldName);

  *anOffset += count;

    // write the delimitor
  *EM_WBH_BUFFER = EM_DELIMITOR;

  (*anOffset)++;

    // write the size of the contents of the field
  aSize = aMsgField->SizeOf();
  count = sizeof(aSize);
  MyStrCpy(EM_WBH_BUFFER, (Char*)&aSize, count);

  *anOffset += count;

  return OK;

#undef EM_WBH_BUFFER
}


/**
 * Writes the field name + delimitor + size into the file.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::WriteFileHeader(File *aFile, MsgField *aMsgField) const
{
  Char *aFieldName;
  Int count, len;
  Size aSize;

    // get the name of the field
  aFieldName = aMsgField->GetName();
  if(aFieldName == NULL)
    return KO;

    // write the name to the file
  count = aFile->WriteString(aFieldName);
  len   = MyStrLen(aFieldName);
  DELETE(aFieldName);
  if(count != len)
    return KO;

    // write the delimitor to the file
  count = aFile->WriteChar(EM_DELIMITOR);
  if(count != sizeof(Char))
    return KO;

    // write the size of the contents of the field
  aSize = aMsgField->SizeOf();
  count = aFile->WriteBuffer(&aSize, sizeof(aSize));
  if(count != sizeof(aSize))
    return KO;

  return OK;
}


/**
 * Loads the fields from a byte stream.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::LoadFromBytes(Void *aBuffer, Size aLength)
{
  MsgField *aMsgField;
  Char *aFieldName;
  Size aSize, anOffset;

    // clear all possible rubish
  Clear();

    // while it is possible, read from aBuffer and create apropriate fields

    // read the field name together with the size of the
    // contents of the field
  anOffset = 0;
  aFieldName = ReadBytesHeader(aBuffer, aLength, &aSize, &anOffset);
  while(aFieldName != NULL)
  {
      // according to the size read, create Bytes/ or FileMsgField
    if(aSize > MAX_BYTE_MSG_FIELD_SIZE)
      aMsgField = new FileMsgField(aFieldName, NULL, logFile);
    else
      aMsgField = new BytesMsgField(aFieldName, NULL, 0, logFile);
    DELETE(aFieldName);
    if(aMsgField == NULL)
    {
      Clear();
      return KO;
    }

      // read the contents of the field
    if(aMsgField->SetAsBytes(((Char*)aBuffer) + anOffset, aSize) != OK)
    {
      DELETE(aMsgField);
      Clear();
      return KO;
    }

      // update anOffset to point to the next field
    anOffset += aSize;
    if(anOffset > aLength)
    {
      DELETE(aMsgField);
      Clear();
      return KO;
    }

      // insert the newly create MsgField into the "fields" FeildQueue
    if(fields->Insert(aMsgField) != OK)
    {
      DELETE(aMsgField);
      Clear();
      return KO;
    }

      // read the next field name together with the size of the
      // contents of th field
    aFieldName = ReadBytesHeader(aBuffer, aLength, &aSize, &anOffset);
  }

  return OK;
}


/**
 * Loads the fields from file.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::LoadFromFile(Char *aFileName)
{
  MsgField *aMsgField;
  File *aFile;
  Char *aFieldName;
  Size aSize;

    // otevrit soubor
  aFile = new File(aFileName, "rb", logFile);
  if(aFile == NULL)
    return KO;

  if(aFile->Open() == KO)
    return KO;

  Clear();

    // while it is possible, read the file and create apropriate fields

    // read the field name together with the size of the
    // contents of the field
  aFieldName = ReadFileHeader(aFile, &aSize);
  while(aFieldName != NULL)
  {
      // according to the size read, create Bytes/ or FileMsgField
    if(aSize > MAX_BYTE_MSG_FIELD_SIZE)
      aMsgField = new FileMsgField(aFieldName, NULL, logFile);
    else
      aMsgField = new BytesMsgField(aFieldName, NULL, 0, logFile);
    DELETE(aFieldName);
    if(aMsgField == NULL)
    {
      DELETE(aFile);
      Clear();
      return KO;
    }

      // read the contents of the field
    if(aMsgField->ReadFromFile(aFile, aSize) != OK)
    {
      DELETE(aMsgField);
      DELETE(aFile);
      Clear();
      return KO;
    }

      // insert the newly create MsgField into the "fields" FeildQueue
    if(fields->Insert(aMsgField) != OK)
    {
      DELETE(aMsgField);
      DELETE(aFile);
      Clear();
      return KO;
    }

      // read the next field name together with the size of the
      // contents of th field
    aFieldName = ReadFileHeader(aFile, &aSize);
  }

    // close the file
  DELETE(aFile);

  return OK;
}


/**
 * Stores information held by MsgFields into a byte stream.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Void *GMessage::StoreToBytes(Size *aSize) const
{
  MsgField *aField;
  Char *aBuffer, *temp;
  Size anOffset;
  Size bytesSize;

    // check the size of the memory to allocate
  aBuffer = new Char[SizeOf()];

    // write all MsgFields into it
  anOffset = 0;
  aField = (MsgField*)fields->First();
  while(aField != NULL)
  {
      // write the field name + delimitor + size
    if(WriteBytesHeader(aBuffer, aField, &anOffset) != OK)
    {
      DELETE(aBuffer);
      return NULL;
    }

      // write the contents
    temp = (Char*)aField->GetAsBytes(&bytesSize);
    if(temp != NULL)
    {
      MyStrCpy(aBuffer + anOffset, temp, bytesSize);
      DELETE(temp);
    }
     
      // update anOffset
    anOffset += bytesSize;

      // next
    aField = (MsgField*)fields->Next();
  }

  if(aSize != NULL)
    *aSize = anOffset;

  return aBuffer;
}


/**
 * Stores information held by MsgFields into a file.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::StoreToFile(Char *aFileName) const
{
  MsgField *aField;
  File *aFile;

    // initial check
  if(aFileName == NULL)
    return KO;

    // open the file
  aFile = new File(aFileName, "wb", logFile);
  if(aFile == NULL)
    return KO;

  if(aFile->Open() != OK)
  {
    DELETE(aFile);
    return KO;
  }

    // write all MsgFields into it
  aField = (MsgField*)fields->First();
  while(aField != NULL)
  {
      // write the field name + delimitor + size
    if(WriteFileHeader(aFile, aField) != OK)
    {
      DELETE(aFile);
      return KO;
    }

      // write the contents
    if(aField->WriteToFile(aFile) != OK)
    {
      DELETE(aFile);
      return KO;
    }

      // next
    aField = (MsgField*)fields->Next();
  }

    // close the file
  DELETE(aFile);

  return OK;
}


/**
 * Stores information held by MsgFields into a newly created MsgField.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
MsgField *GMessage::StoreToMsgField(Char *aFieldName) const
{
  Size aSize;
  MsgField *aMsgField;
  Void *buffer;
  Char *tempName;
  File *tempFile;
  Err result;

    // according to size of this GMessage create appropriate MsgField
  aSize = SizeOf();
  if(aSize > MAX_BYTE_MSG_FIELD_SIZE)
    aMsgField = new FileMsgField(aFieldName, NULL, logFile);
  else
    aMsgField = new BytesMsgField(aFieldName, NULL, 0, logFile);
  if(aMsgField == NULL)
    return NULL;

    // store this GMessage to a temporary place, according to its size
  if(aSize > MAX_BYTE_MSG_FIELD_SIZE)
  {
      // crete a name for a temporary file
    tempName = CreateTempFileName();
    if(tempName == NULL)
    {
      DELETE(aMsgField);
      return NULL;
    }

      // store this GMsg to the temporary file
    if(StoreToFile(tempName) != OK)
    {
      DELETE(tempName);
      DELETE(aMsgField);
      return NULL;
    }

      // load aMsgField from the temporary file
    result = aMsgField->SetAsFile(tempName);
    DELETE(tempName);

      // destroy the temporary file
    tempFile = new File(tempName, NULL, logFile);
    if(tempFile != NULL)
    {
      tempFile->Remove();
      DELETE(tempFile);
    }
  }
  else
  {
      // store this GMsg to a temporary byte stream
    buffer = StoreToBytes(&aSize);
    if(buffer == NULL)
    {
      DELETE(aMsgField);
      return NULL;
    }

      // load aMsgField from a temporary byte stream
    result = aMsgField->SetAsBytes(buffer, aSize);
    DELETE(buffer);
  }

  if(result != OK)
  {
    DELETE(aMsgField);
    return NULL;
  }
  else
    return aMsgField;
}


/**
 * Returns the contents of a MsgField as a byte stream.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Void *GMessage::GetAsBytes(Char *aFieldName, Size *aSize) const
{
  MsgField *aMsgField;

    // find the field
  aMsgField = (MsgField*)fields->GetField(aFieldName);

    // if not found, fail
  if(aMsgField == NULL) {
    // fprintf(stderr, "GetAsBytes: wanted NULL %s\n", aFieldName);
    return NULL;
  }

    // if found, get the contents of the field as a byte stream
  return aMsgField->GetAsBytes(aSize);
}


/**
 * Returns the contents of a MsgField as a NULL terminated string.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Char *GMessage::GetAsString(Char *aFieldName) const
{
  MsgField *aMsgField;

    // find the field
  aMsgField = (MsgField*)fields->GetField(aFieldName);

    // if not found, fail
  if(aMsgField == NULL) {
    // fprintf(stderr, "GetAsString: wanted NULL %s\n", aFieldName);
    return NULL;
  }

    // if found, get the contents of the field as a byte stream
  return aMsgField->GetAsString();
}


/**
 * Returns the contents of a MsgField as an GMessage.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
GMessage *GMessage::GetAsGMessage(Char *aFieldName) const
{
  MsgField *aMsgField;

    // find the field
  aMsgField = (MsgField*)fields->GetField(aFieldName);

    // if not found, fail
  if(aMsgField == NULL) {
    // fprintf(stderr, "GetAsGMessage: wanted NULL %s\n", aFieldName);
    return NULL;
  }

    // if found, get the contents of the field as a byte stream
  return aMsgField->GetAsGMessage();
}


/**
 * Returns the contents of a MsgField as a MsgField.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
MsgField *GMessage::GetAsMsgField(Char *aFieldName) const
{
  MsgField *aMsgField;

    // find the field
  aMsgField = (MsgField*)fields->GetField(aFieldName);

  if(aMsgField == NULL) {
    // fprintf(stderr, "GetAsMsgField: wanted NULL %s\n", aFieldName);
    return NULL;
  }
  aMsgField->SetLogFile(logFile);

    // create COPY of found MsgField
  return (MsgField*)aMsgField->CreateCopy();
}


/**
 * Returns the contents of a MsgField as a file.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::GetAsFile(Char *aFieldName, Char *aFileName) const
{
  MsgField *aMsgField;

    // find the field
  aMsgField = (MsgField*)fields->GetField(aFieldName);

    // if not found, fail
  if(aMsgField == NULL) {
    // fprintf(stderr, "GetAsFile: wanted NULL %s\n", aFieldName);
    return KO;
  }
  if(aMsgField == NULL)
    return KO;

    // if found, get the contents of the field as a file
  return aMsgField->GetAsFile(aFileName);
}


/**
 * Returns the contents of a MsgField as an Size.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::GetAsSize(Char *aFieldName, Size *i) const
{
  Size s;
  Char *c = NULL;
  c = (Char *)GetAsBytes(aFieldName, &s);
  
  if (c != NULL){
    if (s < sizeof(Size)){
      DELETE(c);
      return KO;
    }
    else{
      *i = *(Size*) c;
      DELETE(c);
      return OK;
    }
  }
  else 
    return KO;
}

/**
 * Returns the contents of a MsgField as an int.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::GetAsInt(Char *aFieldName, Int *i) const
{
  Size s;
  Char *c;
  c = (Char *)GetAsBytes(aFieldName, &s);
  
  if (c!=NULL){
    if (s < sizeof(Int)){
      DELETE(c);
      return KO;
    }
    else{
      *i = *(Int*) c;
      DELETE(c);
      return OK;
    }
  }
  else return KO;
}


/**
 * Returns the contents of a MsgField as an int.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::GetAsID(Char *aFieldName, ID *t) const
{  
  Size s;
  Char *c;
  c = (Char *)GetAsBytes(aFieldName, &s);
  
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


/**
 * Returns the contents of a MsgField as an BasicTime.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::GetAsBasicTime(Char *aFieldName, BasicTime *t) const
{
  Size s;
  Char *c;
  c = (Char *)GetAsBytes(aFieldName, &s);

  if (c != NULL){
    if (s< sizeof(TIME_REPREZ)){
      DELETE(c);
      return KO;
    }
    else{
      t->SetTime(*((TIME_REPREZ *)c));
      DELETE(c);
      return OK;
    }
  }
  else return KO;    
}

/**
 * Set the contents of a MsgField by copying it from a byte stream.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::SetAsBytes(Char *aFieldName, Void *aBuffer, Size aSize)
{
  if((aBuffer == NULL) || (aSize == 0))
    return KO;

  MsgField *aMsgField;

    // find the field
  aMsgField = (MsgField*)fields->GetField(aFieldName);

  if(aMsgField == NULL)
  {
			// if not found, create a new field
    if(aSize > MAX_BYTE_MSG_FIELD_SIZE)
      aMsgField = new FileMsgField(aFieldName, NULL, logFile);
    else
      aMsgField = new BytesMsgField(aFieldName, NULL, 0, logFile);
    if(aMsgField == NULL)
      return KO;

      // if not found, insert new BytesField
    fields->Insert(aMsgField);
  }
    // assign the information to store
  aMsgField->SetAsBytes(aBuffer, aSize);

  return OK;
}


/**
 * Set the contents of a MsgField by copying it from a NULL terminated str..
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::SetAsString(Char *aFieldName, Char *aString)
{
  MsgField *aMsgField;

    // find the field
  aMsgField = (MsgField*)fields->GetField(aFieldName);

  if(aMsgField == NULL)
  {
      // if not found, create a new field
    if(MyStrLen(aString) > MAX_BYTE_MSG_FIELD_SIZE)
      aMsgField = new FileMsgField(aFieldName, NULL, logFile);
    else
      aMsgField = new BytesMsgField(aFieldName, NULL, 0, logFile);
    if(aMsgField == NULL)
      return KO;

      // if not found, insert new BytesField
    fields->Insert(aMsgField);
  }
    // assign the information to store
  return aMsgField->SetAsString(aString);
}


/**
 * Set the contents of a MsgField by copying it from an GMessage.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::SetAsGMessage(Char *aFieldName, GMessage *aGMessage)
{
  MsgField *aMsgField;
  Char *aBuffer;
  Char *tempName;
  File *tempFile;
  Size aSize;
  Err result;

  if(aGMessage == NULL)
    return KO;

    // find the field to be set
  aMsgField = (MsgField*)fields->GetField(aFieldName);

    // if GMessage to be nested is too large, it must be
    // copied using the disk
  aSize = aGMessage->SizeOf();
  if(aSize > MAX_BYTE_MSG_FIELD_SIZE)
  {
      // create a temporary file
    tempName = CreateTempFileName();
    if(tempName == NULL)
      return KO;
    tempFile = new File(tempName, "wb", logFile);
    if(tempFile == NULL)
    {
      DELETE(tempName);
      return KO;
    }

      // store GMessage into the temporary file
    result = aGMessage->StoreToFile(tempName);
    if(result == OK)
    {
        // is it needed to create new MsgField?
      if(aMsgField != NULL)
          // NO - change current
        result = aMsgField->SetAsFile(tempName);
      else
      {
          // YES - create new FileMsgField and insert it into current GMessage
        aMsgField = new FileMsgField(aFieldName, NULL, logFile);
        if(aMsgField == NULL)
          result = KO;
        else
        {
          result = aMsgField->SetAsFile(tempName);
          fields->Insert(aMsgField);
        }
      }
    }

      // remove temporary file
    DELETE(tempName);
    tempFile->Remove();
    DELETE(tempFile);

    if(result == KO)
      return KO;
  }

    // GMessage to be nested is small enough to keep it in memory
  else
  {
    aBuffer = (Char*)aGMessage->StoreToBytes();
    if(aBuffer == NULL)
      return KO;

      // is it needed to create new MsgField?
    if(aMsgField != NULL)
    {
        // NO - change current
      result = aMsgField->SetAsBytes(aBuffer, aSize);
      DELETE(aBuffer);
      if(result == KO)
        return KO;
    }
    else
    {
        // YES - create new BytesMsgField and insert it into current GMessage
      aMsgField = new BytesMsgField(aFieldName, aBuffer, aSize, logFile);
      DELETE(aBuffer);
      if(aMsgField == NULL)
        return KO;
      fields->Insert(aMsgField);
    }
  }

  return OK;
}


/**
 * Set the contents of a MsgField by copying it from another MsgField.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::SetAsMsgField(Char *aFieldName, MsgField *aField)
{
  if(aField == NULL)
    return KO;

  MsgField *aMsgField;

    // find the field
  aMsgField = (MsgField*)fields->GetField(aFieldName);

    // if found
  if(aMsgField != NULL)
  {
      // remove it from queue and dispose it off
    fields->Delete(aMsgField);
    DELETE(aMsgField);
  }

    // insert a MsgField with a new name
  aField->SetName(aFieldName);
  fields->Insert((MsgField*)aField->CreateCopy());

  return OK;
}


/**
 * Set the contents of a MsgField by copying it from a file.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::SetAsFile(Char *aFieldName, Char *aFileName)
{
  MsgField *aMsgField;
  Size aSize;

  if(aFileName == NULL)
    return KO;

    // find the field
  aMsgField = (MsgField*)fields->GetField(aFieldName);

  if(aMsgField == NULL)
  {
      // check the file size
    File *f = new File(aFileName, NULL, logFile);
    if(f == NULL)
      return KO;
    aSize = f->GetSize();
    DELETE(f);
      // if not found, create a new field
    if(aSize > MAX_BYTE_MSG_FIELD_SIZE)
      aMsgField = new FileMsgField(aFieldName, NULL, logFile);
    else
      aMsgField = new BytesMsgField(aFieldName, NULL, 0, logFile);
    if(aMsgField == NULL)
      return KO;
      // if not found, insert new BytesField
    fields->Insert(aMsgField);
  }
    // assign the information to store
  aMsgField->SetAsFile(aFileName);

  return OK;
}


/**
 * Sets the contents of a MsgField by copying it from int variable.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::SetAsInt(Char *aFieldName, Int aVariable)
{
  return SetAsBytes(aFieldName, &aVariable, sizeof(Int));
}
// sets the contents of a MsgField by copying it from int variable
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
Err GMessage::SetAsSize(Char *aFieldName, Size aVariable)
{
  return SetAsBytes(aFieldName, &aVariable, sizeof(Size));
}


/**
 * Sets the contents of a MsgField by copying it from int variable.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::SetAsID(Char *aFieldName, ID aVariable)
{
  return SetAsBytes(aFieldName, &aVariable, sizeof(TID));
}


/**
 * Sets the contents of a MsgField by copying it from bastime variable.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::SetAsBasicTime(Char *aFieldName, BasicTime aVariable)
{
  TIME_REPREZ tr;

  tr = aVariable.GetTime();
 
  return SetAsBytes(aFieldName, &tr, sizeof(TIME_REPREZ));
}

/**
 * Clears all MsgFields.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Void GMessage::Clear()
{
  fields->DiscardAll();
}


/**
 * Deletes a MsgField.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::Delete(Char *aFieldName)
{
  MsgField *aMsgField;

    // find the field
  aMsgField = (MsgField*)fields->GetField(aFieldName);
  if(aMsgField == NULL)
    return KO;                   // not found

    // remove it from field queue and dispose it off
  fields->Delete(aMsgField);
  DELETE(aMsgField);

  return OK;
}


/**
 * Changes the name of a MsgField.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err GMessage::Rename(Char *aFieldName, Char *newFieldName)
{
  MsgField *aMsgField;

  if(newFieldName == NULL)
    return KO;

    // find the field
  aMsgField = (MsgField*)fields->GetField(aFieldName);
  if(aMsgField == NULL)
    return KO;

    // rename
  aMsgField->SetName(newFieldName);

  return OK;
}


// by Pechy, 31.1.1999

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
Size GMessage::CheckSum(char *input, Size len)
{

  Size x = 1;

  for (Size i = 0; i < len; ++i) {
    x = (x * x + (unsigned char) input[i]) % 599149;
  }

  return x;
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
Size GMessage::Sum(char *input, Size len)
{

  Size x = 0;

  for (Size i = 0; i < len; ++i) {
    x += (unsigned char) input[i];
  }

  return x;
}

