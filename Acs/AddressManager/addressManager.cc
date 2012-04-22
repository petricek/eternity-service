  // has to be the FIRST include !
#include "./../../Common/H/common.h"

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>

#include "./../../Common/H/labels.h"
#include "./../../Common/H/commands.h"

#include "./../../Common/ConfigFile/H/configFile.h"
#include "./H/addressManager.h"

/**
 * Address manager constructor.
 *
 * Loads Acs certificate from file and if it is not present there then
 * it generates a new one which is written to disk for fututre use.
 *
 * @author  Venca
 * @see     CFG_CERTIFICATES_DIRECTORY, CFG_CERTIFICATE
 */
AddressManager::AddressManager(Acs * parent, LogFile * addressManagerLogFile)
  :Debugable(addressManagerLogFile), AcsOffspring(parent)
{
  lastID = random();
  acsCertificate = new GMessage();
  ConfigFile *cf = acs->configFile;

  LoadCertificates();

  string acsCertif = cf->GetValue(CFG_CERTIFICATES_DIRECTORY) + "/" +
                     cf->GetValue(CFG_CERTIFICATE);

    // check whether acs certificate is stored on disk
  WriteString(llDebug, __FILE__ ":%d:Looking for Acs's certificate in %s.", 
    __LINE__, (char *) acsCertif.c_str());

  if (acsCertificate->LoadFromFile((char *) acsCertif.c_str()) == KO) {
    WriteString(llError, __FILE__ ":%d:Acs's certificate not found.", __LINE__);
    WriteString(llWarning, __FILE__ ":%d:Creating Acs's new certificate.",
                __LINE__);
    acsCertificate = CreateAcsCertificate();
    if(acsCertificate == NULL)
    {
      WriteString(llError, __FILE__ ":%d:generating Acs certificate.", __LINE__);
      exit(KO); 
    }
      // finally, store the newly created certificate
    acsCertificate->StoreToFile((char *) acsCertif.c_str());
    fprintf(stderr, "New certificate created, run acs again and everything "
                    "will be O.K.\n");
    exit(1);
  }
  else {
    WriteString(llDebug, __FILE__ ":%d:Acs's certificate found, what "
                "is there?", __LINE__);
    acsCertificate->DebugHeaders(logFile);
  }
}

/**
 * Acs address manger destructor.
 *
 * It just deletes Acs certificate from memory.
 *
 * @author  Venca
 * @see     AddressManager(), Acs
 */
AddressManager::~AddressManager()
{
  DELETE(acsCertificate);
}

/**
 * Prepare for clean shutdown.
 *
 * This method should implement all the tasks necessary for
 * preparation for a clean shutdown like saving inconsistent data to
 * disk etc. Now it is empty as AddressManger is completely stateless
 * as it stores certificate to disk immediatelly after it is
 * generated.
 *
 * @author  Venca
 * @see     Shutdownable, Killer
 */
void 
AddressManager::Shutdown()
{
}

/**
 * Load certificates from disk.
 *
 * Forces certificate repository to load all certificates from disk.
 *
 * @author  Venca
 * @see     SaveCertificates()
 */
Err 
AddressManager::LoadCertificates()
{
  return acs->certificateRepository->LoadCertificates();
}

/**
 * Save certificates to disk.
 *
 * Forces certificate repository to save all certificates to disk.
 *
 * @author  Venca
 * @see     LoadCertificates()
 */
Err 
AddressManager::SaveCertificates()
{
  return acs->certificateRepository->SaveCertificates();
}

/**
 * Get Mix certificate.
 *
 * Returns a Mix certificate stored on disk.
 *
 * @author  Venca
 * @see     GetAcsCertificate
 */
GMessage * 
AddressManager::GetMixCertificate()
{
  string path = acs->configFile->GetValue(CFG_CERTIFICATES_DIRECTORY);
  string filename = acs->configFile->GetValue(CFG_MIX_CERTIFICATE);
  GMessage * mixCertificate = new GMessage();
  if(mixCertificate->LoadFromFile((char *)(path + "/" + filename).c_str()) == KO)
  {
    WriteString(llError, "loading local mix certificate: %s",(path + "/" + filename).c_str());
    return NULL;
  }
  else
  {
    WriteString(llDebug, "loading local mix certificate: %s",(path + "/" + filename).c_str());
    return mixCertificate;
  }
}

/**
 * Create Acs certifcate.
 *
 * Generates an Acs certificate when there is missing.
 * <p>
 * Structure of the certificate is:
 * <pre> 
 * Certificate-Type: Acs
 * Name: acs-johny
 * Mix-Certificate: 
 *   Address:
 *   ...
 *   Port
 * </pre> 
 * @author  Venca
 * @see     AddressManager, Acs
 */
GMessage * 
AddressManager::CreateAcsCertificate()
{
  GMessage * acsCertificate = new GMessage();
  GMessage * mixCertificate = GetMixCertificate();
  if(mixCertificate == NULL)
  {
    DELETE(acsCertificate);
    WriteString(llError, "got NULL from getmixcertificate");
    return NULL;
  }
  acsCertificate->SetAsString(GM_CERTIFICATE_TYPE, ACTYPE_ACS);
  acsCertificate->SetAsGMessage(GM_MIX_CERTIFICATE, mixCertificate);
  DELETE(mixCertificate);
  acsCertificate->SetAsString(GM_NAME, (char *)(acs->configFile->GetValue(CFG_NAME).c_str()));
  acsCertificate->SetAsString(GM_NOT_VALID_BEFORE, "");
  acsCertificate->SetAsString(GM_NOT_VALID_AFTER, "");
  return acsCertificate;
}
