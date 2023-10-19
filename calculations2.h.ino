#include "Arduino.h"
#include "Calculations.h"


double   calcTemp(int Value) {  //https://playground.arduino.cc/ComponentLib/Thermistor2/
   double Temp;
  Temp = log(10000.0/(1024.0/Value-1)); // for pull-up configuration
   Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp   );
  Temp = Temp - 273.15;                // Convert Kelvin to Celcius
  
   return Temp;//degrees C
}


double calcPw(double Temp) {  //R.C. Rodgers   and G.E. Hill, Brittish Journal of Anaesthesia (1978),50, 415
  const double   A = 7.16728;
  const double B = 1716.984;
  const double C = 232.538;
   double exponent = A - B/(Temp+C); 
  double Pw = pow(10,exponent);
  
   return Pw;//kPa
}


double calcRH(double Td, double Tw, double pWd,   double pWw) {  //https://www.1728.org/relhum.htm
  double RH;
  if(Tw >= Td)   { //to prevent RH>1 due to measurement errors where Tw >= Td
    RH = 1.0;
   }
  else {
    double pav = 101.55; //atmospheric pressure in kPa http://www.klimaatatlas.nl/klimaatatlas.php
     double N = 0.0006687451584; //kPa/K
    RH  = (pWw - N*pav*(1+0.00115*Tw)*(Td-Tw))/pWd;
   }
  
  return RH; //dimensionless
}  


double calcfRHT (double   RH, double T) {   //Own fit from MQ-135 datasheet. It is presumed that CO2 readings   are equally influenced by temperature and RH as ammonia readings in given graphs
   const double Intercept = 1.782663144;
  const double RC_RH     = -0.748177946;
   const double RC_T      = -0.015924756;
  const double RC_RHT    =  0.006677957;
   double calcfRHT = Intercept + RC_RH*RH + RC_T*T + RC_RHT*RH*T;
  
  return   calcfRHT; //Rs/R0 dimensionless
}


double calcRs(int sensorReading)   {   //Calculates Rs from amplified analog signal
  const double gain = 1.0 +   10000.0/1000.0; //amplification of opamp as function of resistances in opamp circuit
   const double Rref = 10.0; //Reference resistance (in kOhm) built in MQ-135 board   
  double sensorOrigReading = sensorReading/gain;
/*formula derivation:
   *sensorOrigReading = 1024*Rref/(Rref+Rsensor)
 *sensorOrigReading = 1024*1/(1+Rsensor/Rref)
   *(1+Rsensor/Rref)  = 1024/sensorOrigReading
 *Rsensor = (1024/sensorOrigReading-1)*Rref
   */
  double Rsensor = (1024.0/sensorOrigReading-1.0)*Rref; 
  
  return   Rsensor; //kOhm
}


double calcCO2(double Rs, double fRHT) {
  const   double R410    = 270.0; //Measured resistance (in kOhm) after one night with open   window
  const double fRHT410 = 1.08;  //calcutated correction factor after one   night with open window
  const double a  =  410.0;
  const double b  = -2.769034857;   //slope from Mad Frog
  
  double CO2 = a*pow((Rs/fRHT)/(R410/fRHT410),b);   //CO2atm = 410 ppm
  
  return CO2;//ppm
}
