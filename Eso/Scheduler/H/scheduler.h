#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../../Eso/H/eso.h"
#include "../../../Six/SixOffspring/H/sixOffspring.h"
#include "../../../Common/GMessage/H/estime.h"
#include "../../../Common/GMessage/H/gMessage.h"
#include "../../../Common/LogFile/H/logFile.h"
#include "../../../Common/Shutdownable/H/shutdownable.h"
#include "../../../Common/Runable/H/runable.h"
#include "../../../Common/MessageQueue/H/messageQueue.h"
#include "./stable.h"
#include "../../TimeSynchronizer/H/timesynch.h"
#include <pthread.h>

#define SCH_PATH "Scheduler/"
//tables
#define SCH_TABLE "sch_table.tbl"
#define SCH_TABLE_TIMEOK "sch_timeOk.tbl"
//columns of tables
#define SCH_TIME "Time"
#define SCH_COMMAND "Command"
#define SCH_INFO "Info"
#define SCH_TIMEOK "TimeOk"

//config file
#define SCH_TIMEOUT 20

/**
 * Schedules time's operation.
 *
 * This is implemented by sending messages (timeouts), when would
 * start some operation. Scheduler runs in stand-alone thread and
 * provides setting timeouts and getting of current time.
 * <p>
 * Timeout has three items:
 * <pre>
 *  - command - name of timeouts
 *  - info - some additional information
 *  - time of expiration
 * </pre>
 *
 * @author Jirka
 * @see Eso
 */ 
class Scheduler: public Runable, public SixOffspring,  public Shutdownable
{
  friend class TimeSynchronizer; 
    
  protected:

    /*@name attributes */
    /*@{*/
      /// sorted table for storing timeouts
    TimeSortedTable *tbl;
    TableFieldQueue *defTbl;
      /// mutex to work with sorted table tbl
    pthread_mutex_t mutexTbl;
      /// state if time is ok or wrong
    bool TimeOk;   
      /// persistent storing of state
    Table *tblTimeOk;
    TableFieldQueue *defTblTimeOk;
      /// mutex to work with table tblTimeOk
    pthread_mutex_t mutex;
      /// state if thread would shutdown
    bool shutdown; 
    /*@}*/

    /*@name methods */
    /*@{*/
      /// Sets state TimeOk
    Err SetTimeOk(bool state);
      /// Gets state TimeOk
    bool GetTimeOk();
      /// Searchs table tbl if is any expired timeout.
    void ProcessTimeOut();
      /// Common method for setting timeout.
    Err CommonSetTimeOut(char *command, GMessage *info, ESTime *pTime);
    /*@}*/

  public:

    /*@name methods */
    /*@{*/
    Scheduler(LogFile *logFile, 
              Eso *eso);
    ~Scheduler();
      /// Starts in stand-alone thread when is Eso starting
    void* Run(void*);
      /// Sets state shutdown to true      
    virtual void Shutdown();
      /// Sets timeout to relative time from current time
    Err SetTimeOut( char *command, GMessage *info, RelTime time ); 
      /// Sets timeout to absolute time
    Err SetTimeOut( char *command, GMessage *info, ESTime time );
      /// Returns true if is timeout set
    bool IsSetTimeOut( char *command ); 
      /// Return current time if is time ok or NULL if is time wrong
    ESTime* GetTime();
    /*@}*/

};
#endif
