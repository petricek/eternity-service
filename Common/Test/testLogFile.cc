#include "../H/configFile.h"
#include "../H/logFile.h"



main(){

  ConfigFile * config = new ConfigFile("test.conf");

  config->PrintAll();

  LogFile * logik = new LogFile("test.log");

	logik->WriteString(llError,"Shit:%d",7);
	logik->SetDebugOptions(0);
	logik->WriteString(llError,"Shit:%d",77);
	logik->SetDebugOptions(llError);
	logik->WriteString(llError,"Shit:%d",777);

	DELETE(logik);

}
