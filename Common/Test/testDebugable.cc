#include "../Debugable/H/debugable.h"


class Traged : Debugable
{
 public:
  int deflt;
	LogFile * tragedLogFile;
  Traged();
  void Log(int i);

};


Traged::Traged()
{

  tragedLogFile = new LogFile("traged.log");

  Debugable(*tragedLogFile);
  deflt = 7; 

}

void Traged::Log(int i)
{

  WriteString(llError,"Cislo trageda: %d", i);
  printf("Cislo trageda: %d", i);

}


main()
{

  Traged * traged = new Traged();

  traged->Log(13);

	DELETE(traged);

}
