  // has to be the *FIRST* include !
#include "./../../Common/H/common.h"

#include <string>

#include "./../../Common/H/labels.h"
#include "./../../Common/Utils/H/mem.h"

#include "./H/messageCreator.h"

/**
 * Creates an onion bottom with Six's identification (or Mix's sometimes).
 *
 * In the bottom there is an identification of the recipient running
 * above the final Mix (sometimes the recipient can be Mix itself).
 * The bottom also includes all symetric keys that were used to
 * encrypt the data part of chunk as the chunk went along the Mixes
 * from this onion. The sym keys are included because we needn't
 * assign a unique ID to the onion and store symetric keys with this
 * ID. Freshly generated public key is also included to the bottom, to
 * make future middle attack harder.
 *
 * @param   xOrigin The name of the recipient.
 * @param   symKeys Symetric keys we have to insert into the bottom.
 * @param   pubKeyInfo Symetric keys we have to insert into the bottom.
 * @return  Bottom GMessage:
 * <pre>
 *   GM_FINAL_STOP			- a nonempty field, identifies 
 *                                        that the peel is tha last one
 *   GM_RECIPIENT_1
 *     GM_ADDRESS			- a name of the recipient
 *   GM_PRIVATE_KEY_INFO		- private key for data decryption
 *   GM_SYMETRIC_DATA_KEY_INFOS		- symetric keys for data decryption
 * </pre>
 * @author  Pechy
 * @see     CreateOnion(), CreateChunk(), middle attack discussion.
 */
GMessage *MessageCreator::CreateOnionBottom( char *xOrigin, 
                                             GMessage *symKeys,
					     GMessage **pubKeyInfo)
{
    // legendary ``eso_johny'' Six machine, the name was kept due to
    // historical reasons, it replaces a standard name ``recipient''
  GMessage *esoJohny = new GMessage();
  esoJohny->SetAsString(GM_ADDRESS, xOrigin);

    // create the real bottom of the future onion:
    // 
    //   Final-Stop
    //   Recipient-1 (ie. the old American legend ``esoJohny'')
    //   Symetric-Data-Key-Infos
    //   Rest-Of-Path is empty, but *MUST* by included, MakeMoreOnionLayers
    //     method relies on this field, whether empty or on
  GMessage *onionBottom = new GMessage();
  onionBottom->SetAsString(GM_FINAL_STOP, "stop, Stop, STOP :-)");
  onionBottom->SetAsGMessage(GM_RECIPIENT_1, esoJohny);
  DELETE(esoJohny);

    // symKeys can be NULL, if we generate bottom for client-acs 
    // communication
  onionBottom->SetAsGMessage(GM_SYMETRIC_DATA_KEY_INFOS, symKeys);

  GMessage *tmpAsymKeyInfo;
  tmpAsymKeyInfo = cipherer->GenerateAsymetricKey();
  (*pubKeyInfo) = tmpAsymKeyInfo->GetAsGMessage(GM_PUBLIC_KEY_INFO);
  GMessage *tmpPrivKeyInfo;
  tmpPrivKeyInfo = tmpAsymKeyInfo->GetAsGMessage(GM_PRIVATE_KEY_INFO);
  onionBottom->SetAsGMessage(GM_PRIVATE_KEY_INFO, tmpPrivKeyInfo);
  DELETE(tmpPrivKeyInfo); DELETE(tmpAsymKeyInfo);

  return onionBottom;
}

/**
 * Add some peels on the top of given onion. 
 *
 * It gets an onion (recipient's identification) and should add some
 * peels. The onion an be a real onion (real Six's identifification)
 * or an onion bottom created with CreateOnionBottom() method (in this
 * case onion bottom simulates actuall onion.
 * <p>
 * We add layers in a loop, in each loop we do:
 * <ul>
 *   <li> add symetric data key to the onion (this key is used to encode 
 *      the data), this __data__ key is taken from a sym keys' message 
 *      (symDataKeys) taken from symetric  keys' gMsg included in 
 *      callBackOnionInfo 
 *   <li> remove GM_REST_OF_PATH from onion, but convert it to stream 
 *     and keep it aside
 *   <li> convert onion (i.e. already without GM_REST_OF_PATH) to 
 *     a stream and add 4 bytes as a length indicator header
 *   <li> cat both streams (onion-stream first, then GM_REST_OF_PATH) 
 *     and encrypt it with symetric __path__ key, taken from symetric 
 *     keys' gMsg included in callBackOnionInfo
 *   <li> add recipients, each will include symetric path key (from 4.) 
 *     encrypted with respective public key taken from respecive 
 *     certificates
 * </ul>
 * <p>
 * Note: if onion is just an onionBottom, we add in the first step a symetric
 * key that is never used.. This is just that we didn't want to add any
 * additional if, the best programmers are lazy programmers :-).
 *
 * @param   callBackOnionInfo
 * <pre>
 *   GM_SYMETRIC_DATA_KEY_INFOS		- used for encryption in particular
 *                                        peels
 *   GM_MIX_CERTIFICATES		- used for creation recipients' 
 *                                        GMessages in particular peels
 * </pre>
 * @param   properties Contains:
 * <pre>
 *   GM_ADD_HOPS			- when we create a chunk
 * </pre>
 * <p> or
 * <pre>
 *   GM_HOP_COUNT			- when we create a onion
 * </pre>
 * @param   onion Onion on which we have to add additional peels.
 * @return  Onion.
 * <pre>
 *   GM_RECIPIENT_1			- see MakeRecipientGMessage()
 *   GM_RECIPIENT_2
 *   GM_REST_OF_PATH			- is an encrypted GMessage
 *     <<GM_RECIPIENT_1>>
 *     <<GM_RECIPIENT_2>>
 *     <<GM_SYMETRIC_DATA_KEY_INFO>>
 *     <<remained octet-stream>>
 *       <<GM_RECIPIENT_1>>
 *       <<GM_RECIPIENT_2>>
 *       <<GM_SYMETRIC_DATA_KEY_INFO>>
 *       <<remained octet-stream>>
 *         ...
 *         ...
 * </pre>
 *   
 * @author  Pechy
 * @see     CreateOnion(), CreateChunk, Cipherer.
 */
Err
MessageCreator::MakeMoreOnionLayers(GMessage *properties,
                                    GMessage *callBackOnionInfo,
                                    GMessage *onion)
{
  Size *hopCount = NULL;
  if ( (hopCount = (Size*) properties->GetAsBytes(GM_ADD_HOPS)) == NULL)
    hopCount = (Size*) properties->GetAsBytes(GM_HOP_COUNT);

  GMessage *symDataKeys = NULL;
  symDataKeys = callBackOnionInfo->GetAsGMessage(GM_SYMETRIC_DATA_KEY_INFOS);

  WriteString(llDebug, __FILE__ ":%d:Symetric keys's gMessage in "
              "MakeMoreOnionLayers().", __LINE__);
  symDataKeys->DebugHeaders(logFile);
  WriteString(llDebug, __FILE__ ":%d:We have to add %lu peels.", __LINE__,
              *hopCount);

  GMessage *mixCertificates = NULL;
  mixCertificates = callBackOnionInfo->GetAsGMessage(GM_MIX_CERTIFICATES);
  WriteString(llDebug, __FILE__ ":%d:Mix certifs' gMessage in "
              "MakeMoreOnionLayers().", __LINE__);
  mixCertificates->DebugHeaders(logFile);

    // make hopCount peels...
  for ( Size tupleNum = 0; tupleNum < *hopCount; ++tupleNum) {

    WriteString(llDebug, __FILE__ ":%d:Started creating a NEW PEEL.", 
                __LINE__);

      // add data symetric key to old peel 
    string *tmpStr = NULL;
      // no ``-Data-'' in the string, because the gMsg of symetric keys is 
      // generic - it returns just ,,keys'', what you will use them for is
      // your bussines
    tmpStr = CreateString(GM_SYMETRIC_KEY_INFO, tupleNum + 1, NO_PARAM);
      // get symetric data key related to this peel
    GMessage *tmpKey = symDataKeys->GetAsGMessage((char *) tmpStr->c_str()); 
    DELETE(tmpStr);
    onion->SetAsGMessage(GM_SYMETRIC_DATA_KEY_INFO, tmpKey); 
    DELETE(tmpKey);
    
      // get GM_REST_OF_PATH from old peel, keep it aside and then DELETE 
      // it from the onion
    Size restOfPathStreamLen = 0;
    char *restOfPathStream = NULL; 
    restOfPathStream = (char *) onion->GetAsBytes(GM_REST_OF_PATH, 
                                                        &restOfPathStreamLen);
    WriteString(llDebug, __FILE__ ":%d:Rest-Of-Path %lu bytes kept aside.", 
                __LINE__, restOfPathStreamLen);

      // bug in gMessage - she doesn't enable null fields
    if (restOfPathStream)
      onion->Delete(GM_REST_OF_PATH);
    else {
      restOfPathStreamLen = 0;
      WriteString(llDebug, __FILE__ ":%d:GM_REST_OF_PATH was NULL.", __LINE__);
    }

      // create a stream (with additionally 4 bytes) from the rest of onion 
      // (ie. without GM_REST_OF_PATH field; 4 bytes ,,header'' carries 
      // information about the length of the stream
    Size onionOnePeelStreamLen = 0;
    char *onionOnePeelStream = NULL;
    AnnotateWith4ByteLength(onion, &onionOnePeelStream, 
                            &onionOnePeelStreamLen);
    WriteString(llDebug, __FILE__ ":%d:After Annotate onion is %lu long, "
                "before it was %lu.", __LINE__, onionOnePeelStreamLen,
		onion->SizeOf());

      // catenate the two streams
    char *newRestOfPath = NULL;
    int newRestOfPathLen = NULL;
    newRestOfPathLen = mixMemCat(onionOnePeelStream, onionOnePeelStreamLen,
                                 restOfPathStream, restOfPathStreamLen,
                                 &newRestOfPath);

    WriteString(llDebug, __FILE__ ":%d:New newRestOfPathLen is %d.", 
                __LINE__, newRestOfPathLen);

    DELETE(onionOnePeelStream);
    if (restOfPathStreamLen) 
      DELETE(restOfPathStream);

    BytesMsgField *pBottomStream = new BytesMsgField();
    pBottomStream->SetAsBytes((void *) newRestOfPath, newRestOfPathLen);
    WriteString(llDebug, __FILE__ ":%d:newRestOfPathLen inserted into "
                "field, size is %lu.", __LINE__, pBottomStream->SizeOf());
    DELETE(newRestOfPath);

      // generate symetric key for encrypting the contemporary peel, ie.
      // a key for __path__ encryption, *not* for __data__ encryption !
    GMessage *symKey = cipherer->GenerateSymetricKey();

      // symKey we have to keep for future in this cycle, we have to 
      // insert it in a encrypted form into the ,,recipient'' fields
    MsgField *eBottomStream = NULL;
    cipherer->EncryptWithSymetricKey(symKey,pBottomStream,&eBottomStream);
    WriteString(llDebug, __FILE__ ":%d:Encrypted newRestOfPath in field "
                "is %lu bytes long.", __LINE__, eBottomStream->SizeOf());
    DELETE(pBottomStream);

      // now start to construct the new peel, we already have pieces to
      // stick it all together :-)
      // 
      // IMPORTANT NOTE: the string GM_REST_OF_PATH will not appear 
      // anywhere in the chunk except it the first peel, see construction
      // steps -> at first we remove GM_REST_OF_PATH and then add it to the
      // new peel, then remove and add again..., only the last loop finally 
      // adds  the GM_REST_OF_PATH that stays there...
    onion->Clear();
    onion->SetAsMsgField(GM_REST_OF_PATH, eBottomStream);
    DELETE(eBottomStream);

      // in each peel, there have to be at least (now exactly) 2 different
      // recipients, so obtain the recipient tuple for this peel (we got 
      // all these tuples in the callBackOnionInfo as a method parameter)
    string *rcptTupleStr = NULL;
    rcptTupleStr = CreateString(GM_MIX_CERTIFICATE_TUPLE, tupleNum + 1, 
                                NO_PARAM);
    GMessage *rcptTuple = NULL;
    rcptTuple = mixCertificates->GetAsGMessage((char *) rcptTupleStr->c_str());
    DELETE(rcptTupleStr);

      // for each (now always 2) recipient in a tuple create gMsg in onion
    Size rcptNum; 
    for ( rcptNum = 0; rcptNum < RECIPIENTS_IN_ONE_PEEL; ++rcptNum) { 

        // obtain mix certificate (recipient's certificate) from the tuple 
        // and create the recipient gMsg
      string *mixCertifStr = NULL;
      GMessage *mixCertif = NULL;
        // recipients are passed with names "Mix-Certificate-x"
      mixCertifStr = CreateString(GM_MIX_CERTIFICATE, rcptNum + 1, NO_PARAM);
      mixCertif = rcptTuple->GetAsGMessage((char*) mixCertifStr->c_str());
      DELETE(mixCertifStr);

      WriteString(llDebug, __FILE__ ":%d:Certificate for recipient %d "
                  "in peel %d.", __LINE__, rcptNum + 1, tupleNum + 1);
      mixCertif->DebugHeaders(logFile);

        // symKey is the one with which we encrypted the 
        // ,,onion+rest_of_path'' stream, we have to include it for the
        // particular hop on the way will be able to decrypt the path as the 
        // chunk goes through the Net

      WriteString(llDebug, __FILE__ ":%d:What sym key I give into "
                  "MakeRecipientGMessage()?", __LINE__);
      symKey->DebugHeaders(logFile);

      GMessage *recipient = MakeRecipientGMessage(mixCertif, symKey);
      DELETE(mixCertif);

      WriteString(llDebug, __FILE__ ":%d:Made recipient in "
		  " MakeMoreOnionLayers", __LINE__);
      recipient->DebugHeaders(logFile);

	// finally insert just created recipient into the onion
      string *rcptStr = NULL;
      rcptStr = CreateString(GM_RECIPIENT, rcptNum + 1, NO_PARAM);
      onion->SetAsGMessage( (char *) rcptStr->c_str(), recipient);

      DELETE(rcptStr); DELETE(recipient);
    }

    DELETE(rcptTuple); DELETE(symKey);
  }

  DELETE(symDataKeys); DELETE(hopCount); DELETE(mixCertificates);

  return OK;
}
