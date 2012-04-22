#include "./H/estime.h"
#include "./H/myString.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h> //time(), gmtime(), acstime()

void BasicTime::SetTime(long t){
  time = t;
}

long BasicTime::GetTime(){
  return time;
}

#define FULLTIME 17

// format of string --> yy:mm:dd:hh:mm:ss
bool BasicTime::SetTime(char *str){
  int day, hour, min, sec, year, month;

  time = 0;
  //validation
  if ( !( (strlen(str) == (FULLTIME)) &&
    isdigit(str[0]) && isdigit(str[1]) && (str[2] == ':') &&
    isdigit(str[3]) && isdigit(str[4]) && (str[5] == ':') &&
    isdigit(str[6]) && isdigit(str[7]) && (str[8] == ':') &&
    isdigit(str[9]) && isdigit(str[10]) && (str[11] == ':') &&
    isdigit(str[12]) && isdigit(str[13]) && (str[14] == ':') &&
    isdigit(str[15]) && isdigit(str[16]) ) ) return false;

  char buf[3] = {0, 0, 0};
  buf[0] = str[0];
  buf[1] = str[1];
  year = atoi(buf);

  buf[0] = str[3];
  buf[1] = str[4];
  month = atoi(buf);

  buf[0] = str[6];
  buf[1] = str[7];
  day = atoi(buf);

  buf[0] = str[9];
  buf[1] = str[10];
  hour = atoi(buf);

  buf[0] = str[12];
  buf[1] = str[13];
  min = atoi(buf);

  buf[0] = str[15];
  buf[1] = str[16];
  sec = atoi(buf);

  //SetTime(day, hour, min, sec);
  time = year * YEAR + month * MONTH + day * DAY + hour * HOUR + min * MIN + sec;
  return true;
};

void BasicTime::SetTime(int day, int hour, int min, int sec){
  time = day * DAY + hour * HOUR + min * MIN + sec;
};

void BasicTime::Write(char *buff){
  sprintf(buff, "%lu", time);
};

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

RelTime::RelTime(long t) {
  SetTime(t);
}

RelTime::RelTime(char *str) {
  SetTime(str);
}

RelTime::RelTime(int day, int hour, int min, int sec) {
  SetTime(day, hour, min, sec);
}

RelTime RelTime::operator+ (RelTime arg_right){
  RelTime rt;
  rt.SetTime( GetTime() + arg_right.GetTime() );
  return rt;
};

RelTime RelTime::operator* (int arg_right){
  RelTime rt;
  rt.SetTime( GetTime() * arg_right);
  return rt;
}

void RelTime::operator= (RelTime arg_right){
  SetTime( arg_right.GetTime() );
};

ESTime RelTime::operator+ (ESTime arg_right){
  ESTime t;
  ((BasicTime*)&t)->SetTime( GetTime() + arg_right.GetTime() );
  return t;
};

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

ESTime::ESTime(){
  long t;

  ::time(&t);
  ((BasicTime*)this)->SetTime( t );
};

ESTime::ESTime(RelTime rt){
  SetTime(rt);
}

void ESTime::SetTime(RelTime rt){
  long t;

  ::time(&t);
  ((BasicTime*)this)->SetTime( t + rt.GetTime() );
};

bool ESTime::SetTime(int year, int month, int day, int hour, int min, int sec){
  struct tm tm1;
  long time;

  tm1.tm_sec = sec;
  tm1.tm_min = min;
  tm1.tm_hour = hour;
  tm1.tm_mday = day;
  tm1.tm_mon = month - 1;
  tm1.tm_year = year;
  time = timelocal(&tm1);
  if (time == -1) return false;
  ((BasicTime*)this)->SetTime( time );
  return true;
};

void ESTime::Write(char *buff){
  struct tm *ptm1;
  long t;

  t = GetTime();
  ptm1 = gmtime( &t );
  MyStrCpy(buff, asctime(ptm1), TIMETEXT, 0, 0);
};

char* ESTime::Print(){
  char *buff = new char[TIMETEXT];

  this->Write(buff);
  buff[TIMETEXT-2] = 0;
  return buff;
};

ESTime ESTime::operator+ (RelTime arg_right){
  ESTime t;
  ((BasicTime*)&t)->SetTime( GetTime() + arg_right.GetTime() );
  return t;
};

void ESTime::operator= (ESTime arg_right){
  ((BasicTime*)this)->SetTime( arg_right.GetTime() );
};

RelTime ESTime::operator- (ESTime arg_right){
  RelTime rt;
  rt.SetTime( GetTime() - arg_right.GetTime() );
  return rt;
};

bool operator== (ESTime arg_left, ESTime arg_right){
  if (arg_left.GetTime() == arg_right.GetTime()) return true;
  else return false;
};

bool operator< (ESTime  arg_left, ESTime  arg_right){
  if (arg_left.GetTime() < arg_right.GetTime()) return true;
  else return false;
};
