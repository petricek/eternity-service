#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./../../H/common.h"
#include "./../../Debugable/H/debugable.h"

#define FILE_DEFAULT_MODE "r+"

/// Temporary filename construction
Char *CreateTempFileName(Char *aLeadingStr = NULL, 
                         Char *anExtension = NULL,
			 LogFile *aLogFile = NULL);

/**
 * Encapsulation of file I/O operations.
 *
 * Description: This file contains the File class which encapsulates disk
 * I/O operations. Changing it could enable compiling the ES servers for
 * other file systems.
 *
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
class File: public Debugable
{
  protected:
  /**@name attributes */
  /*@{*/
    FILE *file;
    Char *fileName;
    Char *fileMode;
    Int error;
  /*@}*/

  /**@name methods */
  /*@{*/
    Int CheckError();
    Err Seek(Long offset, Int whence);
  /*@}*/

  public:
  /**@name methods */
  /*@{*/
    /// constructor
    File(const Char *aName, const Char *aMode = FILE_DEFAULT_MODE, 
                            LogFile *aLogFile = NULL);
    /// destructor
    ~File();

      /// file manipulation
    /*@{*/
    Err Open();
    Err Close();
    Err Rename(Char *newFileName);
    Err Remove();
    /*@}*/

      /// file checking
    /*@{*/
    Char *GetName();
    Size GetSize();
    Int IsOpen();
    Int Exists();
    Int Eof();
    Int GetError();
    /*@}*/

      /// file pointer manipulation
    /*@{*/
    Long Tell();
    Err SeekStart(Long offset = 0);
    Err SeekCurrent(Long offset = 0);
    Err SeekEnd(Long offset = 0);
    /*@}*/

      /// reading and writing
    /*@{*/
    Int ReadBuffer(Void *buffer, size_t number);
    Long ReadLongBuffer(Void *buffer, Long size);
    Int ReadInt(Int *value);
    Int ReadLong(Long *value);
    Int WriteBuffer(Void *buffer, size_t number);
    Long WriteLongBuffer(Void *buffer, Long size);
    Int WriteInt(Int value);
    Int WriteLong(Long value);
    Int WriteChar(Char aChar);
    Int WriteString(Char *aString);
    /*@}*/

      /// helpful file operations
    /*@{*/
    Err CopyStartToPos(File *dest, Long pos, Int blockSize = 1024);
    Err AppendTheRestTo(File *dest, Int blockSize = 1024);
    Err CopyTheRestTo(File *dest, Int blockSize = 1024);
    Err CopyTo(File *dest, Size size, Int blockSize = 1024);
    /*@}*/
  /*@}*/
};

#endif
