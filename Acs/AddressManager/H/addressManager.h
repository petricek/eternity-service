#ifndef ADDRESS_MANAGER_H
#define ADDRESS_MANAGER_H

class AddressManager;

//#include <.h>
#include "../../../Common/H/common.h"

#include "../../../Common/Debugable/H/debugable.h"
#include "../../../Common/Registerable/H/registerable.h"

#include "../../AcsOffspring/H/acsOffspring.h"

#include <vector>

/**
 * Access certificates manager.
 *
 * Takes care of expiring expunging and creating certificates that are
 * stored in a container class called CertificateRepository.
 *
 * @author  Venca
 * @see     CertificateRepository
 */
class AddressManager: public Debugable, public AcsOffspring
{
  protected:
  /**@name attributes */
  /*@{*/
      /// Last used certificate ID
    ID lastID;
      /// Certificate of this Acs
    GMessage *acsCertificate;
  /*@}*/
  /**@name methods */
  /*@{*/
      /// Storing certificates to disk
    Err SaveCertificates();
      /// Loading certificates from disk
    Err LoadCertificates();
  /*@}*/

  public:
  /**@name methods */
  /*@{*/
      /// Constructor
    AddressManager(Acs * parent, LogFile * addressManagerLogFile);
      /// Destructor
    ~AddressManager();

      /// Returns one Mix certificate from CertificateRepository
    GMessage * GetMixCertificate();
      /// Returns one Acs certificate from CertificateRepository
    GMessage * CreateAcsCertificate();

      /// Prepare for clean shutdown
    virtual void Shutdown();

      /// Generates a new Acs certificate
    GMessage * CreateAcsCertificate(u_short port, GMessage **certificate);

      /// Writes a pair of asymetric keys in a file anto the disk
    Err StoreAsymetricKeyPairOntoDisk(GMessage *asymetricKeyPair);
  /*@}*/
};
#endif
