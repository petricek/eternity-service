#ifndef TIMESYNCHRONIZER_H
#define TIMESYNCHRONIZER_H

#include "../../../Common/GMessage/H/table.h"
#include "../../../Common/GMessage/H/estime.h"
#include "../../../Common/GMessage/H/msgField.h"
#include "../../../Common/UidGenerator/H/uidGenerator.h"
#include "../../../Common/LogFile/H/logFile.h"
#include "../../Scheduler/H/scheduler.h"
#include "../../../Common/ACManager/H/acManager.h"
#include "../../../Common/MessageQueue/H/messageQueue.h"
#include "../../Majordomo/H/majordomo.h"

#define TS_PATH "TimeSynchronizer/"
#define TS_GENERATOR "tsid_gen.idx"
  // name of table for storing responses
#define TS_TABLE "ts_table.tbl"

  // fields of GMessage
#define TS_ID "ID"
#define TS_TYPE "Type"
#define TS_REPLY "ReplyToAC"
#define TS_TIME "Time"
  // column of table
#define TS_TIMECOLUMN "Time"

  // Config file parameters
#define TS_SYNCHRONIZE_AT_START 0
  // 0 no
  // 1 yes
#define TS_TYPE_OF_SYNCHRONIZATION 0
  // WITH_OTHER_ESOS 0
  // WITH_ATOMIC_TIME 1
#define TS_TIME_TO_SYNCHRONIZE 100000L //in seconds
#define TS_NUMBER_TO_SYNCHRONIZE 20
#define TS_NUMBER_TO_COMPUTE 3 //15
#define TS_NUMBER_TO_DELETE 0 //3
#define TS_TIME_TO_RESPONSE 40000L //in seconds
#define TS_TIME_TOLERANCE 1000L //in seconds
#define TS_WITH_OTHER_ESOS 0

#define TS_LOGLEVEL llWarning

class Scheduler;

/**
 * This class is responsible for time synchronization.
 * 
 * Sends requests to other Esos, processes responses of other Esos and 
 * informs when time is wrong.
 *
 * @author Jirka
 * @see    Eso
 */
class TimeSynchronizer: public SixOffspring, public Debugable
{
  protected:
  
  /*@name attributes */
  /*@{*/
      /// identifier of synchronization
    ID ts_id;
      /// state if synchronization is running
    bool enableSynch;
      /// number of responses saved in table
    int numberItem;
      /// starting time of synchronization
    ESTime beginSynch;
      /// table for responses from other Esos
    Table *tbl;
    TableFieldQueue *defTbl;
      /// persistent identifier of synchronization
    UniqueIDGenerator *idGen;
  /*@}*/

  /*@name methods */
  /*@{*/
      /// Sets state of time (if is ok or wrong)
    void SetTimeOk(Scheduler *sch, bool state);
  /*@}*/
    
  public:

  /*@name methods */
  /*@{*/
    TimeSynchronizer(LogFile *aLogFile, Eso *eso);
    ~TimeSynchronizer();
      /// Initiate some attributes and set timeout to begin synchronization
    Err Init();
      /// Sends requests to other Esos and set timeouts 
      /// to begin of synchronization and running of synchronization
    Err TO_TimeToSynchronize();
      /// Responses to request for current time
    Err TS_Request(MsgField *dataField);
      /// Saves responses to table and then computes divergence from
      /// current time. Informs when time is wrong.
    Err TS_Response(MsgField *dataField);
      /// Ends synchronization.
    Err TO_Response(MsgField *dataField);
  /*@}*/
};
#endif
