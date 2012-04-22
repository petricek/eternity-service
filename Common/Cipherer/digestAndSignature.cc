  // has to be the FIRST include !
#include "./../H/common.h"

#include "../H/labels.h"

#include "./../RSAEuro/H/rsaref.h"

#include "./H/cipherer.h"

/**
 * Creates a message digest over given data.
 *
 * Creates a message digest over given data. Inside, a RSAEuro
 * function is used. We use only MD5 for now, but type of digest can
 * be specified in parameters.
 *
 * @param   dataToDigest Data to digest.
 * @param   dig Created digest.
 * @param   parameters Parameters - which digest to use.
 * @return  KO if digest creation fails, OK otherwise.
 * @author  Pechy
 * @see     other cryptographic methods
 */
Err 
Cipherer::MessageDigest(MsgField *dataToDigest, MsgField **dig, 
                            GMessage *parameters = NULL)
{
  parameters = NULL;

  unsigned char *digest = new unsigned char[MAX_DIGEST_LEN];
  unsigned int digestLen;

  Size tmpSize;
  unsigned char *block = (unsigned char *) dataToDigest->GetAsBytes(&tmpSize);
  unsigned int blockLen = (unsigned int) tmpSize;

    // now we only support MD5
  int digestAlgorithm = DA_MD5;

  int result;
  if ((result = R_DigestBlock(digest, &digestLen, block, blockLen, 
                              digestAlgorithm)) != IDOK) {
    WriteString(llError, __FILE__ ":%d:RSA R_DigestBlock() function "
                "failed with result %d.", __LINE__, result);
  }
  else {
    (*dig) = new BytesMsgField();
    (*dig)->SetAsBytes((void *) digest, (Size) digestLen);

    WriteString(llDebug, __FILE__ ":%d:RSA R_DigestBlock() produced %u bytes "
                "long digest.", __LINE__, digestLen);
  }

  DELETE(block);
  DELETE(digest);

  if (result == IDOK)
    return OK;
  else
    return KO;
}

/**
 * Signs given data.
 *
 * Signs given data using message digest. For now, only MD5 is
 * supported. Asymetric algorithm is RSA for now. In parameters, there
 * could be given customization.
 *
 * @param   privKeyGM Private key used to sign the data.
 * @param   dataToSign Data to sign.
 * @param   sig Created signature.
 * @param   parameters Parameters - which algorithms to use.
 * @return  KO if sealing fails, OK otherwise.
 * @author  Pechy
 * @see     other cryptographic methods
 */
Err
Cipherer::Sign(GMessage *privKeyGM, MsgField *dataToSign, 
                   MsgField **sig, GMessage *parameters = NULL)
{
  parameters = NULL;

  unsigned char *signature = new unsigned char[MAX_SIGNATURE_LEN];
  unsigned int signatureLen; 

  Size tmpSize;
  unsigned char *block = (unsigned char *) dataToSign->GetAsBytes(&tmpSize);
  unsigned int blockLen = (unsigned int) tmpSize;

    // now we only support MD5
  int digestAlgorithm = DA_MD5; 
  
  R_RSA_PRIVATE_KEY *privateKey;
  privateKey = (R_RSA_PRIVATE_KEY *)privKeyGM->GetAsBytes(GM_PRIVATE_KEY);

  int result;
  if ((result = R_SignBlock(signature, &signatureLen, block, blockLen,
                            digestAlgorithm, privateKey)) != IDOK) {

    WriteString(llError, __FILE__ ":%d:RSA R_SignBlock() function "
                "failed with result %d.", __LINE__, result);
  }
  else {
    (*sig) = new BytesMsgField();
    (*sig)->SetAsBytes((void *) signature, (Size) signatureLen);

    WriteString(llDebug, __FILE__ ":%d:RSA R_SignBlock() produced %u bytes "
                "long signature.", __LINE__, signatureLen); 
  }
                              
  DELETE(privateKey);
  DELETE(block);
  DELETE(signature);

  if (result == IDOK)
    return OK;
  else
    return KO;
}

/**
 * Verifies signed data.
 *
 * Verifies given data, using (for now) only RSA and MD5 algorithms,
 * but in parameters, there could be given customization.
 *
 * @param   pubKeyGM Public key to verify signature.
 * @param   dataToVerify Data to verify.
 * @param   sig Given signature.
 * @param   parameters Parameters - which algorithms to use.
 * @return  KO if verification fails, OK otherwise.
 * @author  Pechy
 * @see     other cryptographic methods
 */
Err 
Cipherer::VerifySignature(GMessage *pubKeyGM, MsgField *dataToVerify, 
                              MsgField *sig, GMessage *parameters = NULL)
{
  parameters = NULL;

  Size tmpSize; 
  unsigned char *signature = (unsigned char *) sig->GetAsBytes(&tmpSize);
  unsigned int signatureLen = (unsigned int) tmpSize;

  unsigned char *block = (unsigned char *) dataToVerify->GetAsBytes(&tmpSize);
  unsigned int blockLen = (unsigned int) tmpSize;

    // now we only support MD5
  int digestAlgorithm = DA_MD5;
 
  R_RSA_PUBLIC_KEY *publicKey;
  publicKey = (R_RSA_PUBLIC_KEY *) pubKeyGM->GetAsBytes(GM_PUBLIC_KEY);

  int result;
  if ((result = R_VerifyBlockSignature(block, blockLen, signature, 
                                       signatureLen, digestAlgorithm, 
				       publicKey)) != IDOK) {
    WriteString(llError, __FILE__ ":%d:RSA R_VerifyBlockSignature() function "
                "failed with result %d.", __LINE__, result);
  }

  DELETE(publicKey);
  DELETE(block);
  DELETE(signature);

  if (result == IDOK)
    return OK;
  else
    return KO;
}
