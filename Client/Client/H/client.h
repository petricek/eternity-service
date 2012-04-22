//*************************************************************************
// File: client.h
// Name:
// Author: Venca
//*************************************************************************

#ifndef CLIENT_H
#define CLIENT_H

#include "./../../../Common/ConfigFile/H/configFile.h"
#include "./../../../Common/LogFile/H/logFile.h"
#include "../../../Common/MessageQueue/H/messageQueue.h"

class Client;

#include <pthread.h>

//#define NO_BANKER

#include "../../../Six/Six/H/six.h"
#include "../../../Common/CertificateRepository/H/certificateRepository.h"

/**
 * Command to perform.
 *
 * This enum specifies the action to be taken - some test methods and Search upload or download
 *
 *
 * @author  Marek Pavel Pechy Venca
 * @see     Client 
 */
enum CommandOption { 
                     coUpload = 1,
		     coDownload = 2, 
		     coTestX = 3, 
		     coTestY = 4,
		     coTestZ = 5, 
		     coTestW = 6,
		     coSearch = 97
                   };

class Banker;

/**
 * Client main class.
 *
 * Consists of severaal objects that together perform all the actions necessary 
 * to make a user interface to Eternity service.
 *
 * @author  Marek Pavel Pechy Venca
 * @see     Six Eso Bank
 */
class Client: public Six
{
  public:

  /**@name attributes */
  /*@{*/
    CommandOption commandOption;
    string fileKeys;
    string fileName;
    CertificateRepository * certificateRepository;
    Banker *banker;
  /*@}*/

  /**@name methods */
  /*@{*/
    Client(string configName);
    virtual ~Client();
    InitializeRandomGenerator();
  /*@}*/

  protected:

  /**@name methods */
  /*@{*/
    virtual void CreateMajordomo();
  /*@}*/
};

#include "./../../Banker/H/banker.h"

#endif

