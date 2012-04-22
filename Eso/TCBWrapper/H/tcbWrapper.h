#ifndef TCB_WRAPPER_H
#define TCB_WRAPPER_H

#include "./tcbTable.h"
#include "./keywords.h"
#include "./const.h"
#include "./../../../Common/Rollbackable/H/rollbackable.h"
#include "./../../../Common/Debugable/H/debugable.h"
#include "./../../../Eso/Eso/H/eso.h"

/**
 * General Trusted Computing Base wrapper.
 * 
 * This is an abstract generally designed class that enables
 * any realization of TCB (either software or hardware). To each
 * such TCB it would be needed to create appropriate TCBWrapper
 * that would do all that stuff defined in this class.
 *
 * @author Pavel
 * @see    Eso
**/
class TCBWrapper: public Rollbackable, public Debugable, public SixOffspring
{
public:
  /**@name methods*/
  /*@{*/
    /// Costructor.
  TCBWrapper(Eso *anEso, LogFile *aLogFile = NULL);
    /// Destructor.
  virtual ~TCBWrapper();

    /// Ability to be rollbacked.
  virtual Err Rollback(const TID tid) = 0;

    /** 
     *  Stores file kept in "data" with ID "fid". "ffid", "keywords", "data" encrypted
     *  with TCBPubKey.
     */
  virtual Err SaveFile(  const ACID acid, 
                         const ID fid, 
			 const MsgField *ffid, 
			 const MsgField *keywords,
                         const MsgField *data) = 0;

    /// Deletes a file.
  virtual Err DeleteFileByFileID(const ID fid) = 0;

    /// Computes MAC by finding a file and concatenating it with OAuth.
  virtual Err ComputeMAC(const ID fid, const MsgField *oAuth, MsgField **mac) = 0;

    /** 
     * Finds all files matching the File Atribute Mask, and returns.
     * their headers that include FinderFileID, FileSize. FAM encrypted by
     * TCB Public Key. 
     */
  virtual Queue *GetFileHeadersByFAM(  const ACID acid,
                                       const MsgField *fam, 
				       const GMessage *clientPubKey) = 0;

    /** 
     * Finds the file with "ffid", decrypts it, reencrypts it with "clientPubKey".
     * and returns it as a MsgField. 
     */
  virtual MsgField *GetFileByFinderFID(  const ACID acid,
                                         const MsgField *ffid, 
					 const GMessage *clientPubKey) = 0;

    /// Finds aproprite private key and returns decrypted "data".
  virtual MsgField *DecryptByACPriv(const ACID acid, const MsgField *data) = 0;
  
    /// Generates and stores AC keys (both AC and  TCB).
  virtual Err GenerateACKeys(ACID *acid, GMessage **acPubKey, GMessage **tcbPubKey) = 0;

    /// Returns pub keys to ACID.
  virtual Err GetACPubKeys(  const ACID, 
                             GMessage **acPubKey = NULL, 
			     GMessage **tcbPubKey = NULL) = 0;

    /// Removes AC keys (both AC and  TCB).
  virtual Err RemoveACKeys(const ACID acid) = 0;


    /** 
     *  Generates new Master Key and  does all reencryption needed (table of
     *  ring keys). 
     */
  virtual Err RefreshMasterKey() = 0;

    /** 
     * Generates new Ring Keys and  does all reencryption needed (table of files and
     * Storage keys, table of certificates, table of transport keys). 
     */
  virtual Err RefreshRingKeys() = 0;
  /*@}*/
};


/**
 * Software Trusted Computing Base wrapper.
 * 
 * This class realizes all TCB tasks purely programmaticaly without need of
 * any hardware.
 *
 * @author Pavel
 * @see    TCBWrapper, Eso
**/
class SWTCBWrapper: public TCBWrapper
{
protected:
  /**@name attributes*/
  /*@{*/
  Cipherer *cipherer;

    /// Symmetric key used as a master key for hierarchical key management.
  GMessage *masterKey;

    /// Table with information about stored files, encrypted with a ring
    /// key from a level below master key in key hierarchy.
  TCBTable *storageTable;

    /// Table with Access Certificates keys, encrypted with its ring key
    /// from a level below master key in key hierarchy.
  TCBTable *certifTable;
  /*@}*/

  /**@name methods*/
  /*@{*/
    /**Methods used only by constructor.*/
    /*@{*/
  Err LoadMasterKey();
  Err GetRingKeys(GMessage ** ringKeys);
  Err InstantiateTCBTables(const GMessage *ringKeys);
    /*@}*/

    /**TCBTables management methods.*/
    /*@{*/
  Err StorTableInsert(  const ID fid, 
                        const MsgField *ffid,
	   	        const Char     *fileName,
		        const GMessage *storageKey,
		        const MsgField *keywords,
		        const Size      fileSize);
  Err StorTableParseRecord(  const Record *storRec,
                             ID        *fid        = NULL, 
                             MsgField **ffid       = NULL,
	   	             Char     **fileName   = NULL,
		             GMessage **storageKey = NULL,
		             Keywords **keywords   = NULL,
		             Size      *fileSize   = NULL);
  Err StorTableFindByFID(  const ID   fid, 
                           const Int  shouldClose = 1, 
	    	           Char     **fileName = NULL,
			   GMessage **storageKey = NULL);
  Err StorTableFindByFFID(  const MsgField *ffid, 
                            Char          **fileName = NULL, 
			    GMessage      **storageKey = NULL,
			    Keywords      **keywords = NULL,
			    Size           *size = NULL);
  Err CertTableInsert(  const ACID      acid, 
                        const GMessage *acPubKey,
                        const GMessage *acPrivKey,
		        const GMessage *tcbPubKey,
		        const GMessage *tcbPrivKey);
  Err CertTableFindByACID(  const ACID acid,
                            const shouldClose = 1,
			    GMessage **acPubKey = NULL,
			    GMessage **acPrivKey = NULL,
			    GMessage **tcbPubKey = NULL,
			    GMessage **tcbPrivKey = NULL);
    /*@}*/
  
    /// Finds aproprite TCB private key and returns decrypted "data".
  MsgField *DecryptByTCBPriv(const ACID acid, const MsgField *data);
    /// Creates encrypted header. Used when searching by keywords.
  GMessage *CreateReplyFileHeader(const GMessage *clientPubKey,
                                  const MsgField *ffid,
				  const Size      size);
  /*@}*/
public:
  /**@name methods*/
  /*@{*/
    /// Software TCBWrapper constructor.
  SWTCBWrapper(Eso *anEso, LogFile *aLogFile = NULL);
    /// Hardware TCBWrapper constructor.
  virtual ~SWTCBWrapper();
    /// Ability to be rollbacked.
  virtual Err Rollback(const TID tid);
    /** Stores file kept in "data" with ID "fid". "ffid", "keywords", "data" encrypted
        with TCBPubKey. */
  virtual Err SaveFile(  const ACID acid, 
                         const ID fid, 
			 const MsgField *ffid, 
			 const MsgField *keywords,
			 const MsgField *data);
    /// Deletes a file.
  virtual Err DeleteFileByFileID(const ID fid);
    /// Computes MAC by finding a file and concatenating it with OAuth.
  virtual Err ComputeMAC(const ID fid, const MsgField *oAuth, MsgField **mac);
    /** Finds all files matching the File Atribute Mask, and returns
        their headers that include FinderFileID, FileSize. FAM encrypted by
        TCB Public Key. */
  virtual Queue *GetFileHeadersByFAM(  const ACID acid,
                                       const MsgField *fam, 
			               const GMessage *clientPubKey);
    /** Finds the file with "ffid", decrypts it, reencrypts it with "clientPubKey"
        and returns it as a MsgField. */
  virtual MsgField *GetFileByFinderFID(  const ACID acid,
                                         const MsgField *ffid, 
				         const GMessage *clientPubKey);
    /// Finds aproprite private key and returns decrypted "data".
  virtual MsgField *DecryptByACPriv(const ACID acid, const MsgField *data);
    /// Generates and stores AC keys (both AC and  TCB).
  virtual Err GenerateACKeys(  ACID *acid, 
                               GMessage **acPubKey, 
			       GMessage **tcbPubKey);
    /// Returns pub keys to ACID.
  virtual Err GetACPubKeys(  const ACID, 
                             GMessage **acPubKey = NULL, 
			     GMessage **tcbPubKey = NULL);
    /// Removes AC keys (both AC and  TCB).
  virtual Err RemoveACKeys(const ACID acid);
    /** Generates new Master Key and  does all reencryption needed (table of
        ring keys). */
  virtual Err RefreshMasterKey();
    /** Generates new Ring Keys and  does all reencryption needed (table of files and
        storage keys, table of certificates, table of transport keys). */
  virtual Err RefreshRingKeys();
    /// Test method for debugging purposes only.
  Void Test();
  /*@}*/
};

#endif
