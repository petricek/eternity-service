#ifndef ESOTIME_H
#define ESOTIME_H

#include <string>
#include <function.h>

  // length of time string for ESTime
#define TIMETEXT 26

  // defines of time period in seconds
#define MIN 60 
#define HOUR 3600 
#define DAY 86400 
#define MONTH 30*DAY
#define YEAR 365*DAY

/**
 * Ancestor for working with time.
 *
 * Implements basic method SetTime, GetTime.
 *
 * @see
 * @author Jirka
 */
class BasicTime {
protected:
/*@name attributes */
/*@{*/
  long time;
/*@}*/  
public:
/*@name methods */
/*@{*/
    /// Set time in seconds
  void SetTime(long t = 0);
    /// Set time in string format (yy:mm:dd:hh:mm:ss),
    /// for easy implementation are month 30 days and year 365 days
  bool SetTime(char *); 
    /// Set time in days, hours, minutes and seconds
  void SetTime(int day, int hour, int min=0, int sec=0);
    /// Get time in seconds
  long GetTime();
    ///Write time in seconds to buffer
  virtual void Write(char *buff);
/*@}*/
};

class ESTime;

/**
 * Class for working with relative time.
 *
 * Implements some operators for easier manipulation with instances of
 * this class.
 *
 * @author Jirka
 * @see
 */
class RelTime : public BasicTime {
public:
/*@name methods */
/*@{*/
  //Constructors
  RelTime(long t = 0);
  RelTime(char *str);
  RelTime(int day, int hour, int min=0, int sec=0);

  /// Some operators for RelTime
  RelTime operator+ (RelTime);
  RelTime operator* (int);
  void operator= (RelTime);
  ESTime operator+ (ESTime);
/*@}*/
};

/**
 * Class for working with real time.
 *
 * Implements some operators, work with time in GMT and add
 * some method for SetTime
 *
 * @author Jirka
 * @see
 */
class ESTime : public BasicTime {
public:
/*@name methods */
/*@{*/
  //Constructors and destructor
  ESTime();
  ESTime(RelTime rt);
  virtual ~ESTime() {};
  
    /// Set time to current time plus RelTime
  void SetTime(RelTime);
    /// Set absolute time (year, month, day, hour, minute and second),
    /// return true if time is set correctly
    //
    // Format of arguments:
    //  year: years since 1900 (for example: 1998 is 98): 70 - 134
    //  month: 1 - 12
    //  day: 1 - 31
    //  hour: 0 - 23
    //  min: 0 - 59
    //  sec: 0 - 59
  bool SetTime(int year, int month, int day, int hour=0, int min=0, int sec=0);
    /// Write time in time format to buffer (man asctime)
  virtual void Write(char *);
    /// Return pointer to string in time format
  char* Print();

    /// Some operators for ESTime
  ESTime operator+ (RelTime);
  void operator= (ESTime);
  RelTime operator- (ESTime);
/*@}*/
};

  /// From these operators STL (function.h) generates all the others
  /// ( >= <= != ...)
  bool operator<  (ESTime, ESTime);
  bool operator== (ESTime, ESTime);

#endif
