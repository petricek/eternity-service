  // has to be the *FIRST* include !
#include "./../../Common/H/common.h"

#include <string>

#include "./../../Common/H/labels.h"

#include "./H/messageCreator.h"

/**
 * Returns Mixs' certificates.
 *
 * When we create chunks or onions, we add some peels onto the top of the
 * particular onion. Each peel means to have 2 recipients, ie. 2 Mixs'
 * certificates.
 *
 * @param   tupleWidth How many recipients are in one peel.
 * @param   hopCount How many certificates' tuples to return.
 * @param   certificates Returned certificates.
 * @return  KO if requested count can't be satisfied, OK otherwise.
 * <pre>
 *   GM_MIX_CERTIFICATE_COUNT
 *   GM_MIX_CERTIFICATE_TUPLE_1
 *     GM_MIX_CERTIFICATE_1
 *     GM_MIX_CERTIFICATE_2
 *   ...
 *   ...
 *   GM_MIX_CERTIFICATE_TUPLE-"Hop-Count"
 * </pre>
 * @author  Pechy
 * @see     CreateOnion(), CreateChunk()
 */
Err 
MessageCreator::ReturnGMsgFullOfMixCertifs(int tupleWidth, Size hopCount,
                                           GMessage **certificates)
{
  int savedOptions = logFile->SetDebugOptions(llAll);

  GMessage *oneCertificate;

  (*certificates) = new GMessage();
  (*certificates)->SetAsBytes(GM_MIX_CERTIFICATE_COUNT, 
                              (void *) &hopCount, sizeof(Size));

  WriteString(llDebug, __FILE__ ":%d:Request for %lu tuples.", __LINE__, 
              hopCount);

  for ( Size i = 0; i < hopCount; ++i) {

    GMessage *tuple = new GMessage();

    for ( Size j = 0; j < (unsigned) tupleWidth; ++j) {

      if ( (oneCertificate = certificateRepository->GetCertificate(ACTYPE_MIX)) 
            != NULL) {

        string *tmpStr = CreateString(GM_MIX_CERTIFICATE, j + 1, NO_PARAM);
        tuple->SetAsGMessage((char *) tmpStr->c_str(), oneCertificate);
        DELETE(oneCertificate); DELETE(tmpStr);
      }
      else {
        WriteString(llWarning, __FILE__ ":%d:No certificate !", __LINE__);
	DELETE(*certificates); DELETE(tuple);
	return KO;
      }
    }

    string *tmpTupleStr = CreateString(GM_MIX_CERTIFICATE_TUPLE, i + 1,
                                       NO_PARAM);
    WriteString(llDebug, __FILE__ ":%d:Returned Mix's certificate tuple:", 
                __LINE__);
    tuple->DebugHeaders(logFile);

    (*certificates)->SetAsGMessage((char *) tmpTupleStr->c_str(), tuple);
    DELETE(tmpTupleStr); DELETE(tuple);
  }

  WriteString(llDebug, __FILE__ ":%d:Returned full msg with certificates:", 
              __LINE__);
  (*certificates)->DebugHeaders(logFile);

  logFile->SetDebugOptions(savedOptions);

  return OK;
}

/**
 * Returns GMessage full of useful stuff for creating onions or adding 
 * new peels onto the top of them.
 *
 * When we create new onions or add some peels onto the of them, we
 * have to symetric keys for encryption and Mixs' certificates to make
 * peels. Generating of new symetric keys and returning of mixs' certificates 
 * should be mutually related. Recipients in one tuple (peel) has to 
 * support the same symetric cipher and on the contrary, the particular 
 * symetric key cipher has to be supported by all recipients in the tuple.
 * We do <b>*NOT*</b> honor this rule for now - but this method (and 
 * several others) could be easily modified to support this (onionProperties
 * would carry more information needed).
 *
 * @param   onionProperties How many peels will be created:
 * <pre>
 *   GM_HOP_COUNT
 * </pre>
 * @param   callBackOnionInfo Returned parameter.
 * @return  GMessage with the stuff:
 * <pre>
 *   GM_SYMETRIC_DATA_KEY_INFOS
 *   GM_MIX_CERTIFICATES
 * </pre>
 * @author  Pechy
 * @see     MakeMoreOnionLayers(), CreateChunk(), CreateOnion()
 */
Err
MessageCreator::ReturnCallBackOnionInfo(GMessage *onionProperties,
                                        GMessage **callBackOnionInfo)
{
  int savedOptions = logFile->SetDebugOptions(llAll);

  WriteString(llDebug, __FILE__ ":%d:ReturnCallBackOnionInfo() entered.", 
              __LINE__);

  WriteString(llDebug, __FILE__ ":%d:What is in onionProperties?", 
              __LINE__);
  onionProperties->DebugHeaders(logFile);

    // be careful ;-) here...
  Size *hopCount;
  if ((hopCount = (Size *) onionProperties->GetAsBytes(GM_HOP_COUNT, NULL)) 
       == NULL) {
    hopCount = (Size *) onionProperties->GetAsBytes(GM_ADD_HOPS, NULL);
    WriteString(llDebug, __FILE__ ":%d:Wanted NULL GM_HOP_COUNT.", 
                __LINE__);
  }

  int tupleWidth = RECIPIENTS_IN_ONE_PEEL;

    // for encrypting GM_DATA part of future message
  GMessage *symDataKeys;
  WriteString(llDebug, __FILE__ ":%d:Hop-Count is %lu.", __LINE__, *hopCount);
  symDataKeys = cipherer->ReturnGMsgFullOfSymKeys( *hopCount);

  GMessage *certificates;
  if (ReturnGMsgFullOfMixCertifs( tupleWidth, *hopCount, &certificates) 
      == KO) {
    WriteString(llWarning, __FILE__ ":%d:ReturnGMsgFullOfMixCertifs "
                "failed.", __LINE__);
    return KO;
  }

    // finally full callBackOnionInfo
  *callBackOnionInfo = new GMessage();
  (*callBackOnionInfo)->SetAsGMessage(GM_SYMETRIC_DATA_KEY_INFOS, symDataKeys);
  (*callBackOnionInfo)->SetAsGMessage(GM_MIX_CERTIFICATES, certificates);

  DELETE(certificates); DELETE(symDataKeys); DELETE(hopCount);

  logFile->SetDebugOptions(savedOptions);

  return OK;
}
