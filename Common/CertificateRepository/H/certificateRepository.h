#ifndef CERTIFICATE_REPOSITORY_H
#define CERTIFICATE_REPOSITORY_H

#define CERTIFICATE_NAME "cert"

#include "./../../Debugable/H/debugable.h"
#include "./../../GMessage/H/gMessage.h"
#include "./../../ConfigFile/H/configFile.h"

#include <multimap.h>
#include <set>

  /**
    * Array of certificates.
    *
    * Contains Certificate pairs of 'type -> Certificate' that allows for searching for 
    * certificates of specified type
    */
typedef multimap< string, GMessage*, less<string> > Certificates;
  /// Certificates iterator
typedef multimap< string, GMessage*, less<string> >::iterator CertificateIterator;
  /// pair 'type -> Certificate'
typedef multimap< string, GMessage*, less<string> >::value_type CertificateValue;

/**
 * Repository for various certificates.
 *
 * Stores certificates in memory and is able to seve them to disk and again load them.
 * It is also able to return certificates of desired type.
 *
 * @author  Venca
 * @see     AddressManager MessageCreator
 */
class CertificateRepository : public Debugable
{
  protected:

  /**@name attributes */
  /*@{*/
    ///Stored certificates
    Certificates certificates;
    /// list of allowed types
    set< string, less<string> > allowedTypes;
    /// Do we allow everything
    bool allowAllTypes;
    ///Configuration file
    ConfigFile * configFile;
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
    /// constructor
    CertificateRepository(ConfigFile * cmConfigFile, LogFile *cmLogFile);
    /// constructor
    CertificateRepository(ConfigFile * cmConfigFile, LogFile *cmLogFile, 
                       set<string,less<string> > allowedTypes);
      /// Return certificates of specified type
    GMessage * GetCertificates(Size count, string type);
      /// Return certificate of specified type
    GMessage * GetCertificate(string type);
      /// Insert certificates into repository
    void PutCertificates(GMessage *certifcates);
      /// Is allowed to store in repository
    bool IsAllowedType(string type);
      /// Load certificates from disk
    Err LoadCertificates();
      /// Save certificates to disk
    Err SaveCertificates();
      /// Free certificates from memory
    void DeleteCertificates();
      /// Delete certificates in certificates directory
    Err ClearDirectory(string dir);
    ~CertificateRepository();

  /*@}*/
};
#endif
