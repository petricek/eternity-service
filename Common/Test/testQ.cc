#include <stdio.h>

#include "../GMessage/H/queue.h"

int main()
{
  Queue *q;
  int i,j,k;
  char c,z,*pomc;
  int *pi,*pj,*pom,**pom1;

  q=new Queue();

  i=1;
  j=2;
  k=3;
  c='a';
  z='b';
  pi=&i;
  pj=&j;
  printf("var  adr_var      value_var\n");
  printf("i     %d           %d\n",&i,i);
  printf("j     %d           %d\n",&j,j);
  printf("k     %d           %d\n",&k,k);
  printf("c     %d           %c\n",&c,c);
  printf("z     %d           %c\n",&z,z);
  printf("pi    %d           %d\n",&pi,pi);
  printf("pj    %d           %d\n",&pj,pj);
  printf("------------------------\n");

  printf("*) Insert: i, j, k\n*) InsertAtPosition: pi(1), pj(2), pj(6), z(6), c(6)\n");
  q->Insert(&i);
  q->Insert(&j);
  q->Insert(&k);
  q->InsertAt(&pi,1);
  q->InsertAt(&pj,2);
  q->InsertAt(&pj,6);
  q->InsertAt(&z,6);
  q->InsertAt(&c,6);
  printf("Q should be: pi(&1) pj(&2) i(1) j(2) k(3) c(a) z(b) pj(&2)\n");

  pom1=(Int **)q->First();
  printf("  %d",*pom1);
  pom1=(Int **)q->Next();
  printf("  %d",*pom1);
  pom=(Int *)q->Next();
  printf("  %d",*pom);
  pom=(Int *)q->Next();
  printf("  %d",*pom);
  pom=(Int *)q->Next();
  printf("  %d",*pom);
  pomc=(char *)q->Next();
  printf("  %c",*pomc);
  pomc=(char *)q->Next();
  printf("  %c",*pomc);
  pom1=(Int **)q->Next();
  printf("  %d     ",*pom1);
  if(q->Next()==NULL) printf("konec fronty; # polozek = %d\n",q->ItemsOfQ());

  printf("\n*) Deleting pj (1st pj from current position)\n");
  q->Delete(&pj);
  pom1=(Int **)q->First();
  printf("  %d",*pom1);
  pom=(Int *)q->Next();
  printf("  %d",*pom);
  pom=(Int *)q->Next();
  printf("  %d",*pom);
  pom=(Int *)q->Next();
  printf("  %d",*pom);
  pomc=(char *)q->Next();
  printf("  %c",*pomc);
  pomc=(char *)q->Next();
  printf("  %c",*pomc);
  pom1=(Int **)q->Next();
  printf("  %d     ",*pom1);
  if(q->Next()==NULL) printf("konec fronty; # polozek = %d\n",q->ItemsOfQ());

  printf("\n*) Deleting at position 1\n");
  pom1=(Int **)q->DeleteAt(1);
  pom=(Int *)q->First();
  printf("  %d",*pom);
  pom=(Int *)q->Next();
  printf("  %d",*pom);
  pom=(Int *)q->Next();
  printf("  %d",*pom);
  pomc=(char *)q->Next();
  printf("  %c",*pomc);
  pomc=(char *)q->Next();
  printf("  %c",*pomc);
  pom1=(Int **)q->Next();
  printf("  %d     ",*pom1);
  if(q->Next()==NULL) printf("konec fronty; # polozek = %d\n",q->ItemsOfQ());

  printf("\n*) Deleting all\n");
  q->DeleteAll();
  if(q->First()==NULL) printf("konec fronty; # polozek = %d\n\n",q->ItemsOfQ());

  DELETE(q);

  return 0;
}