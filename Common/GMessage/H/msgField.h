#ifndef MSGFIELD_H
#define MSGFIELD_H

#include "./file.h"
#include "./fields.h"

#include "./../../LeakDetector/H/leakDetector.h"

#define MAX_BYTE_MSG_FIELD_SIZE 100000

  // forward declaration
class GMessage;

/**
 * Message field.
 *
 * An abstract base class for storing field information in GMessage.
 * Descendants must be able to set its value getting it either as a byte
 * stream or as a file (file name, respectively).
 * <p>
 * Description: This file contains the MsgField class and its descendants
 * BytesMsgField and FileMsgField. The descendants are used to store
 * information kept in fields of GMessage.
 * BytesMsgField stores the information as a byte stream in memory,
 * FileMsgField stores only a name of a (temporary) file that keeps
 * the information.
 * <p>
 * NOTE: All constructors and Set../Get.. methods COPY given data. 
 *
 *
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
class MsgField: public Field//, public LeakDetector
{
  protected:
  /**@name attributes */
  /*@{*/
    /// Stored value
    Void *value;
    /// Stored value size
    Size size;
  /*@}*/

      /// value and size manipulation
  /**@name methods */
  /*@{*/
    Void *CreateCopyOfValue();
    Err SetValue(Void *aBuffer, Size aSize);
  /*@}*/

  public:
  /**@name methods */
  /*@{*/
    /// constructor
    MsgField(Char *aFieldName = NULL, LogFile *aLogFile = NULL);
    /// destructor
    virtual ~MsgField();

    static MsgField *NewMsgField(Size aSize, Char *aFieldName = NULL, 
                                             LogFile *aLogFile = NULL);
    /// Comparison
    virtual Int EqualTo(MsgField *);

    /// Store value
    /*@{*/
    virtual Err WriteToFile(File *aFile) = 0;
    virtual Err ReadFromFile(File *aFile, Size aSize) = 0;
    /*@}*/

    /// Get and set value
    /*@{*/
    virtual Void *GetAsBytes(Size *aSize) = 0;
    virtual Char *GetAsString();
    virtual Err GetAsID( ID *t);

    virtual GMessage *GetAsGMessage() = 0;
    virtual Err GetAsFile(Char *aFileName) = 0;
    virtual Err SetAsBytes(Void *aBuffer, Size aSize) = 0;
    virtual Err SetAsString(Char *aString);
    virtual Err SetAsFile(Char *aFileName) = 0;
    /*@}*/
  /*@}*/
};


/**
 * Bytes message field.
 *
 * Stores the field information as a byte stream.
 *
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
class BytesMsgField: public MsgField
{
  public:
  /**@name methods */
  /*@{*/
    BytesMsgField(Char *aFieldName = NULL, Void *aBuffer = NULL, 
                                           Size aSize = 0, 
                                           LogFile *aLogFile = NULL);

    /// "Copy constructor"
    virtual Field *CreateCopy();
    /// Total size
    virtual Size SizeOf();

    /// Store value
    /*@{*/
    virtual Err WriteToFile(File *aFile);
    virtual Err ReadFromFile(File *aFile, Size aSize);
    /*@}*/

    /// Get and set value
    /*@{*/
    virtual Void *GetAsBytes(Size *aSize = NULL);
    virtual GMessage *GetAsGMessage();
    virtual Err GetAsFile(Char *aFileName);
    virtual Err SetAsBytes(Void *aBuffer, Size aSize);
    virtual Err SetAsFile(Char *aFileName);
    /*@}*/
  /*@}*/
};


/**
 * File message field.
 *
 * Stores the field information a file. It is only the file name what is
 * kept in operational memory.
 *
 *
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
class FileMsgField: public MsgField
{
  protected:
  /**@name methods */
  /*@{*/
    File *OpenFile(Char *aFileMode);
  /*@}*/

  public:
  /**@name methods */
  /*@{*/
    /// constructor
    FileMsgField(Char *aFieldName = NULL, Char *aFileName = NULL, 
                                          LogFile *aLogFile = NULL);
    /// destructor
    virtual ~FileMsgField();

    ///"copy constructor"
    virtual Field *CreateCopy();
    /// Total size
    virtual Size SizeOf();

    /// Store value
    /*@{*/
    virtual Err WriteToFile(File *aFile);
    virtual Err ReadFromFile(File *aFile, Size aSize);
    /*@}*/

    /// Get and set value
    /*@{*/
    virtual Void *GetAsBytes(Size *aSize = NULL);
    virtual GMessage *GetAsGMessage();
    virtual Err GetAsFile(Char *aFileName);
    virtual Err SetAsBytes(Void *aBuffer, Size aSize);
    virtual Err SetAsFile(Char *aFileName);
    /*@}*/
  /*@}*/
};

#endif

