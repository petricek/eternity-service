  // has to be the FIRST include !
#include "./../H/common.h"

  // contains just #include of "rsaeuro.h"
#include "./../RSAEuro/H/rsaref.h"
#include "./../RSAEuro/H/rsa.h"

#include "./H/cipherer.h"
#include "../H/labels.h"

/**
 * Seals given data.
 *
 * Seals given data into a digital envelope, using (for now) only RSA,
 * DES and MD5 algorithms, but in parameters, there could be given
 * customization.
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
Cipherer::Seal(GMessage *privateKeyInfo, GMessage *publicKeyInfo,
                   MsgField *dataIn, MsgField **dataOut,
		   GMessage *parameters = NULL)
{
    // to shut up a warning message
  parameters = NULL;

  unsigned char *content;
  unsigned int contentLen;
  Size tmpSize;
  content = (unsigned char *) dataIn->GetAsBytes(&tmpSize);
    // we could check sanity here...
  contentLen = (unsigned int) tmpSize;

  int digestAlgorithm;
    // no choice but DA_MD5 for now
  digestAlgorithm = DA_MD5;

  R_RSA_PUBLIC_KEY *publicKey;
  publicKey = (R_RSA_PUBLIC_KEY *) publicKeyInfo->GetAsBytes(GM_PUBLIC_KEY);

  R_RSA_PRIVATE_KEY *privateKey;
  privateKey = (R_RSA_PRIVATE_KEY *) privateKeyInfo->GetAsBytes(GM_PRIVATE_KEY);

  R_RANDOM_STRUCT randomStruct;
  R_RandomCreate(&randomStruct);

    // ENCRYPTED_CONTENT_LEN is a macro from rsaeuro.h
  unsigned char *encryptedContent;
  encryptedContent = new unsigned char[ENCRYPTED_CONTENT_LEN(contentLen)];
  unsigned int encryptedContentLen;

    // MAX_PEM_ENCRYPTED_KEY_LEN is a macro from rsaeuro.h
  unsigned char *encryptedKey;
  encryptedKey = new unsigned char[MAX_PEM_ENCRYPTED_KEY_LEN];
  unsigned int encryptedKeyLen;

    // MAX_PEM_ENCRYPTED_SIGNATURE_LEN is a macro from rsaeuro.h
  unsigned char *encryptedSignature;
  encryptedSignature = new unsigned char[MAX_PEM_ENCRYPTED_SIGNATURE_LEN];
  unsigned int encryptedSignatureLen;

  unsigned char iv[8];

  int result;
  if ((result = R_SealPEMBlock(encryptedContent, &encryptedContentLen, 
                               encryptedKey, &encryptedKeyLen, 
			       encryptedSignature, &encryptedSignatureLen,
			       iv, content, contentLen, digestAlgorithm, 
			       publicKey, privateKey, &randomStruct)) != 0) {

    WriteString(llError, __FILE__ ":%d:RSA SealBlock() function failed with "
                "result %d.", __LINE__, result);
    return(KO);
  }

  DELETE(publicKey); DELETE(privateKey); DELETE(content);

  WriteString(llDebug, __FILE__ ":%d:encryptedContentLen: %d", __LINE__,
              encryptedContentLen);
  WriteString(llDebug, __FILE__ ":%d:ENCRYPTED_CONTENT_LEN: %d", __LINE__,
              ENCRYPTED_CONTENT_LEN(contentLen));

  WriteString(llDebug, __FILE__ ":%d:encryptedSignatureLen: %d", __LINE__,
              encryptedSignatureLen);
  WriteString(llDebug, __FILE__ ":%d:MAX_PEM_ENCRYPTED_SIGNATURE_LEN: %d", 
                       __LINE__, MAX_PEM_ENCRYPTED_SIGNATURE_LEN);


  encryptedSignature[encryptedSignatureLen] = '\0';
  WriteString(llDebug, __FILE__ ":%d:Signature: %s", 
                       __LINE__, encryptedSignature);


  WriteString(llDebug, __FILE__ ":%d:encryptedKeyLen: %d", 
              __LINE__, encryptedKeyLen);
  WriteString(llDebug, __FILE__ ":%d:MAX_PEM_ENCRYPTED_KEY_LEN: %d", 
              __LINE__, MAX_PEM_ENCRYPTED_KEY_LEN);

  GMessage *dataOutGM = new GMessage();
  dataOutGM->SetAsBytes(GM_ENCRYPTED_CONTENT, (void *) encryptedContent, 
                        *((Size *) &encryptedContentLen));
  WriteString(llDebug, __FILE__ ":%d:", __LINE__);
  DELETE(encryptedContent);

  dataOutGM->SetAsBytes(GM_ENCRYPTED_KEY, (void *) encryptedKey, 
                        *((Size *) &encryptedKeyLen));
  WriteString(llDebug, __FILE__ ":%d:", __LINE__);
  DELETE(encryptedKey);

  dataOutGM->SetAsBytes(GM_ENCRYPTED_SIGNATURE, (void *) encryptedSignature, 
                        *((Size *) &encryptedSignatureLen));

  WriteString(llDebug, __FILE__ ":%d:", __LINE__);

  DELETE(encryptedSignature);
  dataOutGM->SetAsBytes(GM_IV, (void *) iv, 8); 

  WriteString(llDebug, __FILE__ ":%d:", __LINE__);

    // create BytesMsgField that we are to return
  char *tmpChar = (char *) dataOutGM->StoreToBytes(&tmpSize);
  (*dataOut) = new BytesMsgField();
  (*dataOut)->SetAsBytes((void *) tmpChar, tmpSize);
  DELETE(tmpChar);


  WriteString(llDebug, __FILE__ ":%d:", __LINE__);

  R_RandomFinal(&randomStruct);
  return OK;
}

/**
 * Opens sealed data.
 *
 * Verifies whether given data is not tempered and returns it. For
 * now, we use only RSA, DES and MD5 algorithms, but we can insert
 * parameters with additional information which algs to use (in Seal()
 * method).
 *
 * @param   privKeyGM Private key used for data decryption.
 * @param   publicKeyInfo Public key used to data verification.
 * @param   dataIn Data to process.
 * @param   dataOut Processed data.
 * @return  KO if opening fails, OK otherwise.
 * @author  Pechy
 * @see     other cryptographic methods
 */
Err 
Cipherer::OpenSealed(GMessage *privateKeyInfo, GMessage *publicKeyInfo,
                   MsgField *dataIn, MsgField **dataOut)
{
  Size tmpSize; 
  char *tmpChar = (char *) dataIn->GetAsBytes(&tmpSize);

  GMessage *dataInGM = new GMessage();
  dataInGM->LoadFromBytes(tmpChar, tmpSize);

  unsigned char *encryptedContent;
  unsigned int encryptedContentLen;
  encryptedContent = (unsigned char *) 
    dataInGM->GetAsBytes(GM_ENCRYPTED_CONTENT, &tmpSize);
  encryptedContentLen = (unsigned int) tmpSize;

  unsigned char *encryptedKey;
  unsigned int encryptedKeyLen;
  encryptedKey = (unsigned char *) dataInGM->GetAsBytes(GM_ENCRYPTED_KEY, 
                                                         &tmpSize);
  encryptedKeyLen = (unsigned int) tmpSize;

  unsigned char *encryptedSignature;
  unsigned int encryptedSignatureLen;
  encryptedSignature = (unsigned char *) 
    dataInGM->GetAsBytes(GM_ENCRYPTED_SIGNATURE, &tmpSize);
  encryptedSignatureLen = (unsigned int) tmpSize;
  unsigned char *tmpUChar = new unsigned char[encryptedSignatureLen + 1];
  memcpy(tmpUChar, encryptedSignature, encryptedSignatureLen);
  tmpUChar[encryptedSignatureLen] = '\0';
  WriteString(llDebug, __FILE__ ":%d:Signature: %s", 
                       __LINE__, tmpUChar);
  DELETE(tmpUChar);

  unsigned char *iv;
  iv = (unsigned char *) dataInGM->GetAsBytes(GM_IV);

    // in Seal(), we use DA_MD5 only for now
  int digestAlgorithm = DA_MD5;
  R_RSA_PRIVATE_KEY *privateKey;
  privateKey =(R_RSA_PRIVATE_KEY *) 
    privateKeyInfo->GetAsBytes(GM_PRIVATE_KEY, &tmpSize);

  WriteString(llDebug, __FILE__ ":%d:Private key len is [%ld].", 
              __LINE__, tmpSize);

  R_RSA_PUBLIC_KEY *publicKey;
  publicKey = (R_RSA_PUBLIC_KEY *) publicKeyInfo->GetAsBytes(GM_PUBLIC_KEY);



  WriteString(llDebug, __FILE__ ":%d:encryptedContentLen: %d", __LINE__,
              encryptedContentLen);
  WriteString(llDebug, __FILE__ ":%d:DECRYPTED_CONTENT_LEN: %d", __LINE__,
              DECRYPTED_CONTENT_LEN(encryptedContentLen));

  WriteString(llDebug, __FILE__ ":%d:encryptedSignatureLen: %d", __LINE__,
              encryptedSignatureLen);
  WriteString(llDebug, __FILE__ ":%d:MAX_PEM_ENCRYPTED_SIGNATURE_LEN: %d", 
                       __LINE__, MAX_PEM_ENCRYPTED_SIGNATURE_LEN);

  WriteString(llDebug, __FILE__ ":%d:encryptedKeyLen: %d", 
              __LINE__, encryptedKeyLen);
  WriteString(llDebug, __FILE__ ":%d:MAX_PEM_ENCRYPTED_KEY_LEN: %d", 
              __LINE__, MAX_PEM_ENCRYPTED_KEY_LEN);


    // DECRYPTED_CONTENT_LEN is a macro from rsaeuro.h
  unsigned char *content;
  content = new unsigned char[DECRYPTED_CONTENT_LEN(encryptedContentLen)];
  unsigned int contentLen;

  int result;
  if ( (result = R_OpenPEMBlock(content, &contentLen, encryptedContent,
                                encryptedContentLen, encryptedKey, 
				encryptedKeyLen,encryptedSignature, 
				encryptedSignatureLen, iv, digestAlgorithm, 
				privateKey, publicKey)) != 0) {

    WriteString(llError, __FILE__ ":%d:RSA OpenBlock() function failed with "
                "result %d.", __LINE__, result);
    WriteString(llDebug, __FILE__ ":%d:RSA priv key len [%d], bit len [%d].", 
                __LINE__, sizeof(*privateKey), privateKey->bits);
    
    return(KO);
  }

  DELETE(encryptedContent); DELETE(encryptedKey); DELETE(encryptedSignature);
  DELETE(privateKey); DELETE(publicKey); DELETE(iv);

  (*dataOut) = new BytesMsgField();
  (*dataOut)->SetAsBytes((void *) content, (Size) contentLen);
  DELETE(content);

  return OK;
}
