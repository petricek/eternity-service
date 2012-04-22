#include "./../GMessage/H/estime.h"
#include <stdio.h>

int main() {
  RelTime *prt, *prt2;
  RelTime rt;
  long t;
  char buff[30];
  ESTime t1;

  prt = new RelTime();
  printf("Vytvoreni RelTime\n");
  t = prt->GetTime();
  printf("sekundy: %ld\n", t);
  prt->SetTime("00:00:02:12:12:03");
  t = prt->GetTime();
  printf("sekundy: %ld\n", t);
  prt->SetTime(0, 2, 10);
  printf("Nastaveni na 2h 10min\n");
  t = prt->GetTime();
  printf("sekundy: %ld\n", t);
  prt->Write(buff, 29);
  printf("stejny vypis, ale jinak: %s\n", buff);
  prt->SetTime( t );
  t = prt->GetTime();
  printf("sekundy by mely byt stejny: %ld\n", t);
  
  prt2 = new RelTime(1, 0);
  printf("vytvoreni RelTime(1, 0) = 1 den\n");

  rt = *prt + *prt2;
  t = rt.GetTime();
  printf("soucet prvniho a druheho RelTime je: %ld\n", t);
  printf("prt2 %ld\n", prt2->GetTime());
 
  t1.Write(buff, TIMETEXT);
  printf("cas t1 (Time()) je %s", buff);

  ESTime *pt2 = new ESTime( rt );
  pt2->Write(buff, TIMETEXT);
  printf("cas pt2 (Time( RelTime )*) je %s", buff);
  printf("sec %ld\n", pt2->GetTime());

  prt2->SetTime(2345L);
  printf("prt2 (RT) sec %ld\n", prt2->GetTime()); 
  t1.SetTime( *prt2 );
  printf("cas Time t1 je %ld\n", t1.GetTime());
  t1.Write(buff, TIMETEXT);
  printf("cas t1 je %s", buff);

  printf("rt pred rozdilem %ld\n", rt.GetTime());
  if (t1 < *pt2) {
    rt = *pt2 - t1;
  }
  else rt = t1 - *pt2;

  printf("rozdil mezi casy je %ld sekund\n", rt.GetTime());

  printf("nastavuji 24. 12. 1998\n");
  pt2->SetTime(98, 12, 24);
  pt2->Write(buff, TIMETEXT);
  printf("je nastaveno %s nebot jde o GMT cas\n\n", buff);

  t1.SetTime(98, 12, 24, 0, 0, 0);
  if (*pt2 == t1) printf("casy se rovnaji\n");

  DELETE(prt2);

  prt2 = new RelTime(4);
  *prt2 = *prt2 * 4;
  printf(" test na nasobeni: %ld\n", prt2->GetTime() );
  ESTime estNew;
  char *auxStr;
  printf("pred scitanim %s\n", auxStr = estNew.Print());
  DELETE(auxStr);
  estNew = estNew + *prt2;
  printf("po scitanim %s\n", auxStr = estNew.Print());
  DELETE(auxStr);
  DELETE(prt2);

  DELETE(pt2);
  DELETE(prt);
} 
