// has to be the FIRST include !
#include "./../H/common.h"
#include "../H/labels.h"

#include "./../RSAEuro/H/rsaref.h"

#include "./H/cipherer.h"

/**
 * Encrypt data using symetric key.
 *
 * We use ,,fields'', because it's quite easy to work with them in 
 * connection with gMessages. MsgField is abstract data type, so whenever
 * we need create an instance of that, we use BytesMsgField.
 * <p>
 * eData is encoded octet-stream createt from gMessage, ie. the whole
 * gMessage is saved to an octet-stream, encoded and then BytesMsgField is
 * initialized from it.
 * <p>
 * Uses auxiliary method ProcessWithSymetricKey() defined in another 
 * module, in RSAEuro decrypting with DES is not the same as encrypting.
 *
 * @param   symKey What symetric key to use.
 * @param   pData Plain data to process.
 * @param   eData Encrypted data.
 * @return  KO if action fails, OK otherwise.
 * @author  Pechy
 * @see     other crypthographic methods
 */
Err 
Cipherer::EncryptWithSymetricKey(GMessage * symKey, MsgField * pData, 
                                 MsgField ** eData)
{
  return ProcessWithSymetricKey(symKey, pData, eData, ENCRYPT_FLAG);
}

/**
 * Decrypt data using symetric key.
 *
 * We use ,,fields'', because it's quite easy to work with them in 
 * connection with gMessages. MsgField is abstract data type, so whenever
 * we need create an instance of that, we use BytesMsgField.
 * <p>
 * Uses auxiliary method ProcessWithSymetricKey() defined in another 
 * module, in RSAEuro decrypting with DES is not the same as encrypting.
 *
 * @param   symKey What symetric key to use.
 * @param   eData  Encrypted data
 * @param   pData  Plain data to process.
 * @return  KO if action fails, OK otherwise.
 * @author  Pechy
 * @see     other crypthographic methods
 */
Err
Cipherer::DecryptWithSymetricKey(GMessage * symKey, MsgField * eData, 
                                 MsgField ** dData)
{
  return ProcessWithSymetricKey(symKey, eData, dData, DECRYPT_FLAG);
}

/**
 * Decrypt multiple encrypted data using symetric keys.
 *
 * When we use onions and chunks to make traffic analysis harder, we
 * make a special peel for each hops the chunk goes through. When we
 * make these chunks or process received data, we use encryption or
 * decryption in a loop, each loop for one hop included.
 *
 * @param   symKeys What symetric keys to use.
 * @param   eData  Encrypted data
 * @param   pData  Plain data to process.
 * @return  KO if action fails, OK otherwise.
 * @author  Pechy
 * @see     other crypthographic methods
 */
Err
Cipherer::DecryptWithSymetricKeys(GMessage * symKeys, MsgField * eData, 
                                  MsgField ** dData)
{
  Size keyCount;
  if (symKeys->GetAsSize(GM_SYMETRIC_KEY_COUNT, &keyCount) == KO) {
    return KO;
  }

  string *tmpStr = NULL; GMessage *symKey = NULL;

   // special case, just copy eData to (*dData)
  if (keyCount == 0) {
    Size tmpSize;
    char *tmpChar = (char *) eData->GetAsBytes(&tmpSize);

    (*dData) = new BytesMsgField();
    (*dData)->SetAsBytes((void *) tmpChar, tmpSize);
    DELETE(tmpChar);
    return OK;
  }

  for (unsigned int i = 0; i < keyCount; ++i) {

    tmpStr = CreateString(GM_SYMETRIC_KEY_INFO, keyCount - i, NO_PARAM);
    
    if ((symKey = symKeys->GetAsGMessage((char *) tmpStr->c_str())) == NULL) {
      DELETE(tmpStr);
      return KO;
    }
    else {
      DELETE(tmpStr);
    }
    
    if (DecryptWithSymetricKey(symKey, eData, dData) == KO) {
      DELETE(symKey);
      return KO;
    }

    DELETE(symKey);

    if (i != 0) 
      DELETE(eData);

    eData = *dData;
  }

  return OK;
}

/**
 * Encrypt data using multiple symetric keys.
 *
 * When we use onions and chunks to make traffic analysis harder, we
 * make a special peel for each hops the chunk goes through. When we
 * make these chunks or process received data, we use encryption or
 * decryption in a loop, each loop for one hop included.
 *
 * @param   symKeys What symetric keys to use.
 * @param   pData  Plain data.
 * @param   eData  Encrypted data to process.
 * @return  KO if action fails, OK otherwise.
 * @author  Pechy
 * @see     other crypthographic methods
 */
Err
Cipherer::EncryptWithSymetricKeys(GMessage * symKeys, MsgField * pData, 
                                  MsgField ** eData,
				       // ugly hack, Pechy is lazy
                                  int useAlsoKeyOne)
{
  if (!symKeys || !pData || !eData) {
    return KO;
  }

  Size keyCount;
  if (symKeys->GetAsSize(GM_SYMETRIC_KEY_COUNT, &keyCount) == KO) {
    return KO;
  }

  string *tmpStr = NULL; GMessage *symKey = NULL;

   // special case, just copy pData into eData
  if ( (keyCount == 0) || ((keyCount == 1) && (!useAlsoKeyOne)) ) {
    Size tmpSize;
    char *tmpChar = (char *) pData->GetAsBytes(&tmpSize);

    (*eData) = new BytesMsgField();
    (*eData)->SetAsBytes((void *) tmpChar, tmpSize);
    DELETE(tmpChar);
    return OK;
  }

  for (unsigned int i = 0; i < keyCount; ++i) {

    tmpStr = CreateString(GM_SYMETRIC_KEY_INFO, i + 1, NO_PARAM);
    if ((symKey = symKeys->GetAsGMessage((char *) tmpStr->c_str())) == NULL) {
      DELETE(tmpStr);
      return KO;
    }
    else {
      DELETE(tmpStr);
    }

     // if we are not to use the first key, don't use it :-)
    if (useAlsoKeyOne || i != 0) {
      if (EncryptWithSymetricKey(symKey, pData, eData) == KO) {
        DELETE(symKey);
        return KO;
      }

      if (i > 0)                                                                                              
        if (i != 1 || useAlsoKeyOne)                                                                          
          DELETE(pData);                                                                                      
			                                                  
      pData = *eData;
    }

    DELETE(symKey);
  }

  return OK;
}

/**
 * Generic method for symetric procession..
 *
 * Symetric encryption and decryption is almost identical, so we made this
 * auxiliary method.
 *
 * @param   symKey Symetric key to use.
 * @param   mfDataIn Input data in a MsgField.
 * @param   mfDataOut Output data in a MsgField.
 * @return  KO if procession fails, OK otherwise
 * @author  Pechy
 * @see     other crypthographic methods
 */
Err
Cipherer::ProcessWithSymetricKey(GMessage * symKey,
                                 MsgField * mfDataIn, 
                                 MsgField ** mfDataOut,
                                 int actionFlag)
{
  Size mfDataInLen;
  char *dataInStream = NULL;
  if ((dataInStream = (char *) mfDataIn->GetAsBytes(&mfDataInLen)) == NULL) {
    return KO;
  }

  Size dataOutStreamLen;
  char *dataOutStream = NULL;

  if (ProcessWithSymetricKey_RawVersion(symKey, dataInStream,
                                        mfDataInLen, &dataOutStream,
					&dataOutStreamLen, actionFlag) 
      == OK) {
    (*mfDataOut) = new BytesMsgField();
    (*mfDataOut)->SetAsBytes(dataOutStream, dataOutStreamLen);
    DELETE(dataInStream); DELETE(dataOutStream);
    return OK;
  }
  else {
    DELETE(dataInStream);
    return KO;
  }
}

/**
 * Generic method for symetric procession..
 *
 * Symetric encryption and decryption is almost identical, so we made this
 * auxiliary method. This method process octet stream.
 *
 * @param   symKey Symetric key to use.
 * @param   dataIn Input data stream.
 * @param   dataInLen Input data stream length.
 * @param   dataOut Output data stream.
 * @param   dataOutLen Output data stream length.
 * @return  KO if procession fails, OK otherwise
 * @author  Pechy
 * @see     other crypthographic methods
 */
Err
Cipherer::ProcessWithSymetricKey_RawVersion(GMessage *symKey, char *dataIn,
                                            Size dataInLen, char **dataOut,
                                            Size *dataOutLen, int actionFlag)

{
  DES_CBC_CTX context;
  unsigned char *key = NULL, 
                *iv = NULL;

   // actionFlag determines whether we decrypt or encrypt - it *is*
   // different in action !!
   // note: this function returns void, so no checking here
  DES_CBCInit(&context, 
              key = (unsigned char *) symKey->GetAsBytes(GM_SYMETRIC_KEY),
              iv =(unsigned char*)symKey->GetAsBytes(GM_INITIALIZATION_VECTOR), 
              actionFlag);

  DELETE(iv); DELETE(key);

   // len *has* to be multiple of SYM_KEY_LEN for DES_CBCUpdate to 
   // work, so we do some hacking if it is not
  if ( (dataInLen % SYM_KEY_LEN) != 0) {

     // is't strange if data for decryption is not multiple of SYM_KEY_LEN
    if (actionFlag == DECRYPT_FLAG) {
      WriteString(llWarning, __FILE__ ":%d:Data for decryption not"
                  " multiple of SYM_KEY_LEN !!", __LINE__);
      return KO;
    }
  }
      
   // create new buffer (multiple of SYM_KEY_LEN in length)
  unsigned int inputLen = (((dataInLen - 1) / SYM_KEY_LEN) * SYM_KEY_LEN) 
                           + SYM_KEY_LEN;
  WriteString(llDebug, __FILE__ ":%d:inputLen is %d.", __LINE__, inputLen);
  WriteString(llDebug, __FILE__ ":%d:dataInLen is %d.", __LINE__, dataInLen);
  unsigned char *input = new unsigned char[inputLen];
   // ,,zero'' the new buffer stream ('\0' - to be easily located)
  memset(input, 0, inputLen);
  memcpy(input, dataIn, dataInLen);

  unsigned char *output = new unsigned char[inputLen];

   // does the real action
  if (DES_CBCUpdate(&context, output, input, (unsigned int) inputLen) 
                    != IDOK) {
    WriteString(llWarning, __FILE__ ":%d:DES_Update failed.", __LINE__);
    DELETE(output); DELETE(input);
    return KO;
  }

  (*dataOut) = (char *) output;
  (*dataOutLen) = (Size) inputLen;

  DELETE(input);

   // we should clear the context before return !
  return OK;
}
