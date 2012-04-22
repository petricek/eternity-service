#include "./H/majordomo.h"
#include "./../../Common/GMessage/H/esoAC.h"

/**
 * Generates access certificate for this Bank.
 *
 * Creates asymetric keys and generates access certificate of this
 * Bank using the keys and the onion.
 *
 *
 * @param   onion Onion to use in AC.
 * @param   generatedAC AC generated in this method.
 * @return  error code
 * @author  Marek
 * @see     Six Eso::GenerateThisSixAC
 */
Err 
Majordomo::GenerateThisSixAC(MsgField *onion, GMessage **generatedAC)
{
  EsoAC *ac;
  GMessage *acPubKey;
  ACID acid;
  Err result;
 
  DBG;
  if(GenerateACKeys(&acid, &acPubKey) != OK){
    DBG;
    return KO;
  }

  DBG;
  if((ac = new EsoAC()) == NULL){
    delete acPubKey;
    return KO;
  }

  DBG;
  WriteString(llDebug, "Mjd:CommandOnionGenerated():ac generated");

  result = OK;
  if (ac->SetFields(ACTYPE_BANK, acid, onion, acPubKey, acPubKey) != OK)
    result = KO;

  DELETE(acPubKey);

  *generatedAC = ac;

  return result;
}
 
/**
 * Saves the generated AC to ACManager.
 *
 * This is an overriden method from Six. In Bank it is necessary to
 * set generated AC to ACManager.
 *
 *
 * @param   generatedAC Generated AC to handle.
 * @return  error code
 * @author  Marek
 * @see     Eso::HandleGeneratedAC(), Client::HandleGeneratedAC()
 */
Err 
Majordomo::HandleGeneratedAC(GMessage *generatedAC)
{
  GMessage *mixMsg;

    DBG;
    if(InsertAC(ACTYPE_THIS_SIX, generatedAC) != OK)    
      WriteString(llWarning, "Mjd::Cannot save generated AC");
    DBG;
    mixMsg = CreatePublishCertificateMsg(generatedAC);

      //put it into outMsgQueue
    if(mixMsg == NULL){
      WriteString(llError, "Majordomo::CommandControllACCount():"
                           "Cannot create mixMsg");
    }
    else
      outMsgQueue->Insert(mixMsg);
   DBG;
   return OK;
  }



