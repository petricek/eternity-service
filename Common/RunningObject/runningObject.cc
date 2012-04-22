  // has to be FIRST include !
#include "./../../Common/H/common.h"

#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#include "./../../Common/H/common.h"
#include "./H/runningObject.h"

RunningObject::RunningObject()
{
  runningObjectID = new (RunningObjectID)[MAX_RUNNING_OBJECTS];
}

RunningObject::~RunningObject()
{
  DELETE(runningObjectID);
}

// saves running object ID returned from apropriate 
// Mix::Create"running object"() method for future reference in
// (this) WaitForRunningObject() method (see below)
Err RunningObject::SaveRunningObjectID(RunningObjectName objectName,
                                       RunningObjectID objectID)
{
    // for not to exceed space allocated for array of running objects
  assert(objectName < MAX_RUNNING_OBJECTS);
  runningObjectID[objectName] = objectID;

  return OK;
}

// waits for specified running object to exit
Err RunningObject::WaitForRunningObject(RunningObjectName objectName)
{
    // for not to exceed space allocated for array of running objects
  assert(objectName < MAX_RUNNING_OBJECTS);

  pthread_join(runningObjectID[objectName], NULL);
  return OK;
}
