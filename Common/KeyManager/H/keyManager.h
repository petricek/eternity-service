#ifndef KEYMANAGER_H
#define KEYMANAGER_H

class KeyManager;

  // has to be the FIRST include !
#include "./../../H/common.h"

#include "./../../Cipherer/H/cipherer.h"
#include "./../../ConfigFile/H/configFile.h"
#include "./../../Debugable/H/debugable.h"
#include "./../../GMessage/H/gMessage.h"
#include "./../../MessageQueue/H/messageQueue.h"


  /// Maximum length of hostname
#define MAX_HOST_NAME_STRING_LEN 40

/**
 * Generic key manager.
 *
 * Generates all kinds of keys and manages them - takes care of their expiration
 * and renewal.
 *
 * @author  Pechy, Venca
 * @see     Cipherer AddressManager
 */
class KeyManager: public Debugable
{
  public:
  /**@name attributes */
  /*@{*/
    /// asymetric keys
    GMessage *asymetricKeyPair;
    /// Configuration file
    ConfigFile *configFile;
    /// Encryption class
    Cipherer *cipherer;
    ///Message queue for publishing
    MessageQueue *messageQueue;
  /*@}*/

  /**@name methods */
  /*@{*/
    /// Constructor
    KeyManager(ConfigFile *configFile, 
                           LogFile * keyManagerLogFile,
                           Cipherer *cipherer,
                           MessageQueue *messageQueue);
    /// Destructor
    virtual ~KeyManager();
    
    /// Returns this Mix's private key
    GMessage * GetMixPrivateKey(char *asymCipherType, char *asymCipherType);
    /// Returns this Mix's public key
    GMessage * GetMixPublicKey(char *asymCipherType, char *asymCipherType);
    /// Generates Mix certificate (for this Mix)
    Err CreateMixCertificate(u_short port, GMessage **certificate);
    /// Load keys from disk
    Err LoadAsymetricKeyPairIntoMemory();
  /*@}*/
};
#endif
