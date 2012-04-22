#include <stdio.h>

#include "../GMessage/H/estime.h"
#include "../GMessage/H/squeue.h"
#include <stdio.h>

int main()
{
  TimeSortedQueue *q;
  ESTime *t1, *t2, *t3, *t4;

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
  DELETE(q);

  DELETE(t1);
  DELETE(t2);
  DELETE(t3);

  return 0;
}