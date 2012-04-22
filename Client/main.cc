  // has to be FIRST include !
#include "./../Common/H/common.h"

#include <stdio.h>

#include "./Client/H/client.h"

  /// default config file, if not set by '-f' option
#define CLIENT_CONFIG_FILE "/home/eternity/conf/client.conf"

  ///@name Global parameters for client
/*@{*/
char * configName = CLIENT_CONFIG_FILE;
char * fileKeys = "";
char * fileName = "";
char * storeTime = "";
CommandOption commandOption = coDownload;
char * ffidFileName = "";
Int    esoCount = 0;
/*@}*/

void printHelp();

/**
 * Client main function.
 *
 * Parses command line options and sets global variables according to 
 * switches specified
 * It also creates Client's main object and executes his method Run()
 *
 * @author  Pavel, Marek,Venca
 * @see     Client, Client::Run()
 */
int main(int argc, char ** argv)
{
  extern char *optarg;
  extern int optind;
  int ch;
  Client *client = NULL;
  char *esoCountStr;

  optind = 1;

  while ((ch = getopt(argc, argv, "swxyzc:dupgt:k:f:i:n:")) != -1) {
    switch(ch) {
       // config
      case 'c':
        configName = optarg;
        break;
       // upload file
      case 'p':
      case 'u':
	commandOption = coUpload;
        break;
       // download file
      case 'g':
      case 'd':
	commandOption = coDownload;
        break;
       // time
      case 's':
        commandOption = coSearch;
        break;
       // keywors describing file
      case 'k':
        fileKeys = optarg;
        break;
       // time
      case 't':
        storeTime = optarg;
        break;
       // filename
      case 'f':
        fileName = optarg;
        break;
       // fileID
      case 'n':
        esoCountStr = optarg;
	if(strcmp(esoCountStr, "") != 0) {
	  esoCount = atoi(esoCountStr);
	}
	break;
      case 'i':
        ffidFileName = optarg;
        break;
       // redundancy count
/*      case 'n':
        string strCount = optarg;
	if(! strCount.empty())
	  esoCount = atoi(strCount.c_str());
        break;
*/       // tests
      case 'w':
	printf("TestW\n");
        commandOption = coTestW;
        break;
      case 'x':
	printf("TestX\n");
        commandOption = coTestX;
        break;
      case 'y':
	printf("TestY\n");
        commandOption = coTestY;
        break;
      case 'z':
	printf("TestZ\n");
        commandOption = coTestZ;
        break;
       // Print help
      default:
        printHelp();
        exit(1);
        break;
    }
  }

  client = new Client(string(configName));

    // creates all running objects and wait for them...
  client->Run();

  DELETE(client);
  printf("Client finished ...\n");
}

/**
 * Prints usage instructions.
 *
 * @author  Pavel Marek
 * @see     co_se_toho_tyka
 */
void printHelp()
{
  printf("Usage:\n");
  printf("Upload file:\n");
  printf("    client -u -c <configFile> -f <fileToUpload> -k <keywords> -n <redundancy> \n");
  printf("\n");
  printf("Search for files in the Service:\n");
  printf("    client -s -c <configFile> -i <filePrefixToSaveFFIDsFound> -k <keywords> -n <numberOfEsosToAsk> -f <writeHeadersToFile>\n");
  printf("\n");
  printf("Download file:\n");
  printf("    client -d -c <configFile> -i <fileWithFFIDToDownloadBy> -f <storeToName> -n <numberOfEsosToAsk> \n");
}
