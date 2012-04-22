  // has to be the FIRST include !
#include "./../../Common/H/common.h"

#include <stdio.h>
#include <sys/errno.h>
#include <string.h>

#include "./H/bank.h"

#include "./../../Common/LogFile/H/logFile.h"
#include "./../../Common/H/labels.h"

/**
 * Constructor.
 * 
 * Creates all the objects necessary to run.
 *
 *
 * @param   configName   name of configuration file for Bank
 * @author  Marek
 * @see     ConfigFile
 */
Bank::Bank(string configName)
: Six(configName)
{
  Char *tmp;
   
  logFile->WriteString(llDebug, __FILE__ ":%d:Creating static objects.",
		       __LINE__);

  
  thisSixACManager = new ACManager(ACTYPE_THIS_SIX, tmp = GetPath(ACMGR_TABLE_THIS_SIX),
                                   4, 4, logFile);
  DELETE(tmp);
  logFile->WriteString(llDebug, __FILE__ ":%d: ACManager created",
		       __LINE__);

  payMgr = new PayManager(logFile, this);
  logFile->WriteString(llDebug, __FILE__ ":%d: PayManager created",
		       __LINE__);

		       
  keyMgr = new KeyManager(logFile, GetPath(KEYMGR_TABLE));
  logFile->WriteString(llDebug, __FILE__ ":%d: PayManager created",
		       __LINE__);
		       
		       
    // create Majordomo
  CreateMajordomo();

  logFile->WriteString(llDebug, __FILE__ ":%d:Initializing"
		       , __LINE__);
  Initialize();

  logFile->WriteString(llDebug, __FILE__ ":%d:Bank() finished.", __LINE__);

}

/**
 * Destructor.
 * 
 * Deletes all the objects - frees memory.
 *
 *
 * @author  Marek
 * @see     
 */
Bank::~Bank()
{
  logFile->WriteString(llDebug, __FILE__ ":%d:Deleting static objects ...",
		       __LINE__); 

  delete thisSixACManager;
  delete payMgr;
}

/**
 * Creates Bank Majordomo.
 *
 * Overrides Six::CreateMajordomo method to use proper SixMajordomo.
 *
 *
 * @return  void
 * @author  Marek
 * @see     Six, SixMajordomo, Majordomo
 */
void 
Bank::CreateMajordomo()
{
  majordomo = new Majordomo(this, logFile);
}


/**
 * Initializes Bank.
 *
 * Used especially for debugging. These days it simply asks for onion
 * to generate own access certificate.
 *
 *
 * @return  error code
 * @author  Marek
 * @see     AskForOnion()
 */
Err 
Bank::Initialize()
{
  return majordomo->AskForOnion();
}
