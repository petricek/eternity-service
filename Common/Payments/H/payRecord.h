#ifndef PAY_RECORD_H
#define PAY_RECORD_H

#include "./payDefines.h"
#include "./../../GMessage/H/msgField.h"
#include "./../../GMessage/H/table.h"

  /**@name max length of authentification strings */
  /*@{*/
#define MAX_LENGTH_OF_AUTH       20
  /*@}*/

/**
 * Payment Record.
 *
 * An auxiliary abstract class for one payment record. Each descendant
 * of this class must overwrite FetchFromRecord and FillRecord methods
 * to be able to fetch record from or write record to a table.
 * Ancestor of all payment record classes (in bank, client and eso).
 *
 * @author Ivana
 * @see    Bank, Client, Eso 
 */
class PayRecord
{
  protected:

  /**@name attributes */
  /*@{*/
    Err eCode;

    ESTime time;
    ID payId;
    Char *amount;
    MsgField *sealSAuth;
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
    /// constructor
    PayRecord();
    /// destructor
    virtual ~PayRecord();

    Err IsOK();

    virtual Err FetchFromRecord(Record *r) = 0;
    virtual Err FillRecord(Record *r) = 0;
    virtual Void Clear();

    Err FetchFromTable(Table *t, Char *whichId=NULL, ID value=0);
    Err FetchAndDeleteFromTable(Table *t, Char *whichId=NULL, ID value=0);
    Err WriteToTable(Table *t);
    Err UpdateTable(Table *t);
  /*@}*/

};

#endif
