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
 * AddressManager constructor.
 *
 * Responsible for loading Mix's certificate into memory. Creates the
 * certificate if it is not found.
 *
 * @param   parent Pointer to the Mix object.
 * @param   addressManagerLogFile Log File.
 * @author  Venca, Pechy
 * @see     certificate, Cipherer
 */
AddressManager::AddressManager(Mix * parent, LogFile * addressManagerLogFile)
  :Debugable(addressManagerLogFile), MixOffspring(parent)
{
  lastID = random();
  mixCertificate = new GMessage();
  ConfigFile *cf = mix->configFile;

  LoadCertificates();

  string mixCertif = cf->GetValue(CFG_CERTIFICATES_DIRECTORY) + "/" +
                     cf->GetValue(CFG_CERTIFICATE);

    // check whether mix certificate is stored on disk
  WriteString(llDebug, __FILE__ ":%d:Looking for Mix's certificate in %s.", 
    __LINE__, (char *) mixCertif.c_str());

  if (mixCertificate->LoadFromFile((char *) mixCertif.c_str()) == KO) {
    WriteString(llError, __FILE__ ":%d:Mix's certificate not found.", __LINE__);
    WriteString(llWarning, __FILE__ ":%d:Creating Mix's new certificate.",
                __LINE__);
    CreateMixCertificate((u_short) 
      atoi((cf->GetValue(CFG_FROM_MIX_PORT)).c_str()), &mixCertificate);
      // finally, store the newly created certificate
    mixCertificate->StoreToFile((char *) mixCertif.c_str());
    fprintf(stderr, "New certificate created, run mix again and everything "
                    "will be O.K.\n");
    exit(1);
  }
  else {
    WriteString(llDebug, __FILE__ ":%d:Mix's certificate found, what "
                "is there?", __LINE__);
    mixCertificate->DebugHeaders(logFile);
  }
}

/**
 * Address Manager destructor.
 *
 * Destructs Mix's certificate in memory.
 *
 * @author  Venca
 * @see
 */
AddressManager::~AddressManager()
{
  DELETE(mixCertificate);
}

/**
 * When creating a new certificate, it saves asymetric key pair onto the disk.
 *
 * Saves asymetric key pait onto the disk. This can happen when creating
 * a new certificate (we have to create the new pair, too).
 *
 * @param   asymetricKeyPair Key pair to store.
 * @return  KO if the pair can't be saved, OK otherwise.
 * @author  Pechy
 * @see     Cipherer
 */
Err
AddressManager::StoreAsymetricKeyPairOntoDisk(GMessage *asymetricKeyPair)
{
  string keyPair = mix->configFile->GetValue(CFG_CERTIFICATES_DIRECTORY) 
                   + "/" + mix->configFile->GetValue(CFG_ASYMETRIC_KEY_PAIR);

  WriteString(llDebug, __FILE__ ":%d:What is in new asymetricKeyPair?", 
              __LINE__);
  asymetricKeyPair->DebugHeaders(logFile);

  if (asymetricKeyPair->StoreToFile((char *) keyPair.c_str()) == KO) {
    WriteString(llError, __FILE__ ":%d:Can't save Mix's asymetric key "
                "pair to disk.", __LINE__);
    return KO;
  }

  WriteString(llWarning, __FILE__ ":%d:New Mix's asymetric key pair saved to "
              "disk.", __LINE__);

  return OK;
}

/**
 * Creates a new Mix's certificate.
 *
 * Finds out the hostname of Mix and creates the new certificate and
 * stores it onto the disk.
 *
 * @param   port Port which we include in the new certificate.
 * @param   certificate Where we return the certificate.
 * @return
 * <pre>
 *   GM_CERTIFICATE_TYPE	- the name of a server
 *   GM_ADDRESS			- DNS of IP address
 *   GM_PORT			- port where the server is running
 *   GM_PUBLIC_KEY_INFO		- public key from the server's key pair
 *   GM_NOT_VALID_BEFORE	- validity
 *   GM_NOT_VALID_AFTER		- validity
 * </pre>
 * @see     Cipherer, certificates
 */
Err AddressManager::CreateMixCertificate(u_short port, GMessage **certificate)
{
  char hostName[MAX_HOST_NAME_STRING_LEN];

  if ( gethostname(hostName, MAX_HOST_NAME_STRING_LEN) == -1)
    WriteString(llError, __FILE__ ":%d:Gethostname() failed.", __LINE__);

  GMessage *asymetricKey;
  asymetricKey = mix->cipherer->GenerateAsymetricKey();
  StoreAsymetricKeyPairOntoDisk(asymetricKey);

  WriteString(llDebug, __FILE__ ":%d:Port in certificate is %u.", 
    __LINE__, (unsigned int) port);

  WriteString(llDebug, __FILE__ ":%d:Address in certificate is %s.", 
    __LINE__, hostName);

  GMessage *pubKey = asymetricKey->GetAsGMessage(GM_PUBLIC_KEY_INFO);
    // was here because of debugging purpose
  //GMessage *privKey = asymetricKey->GetAsGMessage(GM_PRIVATE_KEY_INFO);
  DELETE(asymetricKey);

  (*certificate) = new GMessage();
  (*certificate)->SetAsString(GM_CERTIFICATE_TYPE, ACTYPE_MIX);
  (*certificate)->SetAsString(GM_ADDRESS, hostName);
  (*certificate)->SetAsBytes(GM_PORT, (void *) &port, (Size) sizeof(u_short));
  (*certificate)->SetAsString(GM_NOT_VALID_BEFORE, "");
  (*certificate)->SetAsString(GM_NOT_VALID_AFTER, "");
  (*certificate)->SetAsGMessage(GM_PUBLIC_KEY_INFO, pubKey);
  DELETE(pubKey);

  return OK;
}

/**
 * Returns Mix's own certificate.
 *
 * Returns Mix's own certificate, that is stored somewhere in the
 * disk. We consider the Mix machine as a safe one.
 *
 * @param   mixCertificate Where to return the certificate.
 * @return  KO if not found, OK otherwise.
 * @author  Venca
 * @see     certificates
 */
Err
AddressManager::ReturnLocalMixCertificate( GMessage **mixCertificate)
{
  *mixCertificate = new GMessage(*(this->mixCertificate));
  return OK;
}

/**
 * Returns a certificate of any Mix.
 *
 * Returns a certificate of any Mix. These certificates should be in 
 * repository. If there are not enough of certificates, a request for
 * Acs is made and sent.
 *
 * @param   mixCertificate Where to return the certificate.
 * @return  KO if not found, OK otherwise.
 * @author  Venca
 * @see     certificates
 */
Err
AddressManager::ReturnMixCertificate( GMessage **mixCertificate)
{
  *mixCertificate = mix->certificateRepository->GetCertificate(GM_MIX);
  return OK;
}

/**
 * Returns a certificate of any Acs.
 *
 * Returns a certificate of any Acs.
 *
 * @param   acsCertificate Where to return the certificate.
 * @return  KO if not found, OK otherwise.
 * @author  Venca
 * @see     certificates
 */
Err
AddressManager::ReturnAcsCertificate( GMessage **acsCertificate)
{
  *acsCertificate = mix->certificateRepository->GetCertificate(ACTYPE_ACS);
  return OK;
}

/**
 * For shutdowning, not implemented.
 *
 * For shutdowning, not implemented. This method should be called
 * whenever a SIGTERM is caugth.
 *
 * @author  Venca
 * @see
 */
void
AddressManager::Shutdown()
{
}

/**
 * Store given certificates.
 *
 * When Address Manager asks for certificates, this method is called
 * when certificates are returned, ready to be stored. 
 *
 * @param   data Certificates.
 * @return  void
 * @author  Venca
 * @see     certificates
 */
void
AddressManager::HereYouAre(GMessage * data)
{
  GMessage *certs = data->GetAsGMessage(GM_CERTIFICATES);
  mix->certificateRepository->PutCertificates(certs);
  DELETE(certs);
  DELETE(data);
}

/**
 * Stores given certificates.
 *
 * When Address Manager asks for certificates, this method is called
 * when certificates are returned ready to be stored. 
 *
 * @param   data Certificates.
 * @return  void
 * @author  Venca
 * @see     certificates
 */
Err
AddressManager::LoadCertificates()
{
  return mix->certificateRepository->LoadCertificates();
}

/**
 * Saves certificates from memory onto the disk.
 *
 * Saves certificates from memory onto the disk. This method is called
 * before destroying the object AddressManager.
 *
 * @param   void
 * @return  KO if not succesful, OK otherwise
 * @author  Venca
 * @see     certificates
 */
Err
AddressManager::SaveCertificates()
{
  return mix->certificateRepository->SaveCertificates();
}

/**
 * Sends a request to ACS for certificates.
 *
 * When some Six asks for certificates and Mix doesn't have enough 
 * of them of this type, we have to download them from an Acs.
 *
 * @param   count How many certificates to download.
 * @param   type What certificate to download.
 * @return  void
 * @author  Venca
 * @see     certificates, Acs
 */
void
AddressManager::DownloadCertificates(Size count, string type)
{
    // Get certificate of an ACS
  GMessage *acsCert;
  ReturnAcsCertificate(&acsCert);
   // Extract onion
  GMessage * onion = acsCert->GetAsGMessage(GM_ONION);
  DELETE(acsCert);
  
    //Construct request for ACS
  char strCount[10];
  sprintf(strCount, "%ld", count);
  GMessage *request = new GMessage();
  request->SetAsString(GM_COUNT,strCount);
  request->SetAsString(GM_CERTIFICATE_TYPE,(char *)type.c_str());
  char id[10];
  sprintf(id, "%ld", lastID);
  request->SetAsString(GM_REQUEST_ID, id);
  mix->messageIDRepository->Register(lastID, this);
  lastID++;

   // Condtruct message for ACS
  GMessage *acsmessage = new GMessage();
  acsmessage->SetAsGMessage(GM_PARAMETERS, request);
  acsmessage->SetAsString(GM_COMMAND, CMD_GET_CERTIFICATES);

   // Construct parameters for message creator
  GMessage *message = new GMessage();
  message->SetAsGMessage(GM_DATA,acsmessage);
  message->SetAsGMessage(GM_ONION,onion);

  Size hops = atoi(mix->configFile->
    GetValue(CFG_ADD_HOPS).c_str());

  char strHops[10];
  sprintf(strHops, "%ld", hops);
  message->SetAsString("AddHops",strHops);
  
   // Create a message for sending over network
  GMessage *chunk = mix->msgCreator->CreateChunk(message);

   // Pass it further
  mix->chopperTranslatorToPadder->Append(chunk);
}

/**
 * Stores incoming certificates.
 *
 * Stores certificates from ACS to its internal structure.  Asumes
 * that it gets a GMessage of certificates, that are GMessages too.
 *
 * @param   data Certificates.
 * <pre>
 *   GM_CERTIFICATE_1
 *   GM_CERTIFICATE_2
 *   ...
 *   GM_CERTIFICATE_"n"
 * </pre>
 * @return  void
 * @author  Venca
 * @see     certificates, Acs, GMessage
 */
void
AddressManager::CertificatesArrived(GMessage *data)
{
  mix->certificateRepository->PutCertificates(data);
}
