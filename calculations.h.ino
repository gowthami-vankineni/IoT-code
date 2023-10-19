#include "Arduino.h"

//this library holds all physiscs calculations

//Calculate   Temperature from analog input value
double calcTemp(int Value);

//Calculate   vapour pressure from temperature
double calcPw(double Temp);

//Calculate   relative humidity from wet and dry bulb temperature and vapour pressures at respective   temperatures
double calcRH(double Td, double Tw, double Pwd, double Pww);

//Calculate   sensitivity factor (Rs/R0) as function of relative humidity and temperature
double   calcfRHT (double RH, double T); 

//Calculate sensor resistance (Rs) from   analog input, after signal amplification with OpAmp
double calcRs(int sensorReading);

//Calculate   CO2 concentration from measured sensor resistance (Rs) and relative humidity and   temperature correction factor
double calcCO2(double Rs,double fRHT);
