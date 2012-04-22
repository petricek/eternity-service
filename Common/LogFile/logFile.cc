#include "./H/logFile.h"

/**
 * Log file constructor.
 *
 * Opens the specified file for appending and writes start of session
 * in there. 
 * <p>
 * Also sets initial debug options.
 *
 * @param   name  Log file name.
 * @author  Venca
 * @see     ~LogFile(), LogLevel
 */
LogFile::LogFile(const char *name)
{
  debugMessages[llDebug]      = "_";  
  debugMessages[llWarning]    = "Warning";  
  debugMessages[llError]      = "ERROR";  
  debugMessages[llFatalError] = "FATAL_ERROR";  

  fd = fopen(name, "a");
  if(fd == 0)
    fprintf(stderr, "Failed to open logfile %s\n", name);
  else 
  {
    fileName = new char [strlen(name)+1] ;
    strcpy(fileName, name);
    debugOptions = llAll;
    time_t tempTime;
    time(&tempTime);
    char *cas = ctime(&tempTime);
		cas[strlen(cas)-1] = '\0';
    fprintf(fd, "=================================================="
		"==========================\n");
    fprintf(fd, "[%s]:Warning:Log session started\n", cas);
  }
}

/**
 * Log file destructor.
 *
 * Writes end of session to log file and closes log file. 
 *
 * @author  Venca
 * @see     LogLevel
 */
LogFile::~LogFile()
{
  time_t tempTime;
  time(&tempTime);
  char *cas = ctime(&tempTime);
  cas[strlen(cas)-1] = '\0';
  fprintf(fd, "[%s]:Log session finished\n", cas);
  fprintf(fd, "=================================================="
              "==========================\n");     

  fclose(fd);
  DELETE(fileName);
}
/**
 * Write message to log file.
 *
 * Writes a string to log file if its logLevel matches debugOptions
 * of the log file.
 * <p>
 * Also prepends time. Every log entry is on separate line.
 *
 * @param   logLevel   log level of message - warning, error ...
 * @param   format     format string similar to printf()
 * @param   args       Additional variables we want to print
 * @return  void
 * @author  Venca
 * @see     Debugable, printf(), LogLevel
 */
void LogFile::WriteStringVA(LogLevel logLevel, char *format, va_list args) 
{  
     
    // if it is an error write it to stderr
  if((logLevel == llError) || (logLevel == llFatalError))
  {
    vfprintf(stderr, format, args); 
    fprintf(stderr, "\n"); 
  }

    // if it should be logged
  if(logLevel & debugOptions)
  {
    time_t tempTime;
    time(&tempTime);
			     
    char *tempTimeString = ctime(&tempTime);
      // ctime adds '\n' at the end of the string
    tempTimeString[strlen(tempTimeString) - 1] = '\0';
						    
    fprintf(fd,"[%s]:%s:", tempTimeString, debugMessages[logLevel]);
    fflush(fd);
    vfprintf(fd, format, args);
    fprintf(fd, "\n");
    fflush(fd);
  }
}

/**
 * Write message to log file.
 *
 * Writes a string to log file if its logLevel matches debugOptions
 * of the log file.
 * <p>
 * Also prepends time. Every log entry is on separate line.
 *
 * @param   logLevel   log level of message - warning, error ...
 * @param   format     format string similar to printf()
 * @param   ...        Additional variables we want to print
 * @return  void
 * @author  Venca
 * @see     Debugable, printf(), LogLevel
 */
void LogFile::WriteString(LogLevel logLevel, char *format, ...) 
{
  va_list args;

    // if it is an error write it to stderr
  if((logLevel == llError) || (logLevel == llFatalError))
  {
    vfprintf(stderr, format, args); 
    fprintf(stderr, "\n"); 
  }
  // if it should be logged
  if(logLevel & debugOptions)
  {
    va_start(args, format);
   
    time_t tempTime;
    time(&tempTime);

    // Pechy, 26.11.1998, 18:00
    char *tempTimeString = ctime(&tempTime);
    // ctime adds '\n' at the end of the string
    tempTimeString[strlen(tempTimeString) - 1] = '\0';

    fprintf(fd,"[%s]:%s:", tempTimeString, debugMessages[logLevel]);
    fflush(fd);
    vfprintf(fd, format, args); 
    va_end(args);
    fprintf(fd, "\n");
    fflush(fd);
  }

}

/**
 * Change what is logged in log file.
 *
 * Sets debug options = what should be logged. Accepts any combination
 * of logLevels. Returns old value of debug options.
 *
 * @param   options   Log levels that should be logged ored together.
 * @return  Old value of debug options.
 * @author  Venca
 * @see     WriteString(), Debugable
 */
int LogFile::SetDebugOptions(int options)
{
  int tempDebugOptions = debugOptions;
  debugOptions = options;
	return tempDebugOptions;
}
