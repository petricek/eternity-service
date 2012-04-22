#ifndef CLIENT_MAJORDOMO_H
#define CLIENT_MAJORDOMO_H

#include "./../../../Common/Runable/H/runable.h"
#include "./../../../Six/Six/H/six.h"
#include "./../../../Common/H/labels.h"
#include "./../../../Common/GMessage/H/arrayGMessage.h"
#include "./../../Client/H/client.h"

#define PRIVATE_KEY_FILE "private.key"
#define PUBLIC_KEY_FILE "public.key"
#define AC_FILE "thisSix.ac"

#define CFG_SECONDS "seconds_to_wait"
#define CFG_SECONDS_TO_WAIT 300

#define CFG_FFID_LENGTH "ffid_length"
#define CFG_DEFAULT_FFID_LENGTH 16

#define CFG_DEFAULT_RANDOM_STRING_LENGTH 16

#define CFG_DEFAULT_SEARCH_WIDTH "search_width"
#define CFG_DEFAULT_SEARCH_DEPTH "search_depth" 

/**
 * Client majordomo.
 *
 * Implements all the tasks for communication with Eso Mix Bank and is 
 * an expert in protocols used between these entities.
 *
 * @author  Marek, Pavel, Pechy, Venca
 * @see     Mix Client Eso Bank
 */
class Majordomo : public SixMajordomo
{
  protected:

  /**@name attributes */
  /*@{*/
    ArrayGMessage *yesEsoCertificates;
    int receivedAnswers;
    MsgField *cipheredData;
  /*@}*/
   
  /**@name methods */
  /*@{*/
    /// Handle data received command
    virtual Err SwitchDataReceivedCommand(const Char     *command,
					  const MsgField *dataFld,
					  const ACID      acid);
    ///generating client's AC
    Err GenerateClientAC();
    virtual Err GenerateThisSixAC(MsgField *onion, GMessage **generatedAC);
    Err GetThisSixAC(GMessage **ac);
      /// Generates uniqueu access certificate ID
    Err GenerateACID(ACID *acid);
      /// returns Acs asymetric key
    Err GetAsymetricKey(GMessage **acPubKey);
      /// Generates asymetric key for making access certificate
    Err GenerateAsymetricKey();
    
      /// Decrypts data by Acs' private key
    virtual MsgField *DecryptByACPriv(const ACID acid,
                                      const MsgField *dData);
  /*@}*/
  
  public:
  /**@name methods */
  /*@{*/
      /// Constructor
    Majordomo(Client * parent, LogFile * logFile);
      /// destructor
    virtual ~Majordomo();

      /// overriden methods
  /*@{*/
      /// Main thread method that handles all requests
    virtual void * Run(void *);
      /// Returns the AC Manager for the specified type.
    virtual ACManager *GetPtrToACManager(Char *type);
      /// Get access certificates of specified type
    virtual Queue *GetACs(Char *type, const int count = 1, Queue *notIn = NULL);
  /*@}*/

      /// local methods
  /*@{*/
    GMessage *LoadPublicKey();
    GMessage *LoadPrivateKey();
    Err CreateCipheredData();
    MsgField *ParseCipheredData();
    MsgField *ComputeMAC(MsgField *oAuth);
  /*@}*/
   
      ///commands 
  /*@{*/
    GMessage * GetOnion(Size onionLen, Size requestID);
    void DownloadHeaders();
    void DownloadFile();
    void UploadFile();
  /*@}*/

      /// data_received commands
  /*@{*/
    Err DRCReplyAllocation(const MsgField *dataFld);
    Err DRCReplyStorage(const MsgField *dataFld);
    Err DRCAnswerHeader(const MsgField *dataFld);
    Err DRCAnswerFile(const MsgField *dataFld);
  /*@}*/

      /// Helper functions for commands
  /*@{*/
    void FindHeaders(ArrayGMessage *certificates);
    void FindFile(ArrayGMessage *certificates);
    GMessage *CreateSimpleReqForHeaders(GMessage *pubKey);
    GMessage *CreateSimpleReqForFile(GMessage *pubKey);
    void WaitForAnswersToReqForHeaders();
    void WaitForAnswersToReqForFile();
    void RequestStorage(ArrayGMessage *certificates, File *fileToStore);
    void StoreFile(ArrayGMessage *certificates);
    GMessage *CreateSimpleReqForStor(File *fileToStore);
    GMessage *CreateSimpleFileToStor(GMessage *certificate, MsgField *FileIDFld);
    void WaitForAnswersToReqForStor(int requestsSent);
    void WaitForAnswersToFileToStor(int requestsSent);
  /*@}*/

      /// Tests and not finished versions of commands
  /*@{*/
    void * TestW(void *);
    void * TestX(void *);
    void * TestY(void *);
    void * TestZ(void *);
    void SendData(MsgField * data, GMessage * destination, string destinationType = GM_ONION, Size hops = 0);
    GMessage * GetCertificates(string type, Size count);
    long ResourceUsage();
  /*@}*/
  /*@}*/

};

#endif
