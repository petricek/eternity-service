  // has to be the *FIRST* include !
#include "./../../Common/H/common.h"

#include "./../../Common/LogFile/H/logFile.h"

#include "./H/leakDetector.h"
#include <stdlib.h>
#include <map>

  /// Structure containing allocated pointers together with ID's
typedef map< void *, long, less<void *> > Pointers;
  /// Pair of pointer and ID
typedef Pointers::value_type pointerRec;

  /// Variable containing allocated pointers together with ID's
Pointers pointers;

LogFile * newLog = NULL;

/**
 * Overridden operator new.
 *
 * Allocates memory for the object and remembers that along with a
 * unique ID and writes a message into log file including that ID.
 * Later we can find what was allocated and not freed.
 *
 * @param   size    Size of object to allocate.
 * @return  Pointer to the newly allocated space.
 * @author  Venca
 * @see     GMessage, operator delete
 */
void * 
LeakDetector::operator new(size_t size)
{
  //void * ptr = new size;
  void * ptr = malloc(size);
  long id = random();
  pointers[ptr] = id;
  fprintf(stderr,"new : %ld\n",id);
  newLog->WriteString(llDebug,"new : %ld (size: %lu)\n",id,size);
  return ptr;
}

/**
 * Overridden operator delete.
 *
 * Frees memory for the object and writes a message into the log
 * file including the ID that is associated with this pointer so
 * that we can find leaks by comparing ID's and looking for those that
 * were not deleted.
 *
 * @param   what    Pointer to object we want to delete
 * @return  void
 * @author  Venca
 * @see     GMessage, operator new
 */
void  
LeakDetector::operator delete(void * what)
{
  free(what);
  long id = pointers[what];
  fprintf(stderr,"del : %ld\n",id);
  newLog->WriteString(llDebug,"del : %ld\n",id);
  pointers.erase(pointers.find(what));
}
