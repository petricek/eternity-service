#include <stdio.h>

#include "../../../Common/GMessage/H/estime.h"
#include "./../H/squeue.h"
#include <stdio.h>

int main()
{
  TimeSortedQueue *q;
  ESTime *t1, *t2, *t3, *t4;
  bool type;
  
  type = true;
  type = false;

  q = new TimeSortedQueue();
  t1 = new ESTime();
  t2 = new ESTime();
  ((BasicTime*)t2)->SetTime(1L);
  t3 = new ESTime();
  ((BasicTime*)t3)->SetTime(6L);

  q->Insert(t1);
  q->Insert(t2);
  q->Insert(t3);

  t4 = (ESTime *)  q->First();
  printf("1. prvek: %ld\n", t4->GetTime());
  t4 = (ESTime *)  q->Next();
  printf("2. prvek: %ld\n", t4->GetTime());
  t4 = (ESTime *)  q->Next();
  printf("3. prvek: %ld\n", t4->GetTime());

  printf("Konec prace s frontou\n");
  delete q;

  delete t1;
  delete t2;
  delete t3;

  return 0;
}