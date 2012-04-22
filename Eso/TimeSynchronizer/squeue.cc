// *************************************************************************
// 
// File: squeue.cc
// Name: SortedQueue, TimeSortedQueue
// Author: Jirka
// Last Modified: 6.1.1999
// 
// Description: This file extends class Queue. 
//   SortedQueue is abstract class with abstract method Compare and sorted 
//     Insert
//   TimeSortedQueue implements method Compare with comparing by time   
//
// *************************************************************************

#include "../../Common/GMessage/H/estime.h"
#include "./H/squeue.h"
#include <stdio.h>

// ************************************************************************ /
// ********************        SortedQueue          *********************** /
// ************************************************************************ /

Err SortedQueue::Insert(Void *obj) {
  current = first;
  if ((current != NULL) && (Compare(obj, current->object) > 0)) {
    Next();
    while( current != first && (Compare(obj, current->object) > 0) ) Next();
    InsertAtCurrent(obj);
  }
  else InsertAt(obj, 1);
  return OK;
}

// ************************************************************************ /
// *******************      TimeSortedQueue     *************************** /
// ************************************************************************ /

// obj1 < obj2 return -1
// obj1 = obj2 return 0
// obj1 > obj2 return 1

Int TimeSortedQueue::Compare(Void *obj1, Void *obj2) {
  ESTime *pt1, *pt2;
  
  pt1 = (ESTime *) obj1;
  pt2 = (ESTime *) obj2;
//  printf("pt1 %ld\n", pt1->GetTime());
//  printf("pt2 %ld\n", pt2->GetTime());
  if ( (*pt1) < (*pt2) ) return -1;
  if ( (*pt1) > (*pt2) ) return 1;
  return 0;
}

void TimeSortedQueue::Print() {
  ESTime *pTime;
  char *str;
  Int num = ItemsOfQ();
  for(int i=1; i<= num; i++) {
    pTime = (ESTime*) ItemAt(i);
    printf("item %s\n", str = pTime->Print());
    DELETE(str);
  }
}