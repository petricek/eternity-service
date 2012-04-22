//*************************************************************************
// 
// File: allocator.h
// Name: Header for Allocator class
// Author: Pavel 
// Last Modified: 29.12.1998 P
//
// NOTE: not synchronized
// 
//*************************************************************************

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "./../../../Common/Rollbackable/H/rollbackable.h"
#include "./../../../Common/Debugable/H/debugable.h"
#include "./../../../Common/GMessage/H/table.h"
#include "./../../Common/H/constants.h"
#include "./../../../Eso/Eso/H/eso.h"

//#define ALLOC_LOG_FILE         "allocator.log"
#define ALLOC_PATH             ESO_PATH_ALLOCATOR
#define ALLOC_FILES_PATH       ESO_PATH_ALLOCATOR"Files/"
#define ALLOC_TABLE_NAME       "allocations.tbl"
#define ALLOC_TBLFLD_KEY_ID    "ID"
#define ALLOC_TBLFLD_FILE_SIZE "Size"
#define ALLOC_TBLFLD_FILE_NAME "Name"

/** 
 * Class for allocating space on a disk.
 * 
 * This class is responsible for allocating space on the disk, when request
 * to store a file arrives to Eso. It stores
 * all information about allocations in a table with this structure:
 * <br>(TID: ID, FileSize: Size, FileName: String). 
 * 
 * @author Pavel
 * @see    Eso
 */
class Allocator: public Rollbackable, public Debugable, public SixOffspring
{
  protected:
  /**@name attributes*/
  /*@{*/
	  /// Used to store information about allocated space.
    Table *allocTable;
  /*@}*/

  /**@name methods*/
  /*@{*/
      /// Creates definition of allocTable - used only in constructor.
    TableFieldQueue *CreateDefinition();
      /// Creates a dummy place holder file.
    Err CreateAllocFile(const Size fileSize, Char **allocFileName);
      /// Inserts information about allocated space to allocTable.
    Err AllocTableInsert(const TID tID, const Size fileSize, const Char *fileName);
      /** Searches allocTable by transaction ID, enables not closing
          the table after the search. */
    Err AllocTableFind(const TID tID, const Int shouldClose = 1, 
                                          Size *fileSize = NULL, 
                                          Char **allocFileName = NULL);
  /*@}*/
    
  public:
  /**@name methods*/
  /*@{*/
      /// Constructor.
    Allocator(Eso *anEso, LogFile *allocLogFile);
      /// Destructor.
    ~Allocator();
      /// Tries to allocate space for a specified time
    Err AllocSpace(TID tID, Size fileSize);
      /** Tests if there is an allocation (identified by tID)
          with greater or equal size to fileSize. */
    Err AllocValid(TID tID, Size fileSize);
      /// Frees specified allocation space.
    Err FreeSpace(TID tID);

      /// Rollbacks allocation created in tID transaction.
    virtual Err Rollback(TID tID);
      /// A test function.
    void Test();
  /*@}*/
};

#endif

