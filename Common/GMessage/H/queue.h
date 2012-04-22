#ifndef QUEUE_H
#define QUEUE_H

#include "./../../H/common.h"


/**
 * Item of queue.
 *
 * A class which represents one item in queue.
 *
 * @author Ivana
 * @see
 */
class ItemOfQ
{
  public:

  /**@name attributes */
  /*@{*/
    Void *object;
    ItemOfQ *prev, *next;
  /*@}*/

  /**@name methods */
  /*@{*/
    ItemOfQ(Void *o); //Created, it points to itself
  /*@}*/
};


/**
 * Queue class.
 *
 * A class which can store objects in queue.
 * Important notices:<br>
 * 1) every 'delete' in this class deletes only the item not the object itself<br>
 * 2) every method, which returns an object, returns a void pointer; that is why
 * the output has to be retyped<br>
 * example: object = (RealTypeOfObject *)queue->First();
 *
 * @author Ivana
 * @see 
 */
class Queue
{
  protected:

  /**@name attributes */
  /*@{*/
    ItemOfQ *first, *current;
    Int numOfItems;
  /*@}*/

  /**@name methods */
  /*@{*/
    ///Sets "current" pointer to the item at <index>-th position
    Void SetCurrent(Int index);
    ///Sets "current" to the item which object corresponds to <obj>
    Void SetCurrent(Void *obj);
    ///Inserts at "current"; doesn't care about "first" setting
    virtual Err InsertAtCurrent(Void *obj);
    ///Deletes the "current" item (new current will be the next one)
    Void DeleteCurrent();
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
     /// constructor
     Queue();
     /// destructor
     virtual ~Queue();

     ///Inserts at the end of Q
     virtual Err Insert(Void *obj);
     ///Inserts at position <index>; returns the real position of storage
     virtual Int InsertAt(Void *obj, Int index);

     ///Deletes 1st item which "object" corresponds to <obj>
     Void Delete(Void *obj);
     ///Deletes item at <index> position; return the pointer to the object
     Void *DeleteAt(Int index);
     ///Deletes all items in Q
     Void DeleteAll();

     ///Returns the 1st object in Q
     Void *First();
     ///Returns the "current's successor" object; sets the "current" to it
     Void *Next();
     ///Returns the object at position <index>
     Void *ItemAt(Int index);

     ///Returns number of items in Q
     Int ItemsOfQ();
  /*@}*/

};

#endif

