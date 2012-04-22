  // has to be the FIRST include !
#include "./../../Common/H/common.h"

#include <stdio.h>
#include <sys/errno.h>
#include <string.h>
#include <unistd.h>

#include "./H/client.h"

#include "./../../Common/H/labels.h"
#include "./../Majordomo/H/majordomo.h"

  // Global parameters                                                                                          
extern char * configName;
extern char * fileKeys;
extern char * fileName;
extern char * storeTime;
extern CommandOption commandOption;                                                                         


/**
 * Constructor.
 *
 * Creates all the objects necessary to run.
 *
 *
 * @param   configName name of config flie
 * @return  
 * @author  Pavel 
 * @see     client Banker, client Majordomo
 */
Client::Client(string configName) 
: Six(configName)
{
  this->commandOption = commandOption;
  this->fileName = fileName;
  this->fileKeys = fileKeys;

#ifndef NO_BANKER
  banker = new Banker(logFile, this);
#endif

  if (InitializeRandomGenerator() == KO) {
    logFile->WriteString(llWarning, __FILE__ ":%d:Error in seeding "
                         "random generator.", __LINE__);
  }
 
    //
  CreateMajordomo();

  
  certificateRepository = new CertificateRepository(configFile, logFile);

  if(certificateRepository->LoadCertificates() == KO) {
    logFile->WriteString(llError, __FILE__ ":%d:Reading certificates", 
                         __LINE__);
  }
}

/**
 * Destructor of Client.
 *
 * Deletes all objects necessary to delete.
 *
 *
 * @return  
 * @author  Pavel 
 * @see     Banker 
 */
Client::~Client()
{
#ifndef NO_BANKER
  if(banker)
    DELETE(banker);
#endif
}


/**
 * Initializes random generator.
 *
 * We suppose that we will use only random() function, because we seed
 * with srandom().
 * We use /dev/urandom, but /dev/random would be better, but for simplicity
 * and debugging purpose this is enough.
 *
 *
 * @return  error code 
 * @author  Pechy
 * @see     RandomGenerator 
 */
Err 
Client::InitializeRandomGenerator()
{
  int readBytes,
      err = KO;
    // if getting a seed fails, use 1 as a default
  unsigned long randLong = 1;

    // for debugging purpose, we can read the seed for client's config
    // file
  string randomLongString = configFile->GetValue(CFG_DEBUG_RANDOM_LONG_SEED);

  if (randomLongString != "") {
    logFile->WriteString(llDebug, __FILE__ ":%d:Read debug random "
                         "seed %s form " "configFile.", __LINE__,
                         randomLongString.c_str());
    randLong = atol(randomLongString.c_str());
    err = OK;
  }
  else {
    FILE *file;
    if ((file = fopen("/dev/urandom", "r")) == NULL) {
      logFile->WriteString(llWarning, __FILE__ ":%d:Error opening "
                           "/dev/urandom (%d).", __LINE__, errno);
    }
    else {
      if ((readBytes = fread(&randLong, 1, sizeof(long), file)) == 0) {
        logFile->WriteString(llWarning, __FILE__ ":%d:Error reading "
                             "/dev/urandom.", __LINE__);
      }
      else {
        logFile->WriteString(llDebug, __FILE__ ":%d:Read %d bytes from "
                             "/dev/urandom.", __LINE__, readBytes);
        err = OK;
      }

      fclose(file);
    }
  }

  logFile->WriteString(llDebug, __FILE__ ":%d:Seeding with %lu.",
                       __LINE__, randLong);
  srandom(randLong);

  return err;
}


/**
 * Creates Majordomo.
 *
 * Overrides inherited CreateMajordomo. Creates client type of
 * majordomo. 
 *
 *
 * @return  void 
 * @author  Pavel 
 * @see     Majordomo from Client, SixMajordomo
 */
void Client::CreateMajordomo()
{
  majordomo = new Majordomo(this, logFile);
}
