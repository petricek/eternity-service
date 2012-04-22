#include "./H/majordomo.h"
#include "./../../Common/RandomGenerator/H/randomGenerator.h"


/**
 * Generates ID of certificate.
 *
 * Generates random string of the length of ACID.
 *
 *
 * @param   acid Generated acid.
 * @return  error code
 * @author  Marek
 * @see     RandomGenerator
 */
Err 
Majordomo::GenerateACID(ACID *acid)
{
 Void *tmp;
 Err result;
 Size aSize;
DBG; 
  RandomGenerator *randGen = new RandomGenerator(logFile);
  if (randGen == NULL)
    return KO;

  MsgField *acidMsg = randGen->RandomBytes(sizeof(ACID));
  if (acidMsg == NULL){
    DELETE(randGen);
    return KO;
  }
DBG;
  result = OK;
  *acid = *(ACID*)(tmp = acidMsg->GetAsBytes(&aSize));
  if(tmp == NULL)
    result = KO;

  DELETE(tmp);
  DELETE(randGen);
  DELETE(acidMsg);
DBG;
  return result; 
}

/**
 * Generates public and private keys..
 *
 * Calls the cipherer's method to generate public and private keys.
 * Saves them in files.
 *
 *
 * @return  error code
 * @author  Marek
 * @see     Cipherer
 */
Err 
Majordomo::GenerateAsymetricKey()
{

  GMessage *keyPair = six->cipherer->GenerateAsymetricKey();
  if(keyPair == NULL)
  {
    return KO;
  }

  GMessage *acPubKey = keyPair->GetAsGMessage(GM_PUBLIC_KEY_INFO);
  GMessage *acPrivKey = keyPair->GetAsGMessage(GM_PRIVATE_KEY_INFO);
  DELETE(keyPair);

  Err result = OK;
  Char *tmp;
  if(acPrivKey->StoreToFile(tmp = six->GetPath(PRIVATE_KEY_FILE)) != OK)
    result = KO;
  DELETE(tmp);
  if(acPubKey->StoreToFile(tmp = six->GetPath(PUBLIC_KEY_FILE)) != OK)
    result = KO;
  DELETE(tmp);
  
  DELETE(acPrivKey);
  DELETE(acPubKey);

  return result;
}


/**
 * Gets Asymetric key from file.
 *
 * Gets Client's pubKey from file.
 *
 *
 * @param   acPubkey Returned pubKey.
 * @return  error code
 * @author  Marek
 * @see     
 */
Err 
Majordomo::GetAsymetricKey(GMessage **acPubKey)
{
  Char *tmp;
  DBG;
  *acPubKey = new GMessage();
  Err result = ((*acPubKey)->LoadFromFile(tmp = six->GetPath(PUBLIC_KEY_FILE)));
  DELETE(tmp);
  DBG;
  WriteString(llDebug, "Result od GetAsymKey == %s",  (result == OK ? "OK" :"KO"));
  return result;
}


/**
 * Decrypts data by asymetric key.
 *
 * Decrypts data by asymetric key that is saved in a file.
 *
 *
 * @param   acid ID of certificate associated with the key. 
 * @param   data Data to decrypt.
 * @return  decrypted MsgField 
 * @author  Marek
 * @see     Cipherer
 */
MsgField *
Majordomo::DecryptByACPriv(const ACID acid, const MsgField *data)
{
  GMessage *acPrivKey;
  MsgField *dData;
  Char *tmp;
 
DBG;
    //disable warnings
  ID tmpACID;
  tmpACID = acid;

DBG;
    //get ACPrivKey
  acPrivKey = new GMessage();
  if(acPrivKey == NULL)
    return NULL;
DBG;
  if(acPrivKey->LoadFromFile(tmp = six->GetPath(PRIVATE_KEY_FILE)) != OK){
    DELETE(acPrivKey);
    return NULL;
  }

  
DBG;
  if(six->cipherer->DecryptWithPrivateKey(acPrivKey, (MsgField *)data, &dData) != OK)
    WriteString(llDebug, __FILE__":%d:Cannot decrypt by AC private key", __LINE__);

DBG;
  return dData;
}


/**
 * Returns Client's pubKey.
 *
 * Loads Client's pubKey from file.
 *
 *
 * @return  client's pubKey 
 * @author  Marek
 * @see     
 */
GMessage *
Majordomo::LoadPublicKey()
{
  Char *tmp;
  GMessage *acPubKey = new GMessage();
  if(acPubKey == NULL)
    return NULL;
  
  if(acPubKey->LoadFromFile(tmp = six->GetPath(PUBLIC_KEY_FILE)) != OK){
    DELETE(acPubKey);
    return NULL;
  }
  DELETE(tmp);

  return acPubKey;
}


/**
 * Returns Client's privKey.
 *
 * Loads Client's privKey from file.
 *
 *
 * @return  client's privKey 
 * @author  Marek
 * @see     
 */
GMessage *
Majordomo::LoadPrivateKey()
{
  Char *tmp;
  GMessage *acPrivKey = new GMessage();
  if(acPrivKey == NULL)
    return NULL;
  
  if(acPrivKey->LoadFromFile(tmp = six->GetPath(PRIVATE_KEY_FILE)) != OK){
    DELETE(acPrivKey);
    return NULL;
  }
  DELETE(tmp);

  return acPrivKey;
}


