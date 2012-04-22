  // has to be the FIRST include !
#include "./../../Common/H/common.h"

#include <stdio.h>

#include "./../../Common/H/labels.h"

#include "./H/translator.h"

/**
 * Translator constructor.
 *
 * Saves object pointers he gets as parameters.
 *
 * @param   parent Parent, ie. Mix object.
 * @param   translatorLogFile Log file.
 * @author  Venca
 * @see     ~Translator()
 */
Translator::Translator(Mix * parent, LogFile * translatorLogFile)
 :Runable(translatorLogFile), MixOffspring(parent)
{

}

/**
 * Translator destructor.
 *
 * Now does nothing.
 *
 * @author  Pechy or Venca
 */
Translator::~Translator()
{

}

/**
 * The main method that do the things.
 *
 * WARNING: the method is long.
 * <p>
 * This method translates the incoming chunk into the outcoming one.
 * First, the method decrypts the GMessage where is some information
 * useful for further chunk translation. The GMessage is encrypted
 * with public key of this Mix (the public key that was included into
 * the Mix's certificate during certification creation). By decrypting
 * this, Run() gets a symetric key that is used to decrypt information
 * about next possible recipients. The combination of asymetric and
 * symetric cryptography is used because we want to be able to have
 * more than one recipient in each peel. 
 * <p> After decryption by symetric key, we have to check whether there
 * is a final stop indication:
 * <ul>
 *   <li> Yes: we have to decrypt multiple encrypted data (as the data
 *     went through the chain of Mixes related to the recipient's
 *     onion).  The symetric keys should be already in the dectypted
 *     onion bottom (ie.  rest of path bottom). After the dectyption
 *     the data has to be dectypted by a private asymetric key
 *     included also in the bottom. See middle attack discussion for
 *     more info. That the data are passed to Linker.
 *   <li> No (final stop not found): we strip from the decrypted rest of
 *     path field (decrypted by symetric key that was in turn
 *     decrypted by Mix's private key). The stripped data part is
 *     indicated by the first sizeof(Size) bytes in the decrypted rest
 *     of path field. From this part we make a GMessage. This GMessage
 *     contains needed information about the next possible recipients.
 *     After stripping the rest of path field, we have to padd it to
 *     the original length. Then we have to encrypt the data by
 *     another symetric key found in the stripped GMessage. Next, we
 *     take the data, padded rest of path, several recipients (got
 *     from stripped GMessage) and make a new chunk, suitable for
 *     Sender.
 * </ul>
 *
 * @return  void pointer 
 * @author  Pechy, Venca
 * @see     CreateChunk(), CreateOnion(), Cipherer
 */
void * Translator::Run(void *)
{
  MaskAllSignals();

  WriteString(llDebug, __FILE__ ":%d:Entering Translator::Run()", 
	      __LINE__);

  for ( ; ; ) {

      // Get new message from queue
    GMessage *msg = NULL;
    mix->receiverToTranslator->GetMsg(&msg);
     
    WriteString(llDebug, __FILE__ ":%d:Translator got message, what is "
                "there?", __LINE__);
    msg->DebugHeaders(logFile);

      // firstly, process GM_DEBUG, GM_PROTOCOL_VERSION, GM_RECIPIENT, from
      // Recipient process GM_ASYMETRIC_PATH_KEY_INFO and then process 
      // GM_SYMETRIC_PATH_KEY_INFO, then make from part of "Path" gMsg

      // Pass GM_DEBUG unmodified and GM_PROTOCOL_VERSION unmodified
    GMessage *newMsg = new GMessage();
    char *protocolVersion = msg->GetAsString(GM_PROTOCOL_VERSION);
    newMsg->SetAsString(GM_PROTOCOL_VERSION, protocolVersion); 
    if (protocolVersion) 
      DELETE(protocolVersion);

     // Get recipient field with encrypted path key
    GMessage *rcpt = NULL;
    if ((rcpt = msg->GetAsGMessage(GM_RECIPIENT)) == NULL) {
      WriteString(llWarning, __FILE__ ":%d:Recipient not in proper "
                  "format, throwing the message away.", __LINE__);

      DELETE(msg); DELETE(newMsg);
      continue;
    }
    else {
      WriteString(llDebug, __FILE__ ":%d:What is in Recipient?", __LINE__);
      rcpt->DebugHeaders(logFile);
    }
    
    char *asymCipherType;
    char *asymCipherVer;
    GMessage *pubKeyInfo;
    if ((pubKeyInfo = rcpt->GetAsGMessage(GM_PUBLIC_KEY_INFO)) == NULL) {

      WriteString(llWarning, __FILE__ ":%d:pubKeyInfo not in proper "
                  "format, throwing the message away.", __LINE__);

      DELETE(msg); DELETE(newMsg); DELETE(rcpt);
      continue;   
    }
    else {
      asymCipherType = pubKeyInfo->GetAsString(GM_ASYMETRIC_CIPHER_TYPE);
      asymCipherVer=pubKeyInfo->GetAsString(GM_ASYMETRIC_CIPHER_VERSION);
      DELETE(pubKeyInfo);
    } 

    GMessage * mixPrivateKey = NULL;
    if ((mixPrivateKey = mix->keyManager->GetMixPrivateKey(asymCipherType,
         asymCipherVer)) == NULL) {
      WriteString(llWarning, __FILE__ ":%d:GetMixPrivateKey failed.", 
                  __LINE__);
      DELETE(msg); DELETE(newMsg); DELETE(rcpt);
      continue;
    }
    else {
      DELETE(asymCipherType);
      DELETE(asymCipherVer);
    }

    MsgField * eSymPathKeyInfo = NULL;
    if ((eSymPathKeyInfo = 
           rcpt->GetAsMsgField(GM_SYMETRIC_PATH_KEY_INFO)) == NULL) {
      WriteString(llWarning, __FILE__ ":%d:GM_SYMETRIC_PATH_KEY_INFO "
                  "not in message.", __LINE__);
      DELETE(msg); DELETE(newMsg); DELETE(rcpt); DELETE(mixPrivateKey);
      continue;
    }
    else
      DELETE(rcpt);

      // Decrypt symetric key with mix's private key
    GMessage * symKeyPathInfo = NULL;
    if (mix->cipherer->DecryptKeyWithPrivateKey(mixPrivateKey, 
                                                eSymPathKeyInfo,
                                                &symKeyPathInfo) != OK) {
      WriteString(llError, __FILE__ ":%d:Key decrypting failed in "
                  "Translator, throwing message away.", __LINE__);

      DELETE(msg); DELETE(newMsg); DELETE(eSymPathKeyInfo);
      DELETE(mixPrivateKey);
      continue;
    }
    else {
      DELETE(eSymPathKeyInfo);
      DELETE(mixPrivateKey);
      WriteString(llDebug, __FILE__ ":%d:What is in decrypted sym path "
		  "key info?", __LINE__);
      symKeyPathInfo->DebugHeaders(logFile);
    }

      // Decrypt GM_REST_OF_PATH using key from symKeyPathInfo
    MsgField * pPath;
    if ((pPath = msg->GetAsMsgField(GM_REST_OF_PATH)) == NULL) {
      WriteString(llWarning, __FILE__ ":%d:GM_REST_OF_PATH is NULL.", 
                  __LINE__);
      DELETE(msg); DELETE(newMsg); DELETE(symKeyPathInfo);
      continue;
    }
    else 
      WriteString(llDebug, __FILE__ ":%d:GM_REST_OF_PATH is %ld.", __LINE__,
                  pPath->SizeOf());

      // because we concatenate peel and rest of onion, we have to
      // align the data, because decrypting can be performed only
      // on aligned data streams
    if ((pPath->SizeOf() % 8) != 0) {
      WriteString(llDebug, __FILE__ ":%d:GM_REST_OF_PATH is not "
                "multiple of 8.", __LINE__);
      Size tmpSize;
      char *tmpChar = (char *) pPath->GetAsBytes(&tmpSize);
      tmpSize = tmpSize - (tmpSize % 8);
        // cannot do SetAsBytes without DELETE(and) new before?
      DELETE(pPath);
      pPath = new BytesMsgField();
      pPath->SetAsBytes((void *) tmpChar, tmpSize);
      DELETE(tmpChar);
    }

    MsgField * dPath = NULL;
    if (mix->cipherer->DecryptWithSymetricKey(symKeyPathInfo, pPath, 
                                              &dPath) == OK) {
      WriteString(llDebug, __FILE__ ":%d:Decrypted GM_REST_OF_PATH is %ld "
                "bytes long.", __LINE__, dPath->SizeOf());
      DELETE(pPath); DELETE(symKeyPathInfo);
    }
    else {
      WriteString(llWarning, __FILE__ ":%d:Data decrypting failed in "
                  "Translator, throwing message away.", __LINE__);

      DELETE(pPath); DELETE(msg); DELETE(newMsg); DELETE(symKeyPathInfo);
      continue;
    }

      // dPathPtr is a pointer to plain GM_REST_OF_PATH octet stream
    char * dPathPtr = NULL;
    Size dPathLen;
    dPathPtr = (char *)dPath->GetAsBytes(&dPathLen);
    DELETE(dPath);

      // take first 4 bytes as a length on an onion and then strip these
      // bytes off
    Size peelLen = *((Size *)dPathPtr);
    WriteString(llDebug, __FILE__ ":%d:Peel len is %lu.", __LINE__,
                peelLen);

    GMessage * peel = new GMessage();
    //  GMessage * peel = new GMessage(dPathPtr + sizeof(Size), peelLen);
    if (peel->LoadFromBytes(dPathPtr + sizeof(Size), peelLen) == KO) {
      WriteString(llWarning, __FILE__ ":%d:Peel not in proper format.", 
                  __LINE__);
      DELETE(msg); DELETE(newMsg); DELETE(dPathPtr); 
      continue;
    }
    else {
      WriteString(llDebug, __FILE__ ":%d:What is in the peel?", __LINE__);
      peel->DebugHeaders(logFile);
    }

      // Should it be forwarded to another mix or delivered locally?
      // Note: GM_FINAL_STOP should logically be in "Recepient-1" gMsg, but 
      // this way we have things a bit easier...
    char *finalStopString = peel->GetAsString(GM_FINAL_STOP);
    if ( finalStopString != NULL ) {

        // we don't need it any more
      DELETE(dPathPtr);
      DELETE(finalStopString);

      WriteString(llDebug, __FILE__ ":%d:Final stop is here!", __LINE__);

	// Deliver localy after decrypting with all symetric keys
      GMessage *finalRcpt = NULL;
      if ((finalRcpt = peel->GetAsGMessage(GM_RECIPIENT_1)) == NULL) {
	WriteString(llWarning, __FILE__ ":%d:Recipient-1 not in proper "
	            "format.", __LINE__);
	DELETE(msg); DELETE(newMsg); DELETE(peel); 
	continue;
      }

      char * addr = NULL;
      if ((addr = finalRcpt->GetAsString(GM_ADDRESS)) == NULL) {
	WriteString(llWarning, __FILE__ ":%d:Address in Recipient-1 "
	            "NULL.", __LINE__);
	DELETE(msg); DELETE(newMsg); DELETE(peel); DELETE(finalRcpt);
	continue;
      }
      else {
	DELETE(finalRcpt); 
	  // add destination (ie. recipient's address)
	newMsg->SetAsString(GM_X_DESTINATION, addr);
	DELETE(addr);
      }

	// Pass X-Origin unmodified
      char *xOrigin;
      if ((xOrigin = msg->GetAsString(GM_ORIGIN)) == NULL) {
	WriteString(llWarning, __FILE__ ":%d:XOrigin is NULL.", 
	            __LINE__);
	DELETE(msg); DELETE(newMsg); DELETE(peel);
	continue;
      }
      else {
	newMsg->SetAsString(GM_ORIGIN, xOrigin);
	DELETE(xOrigin);
      }

	// onion is encrypted, though through the whole journey the chunk 
	// is being decrypted, so through the 2nd part of the journey we 
	// decrypt plain data, so we have to encrypt data here, understand? 
      MsgField * eData = NULL;
      if ((eData = msg->GetAsMsgField(GM_DATA)) == NULL) {
	WriteString(llWarning, __FILE__ ":%d:GM_DATA in message NULL.", 
	            __LINE__);
	DELETE(msg); DELETE(newMsg); DELETE(peel);
	continue;
      }

	// we just took the last field from the ,,old'' chunk, so DELETE it
      DELETE(msg);

      GMessage * symKeys = NULL;
      if ((symKeys = peel->GetAsGMessage(GM_SYMETRIC_DATA_KEY_INFOS)) 
           == NULL) {
	WriteString(llWarning, __FILE__ ":%d:GM_SYMETRIC_DATA_KEY_INFOS "
	            "in peel.", __LINE__);
	DELETE(newMsg); DELETE(peel); DELETE(eData);
	continue;
      }

      MsgField * dData = NULL;
      if (mix->cipherer->EncryptWithSymetricKeys(symKeys, eData, &dData, 
                                                 0) == KO) {
        WriteString(llWarning, __FILE__ ":%d:EncryptWithSymetricKeys "
	            "failed!", __LINE__);
        DELETE(symKeys); DELETE(eData); DELETE(newMsg); DELETE(peel);
        continue;
      }
      else {
        DELETE(symKeys);
	DELETE(eData);
      }

      GMessage * privKeyInfo = NULL;
      if ((privKeyInfo = peel->GetAsGMessage(GM_PRIVATE_KEY_INFO))
           == NULL) {
        WriteString(llWarning, __FILE__ ":%d:GM_PRIVATE_KEY_INFO "
                    "in peel.", __LINE__);
        DELETE(newMsg); DELETE(peel); DELETE(dData);
        continue;
      }
      DELETE(peel);

      MsgField * plainData = NULL;
      if (mix->cipherer->DecryptWithPrivateKey(privKeyInfo, dData, &plainData) 
          == KO) {
        WriteString(llWarning, __FILE__ ":%d:DecryptWithPrivateKey "
	            "failed!", __LINE__);
        DELETE(privKeyInfo); DELETE(dData); DELETE(newMsg);
        continue;
      }
      else {
        DELETE(privKeyInfo);
	DELETE(dData);
      }

	// insert plain GM_DATA into the new message
      newMsg->SetAsMsgField(GM_DATA, plainData);
      DELETE(plainData);

	// GM_DATA, GM_PROTOCOL_VERSION, GM_DEBUG, GM_X_DESTINATION, 
	// GM_ORIGIN
      mix->translatorToLinker->Append(newMsg);
      WriteString(llDebug, __FILE__ ":%d:Message passed to Linker.", __LINE__);
    }
    else {
	// Forward to Padder after translating with data key
      GMessage * dataKey;
      if ((dataKey = peel->GetAsGMessage(GM_SYMETRIC_DATA_KEY_INFO))
           == NULL) {
        WriteString(llWarning, __FILE__ ":%d:GM_SYMETRIC_DATA_KEY_INFO "
	            "NULL!", __LINE__);
        DELETE(peel); DELETE(msg); DELETE(newMsg); DELETE(dPathPtr);
        continue;
      }

	// this is a bit obscure: we can't pass normal chunk to sender, 
	// because only him can decide whom from the two possible 
	// recipients will be the chunk sent, so this gMsg will contain
	// almost whole chunk and then two recipients - Sender chooses one 
	// of them, adds him to the chunk and finally sends the chunk
      MsgField * eData;
      if ((eData = msg->GetAsMsgField(GM_DATA)) == NULL) {
        WriteString(llWarning, __FILE__ ":%d:Data in message NULL!", 
	            __LINE__);

        DELETE(peel); DELETE(msg); DELETE(newMsg); DELETE(dataKey);
	DELETE(dPathPtr);
        continue;
      }

      MsgField * dData = NULL;
      if (mix->cipherer->DecryptWithSymetricKey(dataKey, eData, &dData) 
          == OK) {
	DELETE(dataKey); DELETE(eData); 
      }
      else {
	WriteString(llWarning, __FILE__ ":%d:Data decrypting failed in "
		    "Translator, throwing message away.", __LINE__);

	DELETE(msg); DELETE(newMsg); DELETE(peel); DELETE(eData);
        DELETE(dPathPtr); DELETE(dataKey);
	continue;
      }

      GMessage * senderMessage = new GMessage();
      GMessage *nextRcpt;
      if ((nextRcpt = peel->GetAsGMessage(GM_RECIPIENT_1)) == NULL) {
	WriteString(llWarning, __FILE__ ":%d:Recipient-1 bad.", __LINE__);

	DELETE(msg); DELETE(newMsg); DELETE(peel); DELETE(dPathPtr);
	DELETE(dData); DELETE(senderMessage);
	continue;
      }
      else {
	senderMessage->SetAsGMessage(GM_RECIPIENT_1, nextRcpt);
	DELETE(nextRcpt);
      }

      if ((nextRcpt = peel->GetAsGMessage(GM_RECIPIENT_2)) == NULL) {
	WriteString(llWarning, __FILE__ ":%d:Recipient-2 bad.", __LINE__);

	DELETE(msg); DELETE(newMsg); DELETE(peel); DELETE(dPathPtr);
	DELETE(dData); DELETE(senderMessage);
	continue;
      }
      else {
	senderMessage->SetAsGMessage(GM_RECIPIENT_2, nextRcpt);
	DELETE(nextRcpt);
      }

      DELETE(peel);

	// GM_PROTOCOL_VERSION and GM_DEBUG and GM_ORIGIN was added to newMsg 
	// somewhere before
      newMsg->SetAsMsgField(GM_DATA, dData);
      DELETE(dData);
      newMsg->DebugHeaders(logFile, "What is in gMsg after data decryption");

      char *newRestOfPath = PaddRestOfPath(dPathPtr, dPathLen, peelLen);
      DELETE(dPathPtr);

      newMsg->SetAsBytes(GM_REST_OF_PATH, newRestOfPath, dPathLen);
      DELETE(newRestOfPath);

	// Pass X-Origin unmodified, this is information for Padder,
	// he *must* strip this before he passes the chunk to sender
      char *tmpChar = NULL;
      if ((tmpChar = msg->GetAsString(GM_ORIGIN)) == NULL) {
	WriteString(llWarning, __FILE__ ":%d:GM_ORIGIN NULL.", __LINE__);

	DELETE(msg); DELETE(newMsg); DELETE(senderMessage);
	continue;
      }
      else {
	senderMessage->SetAsString(GM_ORIGIN, tmpChar);
	DELETE(tmpChar);
      }

      DELETE(msg);

      senderMessage->SetAsGMessage(GM_CHUNK, newMsg);
      newMsg->DebugHeaders(logFile, "Translated message");
      DELETE(newMsg);

      mix->chopperTranslatorToPadder->Append(senderMessage);
      WriteString(llDebug, __FILE__ ":%d:msg passed to Pedro.", __LINE__);
    }
  }
}
