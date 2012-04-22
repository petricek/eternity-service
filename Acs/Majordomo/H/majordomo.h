#ifndef ACS_MAJORDOMO_H
#define ACS_MAJORDOMO_H

#include "../../../Common/H/common.h"
#include "../../../Common/Runable/H/runable.h"
#include "../../AcsOffspring/H/acsOffspring.h"

/**
 * Acs majordomo..
 *
 * Processes various requests for certificates comming from network
 * and takes apropriate action like sends back the requested
 * certificates
 *
 *
 * @author  Venca
 * @see     Acs
 */
class Majordomo : public Runable, public AcsOffspring
{
  public:
    
  /**@name methods */
  /*@{*/
      /// Constructor
    Majordomo(Acs * parent, LogFile * majordomoLogFile);
      /// This method runs in thread and processes all the requests
    virtual void * Run(void *);
  /*@}*/
};
#endif
