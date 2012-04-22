#include "./../RandomGenerator/H/randomGenerator.h"
#include "./../LogFile/H/logFile.h"

void CheckField(MsgField *fld, LogFile *log)
{
  if(fld)
    log->WriteString(llDebug, "MsgField obtained, size = %u", fld->SizeOf());
  else
    log->WriteString(llDebug, "MsgField nowhere");
}

void main()
{
  MsgField *fld;
  RandomGenerator *rnd;
  LogFile *log;

  log = new LogFile("testRandom.log");
  rnd = new RandomGenerator(log);
  if(rnd == NULL)
  {
    DELETE(log);
    return;
  }

  for(int i=1; i<20; i++) {
    fld = rnd->RandomBytes(4);
    printf("%s\n", fld->GetAsString());
    if(fld)
      DELETE(fld);
  }

  DELETE(rnd);
  DELETE(log);
}
