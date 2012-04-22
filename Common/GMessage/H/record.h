#ifndef RECORD_H
#define RECORD_H

#include "./../../H/common.h"
#include "./fieldQueue.h"
#include "./fields.h"
#include "./file.h"


/**
 * Record in the Table.
 *
 * Description: This file contains the Record class which is used for
 * storing and retrieving information to/from a Table
 *
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
class Record
{
  private:
  /**@name attributes */
  /*@{*/
      /// Fields stored in table
    TableFieldQueue *fields;
  /*@}*/

  public:
  /**@name methods */
  /*@{*/
    /// constructor
    Record(TableFieldQueue *modelQueue);
    /// destructor
    ~Record();

      /// conversions
  /*@{*/
    Char *AsString();
    Err ReadFromFile(File *tableFile, Size *count);
    Err WriteToFile(File *tableFile);
  /*@}*/

      /// record information
  /*@{*/
    Size SizeOf();
    Int SizeOfHeader();
    Int IsFixed();
  /*@}*/

      /// field manipulation
  /*@{*/
    TableField *GetField(Char *aFieldName);
    TableField *GetFieldByIndex(Int anIndex);
    Int FieldByIndexEqualTo(Int anIndex, Void *aFieldValue);
    Err SetField(const Char *aFieldName, const Void *aFieldValue);
  /*@}*/
  /*@}*/
};

#endif
