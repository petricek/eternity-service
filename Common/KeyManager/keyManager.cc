  // has to be the FIRST include !
#include "./../H/common.h"

#include <unistd.h>

#include "./H/keyManager.h"
#include "../H/labels.h"
#include "./../Cipherer/H/cipherer.h"

/**
 * Key manager constructor.
 *
 * Stores pointers to objects that are need later for various tasks.<br>
 * It also loads keys saved on disk into memory in case someone wants it.
 *
 * @param   keyManagerConfigFile   Pointer to cinfig that we ask for
 *                                 configuration options.
 * @param   keyManagerLogFile      Log file that we have to write
 *                                 debug messages to.
 * @param   keyManagerCipherer     Cipherer object that is used for
 *                                 key generation.
 * @param   keyManagerMessageQueue Message queue that is used for
 *                                 publishing keys.
 * @author  Pechy
 * @see     Cipherer
 */
KeyManager::KeyManager(ConfigFile *keyManagerConfigFile, 
                       LogFile * keyManagerLogFile,
		       Cipherer *keyManagerCipherer,
		       MessageQueue *keyManagerMessageQueue)
  :Debugable(keyManagerLogFile)
{
  configFile = keyManagerConfigFile;
  cipherer = keyManagerCipherer;
  messageQueue = keyManagerMessageQueue;

  if (LoadAsymetricKeyPairIntoMemory() == KO) {
    // check is made inside the method
    ;
  }
}

/**
 * Key manager destructor.
 *
 * Deletes asymetric key pair from memory.
 *
 * @author  Pechy
 * @see     KeyManager()
 */
KeyManager::~KeyManager()
{
  DELETE(asymetricKeyPair);
}

/**
 * Loads asymetric keys from disk.
 *
 * If there is a problem loading keys then an error is reported and
 * exit called.
 *
 * @return  OK if success else KO.
 * @author  Pechy
 * @see     GetMixPrivateKey(), GetMixPublicKey()
 */
Err KeyManager::LoadAsymetricKeyPairIntoMemory()
{
  string keyPair = configFile->GetValue(CFG_CERTIFICATES_DIRECTORY) + "/" + 
                   configFile->GetValue(CFG_ASYMETRIC_KEY_PAIR);

  if ((char *) keyPair.c_str() == "") {
    WriteString(llError, __FILE__ ":%d:Bad format of configuration file?", 
                __LINE__);
    return KO;
  }

  WriteString(llDebug, __FILE__ ":%d:Looking for Mix's asymetric key "
              "pair in %s.", __LINE__, (char *) keyPair.c_str());

  asymetricKeyPair = new GMessage();

  if (asymetricKeyPair->LoadFromFile((char *) keyPair.c_str()) == KO) {

    WriteString(llError, __FILE__ ":%d:Mix's asymetric key pair not found.", 
      __LINE__);

    fprintf(stderr, "Asymetric key pair not found, exiting.\n");
    exit(1);
  }
  else {
    WriteString(llDebug, __FILE__ ":%d:Mix's asymetric key pair found.", 
      __LINE__);
    asymetricKeyPair->DebugHeaders(logFile, "What is in Mix's "
                                   "asymetric key?");
    GMessage *tmpPub = asymetricKeyPair->GetAsGMessage(GM_PUBLIC_KEY_INFO);
    tmpPub->DebugHeaders(logFile, "What is in public key?");
    DELETE(tmpPub);
    GMessage *tmpPriv = asymetricKeyPair->GetAsGMessage(GM_PRIVATE_KEY_INFO);
    tmpPriv->DebugHeaders(logFile, "What is in private key?");
    DELETE(tmpPriv);
  }

  return OK;
}

/**
 * Returns Mix private key.
 *
 * Extracts private part from asymetric key and returns it. 
 *
 * @return  Pointer to GMessage containing asymetric private key.
 * @author  Pechy
 * @see     Cipherer
 */
GMessage *KeyManager::GetMixPrivateKey(char * /*asymCipherType    */, 
                                       char * /*asymCipherVersion */)
{
  return(asymetricKeyPair->GetAsGMessage(GM_PRIVATE_KEY_INFO)); 
}

/**
 * Returns Mix public key.
 *
 * Extracts public part from asymetric key and returns it. 
 *
 * @return  Pointer to GMessage containing asymetric public key.
 * @author  Pechy
 * @see     Cipherer
 */
GMessage *KeyManager::GetMixPublicKey(char * /*asymCipherType*/,
                                      char * /*asymCipherVersion*/)
{

  return((GMessage *) asymetricKeyPair->GetAsGMessage(GM_PUBLIC_KEY_INFO)); 
}
