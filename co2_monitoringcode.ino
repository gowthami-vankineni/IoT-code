#include <HTTPUpdate.h>

#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal.h>
#include "Time.h"
#include "Calculations.h"

const int rs = 8, en = 7, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;
//pin connected to CS of SD card shield
const int chipSelect = 2;

#define _dd_   20
#define _mm_    1
#define _yy_   20

#define _hr_   17
#define _min_   54
#define _sec_   0

char fileName[13];

//Set start time and date
Time myTime(_hr_,_min_,_sec_,_dd_,_mm_,_yy_); //hr,min,sec,d,m,y

//used connections to Arduino
const byte Td_sensor  = A0;
const byte Tw_sensor  = A1;
const byte CO2_sensor = A2;
const byte checkLight =  9;


//Data are stored in data array (double data[]) to ease printing and writing to SD card and LCD screen. Here the positions of the data in the array are given. 
const byte Td   = 0; //deg Celcius
const byte Tw   = 1;  
const byte RH   = 2; //relative humidity
const byte CO2  = 3; //ppm
const byte pWd  = 4; //kPa
const byte pWw  = 5; 
const byte fRHT = 6; //-
const byte Rs   = 7; //kohm
const byte numData = 8; //number of data in data []


const String degC = String ("\xC2\xB0") + String("C");
const String labels[numData] = {"Td" ,"Tw" ,"RH","CO2","pWd","pWw","fRHT","Rs"  };
const String units [numData] = {degC ,degC ,"-" ,"ppm","Pa" ,"Pa" ,"-"   ,"kohm"};


void setup() {
  pinMode(Td_sensor ,INPUT);
  pinMode(Tw_sensor ,INPUT);
  pinMode(CO2_sensor,INPUT);
  pinMode(checkLight,OUTPUT);

  lcd.begin(16, 2);
  lcd.print("hello, world!");
  
  Serial.begin(9600);
  String fileNameConst = String("WK") + myTime.dateToString("") + String(".txt");
  strcpy(fileName,fileNameConst.c_str());

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  delay(200);
  
  // if the file is available, write to it:
  // prepare datastring with current time and datalabels as column headers
  myTime.updTime();
  String dataString = myTime.dateToString();
  dataString+= ",";
  dataString+= myTime.timeToString();
  for (int i = 0; i<numData; i++) {
    dataString+= ",";   
    dataString+= labels[i];
  }

  File dataFile = SD.open(fileName,FILE_WRITE);
  if (dataFile) {
    dataFile.println();
    dataFile.println(dataString);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  } 
}


void loop() {
  myTime.updTime();
  digitalWrite(checkLight,LOW);   //put here to see if the system is working. now not needed anymore as LCD was installed

  Serial.println(myTime.timeToString() + " " + myTime.dateToString());

  //fill data array with fresh measurement results
  double data [numData];
  data[Td]   = calcTemp(readSensor(Td_sensor));  //degrees C
  data[Tw]   = calcTemp(readSensor(Tw_sensor));  
  data[pWd]  = calcPw(data[Td]);                 //kPa
  data[pWw]  = calcPw(data[Tw]);
  data[RH]   = calcRH(data[Td],data[Tw],data[pWd],data[pWw]); //dimensionless
  data[fRHT] = calcfRHT(data[RH],data[Td]);      //humidity and temperature correction factor, dimensionless
  data[Rs]   = calcRs(readSensor(CO2_sensor));   //kohm
  data[CO2]  = calcCO2(data[Rs],data[fRHT]);     //ppm

  dataToScreen(labels,data,units);

  dataToLCD(labels,data);
  
  // make a string for assembling the data to log:
  String dataString = "";

  dataString += myTime.dateToString();  //start with current time and date
  dataString += ",";
  dataString += myTime.timeToString();  

  for (int i=0; i<numData; i++) {          //add all data to dataString
    dataString += ",";    
    dataString += String(data[i]);
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(fileName, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
  
  //halfway time: show time, date, Rs and CO2
  lcd.clear();
  lcd.print(myTime.timeToString());
  lcd.print("   ");
  lcd.print(myTime.dateToString());
  lcd.setCursor(0,1);  
  lcd.print("Rs=");
  lcd.print(data[Rs],0);
  lcd.print("  CO2=");
  lcd.print(data[CO2],0); 
  
  digitalWrite(checkLight,HIGH);
  const unsigned long loopDuration = 30000;
  while (millis() - myTime.lastMillis() < loopDuration) { //wait until loopDuration 
    ;
  }
}
  

//return sensor value
int readSensor(const byte address) {
  int value = 0;
  byte numMeas = 32; //should not be larger than 2^6=64 (int value may roll over)
  for (int i=0; i<numMeas; i++) {
    value +=  analogRead(address);
    delay(100);  //this makes the sketch slow (32 times 100 times 3 = 9600 millisec (almost 10 sec)
  }
  value /= numMeas;
  return value;
}


//write data to screen
void dataToScreen(const String label[],const double data[],const String unit[]) {  //all arguments declared constant as this procedure is not supposed to change any data
  for (int i=0; i<numData; i++) {
    if (i==RH) { //RH is printed as % instead of fraction
      Serial.print(label[i]), Serial.print(":\t"), Serial.print(data[i]*100,2),  Serial.print("\t"), Serial.println("%");
    }
    else {
      Serial.print(label[i]), Serial.print(":\t"), Serial.print(data[i],2),      Serial.print("\t"), Serial.println(unit[i]);  
    }
  }
  Serial.println();
}


//write data to LCD
void dataToLCD(const String label[],const double data[]) {
  lcd.clear(); 
  for (int i = Td; i<=CO2; i++) {
       lcd.print(label[i]);
       lcd.print("=");
       int decimals = 1;
       if (i==CO2) decimals = 0;
       if (i== RH) decimals = 2;
       lcd.print(data[i],decimals);  
       lcd.print(" ");  
       if (i== Td) lcd.print(" ");
       if (i== Tw) lcd.setCursor(0,1); 
  }
}
