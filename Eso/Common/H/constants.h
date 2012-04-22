#ifndef ESO_CONSTANTS_H
#define ESO_CONSTANTS_H

/**@name Information needed during Eso initialization */
/*@{*/
   /// Home directory for Eso server
#define CFG_ESO_PATH "eso_path"
   /// Initialization flag
#define CFG_INITIALIZE "initialize"
/*@}*/
  
/**@name Max number of tries to get money from bank */
/*@{*/
#define MAX_BANKER_TRIES 5
/// Constants from cfg file
#define CFG_FINDER_SEARCH_TIME_FOR_1_HOP 2L
/*@}*/

/// Version  of protocol 
#define ESOOTHERS_PROTOCOL_VERSION_BETA "Eso_others_Beta"

/**@name Timeouts */
/*@{*/
/// Time out period specification
#define TO_ALLOC_REL_TIMEOUT "00:00:01:00:00:00"
/// Time out period specification for Banker
#define TO_BANKER_REL_TIMEOUT "00:00:00:01:00:00" //one hour
/*@}*/

/**@name Classes' paths relative to Eso path */
/*@{*/
/// Path for data of ACManager
#define ESO_PATH_ACMANAGER "ACManager/"
/// Path for data of Allocator
#define ESO_PATH_ALLOCATOR "Allocator/"
/*@}*/

/**@name ACManagers' tables */
/*@{*/
/// Path where Bank AC is stored 
#define ACMGR_TABLE_BANK     ESO_PATH_ACMANAGER"bank.acs"
#define ACMGR_TABLE_ESO      ESO_PATH_ACMANAGER"eso.acs"
#define ACMGR_TABLE_THIS_ESO ESO_PATH_ACMANAGER"thisEso.acs"
/*@}*/

#endif

