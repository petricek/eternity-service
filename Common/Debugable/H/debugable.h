#ifndef DEBUGABLE_H
#define DEBUGABLE_H

#include "./../../LogFile/H/logFile.h"

#include <string>

#define DISPLAY_STREAM_MAX_LEN 200 

  /** Debugging macros */
  /*@{*/
#define DBG WriteString(llDebug, __FILE__ ":%d:DBG", __LINE__); 
#define DHT(msg,title) msg->DebugHeaders(logFile, __FILE__ title); 
#define DH(msg) msg->DebugHeaders(logFile, __FILE__ "GMessage contains"); 
#define LDBG logFile->WriteString(llDebug, __FILE__ ":%d:DBG", __LINE__); 
  /*@}*/

/**
 * Debugging class.
 *
 * Has the ability to write its progress into log file - it has one and 
 * can write into it using its method WriteString.
 *
 * @author  Venca
 * @see     Runable LogFile
 */
class Debugable {

  public:
  /**@name attributes */
  /*@{*/
    /// What messages should be logged
    int debugOptions;
    /// Log file for debug messages
    LogFile * logFile;
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
    /// Change what is debugged
    virtual int SetDebugOptions(int newLogOptions);
    /// Write message to log file
    virtual void WriteString(LogLevel debugLevel, char * format,...); 
    /// Change log file
    void SetLogFile(LogFile *aLogFile);

    /// constructor
    Debugable(LogFile * debugLogFile = NULL);
    /// destructor
    virtual ~Debugable();

    /// Helper method
    string * CreateString(char *firstStr, int firstInt, int secondInt);
  /*@}*/

};
#endif
