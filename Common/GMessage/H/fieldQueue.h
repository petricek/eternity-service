#ifndef FIELDQUEUE_H
#define FIELDQUEUE_H

#include "./queue.h"
#include "./fields.h"
#include "./../../Debugable/H/debugable.h"

/**
 * Field queue class.
 *
 * enables to work easier with a queue storing Fields
 *
 * @author  Pavel
 * @see     TableFieldQueue
 */
class FieldQueue: public Queue, public Debugable
{
  public:
  /**@name methods */
  /*@{*/
      /// constructor
    FieldQueue(LogFile *aLogFile = NULL);
      /// constructor
    FieldQueue(FieldQueue &aQueue, LogFile *aLogFile = NULL);
      /// destructor
    virtual ~FieldQueue();   // discards the fields kept

      /// Return index of field in queue
    Int GetIndexOf(Char *aFieldName);
      /// return field
    Field *GetField(Char *aFieldName);
      /// return specific field
    Field *GetFieldByIndex(Int anIndex);
      /// Total size of queue
    virtual Size SizeOf();

      /// Insert field into queue
    Err Insert(Field *aField);
      /// Throw fields away
    Void DiscardAll();

      /// queue iteration
    /*@{*/
    Field *First();
    Field *Next();
    /*@}*/
  /*@}*/
};


/**
 * Table field queue.
 *
 * Enables to work easier with a queue storing TableFields
 * The TableFieldQueue class (defined lower) is designed for work with a
 * queue of TableFields and to simplify the work with Table definition
 * (see table.h) by hiding unnecessary things and retyping returned objs
 * to *TableField.
 * <p>NOTE: Unlike the Queue class, the FieldQueue class discards all objs
 *         it holds, when destructor is called.
 *
 *
 * @author  Pavel
 * @see     FieldQueue
 */
class TableFieldQueue: public FieldQueue
{
  public:
  /**@name methods */
  /*@{*/
      /// constructor
    TableFieldQueue(LogFile *aLogFile = NULL);
//    TableFieldQueue(TableFieldQueue &aQueue);

      /// queue information
    Int FieldsFixed();
      /// Return specific field from queue.
    Int FieldByIndexEqualTo(Int anIndex, Void *aValue);
//--    virtual Int SizeOf();

      /// field of queue manipulation
    Err SetField(Char *aFieldName, Void *aValue);

      /// queue iteration
    /*@{*/
    TableField *First();
    TableField *Next();
    /*@}*/
  /*@}*/
};

#endif
