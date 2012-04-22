  // has to be the *FIRST* include !
#include "./../../Common/H/common.h"

#include <string>

#include "./../../Common/H/labels.h"

#include "./H/messageCreator.h"

/**
 * Makes GMessage with recipient's identification.
 *
 * When creating onions and chunks, is each peel there are two
 * recipients' GMessages. Each can be used to send the chunk to.
 *
 * @param   mixCertif Contains the recipient's data (address and port).
 * @param   symKey This symKey is used somewhere else for path encryption.
 *                 For recipient to be able to use it, we encrypt the
 *                 symetric key with recipient's public key.
 * @return  Recipient's GMessage:
 * <pre>
 * GM_PUBLIC_KEY_INFO
 * GM_SYMETRIC_PATH_KEY_INFO
 * GM_ADDRESS
 * GM_PORT
 * </pre>
 *
 * @author  Pechy
 * @see     MakeMoreOnionLayers()
 */
GMessage *
MessageCreator::MakeRecipientGMessage(GMessage *mixCertif, 
                                      GMessage *symKey)
{
  WriteString(llDebug, __FILE__ ":%d:MakeRecipientGMessage() entered.", 
              __LINE__); 

  GMessage *recipient = new GMessage();
  MsgField *tmp;
  if ((tmp = mixCertif->GetAsMsgField(GM_ADDRESS)) == NULL) {
    WriteString(llWarning, __FILE__ ":%d:GM_ADDRESS failed.",__LINE__); 
    DELETE(recipient);
    return NULL;
  }
  else {
    recipient->SetAsMsgField(GM_ADDRESS, tmp);
    DELETE(tmp);
  }

  if ((tmp = mixCertif->GetAsMsgField(GM_PORT)) == NULL) {
    WriteString(llWarning, __FILE__ ":%d:GM_PORT failed.",__LINE__); 
    DELETE(recipient);
    return NULL;
  }
  else {
    recipient->SetAsMsgField(GM_PORT, tmp);
    DELETE(tmp);
  }

  GMessage *pubKey;
  if ((pubKey = mixCertif->GetAsGMessage(GM_PUBLIC_KEY_INFO)) == NULL) {
    WriteString(llWarning, __FILE__ ":%d:GM_PUBLIC_KEY_INFO failed.",__LINE__); 
    DELETE(recipient);
    return NULL;
  }

  MsgField *eSymKey = NULL;

  if (cipherer->EncryptKeyWithPublicKey(pubKey, symKey, &eSymKey) != OK) {
    WriteString(llWarning, __FILE__ ":%d:Encryption failed in "
                "MakeRecipientsGMessage().",__LINE__); 

    DELETE(pubKey); DELETE(recipient);
    return NULL;
  }

  recipient->SetAsMsgField(GM_SYMETRIC_PATH_KEY_INFO, eSymKey); 
  DELETE(eSymKey);

    // we have to include information about what asymetric cipher was used
    // to encrypt a symetric key that was used to encrypt the rest of path,
    // but it would be useless to give a public key to its owner. (owner 
    // is a recipient who processes the peel). The public key itself is
    // not needed.
  pubKey->Delete(GM_PUBLIC_KEY);
  recipient->SetAsGMessage(GM_PUBLIC_KEY_INFO, pubKey); 
  DELETE(pubKey);

  return recipient;
}

/**
 * Add Size integer as a header denoting the lenght of data.
 *
 * When we layer particular peels, we need convert the upper new onion
 * peel (the peel without GM_REST_OF_PATH) into a stream.  To be able
 * to recognize how long is the onion, we have to include 4 bytes
 * header as a length of the onion.
 *
 * @param   onion This onion is converted to an octet stream and annotated
 *                with the header.
 * @param   onionBottomStream The stream that is returned.
 * @param   onionBottomStreamLen Size of returned stream.
 * @return  void
 * @author  Pechy
 * @see     MakeMoreOnionLayers()
 */
void
MessageCreator::AnnotateWith4ByteLength(GMessage *onion, 
                                        char **onionBottomStream,
                                        Size *onionBottomStreamLen)
{
    // firstly convert onion to octet-stream
  Size onionStreamLen;
  char *onionStream = (char *) onion->StoreToBytes( &onionStreamLen);

    // now annotate with 4 bytes header
  char *onionStreamWith4BytesHeader = new char[onionStreamLen + sizeof(Size)];

    // 1st 4 bytes is for header
  memcpy((void *) onionStreamWith4BytesHeader, (void *) &onionStreamLen, 
         sizeof(Size));

    // rest of it is for onionStream
  memcpy( (void *) (onionStreamWith4BytesHeader + sizeof(Size)), 
          (void *) onionStream,
          onionStreamLen);

  DELETE(onionStream);
  *onionBottomStream = onionStreamWith4BytesHeader; 
  *onionBottomStreamLen = onionStreamLen + sizeof(Size);
}

/**
 * Adds local certificates to the certificates that are used for onion 
 * creation.
 *
 * When we create the onion, the onion's bottom identifies the
 * recipient.  For not to create the onion in the two steps, we add
 * local mix certificates to the certificates that are used for onion
 * creation. The new local certificates are the first certificates
 * that are used when the onion is created (identifiing the Mix as a
 * final Mix).
 *
 * @param   callBackOnionInfo Contains the certificates.
 * @return  KO when Mix certificate can't be returned, OK otherwise.
 * @author  Pechy
 * @see     MakeMoreOnionLayers()
 */
Err 
MessageCreator::AddLocalCertifs( GMessage *callBackOnionInfo)
{
    // callBackOnionInfo has GM_MIX_CERTIFICATES and GM_SYMETRIC_DATA_KEYS
  GMessage *mixCertifs = callBackOnionInfo->GetAsGMessage(GM_MIX_CERTIFICATES);

    // we will change the first tuple, it has to be always here, even if 
    // hopCount somewhere is zero (we use hopCount + 1)
  GMessage *firstTuple = NULL;
  firstTuple = mixCertifs->GetAsGMessage(GM_MIX_CERTIFICATE_TUPLE_1);

    // load local mix certificate from the file
  string mixCertif = configFile->GetValue(CFG_CERTIFICATES_DIRECTORY) + "/" +
                     configFile->GetValue(CFG_CERTIFICATE);
  GMessage *localMixCertif = new GMessage();
  if (localMixCertif->LoadFromFile((char *) mixCertif.c_str()) == KO) {
    WriteString(llWarning, __FILE__ ":%d:Local mix certicate not found.", 
                __LINE__); 
    return KO;
  }

    // both recipients are our local mix
  firstTuple->SetAsGMessage(GM_MIX_CERTIFICATE_1, localMixCertif);
  firstTuple->SetAsGMessage(GM_MIX_CERTIFICATE_2, localMixCertif);
  DELETE(localMixCertif);
    // replace first tuple with tuple created from entities of a local
    // mix certificate
  mixCertifs->SetAsGMessage(GM_MIX_CERTIFICATE_TUPLE_1, firstTuple);
  DELETE(firstTuple);

    // return changed tuple into the GM_MIX_CERTIFICATES field in 
    // callBackOnionInfo
  callBackOnionInfo->SetAsGMessage(GM_MIX_CERTIFICATES, mixCertifs);

  DELETE(mixCertifs);

  return OK;
}

/**
 * Padd the rest of path field to the original length.
 *
 * When the chunk goes through the chain of Mixes, the recipient's
 * identification (rest of path) is going down in size. This fact
 * would enable the onion length attack (the enemy could estimate how
 * many hops is the chunk to go to the final Mix), so this method
 * padds the rest of path field with randomly generated bytes.
 *
 * @param   restOfPath Rest of path, downsized by the current peel.
 * @param   restOfPathLen Rest of path length.
 * @return  fixedSize Size to which we padd the restOfPath parameter.
 * @author  Pechy
 * @see     MakeMoreOnionLayers(), Translator methods
 */
char *
MessageCreator::PaddOnionRestOfPath(char *restOfPath, Size restOfPathLen,
                                    Size fixedSize)
{
  char *fixedSizedRestOfPath = new char[fixedSize];
  memcpy(fixedSizedRestOfPath, restOfPath, restOfPathLen);
 
  for (Size i = 0; i < (fixedSize - restOfPathLen); ++i) {
    fixedSizedRestOfPath[restOfPathLen + i] = 
      cipherer->GenerateRandomByte();
  }

  return fixedSizedRestOfPath;
}
