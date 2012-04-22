// *************************************************************************
//
// File: tcbTable.h
// Name: encrypted table for TCB
// Author: Pavel
// Last Modified: 8.2.1999 P
//
// *************************************************************************

#ifndef TCB_TABLE_H
#define TCB_TABLE_H

#include "./../../../Common/GMessage/H/gMessage.h"
#include "./../../../Common/GMessage/H/table.h"
#include "./../../../Common/Cipherer/H/cipherer.h"


/**
 * Encrypted (database like) table.
 *
 * The same like Table, but the file into which the data is stored is saved
 * encrypted with the symetric key. This implementation works with it in a
 * simple way: <br>
 * Open - decrypts the table, all operations then work on the decrypted file.
 * Close - encrypts the table.
 *
 *
 * @author  Pavel
 * @see     Table, TCBWrapper
**/
class TCBTable: public Table
{
protected:
  /**@name attributes */
  /*@{*/
  GMessage *ringKey;
  Cipherer *cipherer;
  Char *fileName;
  /*@}*/

public:
  /**@name methods */
  /*@{*/
    /// Constructor.
  TCBTable(const Char *tableName, TableFieldQueue *definition,
			          const GMessage *tableRingKey,
			          LogFile *aLogFile);
    /// Destructor.
  virtual ~TCBTable();

    /// Opens the TCBTable - decrypts it temporarily
  virtual Err Open();
    /// Closes the TCBTable - encrypts it
  virtual Err Close();
  /*@}*/
};


#endif
