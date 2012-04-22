#include "../../Common/H/configFile.h"

main()
{
  ConfigFile *config = new ConfigFile("test.conf");
  config->PrintAll();
  DELETE(config);
}
