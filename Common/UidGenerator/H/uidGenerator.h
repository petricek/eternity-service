#ifndef UID_GENERATOR_H
#define UID_GENERATOR_H

  // has to be the FIRST include !
#include "./../../H/common.h"

#include <pthread.h>

#include "./../../GMessage/H/table.h"

  // name of the field in which the last ID is stored
#define UIDG_FLD_ID "ID"

/** 
 * Unique ID Generator.
 *
 * This class is responsible for generating unique ID's even in the case
 * of an unexpected termination of the program.
 *
 * @author Pavel
 */
class UniqueIDGenerator
{
protected:
  /**@name attributes */
  /*@{*/
  Table *lastIDTable;
  ID     startID;

  /// Synchronization
  pthread_mutex_t mutex;
  /*@}*/

  /**@name methods */
  /*@{*/
    /// create table definition for lastIDTable
  Err CreateDefinition(TableFieldQueue *definition);
    /// initialize the lastIDTable file 
  Err InitLastIDFile(Char *lastIDTableName);
  /*@}*/
public:
  /**@name methods */
  /*@{*/
    /// create ID Generator at the given path, starting with value startAt
  UniqueIDGenerator(Char *anIDTableName, ID startAt = 0);
    /// get unique ID
  Err GetID(ID *newID);
  /*@}*/
};


#endif
