#include "./H/majordomo.h"


/**
 * Decrypts data by private key.
 *
 * Overriden SixMajordomo method realized by a call to TCBWrapper.
 *
 *
 * @param   acid ID of access certificate which public key was used to encrypt data.
 * @param   eData encrypted data
 * @return  MsgField *
 * @author  Pavel
 * @see     TCBWrapper, SixMajordomo
 */
MsgField *
Majordomo::DecryptByACPriv(const ACID acid, const MsgField *eData)
{
  return ((Eso *) six)->tcbWrapper->DecryptByACPriv(acid, eData);
}

