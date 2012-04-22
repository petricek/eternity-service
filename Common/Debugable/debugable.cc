#include "./H/debugable.h"

#include "./../RSAEuro/H/rsaref.h"

/**
 * Change debug options of the debugable object.
 *
 * Sets new value in debug options that are used to decide whether
 * the message will be written into log file and returns the old
 * value so that the caller can restore these settings.
 *
 * @param   newLogOptions  
 * @return  Old value of debug options
 * @author  Venca
 * @see     WriteString()
 */
int Debugable::SetDebugOptions(int newLogOptions)
{
  int ret;
  ret = this->debugOptions;
  this->debugOptions = newLogOptions;
  return ret;
}

/**
 * Writes a message to LogFile.
 *
 * Checks if the current debug options allow this log level to be
 * written to log file and if so then it asks log file to write it
 * there. This allows for different debug level for different objects.
 *
 * @param   debugLevel  log level of the message (warning, error ...)
 * @param   format      Format string with the same semantics and
 *                      syntax as the printf format string.
 * @param   ...         Additional parameters that should be
 *                      incorporated into into format.
 * @return  co
 * @author  Venca
 * @see     printf(), LogFile, LogLevel
 */
void Debugable::WriteString(LogLevel debugLevel, char * format,...) 
{

  va_list args;

  if ( logFile == NULL ) {
    if(debugLevel > llDebug){
      va_start(args, format);
      fprintf(stderr, format, args);
      va_end(args);
    }
    return;
  }

    // if it should be logged
  if(this->debugOptions & debugLevel) {
    va_start(args, format);
    logFile->WriteStringVA(debugLevel, format, args);
    va_end(args);
  }
}

/**
 * Changes the log file.
 *
 * Remembers pointer to the LogFile and inits debugOptions. Allows
 * for log file switching - may be used to debug a certain method
 * into a different file.
 *
 * @param   aLogFile
 * @return  void
 * @author  Pavel
 * @see     WriteString()
 */
void Debugable::SetLogFile(LogFile *aLogFile)
{
  logFile = aLogFile;
}

/**
 * Debugable constructor.
 *
 * Stores pointer to the log file and initializes debug options.
 *
 * @param   debugLogFile Pointer to log file that should be used for
 *                       debugging.
 * 
 * @author  Venca
 * @see     WriteString, LogFile
 */
Debugable::Debugable(LogFile * debugLogFile)
{
  this->logFile = debugLogFile;
  debugOptions = llAll;
}

/**
 * Debugable destructor.
 *
 * Empty - no need to do anything.
 *
 * @author  Venca
 * @see     Debugable()
 */
Debugable::~Debugable()
{
}

/**
 * Creates a string for numbering certificates.
 *
 * @param   firstStr   String to take as base for building result
 * @param   firstInt   First number to add
 * @param   secondInt  Second number to add
 * @return  Allocated string containing:
 * <pre>
 * ``firstStr''-``firstInt''-``secondInt'', if secondInt is
 * equal to -1, only ``firstStr''-``firstInt'' is created.
 * </pre>
 * 
 * @author  Pechy
 * @see     Translator, MessageCreator
 */
string * Debugable::CreateString(char *firstStr, int firstInt,
                                 int secondInt)
{
    // ``+ 2'' is for "-" and trailing '\0'
  char firstStringInt[DIGITS_IN_NUMBER + 2];
  char secondStringInt[DIGITS_IN_NUMBER + 2];

  secondStringInt[0] = '\0';

  snprintf(firstStringInt, DIGITS_IN_NUMBER + 2, "-%d", firstInt);
  if (secondInt != NO_PARAM)
    snprintf(secondStringInt, DIGITS_IN_NUMBER + 2, "-%d",
             secondInt);

    // clear possible contains
  string *createdString = new string("");

  *createdString = *createdString + firstStr + firstStringInt 
                   + secondStringInt;

  return createdString;
}
