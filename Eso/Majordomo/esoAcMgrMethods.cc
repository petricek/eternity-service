#include "./H/majordomo.h"

/**
 * Returns pointer to ACManager.
 *
 * Returns pointer to ACManager of given type, but does
 * not create a copy of this manager, so DON'T EVER 
 * DELETE this value. All ACManagers are owned by Eso.
 *
 *
 * @param   type one of ACTYPE_... values identifying ACManager
 * @return  ACManager *
 * @author  Marek
 * @see     Eso, SixMajordomo, ACManager
 */
ACManager *
Majordomo::GetPtrToACManager(Char *type)
{
  WriteString(llDebug, "Majordomo::GetPtrToACManager() - start");
  if (strcmp(type, ACTYPE_THIS_SIX) == 0)
    return ((Eso *) six)->thisEsoACManager;
  
  if (strcmp(type, ACTYPE_ESO) == 0)
    return ((Eso *) six)->esoACManager;
  
  if (strcmp(type, ACTYPE_BANK) == 0)
    return ((Eso *) six)->bankACManager;

  return NULL;
}

