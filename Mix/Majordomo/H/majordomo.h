#ifndef MAJORDOMO_H
#define MAJORDOMO_H

#include "./../../../Common/H/common.h"
#include "./../../../Common/Runable/H/runable.h"
#include "./../../MixOffspring/H/mixOffspring.h"

/**
 * Mix majordomo.
 *
 * Processes various requests comming from network and from SIX also
 * distributes messages to apropriate objects.
 * <p>
 * Commands that it handles are: generate_onion get_certificates
 * publish_certificates.
 *
 * @author  Pechy, Venca
 * @see     MessageCreator, GM_COMMAND
 */
class Majordomo : public Runable, public MixOffspring
{
  public:

  /**@name methods */
  /*@{*/
    Majordomo(Mix * parent, LogFile * majordomoLogFile);
    virtual void * Run(void *);
  /*@}*/

  private:

  /**@name methods */
  /*@{*/
    void GetCertificates(GMessage *requets);
    void PublishCertificates(GMessage *requets);
  /*@}*/
};
#endif
