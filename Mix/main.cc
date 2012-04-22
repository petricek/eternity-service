  // has to be FIRST include !
#include "./../Common/H/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "./../Common/LogFile/H/logFile.h"

#include "./Mix/H/mix.h"

/**
 * Mix main function.
 *
 * Parses command line options and sets global variables according to
 * switches specified It also creates Mix's main object and executes
 * his method Run()
 *
 * @author  Venca
 * @see     Mix
 */
int main(int argc, char ** argv)
{

  extern char *malloc_options;
  malloc_options = "A";
  
  extern char *optarg;
  extern int optind;
  int ch;
  char * configName = NULL;

  optind = 1;

  pthread_sigmask(SIG_BLOCK, NULL, NULL);

  while ((ch = getopt(argc, argv, "f:")) != -1) {
    switch(ch) {
      case 'f':
        configName = optarg;
        break;
      default:
        //printHelp();
        exit(1);
        break;
    }
  }

  Mix * mix = NULL;

  if(configName)
  {
    mix = new Mix(string(configName));
  }
  else
  {
    mix = new Mix();
  }

    // creates all running objects and wait for them...
  mix->Run();

  DELETE(mix);
  printf("Mix finished ...\n");

  exit(0);
}
