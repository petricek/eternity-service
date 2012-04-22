#include "./H/majordomo.h"
#include "./../../Common/GMessage/H/esoAC.h"
#include "./../../Common/RandomGenerator/H/randomGenerator.h"

/**
 * Returns pointer to ACManeger.
 *
 * Returns pointer to ACManager of given type, but does
 * not create a copy of this manager, so DON'T EVER 
 * DELETE this value. All ACManagers are owned by Bank.
 *
 *
 * @param   type one of ACTYPE_... values identifying ACManager
 * @return  ACManager of given type
 * @author  Marek
 * @see     Bank, SixMajordomo, ACManager
 */
ACManager *
Majordomo::GetPtrToACManager(Char *type)
{
  WriteString(llDebug, "Majordomo::GetPtrToACManager() - start");
  if (strcmp(type, ACTYPE_THIS_SIX) == 0)
    return ((Bank *) six)->thisSixACManager;

  return NULL;
}

/**
 * Generates certificate key pair.
 *
 * Generates new access certificate key pair.
 *
 *
 * @param   acid        Generated ID of newly generated access
 *                      certificate
 * @param   acPubKey    Public key of generated key pair
 * @return  error code
 * @author  Marek
 * @see     Bank, SixMajordomo, Cipherer
 */
Err 
Majordomo::GenerateACKeys(ACID *acid, GMessage **acPubKey)
{

   //Generate ACID
  Void *tmp;
  Err result;
  Size aSize;

  RandomGenerator *randGen = new RandomGenerator(logFile);
  if (randGen == NULL)
    return KO;

  MsgField *acidMsg = randGen->RandomBytes(sizeof(ACID));
  if (acidMsg == NULL){
    DELETE(randGen);
    return KO;
  }
  DELETE(randGen);

  result = OK;
  *acid = *(ACID*)(tmp = acidMsg->GetAsBytes(&aSize));
  if(tmp == NULL)
    return KO;

   //Generate keyPair
  GMessage *keyPair = six->cipherer->GenerateAsymetricKey();
  if(keyPair == NULL)
  {
    return KO;
  }

  *acPubKey = keyPair->GetAsGMessage(GM_PUBLIC_KEY_INFO);
  GMessage *acPrivKey = keyPair->GetAsGMessage(GM_PRIVATE_KEY_INFO);
  DELETE(keyPair);

  result = ((Bank *) six)->keyMgr->SaveKeys(*acid, *acPubKey, acPrivKey);

  DELETE(acPrivKey);

  return result;
}
