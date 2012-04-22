#ifndef ESOMESSAGE_H
#define ESOMESSAGE_H


#include "./file.h"
#include "./../../H/common.h"
#include "./fields.h"
#include "./msgField.h"
#include "./fieldQueue.h"
#include "./../../LogFile/H/logFile.h"
#include "./../../Debugable/H/debugable.h"
#include "./../../LeakDetector/H/leakDetector.h"

#define EM_DELIMITOR ':'

#define GM_LOG_LABEL "GMessage contains:"

/**
 * Generic message wrapper.
 *
 * enables reading and writing of named fields. Each is one of two
 * types: BytesMsgField or FileMsgField, enabling to handle the
 * contents of the field as a byte stream or as a file.
 * <p>
 * Description: This file contains the GMessage class which is used not only
 * for parsing an incoming message or constructing an outcoming one.
 * It is also used as a general information storing mechanism which enables
 * usage of named fields.<br>
 * Allows nested messages through SetAsMessage, GetAsMessage.
 *
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
class GMessage: public Debugable//, public LeakDetector
{
  protected:
  /**@name attributes */
  /*@{*/
      /// MsgFields
    FieldQueue *fields;
  /*@}*/

  /**@name methods */
  /*@{*/
    Void Initialize();
    Char *ReadBytesHeader(Void *aBuffer, Size aLength, Size *aSize, Size *anOffset);
    Char *ReadFileHeader(File *aFile, Size *aSize);
    Err WriteBytesHeader(Void *aBuffer, MsgField *aMsgField, Size *anOffset) const;
    Err WriteFileHeader(File *aFile, MsgField *aField) const;
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
      /// constructor
    GMessage(LogFile *aLogFile = NULL);
      /// constructor
    GMessage(const GMessage *, LogFile *aLogFile = NULL);
      /// constructor
    GMessage(Char *aFileName, LogFile *aLogFile = NULL);
      /// constructor
    GMessage(Void *aBuffer, Size aSize, LogFile *aLogFile = NULL);
      /// destructor
    ~GMessage();

      /// Total size
    Size SizeOf() const;

      /// debugging
    /*@{*/
    Err DebugHeaders(LogFile *aLogFile, Char *aLogLabel = GM_LOG_LABEL, LogLevel aLogLevel = llDebug);
    Size CheckSum(char *input, Size len);
    Size Sum(char *input, Size len);
    /*@}*/

      /// loading the stored information into the MsgFields
    /*@{*/
    Err LoadFromBytes(Void *aBuffer, Size aSize);
    Err LoadFromFile(Char *aFileName);
    /*@}*/

      /// storing the message
    /*@{*/
    Void *StoreToBytes(Size *aSize = NULL) const;
    Err StoreToFile(Char *aFileName) const;
    MsgField *StoreToMsgField(Char *aFieldName = NULL) const;
    /*@}*/

      /// getting and setting the information contained in the fields
    /*@{*/
    Void *GetAsBytes(Char *aFieldName, Size *aSize = NULL) const;
    Char *GetAsString(Char *aFieldName) const;
    GMessage *GetAsGMessage(Char *aFieldName) const;
    MsgField *GetAsMsgField(Char *aFieldName) const;
    Err GetAsFile(Char *aFieldName, Char *aFileName) const;
    Err GetAsInt(Char *aFieldName, Int *i) const;
    Err GetAsSize(Char *aFieldName, Size *i) const;
    Err GetAsID(Char *aFieldName, ID *t) const;
    Err GetAsBasicTime(Char *aFieldName, BasicTime *t) const;

    Err SetAsBytes(Char *aFieldName, Void *aBuffer, Size aSize);
    Err SetAsString(Char *aFieldName, Char *aString);
    Err SetAsGMessage(Char *aFieldName, GMessage *aGMessage);
    Err SetAsMsgField(Char *aFieldName, MsgField *aMsgField);
    Err SetAsFile(Char *aFieldName, Char *aFileName);
    Err SetAsInt(Char *aFieldName, Int i);
    Err SetAsID(Char *aFieldName, ID t);
    Err SetAsSize(Char *aFieldName, Size s);
    Err SetAsBasicTime(Char *aFieldName, BasicTime t);
    /*@}*/
      
      /// manipulating gMessage
    /*@{*/
    Void Clear();
    Err Delete(Char *aFieldName);
    Err Rename(Char *aFieldName, Char *newFieldName);
    /*@}*/
  /*@}*/
};

#endif

