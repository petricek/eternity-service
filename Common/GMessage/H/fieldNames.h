#include "../../H/commands.h"

#ifndef CONST_H
#define CONST_H

      ///fields in the message to and from MIX
      /*@{*/
    #define ME_MSGFLD_COMMAND GM_COMMAND
    #define ME_MSGFLD_PARAMETERS GM_PARAMETERS
      /*@}*/

      ///fields in parameterflds of messages to and from MIX        
      /*@{*/
    #define PFLD_TO_AC GM_ONION
    #define PFLD_DATA_TO_SEND GM_DATA
    #define PFLD_ADD_HOPS GM_ADD_HOPS
      /*@}*/

      ///commands in MIX-SIX communication
    #define EM_SEND_DATA CMD_SEND_DATA

#endif
