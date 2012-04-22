#ifndef KEYMANAGER_H
#define KEYMANAGER_H

#include "./../../../Common/Rollbackable/H/rollbackable.h"
#include "./../../../Common/Debugable/H/debugable.h"
#include "./../../../Common/GMessage/H/table.h"
#include "./../../../Common/GMessage/H/gMessage.h"
#include "./../../../Common/H/common.h"
#include "./../../../Common/H/labels.h"

#define KEYMGR_TBLFLD_ACID     "ACID"
#define KEYMGR_TBLFLD_PUB_KEY  "PubKey"
#define KEYMGR_TBLFLD_PRIV_KEY "PrivKey"

#define KEYMGR_TABLE "KeyManager/keys.tbl"

/**
 * KeyManager.
 *
 * manages the public and private keys in the table
 * @author Marek
 */
class KeyManager: public Debugable
{
  protected:
  /**@name attributes */
  /*@{*/
    /// Table containing keys
    Table *keyTable;
  /*@}*/
 
  /**@name methods */
  /*@{*/
      /// creates definition of acTable - used only in constructor
    TableFieldQueue *CreateDefinition(); 
  /*@}*/
  
  public:  
          
  /**@name methods */
  /*@{*/
    /// Constructor
    KeyManager(LogFile *aLogFile, Char* aTableName);
    /// Destructor          
    virtual ~KeyManager();
      ///inserts AC into the table
    Err SaveKeys(ACID acid, GMessage *pubKey, GMessage *privKey);
      /// finds the AC according to its ACID
    GMessage *GetPrivKeyByACID(ACID acID);
  /*@}*/
  
};

#endif
