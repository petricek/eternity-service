#ifndef RUNNING_OBJECT_H
#define RUNNING_OBJECT_H

  // has to be the FIRST include !
#include "./../../../Common/H/common.h"

#include <pthread.h>

#define MAX_RUNNING_OBJECTS 15

typedef pthread_t RunningObjectID;

/**
 * Name of running object (thread).
 *
 *
 * @author  Pechy
 * @see     Receiver, Translator, Padder, Sender, Linker, Majordomo, 
 * Chopper, Scheduler, SignalManager
 */
typedef enum { 	RECEIVER, 
                RECEIVER_NET, 
		RECEIVER_SIX,
		TRANSLATOR, 
       		PADDER, 
       		SENDER, 
       		SENDER_NET, 
       		SENDER_SIX, 
		LINKER,
		CHOPPER,
		MAJORDOMO,
		SCHEDULER,
       		SIGNAL_MANAGER } RunningObjectName;

/**
 * Running objecti (thread).
 *
 * class that contains stuff for enabling waiting for running objects 
 *
 * @author  Pechy
 * @see     Receiver, Translator, Padder, Sender, Linker, Majordomo,
 * Chopper, Scheduler, SignalManager
 */
class RunningObject
{
  protected:
  /**@name attributes */
  /*@{*/
    /// array for runnig objects ID's, for mix to be able to wait for them
  RunningObjectID *runningObjectID;
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
    /// constructor
  RunningObject();				
    /// destructor
  ~RunningObject();		

  /// Store ID to be able to wait for the thread
  Err SaveRunningObjectID(RunningObjectName objectName, 
			  RunningObjectID objectID);

  /// Wait for thread to finish
  Err WaitForRunningObject(RunningObjectName objectName);
  /*@}*/
};
#endif
