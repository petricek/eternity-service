  // has to be the FIRST include !
#include "./../../Common/H/common.h"

#include <string>

#include "./../../Common/H/labels.h"
#include "./../../Common/GMessage/H/gMessage.h"
#include "./../../Common/Cipherer/H/cipherer.h"
#include "./../../Common/Utils/H/mem.h"

#include "./H/messageCreator.h"

/**
 * Creates the onion with owner identification in the bottom.
 *
 * Onion is a parallel for ,,an address''. It is the identification of
 * some service that is  running somewhere. The onion
 * carries the information about the chain of Mixes through the chunk
 * has to go to the recipient. In each peel, there can be several
 * recipients (in the last one, only the final recipient has to be
 * included).  This redundancy is useful for reliability (when one Mix
 * in a peel failes, we have still some Mixes which we can try to
 * connect to) as well as for security enhancement. This security
 * enhancement is in the fact that the message consisted of many
 * chunks is not feeded through the one exact channel only, but is
 * spreaded a bit. It is more difficult to tail this stream of chunks
 * for enemy.
 *
 * @param   onionProperties What is needed for onion creation:
 * <pre>
 * GM_HOP_COUNT		- how many peels we are to create
 * GM_ORIGIN		- whose idenfification will be in the very bottom (eg.
 *                        eso_johny, joes_acs etc., the important fact is
 *                        that this string identifies the Six above the final
 *                        Mix, so is is not the Internet address)
 * </pre>
 * @return  Onion.
 * @author  Pechy
 * @see     CreateChunk(), CreateOnionFromMixCertificate(), certificates, sixs' identification, image of onion and message on our www site
 */
GMessage *
MessageCreator::CreateOnion(GMessage *onionProperties)
{

  WriteString(llDebug, __FILE__ ":%d:CreateOnion() entered.", __LINE__);

  WriteString(llDebug, __FILE__ ":%d:What is in onionProperties?", __LINE__);
  onionProperties->DebugHeaders(logFile);

  char *xOrigin = NULL;
  if ((xOrigin = onionProperties->GetAsString(GM_ORIGIN)) == NULL) {
    WriteString(llWarning, __FILE__ ":%d:GM_ORIGIN failed.", 
                 __LINE__);
    return NULL;
  }

    // ugly hack, ask Pechy, he tells you that you are right
    // for explanation of ``++'' see README file
  Size *hopCount = NULL;
  if ((hopCount = (Size *) onionProperties->GetAsBytes(GM_HOP_COUNT))
      == NULL) {
    WriteString(llWarning, __FILE__ ":%d:GM_HOP_COUNT failed.", 
                 __LINE__);
    DELETE(xOrigin);
    return NULL;
  }

    // check whether the request conform to the defined restriction
  if (*hopCount > MAX_HOPS_TO_ASK_FOR) {
    WriteString(llWarning, __FILE__ ":%d:%s wants onion with %lu hops, "
                "but limit is %lu only. Hop count adjusted.", __LINE__, 
		xOrigin, *hopCount, MAX_HOPS_TO_ASK_FOR);
    *hopCount = MAX_HOPS_TO_ASK_FOR;
  }

  ++(*hopCount);
  onionProperties->SetAsBytes(GM_HOP_COUNT, hopCount, sizeof(Size));
  WriteString(llDebug, __FILE__ ":%d:Request for onion with %lu hops.", 
              __LINE__, *hopCount);
  DELETE(hopCount);

    // we have to generate symetric data keys and *also* obtain mix's 
    // certificates, because symetric keys are saved in the very onion 
    // bottom and certificates are related to this (all recipients in 
    // one peel must support the particular cipher) 
    //
    // the method makes use of GM_HOP_COUNT only from the onionProperties
  GMessage *callBackOnionInfo= NULL;
  if (ReturnCallBackOnionInfo( onionProperties, &callBackOnionInfo) == KO) {
    WriteString(llWarning, __FILE__ ":%d:ReturnCallBackOnionInfo failed.", 
                 __LINE__);
    DELETE(xOrigin);
    return NULL;
  }

    // ugly hack, ask Pechy for explanation
  if (AddLocalCertifs( callBackOnionInfo) == KO) {
    WriteString(llWarning, __FILE__ ":%d:AddLocalCertifs failed.", 
                 __LINE__);
    DELETE(xOrigin);
    return NULL;
  }

    // this makes what can be considered an onion (= ,,callback address''),
    // is's just a hack..., we need something to be able start from,
    // normally we get ,,callback address'' from a local Six, when he wants
    // to answer the request from the Net
    //
    // specially now, we need GM_SYMETRIC_DATA_KEY_INFOS here to put them 
    // into the onion bottom
  GMessage *symKeys = NULL;
  if ((symKeys = 
    callBackOnionInfo->GetAsGMessage(GM_SYMETRIC_DATA_KEY_INFOS)) == NULL) {
    WriteString(llWarning, __FILE__ ":%d:GM_SYMETRIC_DATA_KEY_INFOS failed.", 
                __LINE__);
    DELETE(xOrigin);
    return NULL;   
  }

  GMessage *onion = NULL;
  GMessage *publicKeyInfo = NULL;
  if ((onion = CreateOnionBottom(xOrigin, symKeys, &publicKeyInfo)) == NULL) {
    WriteString(llWarning, __FILE__ ":%d:CreateOnionBottom failed.", 
                __LINE__);
    DELETE(xOrigin); DELETE(symKeys);
    return NULL;   
  }
  else {
    WriteString(llDebug, __FILE__ ":%d:What is in generated onion bottom?", 
		__LINE__);
    onion->DebugHeaders(logFile);
    DELETE(symKeys); DELETE(xOrigin);
  }

    // now, whether creating a new onion (ie. NOW !) or layering the real 
    // callback one (in CreateChunk below), we can make use of the 
    // following method, the onion bottom was created just in the way
    // following method could be used to avoid a code duplicity
  if (MakeMoreOnionLayers( onionProperties, callBackOnionInfo, onion) == KO) {
    WriteString(llWarning, __FILE__ ":%d:MakeMoreOnionLayers failed.", 
                __LINE__);
    //DELETE(onion); ????
    return NULL;   
  }
  else {
    DELETE(callBackOnionInfo);
  } 

    // hurahh
  WriteString(llDebug, __FILE__ ":%d:What is in onion after "
              "MakeMoreOnionLayers() in CreateOnion()?", __LINE__);
  onion->DebugHeaders(logFile);
  onion->SetAsGMessage(GM_PUBLIC_KEY_INFO, publicKeyInfo);
  DELETE(publicKeyInfo);

    // now, padd the onion to the defined length
  Size tmpRestOfPathLen;
  char *tmpRestOfPath = (char *) onion->GetAsBytes(GM_REST_OF_PATH, 
                                                   &tmpRestOfPathLen);
  WriteString(llDebug, __FILE__ ":%d:Onion len before padding is %lu", 
              __LINE__, tmpRestOfPathLen);

  char *fixedSizedRestOfPath = PaddOnionRestOfPath(tmpRestOfPath,
                                                   tmpRestOfPathLen,
						   HALF_REST_OF_PATH_LEN);
  DELETE(tmpRestOfPath);
  onion->SetAsBytes(GM_REST_OF_PATH, fixedSizedRestOfPath, 
                    HALF_REST_OF_PATH_LEN);
  DELETE(fixedSizedRestOfPath);

  return onion;
}

/**
 * Message Creator constructor.
 *
 * Saves object pointers he gets in parameters for future use.
 *
 * @param   mcConfigFile Configuration file.
 * @param   messageCreatorLogFile Log file.
 * @param   certificateRepository Certificate repository object.
 * @author  Pechy
 * @see     Translator, Cipherer, ~MessageCreator()
 */
MessageCreator::MessageCreator(ConfigFile *mcConfigFile, 
                               LogFile * messageCreatorLogFile,
                               CertificateRepository * certificateRepository,
			       Cipherer * cipherer,
                               KeyManager * keyManager)
  :Debugable(messageCreatorLogFile)
{
  this->configFile = mcConfigFile;
  this->keyManager = keyManager;
  this->certificateRepository = certificateRepository;
  this->cipherer = cipherer;
}

/**
 * Message Creator destructor.
 *
 * Now does nothing since Message Creator is stateless.
 *
 * @author  Pechy
 * @see	    MessageCreator()
 */
MessageCreator::~MessageCreator()
{

}

/**
 * Creates the onion with Mix's identification in the bottom.
 *
 * Sometimes Mix is the one that asks for something another server
 * (eg. Acs, on the behalf of a Six or Mix himself), so we need an
 * onion, whose recipient in the bottom is directly Mix.
 *
 * @param   onionProperties What is needed for the onion creation.
 * <pre>
 *   GM_DESTINATION		- which Mix object will get the answer
 *   GM_MIX_CERTIFICATE		- local identification of Mix
 *   M_HOP_COUNT		- how many peels to create
 * </pre>
 * @return  Onion.
 * @author  Pechy
 * @see     CreateOnion(), MessageIDRepository
 */
GMessage * 
MessageCreator::CreateOnionFromMixCertificate( GMessage *onionProperties)
{

  WriteString(llDebug, __FILE__ ":%d:CreateOnionFromMixCertificate() "
              "entered.", __LINE__);

  WriteString(llDebug, __FILE__ ":%d:What is in onionProperties?", __LINE__);
  onionProperties->DebugHeaders(logFile);

    // this ,,key'' won't be used at all, but MakeMoreOnionLayers relies 
    // on it, it's another fucking hack by Pechy...
  GMessage *symKeyInfo1 = new GMessage();
  symKeyInfo1->SetAsString("There-Can-Be-Anything", "");
  GMessage *symKeys = new GMessage();
  symKeys->SetAsGMessage("Symetric-Key-Info-1", symKeyInfo1);
  DELETE(symKeyInfo1);

    // 1 if for the smallest onion, we have Mix's certificate, where
    // Acs is running, but we can operate with onions only, so 
    // make an onion from an IP address
  Size tmpCount = 1;
  symKeys->SetAsBytes(GM_SYMETRIC_KEY_COUNT, &tmpCount, sizeof(Size));

  GMessage *callBackOnionInfo = new GMessage();
  callBackOnionInfo->SetAsGMessage(GM_SYMETRIC_DATA_KEY_INFOS, symKeys);

  char *destination = onionProperties->GetAsString(GM_DESTINATION);
    // symKeys here won't be used at all, but for ,,consistency''...
  GMessage *onion = NULL;
  GMessage *publicKeyInfo = NULL;
  if ((onion = CreateOnionBottom(destination, symKeys, &publicKeyInfo)) 
      == NULL) {
    WriteString(llDebug, __FILE__ ":%d:CreateOnionBottom failed.", __LINE__);
    DELETE(destination); DELETE(symKeys); DELETE(callBackOnionInfo);
    return NULL;
  }
  else {
    DELETE(destination);
    DELETE(symKeys);
  }

  GMessage *certificate = onionProperties->GetAsGMessage(GM_MIX_CERTIFICATE);

    // make a tuple form GM_MIX_CERTIFICATEs we got in onionProperties
  GMessage *tuple = new GMessage();
  tuple->SetAsGMessage(GM_MIX_CERTIFICATE_1, certificate);
  tuple->SetAsGMessage(GM_MIX_CERTIFICATE_2, certificate);
  DELETE(certificate);

  GMessage *tuples = new GMessage();
  tuples->SetAsGMessage("Mix-Certificate-Tuple-1", tuple);
  DELETE(tuple);

  callBackOnionInfo->SetAsGMessage(GM_MIX_CERTIFICATES, tuples);
  DELETE(tuples);

  Size hopCount = 1;
  onionProperties->SetAsBytes(GM_HOP_COUNT, &hopCount, sizeof(Size));

  if (MakeMoreOnionLayers( onionProperties, callBackOnionInfo, onion) == KO) {
    WriteString(llDebug, __FILE__ ":%d:MakeMoreOnionLayers failed.", __LINE__);
    DELETE(callBackOnionInfo);
    return NULL;
  }
  else {
    DELETE(callBackOnionInfo);
  }

    // hurahh...
  WriteString(llDebug, __FILE__ ":%d:What is in onion after "
              "MakeMoreOnionLayers() in CreateOnionFromMixCertificate()?", 
	       __LINE__);
  onion->DebugHeaders(logFile);
  onion->SetAsGMessage(GM_PUBLIC_KEY_INFO, publicKeyInfo);
  DELETE(publicKeyInfo);

    // now, padd the onion to the defined length
  Size tmpRestOfPathLen;
  char *tmpRestOfPath = (char *) onion->GetAsBytes(GM_REST_OF_PATH, 
                                                   &tmpRestOfPathLen);
  WriteString(llDebug, __FILE__ ":%d:Onion len before padding is %lu", 
              __LINE__, tmpRestOfPathLen);

  char *fixedSizedRestOfPath = PaddOnionRestOfPath(tmpRestOfPath,
                                                   tmpRestOfPathLen,
						   HALF_REST_OF_PATH_LEN);
  DELETE(tmpRestOfPath);
  onion->SetAsBytes(GM_REST_OF_PATH, fixedSizedRestOfPath, 
                    HALF_REST_OF_PATH_LEN);
  DELETE(fixedSizedRestOfPath);

  return onion;
}

/**
 * Creates a chunk. Chunk is a part of message, is of fixed size.
 *
 * When anybody wants to send a message, the message can be of any
 * size.  To make traffic analysis harder, we send the message in fix
 * sized chunks.  The method creates one chunk. 
 *
 * @param   	chunkProperties Information needed to create the chunk:
 * <pre>
 *   GM_ONION		- chunk's recipient idenfification
 *   GM_ADD_HOPS	- how many peels to add on the top of the onion
 *   GM_DATA		- the data thas are to be transmitted inside 
 *                        the chunk, we are not interested in its structure
 *                        here.
 * </pre>
 *
 * @return  Cooked chunk:
 * <pre>
 *   GM_RECIPIENT_1		- see CreateOnion() above for discussion on
 *                                on recipients in a peel
 *   GM_RECIPIENT_2
 *   GM_CHUNK
 *     GM_DEBUG			- some string, for debugging purpose
 *     GM_PROTOCOL_VERSION	- Mix-Mix protocol identification
 *     GM_DATA			- encrypted data 
 *     GM_REST_OF_PATH		- rest of path
 * </pre>
 * @author  Pechy
 * @see     CreateOnion(), MessageIDRepository, Receiver, Sender.
 */
GMessage *MessageCreator::CreateChunk(GMessage * chunkProperties)
{
  WriteString(llDebug, __FILE__ ":%d:What is in chunkProperties?", 
              __LINE__);
  chunkProperties->DebugHeaders(logFile);
  
  Size *hopCount = NULL;
  if ((hopCount = (Size *) chunkProperties->GetAsBytes(GM_ADD_HOPS)) == NULL) {
    WriteString(llWarning, __FILE__ ":%d:GM_ADD_HOPS failed.", __LINE__);
    return NULL;
  }

    // check whether the request conform to the defined restriction
  if (*hopCount > MAX_HOPS_TO_ASK_FOR) {
    WriteString(llWarning, __FILE__ ":%d:Somebody wants to peel %lu hops, "
                "but limit is %lu only. Hop count adjusted.", __LINE__,
                 *hopCount, MAX_HOPS_TO_ASK_FOR);
    *hopCount = MAX_HOPS_TO_ASK_FOR;
    chunkProperties->SetAsBytes(GM_ADD_HOPS, hopCount, sizeof(Size));
  }
  DELETE(hopCount);

    // see CreateOnion for comment
  GMessage *callBackOnionInfo = NULL;
  if (ReturnCallBackOnionInfo( chunkProperties, &callBackOnionInfo) == KO) {
    WriteString(llWarning, __FILE__ ":%d:ReturnCallBackOnionInfo failed.",
                __LINE__);
    return NULL;
  }

  WriteString(llDebug, __FILE__ ":%d:What is in returned callBackOnionInfo?", 
              __LINE__);
  callBackOnionInfo->DebugHeaders(logFile);

    // GM_ONION is a ,,callback'' address that we have to start with (we
    // layer some additional peels to hide the sender (us)
  GMessage *onion = NULL;
  if ((onion = chunkProperties->GetAsGMessage(GM_ONION)) == NULL) {
    WriteString(llWarning, __FILE__ ":%d:GM_ONION failed.", __LINE__);
    DELETE(callBackOnionInfo);
    return NULL;
  }

  MsgField *gmData = NULL;
  if ((gmData = chunkProperties->GetAsMsgField(GM_DATA)) == NULL) {
    WriteString(llWarning, __FILE__ ":%d:GM_DATA failed.", __LINE__);
    DELETE(callBackOnionInfo); DELETE(onion);
    return NULL;
  }

  GMessage *publicKeyInfo = NULL;
  if ((publicKeyInfo = onion->GetAsGMessage(GM_PUBLIC_KEY_INFO)) == NULL) {
    WriteString(llWarning, __FILE__ ":%d:GM_PUBLIC_KEY_INFO failed.",
                 __LINE__);
    DELETE(callBackOnionInfo); DELETE(onion); DELETE(gmData);
    return NULL;
  }
  else {
    onion->Delete(GM_PUBLIC_KEY_INFO);
  }

    // see CreateOnion for comment...
  if (MakeMoreOnionLayers(chunkProperties, callBackOnionInfo, onion) == KO) {
    WriteString(llWarning, __FILE__ ":%d:MakeMoreOnionLayers failed.", 
                __LINE__);
    DELETE(callBackOnionInfo); DELETE(onion); DELETE(gmData);
    return NULL;
  }
  else {
    WriteString(llDebug, __FILE__ ":%d:What is in onion after "
		" MakeMoreOnionLayers?", __LINE__);
    onion->DebugHeaders(logFile);
  }

    // against the ,,middle atack'', we have to use asym-key pair for
    // data encryption
  MsgField *eData = NULL;
  if (cipherer->EncryptWithPublicKey(publicKeyInfo, gmData, &eData) == KO) {
    WriteString(llWarning, __FILE__ ":%d:EncryptWithPublicKey failed.", 
                __LINE__);
    DELETE(gmData); DELETE(onion); DELETE(publicKeyInfo);
    DELETE(callBackOnionInfo);
    return(NULL);
  }
  else {
    DELETE(gmData);
    DELETE(publicKeyInfo);
  }
 
    // now we have layered the GM_ONION, but also we have to create peels
    // over GM_DATA, so firstly get symetric keys (that were already used
    // for layering the onion in MakeMoreOnionLayers just above)
    //
    // no need for error checking, was in ReturnCallBackOnionInfo()
  GMessage *symKeys;
  symKeys = callBackOnionInfo->GetAsGMessage(GM_SYMETRIC_DATA_KEY_INFOS);
  DELETE(callBackOnionInfo);

    // to encrypt or to decrypt..., ask Shakespeare; he told us but we 
    // forgot that
  MsgField *eDataStream;
  if (cipherer->EncryptWithSymetricKeys(symKeys, eData, &eDataStream) 
     == KO) {
    WriteString(llWarning, __FILE__ ":%d:EncryptWithSymetricKeys failed.", 
                __LINE__);

    DELETE(eData); DELETE(symKeys); DELETE(onion);
    return(NULL);
  }
  else {
    DELETE(eData);
    DELETE(symKeys);
  }

    // finally, create the chunk that is to go througt The Net, except 
    // adding recipients - one of them is choosen by Sender and included
    // into the chunk (see method header)
  GMessage *chunk = new GMessage();
  chunk->SetAsMsgField(GM_DATA, eDataStream);
  DELETE(eDataStream);
  chunk->SetAsString(GM_PROTOCOL_VERSION, MIX_MIX_PROTOCOL_VERSION);
    // include what you want as a debug string... (chunk origin ???)
  chunk->SetAsString(GM_DEBUG, LBL_NO_DEBUG_NOTE);

    // padd GM_REST_OF_PATH to be always the same size, no need for error
    // checking (was in MakeMoreOnionLayers())
  Size tmpRestOfPathLen;
  char *restOfPath = (char *) onion->GetAsBytes(GM_REST_OF_PATH, 
                                                &tmpRestOfPathLen); 
  char *newRestOfPath = PaddOnionRestOfPath(restOfPath, tmpRestOfPathLen,
                                            FULL_REST_OF_PATH_LEN);
  DELETE(restOfPath);
  chunk->SetAsBytes(GM_REST_OF_PATH, newRestOfPath, FULL_REST_OF_PATH_LEN);
  WriteString(llDebug, __FILE__ ":%d:Chunk with newRestOfPath?", __LINE__);
  chunk->DebugHeaders(logFile);
  DELETE(newRestOfPath);

    // almost hidden here in our code, The Chunk is started to be 
    // created NOW !!!
  GMessage *msgForSender = new GMessage();
  msgForSender->SetAsGMessage(GM_CHUNK, chunk);
  DELETE(chunk);

    // the first two recipients we have to take from just layered onion, no
    // need for error checking, was in  MakeMoreOnionLayers())
  GMessage *recipient = onion->GetAsGMessage(GM_RECIPIENT_1);
  msgForSender->SetAsGMessage(GM_RECIPIENT_1, recipient);
  DELETE(recipient);
  recipient = onion->GetAsGMessage(GM_RECIPIENT_2);
  DELETE(onion);
  msgForSender->SetAsGMessage(GM_RECIPIENT_2, recipient);
  DELETE(recipient);

  return msgForSender;
}
