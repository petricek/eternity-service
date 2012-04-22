#ifndef _TABLE_H
#define _TABLE_H

#include "./record.h"
#include "./file.h"
#include "./../../Debugable/H/debugable.h"

/**
 * Table class.
 *
 * enables to define a table, to insert, update or delete records,
 * to iterate trough the table, to search appropriate records by
 * the value in the specified field.<p>
 * Description: This file contains the Table class which enables
 * persistent "table-like" storage of information with sequential
 * access only.
 * ATTENTION:
 * <p>
 * <li>
 * Methods First, Next, FindFirst, FindNext return NEW INSTANCE of class
 * Record. Therefore every object returned by these should be disposed off.
 *
 * <li>
 * Table takes no responsibility for freeing searchValue (used in FindFirst
 * and FindNext methods) because searching e.g. for an integer is simply done
 * this way:
 * <pre>
 *   Int searchFor = 100;
 *   Record *r = table->FindFirst("Integer field", searchFor);
 * </pre>
 * In such a case there would be no sense to free searchFor, even more
 * it would be a mistake.
 * <p>
 * Remember that changing contents of memory pointed to by searchValue
 * means searching for new value (FindFirst doesn't create its own copy of
 * *searchValue - it cannot, because it would have to know how many bytes it
 * should copy. That seemed ugly to me.).
 * @author  Pavel
 * @see  TableField
 */
class Table: public Debugable
{
  protected:
  /**@name attributes */
  /*@{*/
    TableFieldQueue *definition;
    File *file;
    Int searchField;
    Void *searchValue;
      // if all fields are fixed, this is the size of the whole record
      // 0 otherwise
    Size recordLength;
    Size lastRecordLength;
  /*@}*/

  /**@name methods */
  /*@{*/
    Err WriteRecord(Record *aRecord);
    Record *ReadRecord();
    Size ReadRecordLength();
    Int CanDelete();
    Int CanUpdate();
    Char *CreateTmpFileName();
  /*@}*/

  public:
  /**@name methods */
  /*@{*/
    /// constructor
    Table(const Char *fileName, TableFieldQueue *aDefinition, 
                                LogFile *aLogFile = NULL);
    /// destructor
    ~Table();

      /// information providers
    /*@{*/
    Char* GetFileName();
    Int IsOpen();
    Int IsFixed();
    /*@}*/

      /// table manipulation
  /*@{*/
    virtual Err Open();
    virtual Err Close();
    virtual Err Remove();
  /*@}*/

      /// table iteration
  /*@{*/
    Record *First();
    Record *Next();
  /*@}*/

      /// record manipulation
  /*@{*/
    Record *NewRecord();
    Err InsertRecord( Record *aRecord );
    Err DeleteRecord();
    Err UpdateRecord( Record *aRecord );
  /*@}*/

      /// record searching
  /*@{*/
  /*@}*/
    Record *FindFirst( const Char *aFieldName, const Void *aFieldValue );
    Record *FindNext();
  /*@}*/
};

#endif
