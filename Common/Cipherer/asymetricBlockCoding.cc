  // has to be the FIRST include !
#include "./../H/common.h"

  // contains just #include of "rsaeuro.h"
#include "./../RSAEuro/H/rsaref.h"
#include "./../RSAEuro/H/rsa.h"

#include "./H/cipherer.h"
#include "../H/labels.h"

/**
 * Decrypts one PKCS1 block by public key.
 *
 * Decrypts one PKCS1 block by public key, the block is defined by
 * PKCS1 standard. Inside we use RSAEuro function.
 *
 * @param   gmPublicKey Public key.
 * @param   dataIn Input data octet stream.
 * @param   dataInLen Length of input data octet stream.
 * @param   dataOut Output data octet stream.
 * @param   dataOutLen Length of output data octet stream.
 * @return  KO if decryption fails, OK otherwise.
 * @author  Pechy
 * @see     other encryption methods
 */
Err
Cipherer::DecryptBlockWithPublicKey(GMessage *gmPublicKey, char *dataIn,
                                    Size dataInLen, char **dataOut,
                                    Size *dataOutLen)
{
  //int oldOpts = SetDebugOptions(llWarning | llError | llFatalError);
  int oldOpts = SetDebugOptions(llDebug);

  R_RSA_PUBLIC_KEY *publicKey = NULL;
  publicKey = (R_RSA_PUBLIC_KEY *) gmPublicKey->GetAsBytes(GM_PUBLIC_KEY);

    // + 1 is required by RSARef
  (*dataOut) = new char[MAX_RSA_MODULUS_LEN + 1];

  WriteString(llDebug, __FILE__ ":%d:dataInLen is %lu.", __LINE__, dataInLen);

  int result;
  if ( (result = RSAPublicDecrypt((unsigned char *) *dataOut, 
                                  (unsigned int *) dataOutLen, 
                                  (unsigned char *) dataIn, 
				  (unsigned int) dataInLen, publicKey))
        != IDOK) {

    WriteString(llWarning, __FILE__ ":%d:RSA key encryption failed, result "
                "is %d.", __LINE__, result);
    DELETE(*dataOut); DELETE(publicKey);
    return KO;
  }
  else {
    WriteString(llDebug, __FILE__ ":%d:dataOutLen is %lu", __LINE__,
                (Size) dataOutLen);
    DELETE(publicKey);
  }

  SetDebugOptions(oldOpts);
  return OK;
}

/**
 * Decrypts one PKCS1 block by private key.
 *
 * Decrypts one PKCS1 block by private key, the block is defined by
 * PKCS1 standard. Inside we use RSAEuro function.
 *
 * @param   gmPrivateKey Private key.
 * @param   dataIn Input data octet stream.
 * @param   dataInLen Length of input data octet stream.
 * @param   dataOut Output data octet stream.
 * @param   dataOutLen Length of output data octet stream.
 * @return  KO if decryption fails, OK otherwise.
 * @author  Pechy
 * @see     other encryption methods
 */
Err
Cipherer::DecryptBlockWithPrivateKey(GMessage *gmPrivateKey, char *dataIn,
                                    Size dataInLen, char **dataOut,
                                    Size *dataOutLen)
{
  int oldOpts = SetDebugOptions(llWarning | llError | llFatalError);

  R_RSA_PRIVATE_KEY *privateKey = NULL;
  privateKey = (R_RSA_PRIVATE_KEY *) gmPrivateKey->GetAsBytes(GM_PRIVATE_KEY);

    // + 1 is required by RSARef
  (*dataOut) = new char[MAX_RSA_MODULUS_LEN + 1];
  WriteString(llDebug, __FILE__ ":%d:dataInLen is %lu", __LINE__,
	      (Size) dataInLen);

  int result;
  if ( (result = RSAPrivateDecrypt((unsigned char *) *dataOut,
                                   (unsigned int *) dataOutLen, 
                                   (unsigned char *) dataIn,
				   (unsigned int) dataInLen, 
				   privateKey)) 
        != IDOK) {

    WriteString(llWarning, __FILE__ ":%d:RSA key encryption failed, result "
                "is %d.", __LINE__, result);
    DELETE(*dataOut); DELETE(privateKey);
    return KO;
  }
  else {
    WriteString(llDebug, __FILE__ ":%d:dataOutLen is %lu", __LINE__,
                (Size) dataOutLen);
    DELETE(privateKey);
  }

  SetDebugOptions(oldOpts);
  return OK;
}

/**
 * Encrypts one PKCS1 block by public key.
 *
 * Encrypts one PKCS1 block by public key, the block is defined by
 * PKCS1 standard. Inside we use RSAEuro function.
 *
 * @param   gmPublicKey Public key.
 * @param   dataIn Input data octet stream.
 * @param   dataInLen Length of input data octet stream.
 * @param   dataOut Output data octet stream.
 * @param   dataOutLen Length of output data octet stream.
 * @return  KO if encryption fails, OK otherwise.
 * @author  Pechy
 * @see     other encryption methods
 */
Err
Cipherer::EncryptBlockWithPublicKey(GMessage *gmPublicKey, char *dataIn,
                                    Size dataInLen, char **dataOut,
                                    Size *dataOutLen)
{
  int oldOpts = SetDebugOptions(llWarning | llError | llFatalError);

    // PKCS1 requires some randomness for public key encryption
  R_RANDOM_STRUCT randomStruct;
  R_RandomCreate(&randomStruct);

  R_RSA_PUBLIC_KEY *publicKey = NULL;
  publicKey = (R_RSA_PUBLIC_KEY *) gmPublicKey->GetAsBytes(GM_PUBLIC_KEY);

  WriteString(llDebug, __FILE__ ":%d:dataInLen is %lu", __LINE__,
	      (Size) dataInLen);

    // check whether we can encrypt the key with RSAPublicEncrypt()
  Size maxKeyLen;
  maxKeyLen = ((publicKey->bits + 7) / BITS_IN_BYTE) - RSAREF_RESERVED_BYTES;
  if (dataInLen >= maxKeyLen) {
    WriteString(llDebug, __FILE__ ":%d:Key to encrypt too large (%lu), max "
              "enabled len is %lu.", __LINE__, dataInLen, maxKeyLen);
    DELETE(publicKey);
    return KO;
  }

    // + 1 is required by RSARef
  (*dataOut) = new char[MAX_RSA_MODULUS_LEN + 1];

  int result;
  if ( (result = RSAPublicEncrypt((unsigned char *) *dataOut,
                                  (unsigned int *) dataOutLen,
                                  (unsigned char *) dataIn,
				  (unsigned int) dataInLen,
				  publicKey,
                                  &randomStruct)) != IDOK) {

    WriteString(llError, __FILE__ ":%d:RSA key encryption failed, result "
                "is %d.", __LINE__, result);
    DELETE(*dataOut); DELETE(publicKey);
    return KO;
  }
  else {
    WriteString(llDebug, __FILE__ ":%d:dataOutLen is %lu", __LINE__,
                (Size) dataOutLen);
    DELETE(publicKey);
  }

    // be secret, buddy
  R_RandomFinal(&randomStruct);

  SetDebugOptions(oldOpts);
  return OK;
}

/**
 * Encrypts one PKCS1 block by private key.
 *
 * Encrypts one PKCS1 block by private key, the block is defined by
 * PKCS1 standard. Inside we use RSAEuro function.
 *
 * @param   gmPrivateKey Private key.
 * @param   dataIn Input data octet stream.
 * @param   dataInLen Length of input data octet stream.
 * @param   dataOut Output data octet stream.
 * @param   dataOutLen Length of output data octet stream.
 * @return  KO if encryption fails, OK otherwise.
 * @author  Pechy
 * @see     other encryption methods
 */
Err
Cipherer::EncryptBlockWithPrivateKey(GMessage *gmPrivateKey, char *dataIn,
                                    Size dataInLen, char **dataOut,
                                    Size *dataOutLen)
{
  int oldOpts = SetDebugOptions(llWarning | llError | llFatalError);

  R_RSA_PRIVATE_KEY *privateKey = NULL;
  privateKey = (R_RSA_PRIVATE_KEY *) gmPrivateKey->GetAsBytes(GM_PRIVATE_KEY);

    // check whether we can encrypt the key with RSAPrivateEncrypt()
  Size maxKeyLen;
  maxKeyLen = ((privateKey->bits + 7) / BITS_IN_BYTE) - RSAREF_RESERVED_BYTES;
  if (dataInLen >= maxKeyLen) {
    WriteString(llDebug, __FILE__ ":%d:Key to encrypt too large (%lu), max "
              "enabled len is %lu.", __LINE__, dataInLen, maxKeyLen);
    DELETE(privateKey);
    return KO;
  }

  WriteString(llDebug, __FILE__ ":%d:dataInLen is %lu", __LINE__,
	      (Size) dataInLen);

    // + 1 is required by RSARef
  (*dataOut) = new char[MAX_RSA_MODULUS_LEN + 1];

  int result;
  if ( (result = RSAPrivateEncrypt((unsigned char *) *dataOut,
                                   (unsigned int *) dataOutLen,
                                   (unsigned char *) dataIn,
                                   (unsigned int) dataInLen,
				   privateKey)) 
        != IDOK) {

    WriteString(llError, __FILE__ ":%d:RSA key encryption failed, result "
                "is %d.", __LINE__, result);
    DELETE(*dataOut); DELETE(privateKey);
    return KO;
  }
  else {
    WriteString(llDebug, __FILE__ ":%d:dataOutLen is %lu", __LINE__,
                (Size) dataOutLen);
    DELETE(privateKey);
  }

  SetDebugOptions(oldOpts);
  return OK;
}
