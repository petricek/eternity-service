  // has to be FIRST include !
#include "./../Common/H/common.h"

#include <stdio.h>
#include <unistd.h>
#include "./Bank/H/bank.h"

/**
 * Bank main function.
 *
 * Parses options Bank can be given from command line and then creates
 * an instance of class Bank which then runs the service.
 *
 *
 * @author  Venca
 * @see     Bank
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

    Bank * bank = NULL;

    if(configName)
    {
      bank = new Bank(string(configName));
    }
    else
    {
      bank = new Bank();
    }

 
    // creates all running objects and wait for them...
  bank->Run();

  DELETE(bank);
  printf("Bank finished ...\n");

  exit(0);
}
