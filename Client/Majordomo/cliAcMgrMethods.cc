#include "./H/majordomo.h"
#include "./../../Common/GMessage/H/esoAC.h"


/**
 * Returns always NULL.
 *
 * This is an overriden Six method. Because Client has no ACManagers,
 * this method always returns NULL.
 *
 *
 * @param   char* - not used
 * @return  NULL
 * @author  Marek
 * @see     SixMajordomo
 */
ACManager *
Majordomo::GetPtrToACManager(Char *)
{
  return NULL;
}


/**
 * Gets ACs.
 *
 * Overriden Six method. Client has always only one THIS_SIX AC, that
 * is generated every time Client wants to communicate. The getting of
 * THIS_SIX AC is thus overriden, otherwise the inherited method is
 * called.
 * 
 *
 *
 * @param   type Type of ACs to get.
 * @param   count Count of ACs to get.
 * @param   notIn Queue of ACs not wanted.
 * @return  Queue of ACs of given type.
 * @author  Marek
 * @see     SixMajordomo
 */
Queue *
Majordomo::GetACs(Char *type, const int count = 1, Queue *notIn = NULL)
{
DBG;

  if(strcmp(type, ACTYPE_THIS_SIX) == 0) {
    Queue *acQueue;
    GMessage *ac;

DBG;
    if(GetThisSixAC(&ac) != OK){
      return NULL;
    }
DBG;
    if((acQueue = new Queue()) == NULL){
      DELETE(ac);
      return NULL;
    }
DBG;
    acQueue->Insert(ac); 
DBG;
    return acQueue;
  }
  else
    return SixMajordomo::GetACs(type, count, notIn);
}


/**
 * Generates this Six AC.
 *
 * This Six AC is generated every time Client wants to communicate.
 * New tuple of keys is generated and saved.
 *
 *
 * @param   onion Onion to include to AC.
 * @param   generatedAC AC that was generated.
 * @return  error code
 * @author  Marek
 * @see     SixMajordomo
 */
Err 
Majordomo::GenerateClientAC()
{
  Err result;

    MsgField *onionMsgFld;
    GMessage *onion;

    Size hopCount = 0;
    string hopCountString = six->configFile->GetValue(CFG_DEFAULT_ONION_LENGTH);
    hopCount = ((hopCountString != "") ? \
      atol(hopCountString.c_str()) : CFG_HOPS_IN_AC);

    if((onion = GetOnion(hopCount, GetTID())) == NULL)
      return NULL;

    if((onionMsgFld = onion->StoreToMsgField()) == NULL){
      DELETE(onion);
      return NULL;
    }
    DELETE(onion);

  GMessage *ac;
  if(GenerateThisSixAC(onionMsgFld, &ac) != OK){
    return KO;
  }
 DBG;
  Char *tmp;
  if(ac->StoreToFile(tmp = six->GetPath(AC_FILE)) != OK)
    result = KO;
  DELETE(ac);
  DELETE(tmp);
DBG;
 return OK;
}

Err 
Majordomo::GenerateThisSixAC(MsgField *onionMsgFld, GMessage **generatedAC)
{
  ACID acid;
  GMessage *acPubKey;
  Err result;
    
 DBG; 
  if(GenerateACID(&acid)  != OK ) 
    return KO;
DBG;
  if(GetAsymetricKey(&acPubKey) != OK)
    return KO;
DBG;
  EsoAC *ac;
  if((ac = new EsoAC()) == NULL){
    DELETE(acPubKey);
    return KO;
  }
DBG;
  result = OK;
  if(ac->SetFields(ACTYPE_THIS_SIX, acid, onionMsgFld, acPubKey, acPubKey) != OK){
    result = OK;
  }

  DELETE(onionMsgFld);
  DELETE(acPubKey);
  if(result != OK){
    DELETE(ac);
    return KO;
  }
DBG;
  *generatedAC = ac;
DBG;
 return OK;
}


Err
Majordomo::GetThisSixAC(GMessage **ac)
{
  Char *tmp;
  DBG;
  *ac = new GMessage();
  Err result = ((*ac)->LoadFromFile(tmp = six->GetPath(AC_FILE)));
  DELETE(tmp);
  DBG;
  return result;
}
