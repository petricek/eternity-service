#include "./H/majordomo.h"

/**
 * Decrypts data with access private key.
 *
 * Identifies proper private key by access certificate ID and then
 * decrypts given data. Decrypted data are returned.
 *
 *
 * @param   acid        ID of access certificate which private key is
 *                      to be used for decrypting process
 * @param   eData       Encrypted data
 * @return  co
 * @author  Marek
 * @see     SixMajordomo, KeyManager, Cipherer
 */
MsgField *
Majordomo::DecryptByACPriv(const ACID acid, const MsgField *eData)
{
  GMessage *privKey = ((Bank *) six)->keyMgr->GetPrivKeyByACID(acid);
  if(privKey == NULL)
    return NULL;
    
  MsgField *dData;
  ((Bank *)six)->cipherer->DecryptWithPrivateKey(privKey, (MsgField *)eData, &dData);
  
  if(dData == NULL)
    return NULL;
    
  return dData;
}

