#include "./H/queue.h"

/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Queue is implemented as "bidirectional cyclic list".
 * It is important that any "DELETE(method)" deletes only the item in Q
 * not the object itself.
 *
 *
 * @param   parametr
 * @return  co
 * @author  Ivanka
 * @see     co_se_toho_tyka
 */
ItemOfQ::ItemOfQ(Void *o)
{ 
  prev = next = this;
  object = o;
}    


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Ivanka
 * @see     co_se_toho_tyka
 */
Queue::Queue()
{ 
  first = current = NULL; 
  numOfItems = 0;
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Ivanka
 * @see     co_se_toho_tyka
 */
Queue::~Queue()
{
  DeleteAll();
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Sets the current pointer to the item at position <index>
 * we don't mind, if the index > numOfItems because the Q is cyclic.
 *
 *
 * @param   parametr
 * @return  co
 * @author  Ivanka
 * @see     co_se_toho_tyka
 */
Void Queue::SetCurrent(Int index)
{
  Int i;

  if(numOfItems>0)
    for( current=first,i=1 ; i<index ; i++,current=current->next );
  else //Q is empty
    current=NULL;
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Sets "current" to the item which object corresponds to <obj>
 * if there is no such item, "current" will be the same.
 *
 *
 * @param   parametr
 * @return  co
 * @author  Ivanka
 * @see     co_se_toho_tyka
 */
Void Queue::SetCurrent(Void *obj)
{
  ItemOfQ *pom;

  if(obj!=NULL && current!=NULL && current->object!=obj)
  {
    pom=current;//to recognize end of Q
    for(current=current->next;
        current!=pom && current->object!=obj;
        current=current->next);
  }
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Inserts the object at "current" ("shift old_current to the right")
 * doesn't care about "first" setting !! every 'insert method' in Q
 * should first set the "current" and then use this method
 *
 *
 * @param   parametr
 * @return  co
 * @author  Ivanka
 * @see     co_se_toho_tyka
 */
Err Queue::InsertAtCurrent(Void *obj)
{
  ItemOfQ *tmp=new ItemOfQ(obj);

  if(tmp == NULL)
    return KO;

  if(current!=NULL){ //equiv. numOfItems>0
    tmp->prev=current->prev;
    tmp->next=current;
    current->prev->next=tmp;
    current->prev=tmp;
  }
  current=tmp;
  numOfItems++;

  return OK;
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Deletes "current" item; sets new "current" to it's successor
 * does care about "first" setting !! every 'DELETE(method)' in Q
 * should first set the "current" and then use this method (except
 * 'DeleteAll' or st. like this).
 *
 *
 * @param   parametr
 * @return  co
 * @author  Ivanka
 * @see     co_se_toho_tyka
 */
Void Queue::DeleteCurrent()
{
  ItemOfQ *tmp=NULL;

  if(current!=NULL){//equiv. numOfItems>0
    if(current==first){//! later we have to set "first"
      if(current->next==current){//Q has only 1 member
        DELETE(current);
      }
      else{//Q has more than 1 member
        tmp=current->next;
        current->prev->next=current->next;
        current->next->prev=current->prev;
        DELETE(current);
      }
      first=tmp;
    }
    else{// ==> Q has more than 1 member (1st and current at least)
      tmp=current->next;
      current->prev->next=current->next;
      current->next->prev=current->prev;
      DELETE(current);
    }
    current=tmp;//the next one or NULL
    numOfItems--;
  }
}


/**
 * Inserts the object to the end of queue.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Ivanka
 * @see     co_se_toho_tyka
 */
Err Queue::Insert(Void *obj)
{
  SetCurrent(1);
  if(InsertAtCurrent(obj) == KO)
    return KO;
  
  if(numOfItems==1) first=current;

  return OK;
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Inserts the object at position <index> (if it's possible)
 * returns the index, where the object is really stored.
 *
 *
 * @param   parametr
 * @return  co
 * @author  Ivanka
 * @see     co_se_toho_tyka
 */
Int Queue::InsertAt(Void *obj, Int index)
{
  Int i;

  if(index<1 || index>numOfItems){
    i=numOfItems+1;
    index=1;
  }
  else
   i=index;

  SetCurrent(index);

  if(InsertAtCurrent(obj) == KO)
    return -1;

  if(i==1)
    first=current;

  return i;
}


/**
 * Deletes 1st item (from "current") which "object" corresponds to <obj>.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Ivanka
 * @see     co_se_toho_tyka
 */
Void Queue::Delete(Void *obj)
{
  SetCurrent(obj);
  if(current!=NULL && current->object==obj)
    DeleteCurrent();
}


/**
 * Deletes item at position <index>; returns the object.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Ivanka
 * @see     co_se_toho_tyka
 */
Void *Queue::DeleteAt(Int index)
{
  Void *tmp=NULL;

  if(1<=index && index<=numOfItems){
    SetCurrent(index);
    tmp=current->object;
    DeleteCurrent();
  }
  return tmp;
}


/**
 * Deletes all items in Q (not objects!).
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Ivanka
 * @see     co_se_toho_tyka
 */
Void Queue::DeleteAll()
{
  //we have to know the end of Q
  if(first!=NULL) first->prev->next=NULL;
  for( ; first!=NULL ; ){
    current=first->next;
    DELETE(first);
    first=current;
    numOfItems--;
  }
  first=current=NULL;
}

/**
 * Returns the 1st  object; sets "current" to this item.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Ivanka
 * @see     co_se_toho_tyka
 */
Void *Queue::First()
{ 
  current=first;
  return ( first == NULL ) ? NULL : first->object; 
}


/**
 * Returns the next (=current's successor) object; sets "current"
 * to this item.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Ivanka
 * @see     co_se_toho_tyka
 */
Void *Queue::Next()
{
  if( numOfItems==0 ) return NULL;
  current=current->next;
  if( current==first ) return NULL;//equiv. end of Q
  return current->object;
}


/**
 * Returns object at the position <index> sets "current" to this item.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Ivanka
 * @see     co_se_toho_tyka
 */
Void *Queue::ItemAt(Int index)
{
  if( index<1 || index>numOfItems) return NULL;
  SetCurrent(index);
  return current->object;
}


/**
 * Returns number of items in Q.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Ivanka
 * @see     co_se_toho_tyka
 */
Int Queue::ItemsOfQ()
{ 
  return numOfItems;
}
