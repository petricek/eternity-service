#ifndef LOG_FILE_H
#define LOG_FILE_H

  // has to be the FIRST include !
#include "./../../H/common.h"

#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#include <string>
#include <map>
/** 
 * Log level.
 *
 * Type that specifies all the possible levels of debug messages.
 *
 * @author Venca
 * @see LogFile, Debugable
 */
enum LogLevel { llNone = 0,
                llDebug = 1, 
                llWarning = 2,
		llError = 4,
		llFatalError = 8 ,
		llAll = 255 };

/**
 * Log file.
 *
 * class that encapsulates file for logging debug messages.
 * Has the ability to log just certain types of messages.
 *
 * @author Venca
 * @see LogLevel, Debugable
 */
class LogFile
{
  
  protected:

  /**@name attributes */
  /*@{*/
      /// Descriptions for types of messages
    map< int, char *, less<int> > debugMessages;
      /// Where to write all the messages
    char *fileName;
    FILE *fd;
      /// What to write
    int debugOptions;
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
      /// create log called name
    LogFile(const char *name);
    virtual ~LogFile();
      /// set what to log xor-ed log levels
    int SetDebugOptions(int options);
      /// Print to log 
    virtual void WriteString(LogLevel logLevel, char *format, ...);
      /// Print to log 
    virtual void WriteStringVA(LogLevel logLevel, char *format, va_list args);
  /*@}*/
};
#endif
