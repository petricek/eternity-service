  // has to be the *FIRST* include !
#include "./../../Common/H/common.h"

#include <stdlib.h>
#include <unistd.h>

#include "./../../Common/H/labels.h"
#include "./../../Common/H/commands.h"
#include "./H/majordomo.h"

  // Global parameters                                                                                          
extern char * configName;
extern char * fileKeys;
extern char * fileName;
extern char * storeTime;
extern CommandOption commandOption;

/**
 * Constructor of Majordomo in Client.
 *
 * Initializes the specific attributes.
 *
 *
 * @param   parent Parent of the object.
 * @param   logFile LogFile to write to.
 * @author  Pavel
 * @see     SixCertificates
 */
Majordomo::Majordomo(Client * parent, LogFile * logFile)
: SixMajordomo(parent, logFile)
{
  yesEsoCertificates = NULL;
  receivedAnswers = 0;
  cipheredData = NULL;
}

/**
 * Destructor of Majordomo.
 *
 * Destructor of Majordomo in Client
 *
 *
 * @return  co
 * @author  pavel
 * @see     
 */
Majordomo::~Majordomo()
{
}


/**
 * Run method in Client Majordomo.
 *
 * Generates asymetric key and according to the switches given by user
 * runs specified method.
 *
 *
 * @return  void *
 * @author  Pavel
 * @see     
 */
//-------------------------------------------------------------------------
// Run
// ~~~
//
//-------------------------------------------------------------------------
void * Majordomo::Run(void * )
{

  GenerateAsymetricKey();
  GenerateClientAC();
  sleep(1);
fprintf(stderr, "<CLIENT>=================================================\n");
fprintf(stderr, "<CLIENT>: Client started\n");
  DBG;
  switch (commandOption)
  {
    case coTestW:
      fprintf(stderr, "coTestW:\n");
      DBG;
      return TestW(NULL);
      break;
    case coTestX:
      fprintf(stderr, "coTestX:\n");
      DBG;
      return TestX(NULL);
      break;
    case coTestY:
      fprintf(stderr, "coTestY:\n");
      DBG;
      return TestY(NULL);
      break;
    case coTestZ:
      fprintf(stderr, "coTestZ:\n");
      DBG;
      return TestZ(NULL);
      break;
    case coDownload:
      fprintf(stderr, "<CLIENT>: Running command Download:\n");
      fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
      DBG;
      DownloadFile();
      break;
    case coUpload:
      fprintf(stderr, "<CLIENT>: Running command Upload:\n");
      fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
      DBG;
      UploadFile();
      break;
    case coSearch:
      fprintf(stderr, "<CLIENT>: Running command Search:\n");
      fprintf(stderr, "<CLIENT>-------------------------------------------------\n");
      DBG;
      DownloadHeaders();
      break;
  }
  return NULL;
}
