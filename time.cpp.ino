#include "Arduino.h"
#include "Time.h"

Time::Time (byte   hr, byte mnt, byte sec, byte d, byte m, byte y) { 
  _timeStart = (hr*60UL+mnt)*60UL+   sec; //initialize _timeStart 
  _lastMillis = millis();               //last   time millis() was called
  _curTime = _lastMillis/1000UL;        //time since   start of program in seconds (rollover after 49000 days, rollover is prevented by   updDate() after 1 day)

  //initialize _Time[]
  _Time[0]      = hr;
   _Time[1]      = mnt;
  _Time[2]      = sec; 

  //initialize _Date[]
   _Date[0]      = d;
  _Date[1]      = m;
  _Date[2]      = y;

  _dayRollover   = false; 
}


void Time::updTime() {
  unsigned long curMillis   = millis();
  _curTime   += curMillis/1000UL - _lastMillis/1000UL;  //update   _curTime
  _lastMillis = curMillis;                              //record _lastMilles   for use during next call of updTime()

  unsigned long curTime = _curTime   + _timeStart;
  if               (curTime/(24UL*60UL*60UL) > 0) _dayRollover   = true;  //if time goes beyond 23:23:59 dayrollover occurs and time returs toe
   //update _Time[]
  curTime      =    curTime%(24UL*60UL*60UL);
  _Time[0]      =    curTime/(60UL*60UL);
  curTime      =    curTime%(60UL*60UL);
  _Time[1]      =    curTime/60UL;
  curTime      =    curTime%60UL;
  _Time[2]     =     curTime;

  //if needed, update _Date
  if (_dayRollover) updDate();   //if dayrollover, then adjust date
}


void Time::updDate() {
  const   byte maxDays[13] = {0,31,29,31,30,31,30,31,31,30,31,30,31}; //2020 is leap year,   next year change num days of February
  const byte day   = 0; //makes code more   easy to read
  const byte month = 1;
  const byte year  = 2;
  
  _Date[day]++;
   if (_Date[day] > maxDays[_Date[month]]) {  //if monthrollover then adjust month
     _Date[day]     = 1;
    _Date[month]++;
    if (_Date[month] > 12) {                 //if   yearrollover then adjust year
      _Date[month] = 1;
      _Date[year]++;
     }
  }
  _curTime = _curTime - (24UL*60UL*60UL);    //update _curTime (subtract   one day)
  _dayRollover = false;
}


unsigned long Time::lastMillis()   {
  return _lastMillis;
}


String Time::timeToString(String sign)   {
  return toString(_Time, sign);
}


String Time::dateToString(String   sign) {
  return toString(_Date, sign);
}


String Time::toString(byte*   tostring, String sign) {
  String string = "";
  string += leadingZeros(tostring[0]);
   for (byte i=1; i<3; i++) {
     string+= sign;
     string+= leadingZeros(tostring[i]);
   }
  return string;
}


String Time::leadingZeros(byte value) {
   String string = "";
  if (value < 10) {
    string += "0";
  }
   string += String(value);   
  return string;
}
