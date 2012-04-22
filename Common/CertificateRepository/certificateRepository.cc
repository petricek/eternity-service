// Stores all different types of certificates and sends it to those who 
// request it.
//
// For random data we use random(), initializing of the random  for 
// random() function is done in Mix::Run().

  // has to be the *FIRST* include !
#include "./../H/common.h"

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>

#include "./H/certificateRepository.h"
#include "./../H/labels.h"

#include <string>

/**
 * Certificate repository constructor.
 *
 * Certificate repository stores different types of certificates and is able to return 
 * certificates of specified type. Certificates are stored internally in a STL container called map
 * that is implemented like lookup tree so the lookups of certificates efficient.
 *
 * @param   cmConfigFile   Configuration file.
 * @param   cmLogFile      Log file for debug messages.
 * @author  Venca
 * @see     AddressManager, GetCertificate()
 */
CertificateRepository::CertificateRepository(ConfigFile * cmConfigFile, 
                                             LogFile *cmLogFile)
 : Debugable( cmLogFile )
{
  WriteString(llDebug, __FILE__ ":%d:CertificateRepository constructor "
              "entered.", __LINE__);

  allowAllTypes = true;
  configFile = cmConfigFile;
}

/**
 * Certificate repository constructor.
 *
 * Certificate repository stores different types of certificates and is able to return 
 * certificates of specified type. Certificates are stored internally in a STL container called map
 * that is implemented like lookup tree so the lookups of certificates efficient.<br>
 * This version of constructor takes an additional parameter that specifies what certificates can be stored 
 * in this instance of certificate repository.
 *
 * @param   cmConfigFile     Configuration file.
 * @param   cmLogFile        Log file for debug messages.
 * @param   allowedTypes     list (STL list) of types that can be stored in repository.
 * @author  Venca
 * @see     AddressManager, GetCertificate()
 */
CertificateRepository::CertificateRepository(ConfigFile * cmConfigFile,
                                       LogFile *cmLogFile, 
                                       set<string, less<string> > allowedTypes)
 : Debugable( cmLogFile )
{
  allowAllTypes = false;
  configFile = cmConfigFile;
  this->allowedTypes = allowedTypes;
}

/**
 * Certificate repository destructor.
 *
 * Frees all stored certificates from memmory.
 *
 * @author  Venca
 * @see     PutCertificate(), CertificateRepository()
 */
CertificateRepository::~CertificateRepository()
{
  WriteString(llDebug, __FILE__ ":%d:CertificateRepository destructor"
              "entered.", __LINE__);
  DeleteCertificates();
}

/**
 * Returns a bunch of certificates.
 *
 * Get count of random certificates with type "type". Internally implemented as count 
 * of calls to GetCertificate()
 *
 * @author  Venca
 * @param  count   Number of certificates to return.
 * @param  type    Type of certificates we want.
 * @see     GetCertificate(), PutCertificates()
 */
GMessage * 
CertificateRepository::GetCertificates(Size count, string type)
{
  int oldOpt = SetDebugOptions(llWarning | llError | llFatalError);

  WriteString(llDebug, __FILE__ ":%d:Wanted %lu certifs of type '%s'.", 
              __LINE__, count, type.c_str());

  GMessage *certifs = new GMessage();
  char id[10];

  GMessage *tmpGM = NULL;

  for(Size i = 0; i < count; i++)
  {
    sprintf(id, "%lu", i + 1);
    tmpGM = GetCertificate(type);
    if(tmpGM == NULL)
    {
      DELETE(certifs);
      WriteString(llError, __FILE__ ":%d:returning null certificates",
        __LINE__);
      return NULL;
    }

    WriteString(llDebug, __FILE__ ":%d:Inserting Certificate-%s.", 
                __LINE__, id); 
    certifs->SetAsGMessage( (char*)(string("Certificate-") + id).c_str(), tmpGM);
    DELETE(tmpGM);
  }

  //certifs->DebugHeaders(logFile,"returning certifs");

  SetDebugOptions(oldOpt);

  return certifs;
}

/**
 * Returns random certificate.
 *
 * Return random certificate of type "type".
 *
 * @param   type   Type of certificate to return
 * @return  Pointer to a new GMessage containing sertificate of type 'type' 
 * @author  Venca
 * @see     PutCertificate(), AddressManager
 */
GMessage * 
CertificateRepository::GetCertificate(string type)
{
  int oldOpt = SetDebugOptions(llWarning | llError | llFatalError);

  CertificateIterator start;  
  start = certificates.lower_bound(type);
  Size count = certificates.count(type);
  WriteString(llDebug, __FILE__ ":%d:Count of %s is %lu.", __LINE__,
              type.c_str(), count);

  if (count == 0)
  {
    WriteString(llError, __FILE__ ":%d:Returning null certificate", __LINE__);
    return NULL;
  }
  Size offset = random() % count;

  WriteString(llDebug, __FILE__ ":%d:Offset is %lu.", __LINE__, offset);
  //((*start).second)->DebugHeaders(logFile,"start:");

  while(offset) {
    start++;
    offset--;
  }

  //((*(start)).second)->DebugHeaders(logFile,"chosen:");
    // A hack that makes a copy of GMessage
  GMessage * tempCert = new GMessage(); 
  tempCert->SetAsGMessage("Convert", (*(start)).second );
  GMessage * cert = tempCert->GetAsGMessage("Convert");
  DELETE(tempCert);
  //cert->DebugHeaders(logFile,"returning:");

  SetDebugOptions(oldOpt);

  return cert;
}

/**
 * Inserts certificate into certificate repository.
 *
 * Checks the type against its list of allowed types and if it is there then the certificate is stored.
 *
 * @param   data   Type of certificate to return
 * @author  Venca
 * @see     GetCertificate(), AddressManager
 */
void
CertificateRepository::PutCertificates(GMessage *data)
{
  char id[10];
  int i = 1;
  GMessage *cert;
  data->DebugHeaders(logFile,"put into certificate repository");
    // For every certificate that arrived
  sprintf(id,"%d",i);
  for(cert = data->GetAsGMessage((char *)(string("Certificate-") + id).c_str());
      cert != NULL;
      i++,
      sprintf(id,"%d",i),
      cert = data->GetAsGMessage((char *)(string("Certificate-") + id).c_str())) 
  {
    string type = cert->GetAsString(GM_CERTIFICATE_TYPE);
      // Test if it is an allowed certificate type
    if(IsAllowedType(type))
    {
        // and store it
      this->certificates.insert(CertificateValue( type, cert ));
    }
    else
        // or warn me
      WriteString(llWarning, "Not allowed certificate type: %s", type.c_str());
  }
    // Save all certificates to disk
  SaveCertificates();

}

/**
 * Is the type allowed to be stored in repository.
 *
 * Checks if the type is between those allowed types that were specified in constructor - default is
 * allow all.
 *
 * @param   type     Certificate type to check.
 * @return  true if allowed false otherwise.
 * @author  Venca
 * @see     PutCertificate(), PutCertificates()
 */
bool 
CertificateRepository::IsAllowedType(string type)
{
  if(allowAllTypes)
    return true;
  else
  {
    if(allowedTypes.find(type) == allowedTypes.end())
    {
      return false;
    }
    else return true;
  }
}

/**
 * Load certificates from disk.
 *
 * From directory specified in config file reads number of certificates 
 * and then inserts all the certififcates into its search tree.
 *
 * @return  OK if siccessful KO otherwise.
 * @author  Venca
 * @see     SaveCertificates()
 */
Err
CertificateRepository::LoadCertificates()
{
  GMessage *certificate=NULL;
  string name = "cert";
  string filename;
  char id[10];
  Size count = 0;
    // Get certificate directory from config file
  string certificatesDir; 
  if(configFile == NULL)
    return KO;
  certificatesDir = configFile->GetValue(CFG_CERTIFICATES_DIRECTORY);
  WriteString(llDebug, "Certificates Directory:%s:",certificatesDir.c_str());

   // Find out how many certificates there are in the dir
  FILE * f = fopen((certificatesDir + "/Count").c_str(),"r");
  if(f == NULL)
  {
    WriteString(llError, "Error opening file:",(certificatesDir + "/Count").c_str());
    return KO;
  }
    
  int * pcount = new int;
  fscanf(f, "%d", pcount);
  fclose(f);
  count = *pcount;
  
  WriteString(llDebug, "Loading %ld certificates", count);

    // For every file in the directory
  for(Size i=0; i< count; i++)
  {
    sprintf(id, "%ld", i);
    filename = certificatesDir + "/" + name + id;

      // Extract certificate from file
    certificate = new GMessage();
    if(certificate->LoadFromFile((char *)filename.c_str()) == KO)
    {
      WriteString(llFatalError, 
                  "Error reading certificate: %s",
                  (char *)filename.c_str());
      return KO;
    }

    certificate->DebugHeaders(logFile,"Certificate from disk:");
      // and store it with its type
    string type = string(certificate->GetAsString(GM_CERTIFICATE_TYPE)); 
    certificates.insert( CertificateValue(type, certificate) );
  }

  return OK;
}

/**
 * Delete certificates from memmory.
 *
 * Goes through all the certificates stored in repository and deletes them.
 *
 * @return  void
 * @author  Venca
 * @see     LoadCertificates(), ~CertificateRepository()
 */
void
CertificateRepository::DeleteCertificates()
{
  GMessage *certificate = NULL;
              
  Size i;
  CertificateIterator iter;
    // For all certificates in our internal structure
  for(iter=certificates.begin(),i = 0; 
      !(iter == certificates.end()); 
      iter++)
  { 
    // DELETE it
    certificate = (*iter).second;
    certificates.erase(iter);
    DELETE(certificate)
  }
}

/**
 * Save certificates to disk.
 *
 * Clears the directory specified in configuration file and then writes 
 * all the certificates in repository to separate files. It also writes 
 * a certificate Count in a special file.
 *
 * @return  OK if successful KO otherwise.
 * @author  Venca
 * @see     LoadCertificates()
 */
Err
CertificateRepository::SaveCertificates()
{
  GMessage *certificate = NULL;
  string name = CERTIFICATE_NAME;
  string filename;
  char id[10];
              
    // Get directory from config file
  string certificatesDir;
  if(configFile == NULL)
    return KO;
  certificatesDir = configFile->GetValue(CFG_CERTIFICATES_DIRECTORY);

  ClearDirectory(certificatesDir);
  
  Size i;
  CertificateIterator iter;
  Err result = OK;
    // For all certificates in our internal structure
  for(iter=certificates.begin(),i = 0; 
      !(iter == certificates.end()); 
      iter++,i++)
  { 
      // Write certificate to file
    sprintf(id, "%lu", i);
    filename = certificatesDir + "/" + name + id;
    certificate = (*iter).second;
    if(certificate->StoreToFile((char *)filename.c_str()) == KO) {
      fprintf(stderr, "by Pechy -> certificate->StoreToFile FAILED !\n");
      result = KO;
    }
  }

  // write number of certificates in the dir
  FILE * fd = fopen((certificatesDir + "/Count").c_str(),"w");
  fprintf(fd, "%lu",i);
  fclose(fd);
  return result;
}

/**
 * Clears certificates from directory.
 *
 * deletes all the certificates stored in this directory (cert0 ... certn)
 * Useful before writting new certificates.
 *
 * @param   dir     Directory to clear
 * @return  OK if successful KO otherwise
 * @author  Venca
 * @see     SaveCertificates()
 */
Err
CertificateRepository::ClearDirectory(string dir)
{
  
  int len = strlen(CERTIFICATE_NAME);
  DIR *dirp = opendir(dir.c_str());
  if(dirp == NULL)
    return KO;
  struct dirent* dp;
  while ((dp = readdir(dirp)) != NULL)
  {
    if (!strncmp(dp->d_name, CERTIFICATE_NAME, len))
    {
      string filename = dir + "/" + dp->d_name;
      if(unlink(filename.c_str()))
      {
        WriteString(llError, __FILE__ "Error unlinking file: %s", 
	            filename.c_str());
      }
    }
  }
  
  (void)closedir(dirp);

  return OK;

}
