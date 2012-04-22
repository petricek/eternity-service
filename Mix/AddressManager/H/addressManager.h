#ifndef ADDRESS_MANAGER_H
#define ADDRESS_MANAGER_H

class AddressManager;

#include "../../../Common/H/common.h"

#include "../../../Common/Debugable/H/debugable.h"
#include "../../../Common/Registerable/H/registerable.h"

#include "../../MixOffspring/H/mixOffspring.h"

#include <vector>

/**
 * Mix address manager.
 *
 * Takes care of generation storage and expiration of mix's access
 * certificates. This includes access certificates of Acs' and other
 * Mixes.
 *
 * @author  Pechy, Venca
 * @see     Mix, CertificateRepository
 */
class AddressManager: public Debugable, public MixOffspring, 
                      public Registerable
{
  protected:

  /**@name attributes */
  /*@{*/
    ID lastID;
    GMessage *mixCertificate;
  /*@}*/
  /**@name methods */
  /*@{*/
      /// downloading certificates from ACS
    void DownloadCertificates(Size count, string type);
      /// Storing certificates to disk
    Err SaveCertificates();
      /// Loading certificates from disk
    Err LoadCertificates();
  /*@}*/

  public:
  /**@name methods */
  /*@{*/
    AddressManager(Mix * parent, LogFile * addressManagerLogFile);
    ~AddressManager();

    Err ReturnMixCertificate( GMessage **mixCertificate);
    Err ReturnLocalMixCertificate( GMessage **mixCertificate);
    Err ReturnAcsCertificate( GMessage **acsCertificate);
    virtual void Shutdown();
    virtual void HereYouAre(GMessage * data);

    void CertificatesArrived(GMessage *data);

    Err CreateMixCertificate(u_short port, GMessage **certificate);

    Err StoreAsymetricKeyPairOntoDisk(GMessage *asymetricKeyPair);
  /*@}*/
};
#endif
