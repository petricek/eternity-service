#ifndef ROLLBACKABLE_H
#define ROLLBACKABLE_H

#include "./../../H/common.h"

/**
 * Class taht is able to roll back its transaction.
 *
 * In cooperation with TransactionManager is able to roll back 
 * some transactions that failed.
 *
 * @author  Pavel
 * @see     TransactionManager
 */
class Rollbackable
{
  public:
  /**@name methods */
  /*@{*/
    /// Raoll back and return to a consistent state
    virtual Err Rollback(TID) = 0;
  /*@}*/
};

#endif
