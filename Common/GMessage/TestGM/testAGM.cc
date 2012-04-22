#include "./../H/arrayGMessage.h"

void main()
{
  LogFile *log = new LogFile("testAGM.log");
  ArrayGMessage *agm = new ArrayGMessage(log);
  agm->LoadFromLines("Keyword", "testFile  bestFile  mestFile  kreten");
  agm->DebugHeaders(log);
  DELETE(agm);
  DELETE(log);
}
