  // has to be FIRST include !
#include "./../Common/H/common.h"

#include <stdio.h>

#include "./Eso/H/eso.h"

/**
 * Eso main function.
 *
 * Parses options Eso can be given from command line and then creates
 * an instance of class Eso which then runs the service.
 *
 *
 * @author  Venca
 * @see     Eso
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

    Eso * eso = NULL;

    if(configName)
    {
      eso = new Eso(string(configName));
    }
    else
    {
      eso = new Eso();
    }

 
    // creates all running objects and wait for them...
  eso->Run();

  DELETE(eso);
  printf("Eso finished ...\n");

  exit(0);
}
