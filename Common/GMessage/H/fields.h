#ifndef FIELDS_H
#define FIELDS_H

//-- debugging (Print)
  #include <stdio.h>

#include "./../../H/common.h"
#include "./myString.h"
#include "./estime.h"
#include "./../../Debugable/H/debugable.h"
#include "./../../GMessage/H/file.h"

#define SIZE_OF_HEADER (sizeof(Size))
#define TIME_REPREZ long
#define DEFAULT_TIME 0L

  // forward declarations
class GMessage;
class MsgField;


/**
 * Ancestor of all fields used in GMessage and Table.
 *
 * Ancestor of fields used in GMessage and Table with 
 * one attribute - name. Setting and checking of name is implemented.
 *
 * @author  Marek, Pavel
 * @see     GMessage, Table, MsgField, TableField
 */
class Field: public Debugable
{
  protected:
  /**@name attributes */
  /*@{*/
      ///name of the field
    Char *name;
  /*@}*/

  public:
  /**@name methods */
  /*@{*/
      ///constructor
    Field(Char *aName, LogFile *aLogFile = NULL);
      ///destructor
    virtual ~Field();
      ///copying of the field
    virtual Field *CreateCopy() = 0;

      ///name setting
    Void SetName(Char *aName);

      ///returns name
    Char *GetName();
      ///checking of the name
    Int NameEqualTo(Char *aName);
      ///checks the size of the field
    virtual Size SizeOf() = 0;
  /*@}*/
};


/**
 * Ancestor of all fields used in Record of Table.
 *
 * Ancestor of fields used in Record of Table.
 * Virtual methods for reading and writing itself to file and for
 * checking the value are defined here.
 *
 *
 * @author  Marek, Pavel
 * @see     Record, Table
 */
class TableField: public Field
{
  public:
  /**@name methods */
  /*@{*/
      /// constructor
    TableField(Char *aName, LogFile *aLogFile = NULL): Field(aName, aLogFile) {};
      /// destructor
    virtual ~TableField() {};

      /// debugging method
    virtual char *Print() = 0;

      /// value checking
    virtual Int EqualTo(void *aValue) = 0;
      /// Is fixed ?
    virtual Int IsFixed() = 0;

      /// conversions
    virtual Err SetValue(Void *aValue) = 0;
      /// Read field from file
    virtual Err ReadFromFile(File *aFile, Size *bytesToEORec) = 0;
      /// Write field to file
    virtual Err WriteToFile(File *aFile) = 0;

      /// other conversions
    virtual Char *AsString() = 0;
  /*@}*/
};


/**
 * Int filed.
 *
 * TableField, that holds int value.
 *
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
class IntField: public TableField
{
  protected:
  /**@name attributes */
  /*@{*/
    /// stored value
    Int value;
  /*@}*/

  public:
  /**@name methods */
  /*@{*/
      /// constructor
    IntField(Char *aName, Int aValue = 0, LogFile *aLogFile = NULL);
      /// destructor
    virtual ~IntField() {};
      /// "Copy constructor"
    virtual Field *CreateCopy();

    /// debugging
    virtual char *Print() {
      char *c = new char[10];
      sprintf(c, "%d", value);
      return c;
    };

      /// data checking
    virtual Int EqualTo(void *aValue);
      /// Total size
    virtual Size SizeOf();
      /// Has fixed size?
    virtual Int IsFixed() { return 1; };
      /// Return the stored int
    Int GetIntValue() {return value; };

      /// conversion
    virtual Err SetValue(Void *aValue);
      /// Read field from file
    virtual Err ReadFromFile(File *aFile, Size *bytesToEORec);
      /// Write field to file
    virtual Err WriteToFile(File *aFile);

      ///other conversions
    virtual Char *AsString();
  /*@}*/

};


/**
 * String field.
 *
 * TableField, that holds Char* value
 *
 *
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
class StringField: public TableField
{
  protected:
  /**@name attributes */
  /*@{*/
      // Stored value
    Char *value;
  /*@}*/

  public:
  /**@name methods */
  /*@{*/
      /// constructor
    StringField(Char *aName, Char *aValue = NULL, LogFile *aLogFile = NULL);
      /// destructor
    virtual ~StringField();
      /// "copy constructor"
    virtual Field *CreateCopy();

    /// debugging
    virtual char* Print() {
      return(AsString());
    };

    ///data checking
    virtual Int EqualTo(void *aValue);
      /// Total size
    virtual Size SizeOf();
      /// has fixed size ?
    virtual Int IsFixed() { return 0; };

      ///conversions
    virtual Err SetValue(Void *aValue);
      /// Read field from file
    virtual Err ReadFromFile(File *aFile, Size *bytesToEORec);
      /// Write field to file
    virtual Err WriteToFile(File *aFile);

      ///other conversions
    virtual Char *AsString();
  /*@}*/
};


/**
 *  Fixed string field.
 *
 * TableField, that holds char* value with fixed length
 *
 *
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
class FixedStringField: public StringField
{
  protected:
  /**@name attributes */
  /*@{*/
    /// stored value
    Int length;
  /*@}*/

  public:
  /**@name methods */
  /*@{*/
    /// constructor
    FixedStringField(Char *aName, Int length, Char *aValue = NULL, LogFile *aLogFile = NULL);
    /// destructor
    virtual ~FixedStringField() {};
    ///"copy constructor"
    virtual Field *CreateCopy();

    ///data checking
    virtual Size SizeOf();
      /// has fixed size ?
    virtual Int IsFixed() { return 1; };

      ///conversions
    virtual Err SetValue(Void *aValue);
      /// Read field from file
    virtual Err ReadFromFile(File *aFile, Size *bytesToEORec);
      /// Write field to file
    virtual Err WriteToFile(File *aFile);

      ///other conversions
    virtual Char *AsString();
  /*@}*/
};


/**
 * ID field.
 *
 * TableField, that holds ID value
 *
 *
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
class IDField: public TableField
{
  protected:
  /**@name attributes */
  /*@{*/
    /// stored value
    ID value;
  /*@}*/

  public:
  /**@name methods */
  /*@{*/
    /// constructor
    IDField(Char *aName, ID aValue = 0, LogFile *aLogFile = NULL);
    /// destructor
    virtual ~IDField() {};
    /// "copy constructor"
    virtual Field *CreateCopy();

    ///debugging
    virtual char *Print() {
      char *c = new char[10];
	// Venca opravil "%d" na "%ld"
      sprintf(c, "%ld", value);
      return c;
    };

    ///data checking
    virtual Int EqualTo(void *aValue);
      /// Total size
    virtual Size SizeOf();
      /// has fixed size ?
    virtual Int IsFixed() { return 1; };
      /// Return stored valued
    Err GetID(ID *anID);
      /// Return stored valued
    ID GetIDValue() {return value; };
    
      //conversions
    virtual Err SetValue(Void *aValue);
      /// Read field from file
    virtual Err ReadFromFile(File *aFile, Size *bytesToEORec);
      /// Write field to file
    virtual Err WriteToFile(File *aFile);

      //other conversions
    virtual Char *AsString();
  /*@}*/
};


/**
 * Size field.
 *
 * TableField, that holds Size value
 *
 *
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
class SizeField: public IDField 
{
  public:
  /**@name methods */
  /*@{*/
    /// constructor
    SizeField(Char *aFieldName, Size aValue = 0, LogFile *aLogFile = NULL);
    /// return stored value
    Size GetSizeValue() { return GetIDValue(); };
  /*@}*/
};


/**
 * Time field.
 *
 * TableField, that holds Time value
 *
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
class TimeField: public TableField
{
  protected:
  /**@name attributes */
  /*@{*/
      /// Stored value
    TIME_REPREZ value;
  /*@}*/

  public:
  /**@name methods */
  /*@{*/
    /// constructor
    TimeField(Char *aName, BasicTime *aValue = NULL, LogFile *aLogFile = NULL);
    /// constructor
    TimeField(Char *aName, TIME_REPREZ aValue, LogFile *aLogFile = NULL);
    /// destructor
    virtual ~TimeField() {};
    /// "copy constructor"
    virtual Field *CreateCopy();

    ///debugging
    virtual char *Print() {
      char *c = new char[sizeof(TIME_REPREZ)];
      sprintf(c, "%ld", value);
      return c;
    };

    ///data checking
    virtual Int EqualTo(void *aValue);
      /// Total size
    virtual Size SizeOf();
      /// has fixed size ?
    virtual Int IsFixed() { return 1; };
      /// return stored value
    ESTime GetESTimeValue();
      /// return stored value
    RelTime GetRelTimeValue();

      ///conversions
    virtual Err SetValue(Void *aValue);
      /// Read field from file
    virtual Err ReadFromFile(File *aFile, Size *bytesToEORec);
      /// Write field to file
    virtual Err WriteToFile(File *aFile);

      ///other conversions
    virtual Char *AsString();
  /*@}*/
};

/**
 * Table field.
 *
 * TableField, that holds an array of bytes in format <len, array>
 *
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
class BytesField: public TableField
{
  protected:
  /**@name attributes */
  /*@{*/
      /// Stored value
    MsgField *value;
  /*@}*/

  public:
  /**@name methods */
  /*@{*/
    /// constructor
    BytesField(Char *aName, MsgField *aValue = NULL, LogFile *aLogFile = NULL);
    /// destructor
    virtual ~BytesField() ;
    /// "copy constructor"
    virtual Field *CreateCopy();

    ///debugging
    virtual char* Print() {
      return AsString();
    };

    /// data checking
    virtual Int EqualTo(void *aValue);
      /// Total size
    virtual Size SizeOf();
      /// has fixed size ?
    virtual Int IsFixed() { return 0; };

      /// Return value as GMessage
    GMessage *GetAsGMessage();
      /// Return value as MsgField
    MsgField *GetAsMsgField();
      /// Set value from GMessage
    Err SetAsGMessage(GMessage *aGMessage);
      /// Set value from MsgField
    Err SetAsMsgField(MsgField *aMsgField);

      /// conversions
    virtual Err SetValue(Void *aValue);
      /// Read field from file
    virtual Err ReadFromFile(File *aFile, Size *bytesToEORec);
      /// Write field to file
    virtual Err WriteToFile(File *aFile);

      ///other conversions
    virtual Char *AsString();
  /*@}*/
};

#endif
