#ifndef STABLE_H
#define STABLE_H

#include "../../../Common/GMessage/H/table.h"

/**
 * Sorted table by time.
 *
 * Sorts table by first column which have to be TimeField.
 *
 * @author Jirka
 * @see Scheduler
 */
class TimeSortedTable : public Table {
public:
  /*@name methods */
  /*@{*/
  TimeSortedTable( Char *fileName, TableFieldQueue *aDefinition): Table(fileName, aDefinition) {};
    /// Sorted insert 
  virtual Err InsertRecord(Record *aRecord);
  /*@}*/
};

#endif
