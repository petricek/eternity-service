#include "./H/file.h"
#include "./H/myString.h"

#define MAXINT 65535


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  Created name.
 * @author  Pavel
 * @see
 */
Char *CreateName(Int i, Char *aLeadingStr, Char *anExtension, LogFile *aLogFile)
{
  Char *tempName;
  Int  aLeadStrLen;

  if(aLogFile != NULL)
      aLogFile->WriteString(llDebug, "  CreateName - start, %d", i);

  aLeadStrLen = MyStrLen(aLeadingStr);
  tempName = new Char[10 + aLeadStrLen + MyStrLen(anExtension)];
  if(tempName == NULL)
    return NULL;
  if(aLeadingStr != NULL)
    strcpy(tempName, aLeadingStr);
  else
  {
    tempName[0] = '%';
    aLeadStrLen = 1;
  }

  sprintf(tempName + MyStrLen(aLeadingStr), "%d", i);
  if(anExtension != NULL)
  {
    strcat(tempName, ".");
    strcat(tempName, anExtension);
  }

  if(aLogFile != NULL)
      aLogFile->WriteString(llDebug, "  CreateName - end, %s", tempName);
  return tempName;
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
Char *CreateTempFileName(Char *aLeadingStr, Char *anExtension, LogFile *aLogFile)
{
  Char *tempFileName;
  File *aFile;
  Int i;

//  if(aLeadingStr)
//    printf("\n%s\n", aLeadingStr);

  if(aLogFile != NULL)
    aLogFile->WriteString(llDebug, "CreateTempFileName - start");

    // create first name
  i = 0;
  tempFileName = CreateName(i, aLeadingStr, anExtension, aLogFile);
  if(tempFileName == NULL)
    return NULL;

  if(aLogFile != NULL)
      aLogFile->WriteString(llDebug, "  %s", tempFileName);

    // create a file of that name
  aFile = new File(tempFileName, NULL, aLogFile);
  if(aFile == NULL)
  {
    if(aLogFile != NULL)
      aLogFile->WriteString(llDebug, "  cannot create File instance");
    DELETE(tempFileName);
    return NULL;
  }

    // create new filenames untli not existing is created
  while(aFile->Exists())
  {
    i++;

    if(aLogFile != NULL)
      aLogFile->WriteString(llDebug, "    file with that name already exists");

    DELETE(tempFileName);
    tempFileName = CreateName(i, aLeadingStr, anExtension, aLogFile);
    if(tempFileName == NULL)
      break;

    if(aLogFile != NULL)
	aLogFile->WriteString(llDebug, "  %s", tempFileName);

    DELETE(aFile);
    aFile = new File(tempFileName);
    if(aFile == NULL)
      break;
  }

  if(aFile != NULL)
    DELETE(aFile);

  return tempFileName;
}


/**
 * Constructor for File.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
File::File(const Char *aName, const Char *aMode, LogFile *aLogFile)
:Debugable(aLogFile)
{
//  Debugable::WriteString(llDebug, "File::File(%s)", aName);

    // file
  file = NULL;
  error = 0;

    // fileName
  if(aName)
  {
    fileName = new Char[MyStrLen(aName) + 1];
    strcpy(fileName, aName);
  }

    // fileMode
  if(aMode)
  {
    fileMode = new Char[MyStrLen(aMode) + 1];
    strcpy(fileMode, aMode);
  }
  else
  {
    fileMode = new Char[MyStrLen(FILE_DEFAULT_MODE) + 1];
    strcpy(fileMode, FILE_DEFAULT_MODE);
  }
}


/**
 * Destructor for File.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
File::~File()
{
//  Debugable::WriteString(llDebug, "File::~File(%s)", fileName);

  Close();
  if(fileName)
    DELETE(fileName);
  if(fileMode)
    DELETE(fileMode);
}


/**
 * Checks the error state of "file". Returns and clears it. 
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int File::CheckError()
{
  error = ferror(file);
  if(error)
    clearerr(file);

  return(error);
}


/**
 * Moves the file pointer by calling fseek(). 
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err File::Seek(Long offset, Int whence)
{
  if(file == NULL)
    return KO;
  if((fseek(file, offset, whence) == 0) && (!CheckError()))
    return OK;
  else
    return KO;
}


/**
 * Tries to open a file specified by its filename in specified mode.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err File::Open()
{
  file = fopen(fileName, fileMode);
  if((file != NULL) && (!CheckError()))
    return OK;
  else
    return KO;
}


/**
 * Closes the file.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err File::Close()
{
  if(file)
  {
    Err result = (fclose(file) == 0) && (!CheckError());
    file = NULL;
    if(result)
      return OK;
    else
      return KO;
  }
  else
    return OK;
}


/**
 * Renames the file.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err File::Rename(Char *newFileName)
{
  Long pos;
  Int wasOpen = 0;
  // Err result;

    // can the file be renamed?
  if(fileName == NULL)
    return KO;

    // if open remember current position and close
  if(IsOpen())
  {
    pos = Tell();
    wasOpen = 1;
    Close();
  }

    // copy the old file to the new one
  File *From = new File(fileName,"rb");
  File *To = new File(newFileName,"wb");
  From->Open();
  To->Open();

    // check everything is alright
  if((!From->IsOpen()) || (!To->IsOpen()))
    return KO;

    // copy contents
  From->AppendTheRestTo(To);

    // remove original file
  From->Remove();

  DELETE(From);
  DELETE(To);

    // change the "fileName" attribute
  DELETE(fileName);
  fileName = new Char[MyStrLen(newFileName) + 1];
  strcpy(fileName, newFileName);

    // if was open, open it and seek the previous position
  if(wasOpen)
  {
      // if the file was opened for writing, opening it for writing
      // again would DELETE(the) contents of the file, which is not
      // what we want it to do
    if(*fileMode == 'w')
      *fileMode = 'a';
    Open();
    SeekStart(pos);
  }

  return OK;
}


/**
 * Removes the file.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err File::Remove()
{
    // can the file be removed?
  if(fileName == NULL)
    return KO;

  if(IsOpen())
    Close();

  if(remove(fileName) == -1)
    return KO;
  else
    return OK;
}


/**
 * Return the name of this file.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Char *File::GetName()
{
  if(fileName != NULL)
  {
    Char *fName;

    fName = new Char[MyStrLen(fileName) + 1];
    strcpy(fName, fileName);

    return fName;
  }
  else {
    return NULL;
  }
}


/**
 * Return the size of this file, doesn't move the file pointer.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Size File::GetSize()
{
  Int wasOpen;
  Int wasWriteMode;
  Size currPos, size;

    // if not open, open it. Remember previous position, don't
    // overwrite
  wasOpen = 1;
  if(!IsOpen())
  {
      // safe open mode
    wasWriteMode = 0;
    if(*fileMode == 'w')
    {
      *fileMode = 'r';
      wasWriteMode = 1;
    }

      // open it
    if(Open() == KO)
      return 0;
    wasOpen = 0;
  }
  else
      // remember current position
    currPos = Tell();

    // seek the end of file, check position
  SeekEnd(0);
  size = Tell();

    // restore previous settings
  if(!wasOpen)
  {
    if(wasWriteMode)
      *fileMode = 'w';
    Close();
  }
  else
    SeekStart(currPos);

  return size;
}


/**
 * Checks if the file is open.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int File::IsOpen()
{
  return (file != NULL);
}


/**
 * Checks if the file already exists.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int File::Exists()
{
  if(IsOpen())
    return 1;

  Char origFileMode[4];
  Err result;

  strcpy(origFileMode, fileMode);
  strcpy(fileMode, "rb");

  result = Open();

  if(IsOpen())
    Close();

  strcpy(fileMode, origFileMode);

  return (result == OK);
}


/**
 * Checks for End Of File.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int File::Eof()
{
  if(file == NULL)
    return 0;
  else
    return (feof(file)) && (!CheckError());
}


/**
 * Returns the error code of the last file operation.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int File::GetError()
{
  return error;
}


/**
 * Returns the position of the file pionter.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Long File::Tell()
{
  return ftell(file);
}


/**
 * Moves the file pointer realtively to the beginnig of the file.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err File::SeekStart(Long offset)
{
  return Seek(offset, SEEK_SET);
}


/**
 * Moves the file pointer realtively to the current position.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err File::SeekCurrent(Long offset)
{
  return Seek(offset, SEEK_CUR);
}


/**
 * Moves the file pointer realtively to the end of the file.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err File::SeekEnd(Long offset)
{
  return Seek(offset, SEEK_END);
}


/**
 * Reads "number" of bytes from the "file" into the "buffer".
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int File::ReadBuffer(Void *buffer, size_t number)
{
  if(file == NULL)
    return 0;
  else
  {
    Int bytesRead = fread(buffer, 1, number, file);
    CheckError();
    SeekCurrent(0);
    return bytesRead;
  }
}

/**
 * Read "size" of bytes from the "file" into the "buffer".
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Long File::ReadLongBuffer(Void *buffer, Long size)
{
  Long pos, blockSize, sum;

  if(file == NULL)
    return 0;

  pos = 0;
  sum = 0;

  blockSize = (size < MAXINT) ? size : MAXINT;
  while(blockSize > 0)
  {
    sum += ReadBuffer((char*)buffer + pos, blockSize);
    pos += blockSize;
    blockSize = (size < (pos + MAXINT)) ? size - pos : MAXINT;
    if((pos != sum) || (error != 0))
      break;
  }

  return sum;
}


/**
 * Reads an integer from the "file" into "value".
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int File::ReadInt(Int *value)
{
  if(file == NULL)
    return 0;
  else
    return ReadBuffer(value, sizeof(Int));
}


/**
 * Reads a long from the "file" into "value".
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int File::ReadLong(Long *value)
{
  if(file == NULL)
    return 0;
  else
    return ReadBuffer(value, sizeof(Long));
}


/**
 * Writes "number" of bytes from the "buffer" into the "file".
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int File::WriteBuffer(Void *buffer, size_t number)
{
  if(file == NULL)
    return 0;
  else
  {
    Int bytesWritten = fwrite(buffer, 1, number, file);
    CheckError();
    SeekCurrent(0);
    return bytesWritten;
  }
}


/**
 * Writes "size" of bytes from the "buffer" into the "file".
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Long File::WriteLongBuffer(Void *buffer, Long size)
{
  Long pos, blockSize, sum;

  if(file == NULL)
    return 0;

  pos = 0;
  sum = 0;

  blockSize = (size < MAXINT) ? size : MAXINT;
  while(blockSize > 0)
  {
    sum +=WriteBuffer((char*)buffer + pos, blockSize);
    pos += blockSize;
    blockSize = (size < (pos + MAXINT)) ? size - pos : MAXINT;
    if((pos != sum) || (error != 0))
      break;
  }

  return sum;
}


/**
 * Writes an integer to the "file".
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int File::WriteInt(Int value)
{
  if(file == NULL)
    return(0);
  else
    return(WriteBuffer(&value, sizeof(Int)));
}


/**
 * Writes a long "value" into the "file".
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int File::WriteLong(Long value)
{
  if(file == NULL)
    return 0;
  else
    return WriteBuffer(&value, sizeof(Long));
}


/**
 * Writes a char to the "file".
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int File::WriteChar(Char aChar)
{
  return WriteBuffer(&aChar, sizeof(Char));
}


/**
 * Writes a string to the "file".
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int File::WriteString(Char *aString)
{
  return WriteBuffer(aString, MyStrLen(aString));
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * This method assumes all files (this and "dest") open. If it's true
 * it copies pos bytes from the beginning of this file to "dest".
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err File::CopyStartToPos(File *dest, Long pos, Int blockSize)
{
  Err result;

  result = SeekStart();
  if(result == KO)
    return KO;

  result = CopyTo(dest, pos, blockSize);

  return result;
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * This method assumes all files (this and "dest") open. If it's true
 * it copies all bytes from the current position of this file
 * to the end of "dest".
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err File::AppendTheRestTo(File *dest, Int blockSize)
{
  if(dest == NULL)
    return KO;

  dest->SeekEnd();
  return CopyTheRestTo(dest, blockSize);
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * This method assumes all files (this and "dest") open. If it's true
 * it copies all bytes from the current position of this file
 * to "dest".
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err File::CopyTheRestTo(File *dest, Int blockSize)
{
  Char *buffer;
  Int count;

  if((dest == NULL) || (!dest->IsOpen()) || (!IsOpen()))
    return KO;

  buffer = new Char[blockSize];

  count = ReadBuffer(buffer, blockSize);
  if((error != 0) || (dest->GetError() != 0))
    return KO;
  while(count > 0)
  {
    dest->WriteBuffer(buffer, count);
    count = ReadBuffer(buffer, blockSize);
    if((error != 0) || (dest->GetError() != 0))
      return KO;
  }
  DELETE(buffer);

  return OK;
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * This method assumes all files (this and "dest") open. If it's true
 * it copies "size" bytes from the current position of this file
 * to "dest".
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err File::CopyTo(File *dest, Size pos, Int blockSize)
{
  Char *buffer;
  Int count;
  Size sum, size, diff;

  buffer = new Char[blockSize];
  sum = 0;

  size = (pos < (Size)blockSize) ? pos : blockSize;
  count = ReadBuffer(buffer, size);
  if((error != 0) || (dest->GetError() != 0))
  { 
    Debugable::WriteString(llDebug, "File::CopyTo(): nedari se 1");
    return KO;
  }
  while(count > 0)
  {
    dest->WriteBuffer(buffer, count);
    sum += count;
    diff = pos - sum;
    size = (pos < (sum + (Long)blockSize)) ? diff : blockSize;
    count = ReadBuffer(buffer, size);
    if((error != 0) || (dest->GetError() != 0))
      break;
  }
  DELETE(buffer);
  //Debugable::WriteString(llDebug, "File::CopyTo(): sum = %u, count = %d", sum,
  //count);

  return (sum == pos) ? OK : KO;
}
