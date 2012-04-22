  // has to be FIRST include !
#include "./../Common/H/common.h"

#include <stdio.h>
#include "./Acs/H/acs.h"

/**
 * Acs main function.
 *
 * Parses command line and sets global variables according to switches
 * specified.  It also creates Acs' main object and runs its method
 * Run().
 *
 * @author  Venca
 * @see     Acs
 */
int main(int argc, char ** argv)
{
  extern char *optarg;
  extern int optind;
  int ch;
  char * configName = NULL;

  optind = 1;

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

  Acs * acs = NULL;

  if(configName)
  {
    acs = new Acs(string(configName));
  }
  else
  {
    acs = new Acs();
  }

    // creates all running objects and wait for them...
  acs->Run();

  DELETE(acs);
  printf("Acs finished ...\n");
}
