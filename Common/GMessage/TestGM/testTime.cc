#include "./../../LogFile/H/logFile.h"
#include "./../../GMessage/H/estime.h"

#define TIMEOUT "00:00:01:00:00:00"

int main()
{
  LogFile *lf = NULL;
  Char *tmp;
  
  lf = new LogFile("time.log");

  ESTime *t = new ESTime();

  lf->WriteString(llDebug, "puvodni cas %s", tmp = t->Print());  
  DELETE(tmp);
  RelTime *rt = new RelTime(TIMEOUT);

  ESTime g = *t + *rt;

  lf->WriteString(llDebug, "novy cas %s", tmp = g.Print());
  DELETE(tmp);

}
