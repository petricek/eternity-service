#include "./H/esoAC.h"

/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
Err 
EsoAC::SetFields(Char *certType, ID certID,
                 MsgField *mixCert, 
		 GMessage *pubKey, 
		 GMessage *tcbPubKey,
                 ESTime *notValidBefore, 
		 ESTime *notValidAfter)
{
  Err result;
  ESTime *notBefore;
  ESTime *notAfter;

  if((notBefore = new ESTime()) == NULL)
    return KO;
  if (notValidBefore)
    *notBefore = *notValidBefore;
  
  if((notAfter = new ESTime(CFG_AC_VALIDATION)) == NULL){
    DELETE(notBefore);
    return KO;
  }
  if (notValidAfter)
    *notAfter = *notValidAfter;


  result = OK;
  if(SetAsString(ACFLD_TYPE, certType) != OK)
    result = KO;
  if(SetAsID(ACFLD_ACID, certID) != OK)
    result = KO;
  if(SetAsMsgField(ACFLD_MIX_CERTIFICATE, mixCert) != OK)
    result = KO;
  if(SetAsGMessage(ACFLD_PUB_KEY, pubKey) != OK)
    result = KO;
  if(SetAsGMessage(ACFLD_TCB_PUB_KEY, tcbPubKey) != OK)
    result = KO;
  if(SetAsBasicTime(ACFLD_NOT_VALID_BEFORE, *notBefore) != OK)
    result = KO;
  if(SetAsBasicTime(ACFLD_NOT_VALID_AFTER, *notAfter) != OK)
    result = KO;

  DELETE(notBefore);
  DELETE(notAfter);
  return result;
}

/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
Char *
EsoAC::GetType()
{
  return GetAsString(ACFLD_TYPE);
}

