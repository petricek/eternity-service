#include "./../H/keywords.h"

int main()
{
  LogFile *log = new LogFile("testKeywords.log");
  Keywords *kwds = new Keywords(log);

  kwds->AddKeyword("prvni");
  kwds->AddKeyword("druhy");
  kwds->AddKeyword("treti");
  kwds->AddKeyword("ctvrty");
  kwds->AddKeyword("paty");
  kwds->AddKeyword("sesty");

  kwds->DebugHeaders(log);

  kwds->MatchesPattern("prvni");
  kwds->MatchesPattern("druhy");
  kwds->MatchesPattern("treti");
  kwds->MatchesPattern("ctvrty");
  kwds->MatchesPattern("paty");
  kwds->MatchesPattern("sesty");
  kwds->MatchesPattern("xxx");

  DELETE(kwds);
  DELETE(log);
  return 0;
}
