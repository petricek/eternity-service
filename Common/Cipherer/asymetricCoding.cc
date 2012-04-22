  // has to be the FIRST include !
#include "./../H/common.h"

  // contains just #include of "rsaeuro.h"
#include "./../RSAEuro/H/rsaref.h"
#include "./../RSAEuro/H/rsa.h"
#include "./../Utils/H/mem.h"

#include "./H/cipherer.h"
#include "../H/labels.h"

/**
 * Encrypts data with private key.
 *
 * Encrypts data with private key. The data is in msgField, output is
 * inserted in a msgField too. The msgField is freshly allocated.
 *
 * @param   privateKey Private key.
 * @param   pData Plain data to process.
 * @param   eData Processed data.
 * @return  KO if an action fails, OK otherwise.
 * @author  Pechy
 * @see     other cryptography methods
 */
Err
Cipherer::EncryptWithPrivateKey(GMessage * privateKey,
                                MsgField * pData,
                                MsgField ** eData)
{
  //int oldOpts = SetDebugOptions(llWarning | llError | llFatalError);
  int oldOpts = SetDebugOptions(llDebug);

    // encrypt pData with a just generated symetric key
  GMessage *pSymKey = GenerateSymetricKey();
  pSymKey->DebugHeaders(logFile, "Generated pSymKey symetric key.");

  Size pTmpDataStreamLen;
  char *pTmpDataStream = NULL;
  if ((pTmpDataStream = (char *) pData->GetAsBytes(&pTmpDataStreamLen)) 
     == NULL) {
    DELETE(pSymKey);
    SetDebugOptions(oldOpts);
    return KO;
  }

  char *streamWithSizeHeader = AddSizeHeaderAsDataSize(pTmpDataStream,
                                                       pTmpDataStreamLen);
  GMessage *tmp = new GMessage();
  tmp->SetAsBytes("Fak", streamWithSizeHeader, pTmpDataStreamLen 
                  + sizeof(Size));
  tmp->DebugHeaders(logFile, "After AddSizeHeaderAsDataSize");

  Size eTmpDataOutLen;
  char *eTmpDataOut = NULL;
  if (ProcessWithSymetricKey_RawVersion(pSymKey, streamWithSizeHeader,
                                        pTmpDataStreamLen + sizeof(Size),
					&eTmpDataOut, &eTmpDataOutLen,
					ENCRYPT_FLAG) == KO) {

    DELETE(pTmpDataStream); DELETE(streamWithSizeHeader);
    DELETE(pSymKey);
    SetDebugOptions(oldOpts);
    return KO;
  }
  else {
    WriteString(llDebug, __FILE__ ":%d:eTmpDataOutLen is %lu.", __LINE__,
                eTmpDataOutLen);
    DELETE(pTmpDataStream);
    DELETE(streamWithSizeHeader);
  }

  Size pSymKeyStreamLen;
  char *pSymKeyStream = (char *) pSymKey->StoreToBytes(&pSymKeyStreamLen);
  DELETE(pSymKey);

  Size eTmpKeyStreamLen;
  char *eTmpKeyStream = NULL;
  if (EncryptBlockWithPrivateKey(privateKey, pSymKeyStream, pSymKeyStreamLen,
                                 &eTmpKeyStream, &eTmpKeyStreamLen) == KO) {
    DELETE(pSymKeyStream);
    SetDebugOptions(oldOpts);
    return KO;
  }
  else {
    WriteString(llDebug, __FILE__ ":%d:eTmpKeyStreamLen is %lu.", 
                __LINE__, eTmpKeyStreamLen);
    DELETE(pSymKeyStream);
  }

  char *wholeStream = NULL;
  Size wholeStreamLen;
  assert(eTmpKeyStreamLen == RSA_BLOCK_LEN);
  wholeStreamLen = mixMemCat(eTmpKeyStream, eTmpKeyStreamLen, eTmpDataOut, 
                             eTmpDataOutLen, &wholeStream);
  DELETE(eTmpKeyStream); DELETE(eTmpDataOut);

  if (wholeStreamLen % SYM_KEY_LEN != 0) {
    WriteString(llWarning, __FILE__ ":%d:wholeStreamLen not 8 multiple, "
                "but should be.", __LINE__);
    DELETE(wholeStream);
    SetDebugOptions(oldOpts);
    return KO;
  }
  else {
      // insert the bytestream into the eData BytesMsgField
    (*eData) = new BytesMsgField();
    (*eData)->SetAsBytes((void *) wholeStream, wholeStreamLen);
    WriteString(llDebug, __FILE__ ":%d:wholeStreamLen is %lu.", __LINE__,
                wholeStreamLen);
    DELETE(wholeStream);
    SetDebugOptions(oldOpts);
    return OK;
  }
}

/**
 * Encrypt data with public key.
 *
 * This method is not too clean, we count on the fact that 
 * RSAPublicEncrypt() function from RSAEuro encodes one chunk (ie. stream
 * with current modulus in length) and symetric key's gMessage is short.
 * <p>
 * Currently we use 512 bits long modulus, so we can have symetric key's
 * gMessages 64 bytes long (ie. gMessage converted to an octet stream can
 * be maximally 64 bytes in length).
 *
 * @param   parametr
 * @return  Error code OK - KO
 * @author  Pechy
 * @see     EncryptWithPrivateKey()
 */
Err
Cipherer::EncryptWithPublicKey(GMessage * publicKey, 
                               MsgField * pData,
                               MsgField ** eData)
{
  //int oldOpts = SetDebugOptions(llWarning | llError | llFatalError);
  int oldOpts = SetDebugOptions(llDebug);

    // encrypt pData with a just generated symetric key
  GMessage *pSymKey = GenerateSymetricKey();
  pSymKey->DebugHeaders(logFile, "Generated pSymKey symetric key.");

  GMessage *tmpFak = new GMessage();
  tmpFak->SetAsMsgField("In EncryptWithPublicKey", pData);
  tmpFak->DebugHeaders(logFile, "XXX in EncryptWithPublicKey");
  DELETE(tmpFak);

  Size pTmpDataStreamLen;
  char *pTmpDataStream = NULL;
  if ((pTmpDataStream = (char *) pData->GetAsBytes(&pTmpDataStreamLen)) 
     == NULL) {
    DELETE(pSymKey);
    SetDebugOptions(oldOpts);
    return KO;
  }

  char *streamWithSizeHeader = AddSizeHeaderAsDataSize(pTmpDataStream,
                                                       pTmpDataStreamLen);
  GMessage *tmp = new GMessage();
  tmp->SetAsBytes("Fak", streamWithSizeHeader, pTmpDataStreamLen 
                  + sizeof(Size));
  tmp->DebugHeaders(logFile, "After AddSizeHeaderAsDataSize");

  Size eTmpDataOutLen;
  char *eTmpDataOut = NULL;
  if (ProcessWithSymetricKey_RawVersion(pSymKey, streamWithSizeHeader,
                                        pTmpDataStreamLen + sizeof(Size),
					&eTmpDataOut, &eTmpDataOutLen,
					ENCRYPT_FLAG) == KO) {

    DELETE(pTmpDataStream); DELETE(streamWithSizeHeader);
    DELETE(pSymKey);
    SetDebugOptions(oldOpts);
    return KO;
  }
  else {
    WriteString(llDebug, __FILE__ ":%d:eTmpDataOutLen is %lu.", __LINE__,
                eTmpDataOutLen);
    DELETE(pTmpDataStream);
    DELETE(streamWithSizeHeader);
  }

  Size pSymKeyStreamLen;
  char *pSymKeyStream = (char *) pSymKey->StoreToBytes(&pSymKeyStreamLen);
  DELETE(pSymKey);

  Size eTmpKeyStreamLen;
  char *eTmpKeyStream = NULL;
  if (EncryptBlockWithPublicKey(publicKey, pSymKeyStream, pSymKeyStreamLen,
                                &eTmpKeyStream, &eTmpKeyStreamLen) == KO) {
    DELETE(pSymKeyStream);
    SetDebugOptions(oldOpts);
    return KO;
  }
  else {
    WriteString(llDebug, __FILE__ ":%d:eTmpKeyStreamLen is %lu.", 
                __LINE__, eTmpKeyStreamLen);
    DELETE(pSymKeyStream);
  }

  char *wholeStream = NULL;
  Size wholeStreamLen;
  assert(eTmpKeyStreamLen == RSA_BLOCK_LEN);
  wholeStreamLen = mixMemCat(eTmpKeyStream, eTmpKeyStreamLen, eTmpDataOut, 
                             eTmpDataOutLen, &wholeStream);
  DELETE(eTmpKeyStream); DELETE(eTmpDataOut);

  if (wholeStreamLen % SYM_KEY_LEN != 0) {
    WriteString(llWarning, __FILE__ ":%d:wholeStreamLen not 8 multiple, "
                "but should be.", __LINE__);
    DELETE(wholeStream);
    SetDebugOptions(oldOpts);
    return KO;
  }
  else {
      // insert the bytestream into the eData BytesMsgField
    (*eData) = new BytesMsgField();
    (*eData)->SetAsBytes((void *) wholeStream, wholeStreamLen);
    WriteString(llDebug, __FILE__ ":%d:wholeStreamLen is %lu.", __LINE__,
                wholeStreamLen);
    DELETE(wholeStream);

    {
      GMessage *tmp = new GMessage();
      tmp->SetAsMsgField("In EncryptWithPublicKey BEFORE return", (*eData));
      tmp->DebugHeaders(logFile, "XXX in EncryptWithPublicKey");
      DELETE(tmp);
    }

    SetDebugOptions(oldOpts);
    return OK;
  }
}


/**
 * Decrypts data with public key.
 *
 * Decrypts data with public key. The data is in msgField, output is
 * inserted in a msgField too. The msgField is freshly allocated.
 *
 * @param   publicKey Public key.
 * @param   eData Encrypted data to process.
 * @param   pData Processed data.
 * @return  KO if an action fails, OK otherwise.
 * @author  Pechy
 * @see     other cryptography methods
 */
Err
Cipherer::DecryptWithPublicKey(GMessage * publicKey,
                               MsgField * eData,
                               MsgField ** pData)
{
  //int oldOpts = SetDebugOptions(llWarning | llError | llFatalError);
  int oldOpts = SetDebugOptions(llDebug);

  Size eTmpDataStreamLen;
  char *eTmpDataStream = NULL;
  if ((eTmpDataStream = (char *) eData->GetAsBytes(&eTmpDataStreamLen)) 
      == NULL) {
    SetDebugOptions(oldOpts);
    return KO;
  }
  else {
    if (eTmpDataStreamLen % 8 != 0) {
      WriteString(llWarning, __FILE__ ":%d:eData not 8 multiple, "
                  "but should be.", __LINE__);
      DELETE(eTmpDataStream);
      SetDebugOptions(oldOpts);
      return KO;
    }
    else {
      WriteString(llDebug, __FILE__ ":%d:eTmpDataStreamLen is %lu", __LINE__,
                  eTmpDataStreamLen);
    }
  }

  Size pTmpKeyStreamLen;
  char *pTmpKeyStream = NULL;
  if (DecryptBlockWithPublicKey(publicKey,
                                eTmpDataStream, RSA_BLOCK_LEN, 
			        &pTmpKeyStream, 
			        &pTmpKeyStreamLen) == KO) {
    WriteString(llWarning, __FILE__ ":%d:DecryptBlockWithPublicKey "
                "failed.", __LINE__);
    DELETE(eTmpDataStream);
    SetDebugOptions(oldOpts);
    return KO;
  }
  else {
    WriteString(llDebug, __FILE__ ":%d:pTmpKeyStreamLen is %lu.", __LINE__,
                pTmpKeyStreamLen);
  }

  GMessage *pSymKey = new GMessage();
  if (pSymKey->LoadFromBytes(pTmpKeyStream, pTmpKeyStreamLen) == KO) {
    WriteString(llWarning, __FILE__ ":%d:LoadFromBytes failed.", __LINE__);
    DELETE(pTmpKeyStream);
    SetDebugOptions(oldOpts);
    return KO;
  }
  else {
    DELETE(pTmpKeyStream);
    pSymKey->DebugHeaders(logFile, "Odsifrovany symetricky klic?");
  }

  Size pTmpDataOutLen;
  char *pTmpDataOut = NULL;
  if (ProcessWithSymetricKey_RawVersion(pSymKey, 
                                        eTmpDataStream + RSA_BLOCK_LEN,
                                        eTmpDataStreamLen - RSA_BLOCK_LEN,
					&pTmpDataOut, &pTmpDataOutLen,
					DECRYPT_FLAG) == KO) {

    WriteString(llWarning, __FILE__ ":%d:ProcessWithSymetricKey_RawVersion "
                "failed.", __LINE__);
    DELETE(pSymKey); DELETE(eTmpDataStream);
    SetDebugOptions(oldOpts);
    return KO;
  }
  else {
    GMessage *tmp = new GMessage();
    tmp->SetAsBytes("Fak", pTmpDataOut, pTmpDataOutLen);
    tmp->DebugHeaders(logFile, "po odsifrovani dat bez klice v DecrAsymPriv");
    DELETE(tmp);
    DELETE(eTmpDataStream);
    DELETE(pSymKey);
  }

  Size dataLen;
  char *data = NULL;
  data = ExtractDataFromSizeHeaderAnnotatedStream(pTmpDataOut, pTmpDataOutLen,
				           &dataLen);
  if (data == NULL) {
    WriteString(llWarning,
                __FILE__ ":%d:ExtractDataFromSizeHeaderAnnotatedStream() "
                "failed.", __LINE__);
    SetDebugOptions(oldOpts);
    DELETE(pTmpDataOut);
    return KO;
  }
  else {
    WriteString(llDebug, __FILE__ ":%d:dataLen is %lu", __LINE__, dataLen);
    GMessage *tmp = new GMessage();
    tmp->SetAsBytes("Fak", data, dataLen);
    tmp->DebugHeaders(logFile, "po ExtractDataFromSizeHeaderAnnotatedStream");
  }

  (*pData) = new BytesMsgField();
  (*pData)->SetAsBytes((void *) data, dataLen);
  DELETE(data);
  SetDebugOptions(oldOpts);
  return OK;
}

/**
 * Decrypts data with private key.
 *
 * Decrypts data with private key. The data is in msgField, output is
 * inserted in a msgField too. The msgField is freshly allocated.
 *
 * @param   privateKey Private key.
 * @param   eData Encrypted data to process.
 * @param   pData Processed data.
 * @return  KO if an action fails, OK otherwise.
 * @author  Pechy
 * @see     other cryptography methods
 */
Err
Cipherer::DecryptWithPrivateKey(GMessage * privateKey, 
                                MsgField * eData,
                                MsgField ** pData)
{
  //int oldOpts = SetDebugOptions(llWarning | llError | llFatalError);
  int oldOpts = SetDebugOptions(llDebug);

  GMessage *tmp = new GMessage();
  tmp->SetAsMsgField("In DecryptWithPrivateKey", eData);
  tmp->DebugHeaders(logFile, "XXX in DecryptWithPrivateKey");
  DELETE(tmp);

  Size eTmpDataStreamLen;
  char *eTmpDataStream = NULL;
  if ((eTmpDataStream = (char *) eData->GetAsBytes(&eTmpDataStreamLen)) 
      == NULL) {
    SetDebugOptions(oldOpts);
    return KO;
  }
  else {
    if (eTmpDataStreamLen % 8 != 0) {
      WriteString(llWarning, __FILE__ ":%d:eData not 8 multiple, "
                  "but should be.", __LINE__);
      DELETE(eTmpDataStream);
      SetDebugOptions(oldOpts);
      return KO;
    }
    else {
      WriteString(llDebug, __FILE__ ":%d:eTmpDataStreamLen is %lu", __LINE__,
                  eTmpDataStreamLen);
    }
  }

  Size pTmpKeyStreamLen;
  char *pTmpKeyStream = NULL;
  if (DecryptBlockWithPrivateKey(privateKey,
                                 eTmpDataStream, RSA_BLOCK_LEN, 
				 &pTmpKeyStream, 
				 &pTmpKeyStreamLen) == KO) {
    WriteString(llWarning, __FILE__ ":%d:DecryptBlockWithPrivateKey "
                "failed.", __LINE__);
    DELETE(eTmpDataStream);
    SetDebugOptions(oldOpts);
    return KO;
  }
  else {
    WriteString(llDebug, __FILE__ ":%d:pTmpKeyStreamLen is %lu.", __LINE__,
                pTmpKeyStreamLen);
  }

  GMessage *pSymKey = new GMessage();
  if (pSymKey->LoadFromBytes(pTmpKeyStream, pTmpKeyStreamLen) == KO) {
    WriteString(llWarning, __FILE__ ":%d:LoadFromBytes failed.", __LINE__);
    DELETE(pTmpKeyStream);
    SetDebugOptions(oldOpts);
    return KO;
  }
  else {
    DELETE(pTmpKeyStream);
    pSymKey->DebugHeaders(logFile, "Odsifrovany symetricky klic?");
  }

  Size pTmpDataOutLen;
  char *pTmpDataOut = NULL;
  if (ProcessWithSymetricKey_RawVersion(pSymKey, 
                                        eTmpDataStream + RSA_BLOCK_LEN,
                                        eTmpDataStreamLen - RSA_BLOCK_LEN,
					&pTmpDataOut, &pTmpDataOutLen,
					DECRYPT_FLAG) == KO) {

    WriteString(llWarning, __FILE__ ":%d:ProcessWithSymetricKey_RawVersion "
                "failed.", __LINE__);
    DELETE(pSymKey); DELETE(eTmpDataStream);
    SetDebugOptions(oldOpts);
    return KO;
  }
  else {
    GMessage *tmp = new GMessage();
    tmp->SetAsBytes("Fak", pTmpDataOut, pTmpDataOutLen);
    tmp->DebugHeaders(logFile, "po odsifrovani dat bez klice v DecrAsymPriv");
    DELETE(tmp);
    DELETE(eTmpDataStream);
    DELETE(pSymKey);
  }

  Size dataLen;
  char *data = NULL;
  data = ExtractDataFromSizeHeaderAnnotatedStream(pTmpDataOut, pTmpDataOutLen,
				           &dataLen);
  if (data == NULL) {
    WriteString(llWarning,
                __FILE__ ":%d:ExtractDataFromSizeHeaderAnnotatedStream() "
                "failed.", __LINE__);
    SetDebugOptions(oldOpts);
    DELETE(pTmpDataOut);
    return KO;
  }
  else {
    WriteString(llDebug, __FILE__ ":%d:dataLen is %lu", __LINE__, dataLen);
    GMessage *tmp = new GMessage();
    tmp->SetAsBytes("Fak", data, dataLen);
    tmp->DebugHeaders(logFile, "po ExtractDataFromSizeHeaderAnnotatedStream");
  }

  (*pData) = new BytesMsgField();
  (*pData)->SetAsBytes((void *) data, dataLen);
  DELETE(data);

  {
    GMessage *tmp = new GMessage();
    tmp->SetAsMsgField("In DecryptWithPrivateKey BEFORE return", (*pData));
    tmp->DebugHeaders(logFile, "XXX in DecryptWithPrivateKey");
    DELETE(tmp);
  }

  SetDebugOptions(oldOpts);
  return OK;
}
