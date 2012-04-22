#ifndef SQUEUE_H
#define SQUEUE_H

#include "../../../Common/GMessage/H/queue.h"

/**
 * Ancestor of all sorted queues.
 * 
 * This class is a basic class for classes suporting sorting and
 * priorities.
 *
 * @author Jirka
 */
class SortedQueue : public Queue {
public:
  /*@name methods */
    /// abstract method for comparing two objects
  virtual Int Compare(Void *obj1, Void *obj2) = 0;
    /// Sorted insert 
  virtual Err Insert(Void *obj);
};

/**
 * Queue sorted by ESTime.
 *
 * This queue class supports sorting by ESTime.
 *
 * @author Jirka
 * @see TimeSynchronizer
 */
class TimeSortedQueue : public SortedQueue {
public:
  /*@name methods */
    /// Compare two ESTime
  virtual Int Compare(Void *obj1, Void *obj2);
    /// Print sorted queue to output
  void Print();
};
#endif
