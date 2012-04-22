  // has to be the FIRST include !
#include "./../H/common.h"

  // contains just #include of "rsaeuro.h"
#include "./../RSAEuro/H/rsaref.h"
#include "./../RSAEuro/H/rsa.h"

#include "./H/cipherer.h"
#include "../H/labels.h"

/**
 * Encrypts symetric key with asymetric.
 *
 * Due to efficiency and implementation, we use sealing, so this
 * method encrypts a symetric key (of GMessage type) with public key.
 *
 * @param   pubKey Public key.
 * @param   pSymKey Symetric key to process.
 * @param   eSymKey Encrypted symetric key to process.
 * @return  KO if encryption failes, OK otherwise.
 * @author  Pechy
 * @see     other cryptography methods.
 */
Err
Cipherer::EncryptKeyWithPublicKey(GMessage *pubKey,
                                  GMessage *pSymKey,
                                  MsgField **eSymKey)
{
  //int oldOpts = SetDebugOptions(llWarning | llError | llFatalError);
  int oldOpts = SetDebugOptions(llDebug);

  Size pSymKeyStreamLen;
  char *pSymKeyStream = NULL;
  pSymKeyStream = (char *) pSymKey->StoreToBytes(&pSymKeyStreamLen);
  WriteString(llDebug, __FILE__ ":%d:Plain symetric key info len is %lu.", 
              __LINE__, pSymKeyStreamLen);

  char *tmpDataOut = NULL;
  Size tmpDataOutLen;
  if (EncryptBlockWithPublicKey(pubKey, pSymKeyStream,
                                pSymKeyStreamLen, &tmpDataOut,
				&tmpDataOutLen) == KO) {
    DELETE(pSymKeyStream);
    SetDebugOptions(oldOpts);
    return(KO);
  }
  else {
    DELETE(pSymKeyStream);
    (*eSymKey) = new BytesMsgField();

    if ((*eSymKey)->SetAsBytes(tmpDataOut, tmpDataOutLen) == KO) {
      WriteString(llWarning, __FILE__ ":%d:LoadFromBytes failed, "
		  "tmpDataOutLen was %lu.", __LINE__, tmpDataOutLen);
      DELETE(*eSymKey);
      SetDebugOptions(oldOpts);
      return KO;
    }
    else {
      DELETE(tmpDataOut);
      SetDebugOptions(oldOpts);
      return OK;
    }
  }
}

/**
 * Decrypts symetric key with private key.
 *
 * Due to efficiency and implementation, we use sealing, so this
 * method decrypts a symetric key (of GMessage type) with private key.
 *
 * @param   privKey Private key.
 * @param   eSymKey Symetric key to process.
 * @param   pSymKey Decrypted symetric key to process.
 * @return  KO if encryption failes, OK otherwise.
 * @author  Pechy
 * @see     other cryptography methods.
 */
Err
Cipherer::DecryptKeyWithPrivateKey(GMessage *privKey, 
                                   MsgField *eSymKey, 
                                   GMessage **pSymKey)
{
  //int oldOpts = SetDebugOptions(llWarning | llError | llFatalError);
  int oldOpts = SetDebugOptions(llDebug);

  Size eSymKeyStreamLen;
  char *eSymKeyStream = NULL;
  eSymKeyStream = (char *) eSymKey->GetAsBytes(&eSymKeyStreamLen);
  WriteString(llDebug, __FILE__ ":%d:Encrypted symetric key info len is %lu.", 
              __LINE__, eSymKeyStreamLen);

  char *tmpDataOut = NULL;
  Size tmpDataOutLen;
  if (DecryptBlockWithPrivateKey(privKey, eSymKeyStream,
                                 eSymKeyStreamLen, &tmpDataOut,
				 &tmpDataOutLen) == KO) {
    DELETE(eSymKeyStream);
    SetDebugOptions(oldOpts);
    return(KO);
  }
  else {
    DELETE(eSymKeyStream);
    (*pSymKey) = new GMessage();

    if ((*pSymKey)->LoadFromBytes(tmpDataOut, tmpDataOutLen) == KO) {
      WriteString(llWarning, __FILE__ ":%d:LoadFromBytes failed, "
		  "tmpDataOutLen was %lu.", __LINE__, tmpDataOutLen);
      DELETE(*pSymKey); DELETE(tmpDataOut);
      SetDebugOptions(oldOpts);
      return KO;
    }
    else {
      DELETE(tmpDataOut);
      SetDebugOptions(oldOpts);
      return OK;
    }
  }
}
