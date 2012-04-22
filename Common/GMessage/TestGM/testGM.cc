#include "./../../LogFile/H/logFile.h"
#include "./../../GMessage/H/gMessage.h"

int main()
{
  LogFile *lf = NULL;
  GMessage *gm = NULL;
  Char *str = NULL;
  
  lf = new LogFile("gm.log");
  if( lf == NULL ) return 1;

  if( (gm = new GMessage()) == NULL ){
    delete lf;
    return 1;
  }

  //nacpeme tam nejaky data
  gm->SetAsID("PAYMENT_ID", 2);
  gm->SetAsInt("SUCCESS", 0);
  gm->DebugHeaders(lf);

  //pokusime se tam nacpat NULL
  if( gm->SetAsMsgField("O_AUTHENTIFICATION", NULL) != OK )
    lf->WriteString(llError, "Do GM cpeme NULL polozku");
  gm->DebugHeaders(lf);

  gm->SetAsString("AMOUNT0", "");
  gm->SetAsString("AMOUNT1", "1");
  gm->SetAsString("AMOUNT2", "22");
  gm->SetAsString("AMOUNT3", "333");
  gm->SetAsString("AMOUNT4", "4444");
  gm->DebugHeaders(lf);

  gm->StoreToFile("gm.file");
  
  lf->WriteString(llDebug, "Prave se pokousime z GM dostat AMOUNTy");
  str = gm->GetAsString("AMOUNT0");
  lf->WriteString(llDebug, "AMOUNT0:   %s   a takhle to dopadlo >:-(", str);
  if( str ) delete str;
  str = gm->GetAsString("AMOUNT1");
  lf->WriteString(llDebug, "AMOUNT1:   %s   a takhle to dopadlo", str);
  if( str ) delete str;
  str = gm->GetAsString("AMOUNT2");
  lf->WriteString(llDebug, "AMOUNT2:   %s   a takhle to dopadlo", str);
  if( str ) delete str;
  str = gm->GetAsString("AMOUNT3");
  lf->WriteString(llDebug, "AMOUNT3:   %s   a takhle to dopadlo", str);
  if( str ) delete str;
  str = gm->GetAsString("AMOUNT4");
  lf->WriteString(llDebug, "AMOUNT4:   %s   a takhle to dopadlo", str);
  if( str ) delete str;

  lf->WriteString(llDebug, "Vypada to, ze gm->DebugHeaders bere jen stringy liche delky ;-{");

  delete gm;
  delete lf;
  return 0;
}
