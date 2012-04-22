#include "H/majordomo.h"
#include "../../Common/H/labels.h"

/**
 * Bank Majordomo constructor.
 *
 * Initializes majordomo.
 *
 *
 * @author  Marek
 * @see     Bank, SixMajordomo
 */
Majordomo::Majordomo(Bank * parent, LogFile * majordomoLogFile)
  : SixMajordomo(parent, majordomoLogFile)
{
  Initialize();
}

/**
 * Initializes Bank Majordomo.
 *
 * Certain (but really COOL) initialization.
 *
 *
 * @return  error code
 * @author  Marek
 * @see     
 */
Err
Majordomo::Initialize()
{
  return OK;
}

/**
 * Gets current time.
 *
 * Uses ESTime method to check current time.
 *
 *
 * @return  error code
 * @author  Marek
 * @see     
 */
ESTime *
Majordomo::GetTime()
{
  return new ESTime();
}


/**
 * Unseals sAuth.
 *
 * Given seald sAuth returns unsealed one using DecryptByACPriv.
 *
 *
 * @param   myACID  ID of access certificate which public key was used
 *                  to seal sAuth
 * @param   sealSA  Sealed sAuth
 * @return  error code
 * @author  Marek
 * @see     DecryptByACPriv()
 */
MsgField *
Majordomo::UnsealSAuth(const ACID myACID, const MsgField *sealSA)
{
  return DecryptByACPriv(myACID, sealSA);
}
