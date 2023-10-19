/*
 *Time.h - Time library
 *Records starting time and date of project   (as given in source code by programmer)
 *Can return String with current Time   (hh:mm:ss) 
 *Can return String with current date (dd-mm-yy)
 *Can return   millis since start Time
 *Created by Koen Meesters, december 2019
 *Last edited   Februari 2020
 */
#ifndef Time_h
#define Time_h

#include "Arduino.h"

class   Time {
  public:
    Time(byte hr = 0, byte mnt = 0, byte sec = 0, byte d   = 1, byte m = 1, byte y = 20); // date by default initialized to 1-1-2020
    
     String timeToString(String sign = ":"); //returns time in a String, default   sign for time is :
    String dateToString(String sign = "-"); //returns date   in a String, default sign for date is -

    unsigned long lastMillis(); 
     void updTime();                         //update the time in _Time[3]
    
   private:
    byte _Time[3];                          //stores time in 3 bytes   hr min sec
    byte _Date[3];                          //stores date in 3 bytes   day month year
    bool _dayRollover;                      //flag set if time   goes beyond 23:59:59   

    unsigned long _timeStart;               //start   time in seconds
    unsigned long _curTime;                 //current time in   seconds
    unsigned long _lastMillis;              //millis() last time updTime()   was run 


    void updDate();                         //update the date   in _Date[3] (should be run at least every day, otherwise it misses 1 dayrollover)
     
    String toString(byte*, String sign);    //makes a string from _Time[3]   or _Date[3]
    String leadingZeros(byte value);        //adds leading zeros   if number is smaller than 10 ( 3 --> "03") 
};

#endif
