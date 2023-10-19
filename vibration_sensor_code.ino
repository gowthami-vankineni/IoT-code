#include <LiquidCrystal.h>
int sensorValue;
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
byte NTCPin = A0;
#define rSerie 10000
#define rNominal 10000
#define tNominal 298.15
#define coeficientB 3950
void setup() { lcd.begin(16, 2);
Serial.begin(9600); 
  
  
}
void loop(){sensorValue = analogRead(0);       // read analog input pin 0
Serial.print("AirQua=");
Serial.print(sensorValue, DEC);               // prints the value read
Serial.println(" PPM");
lcd.setCursor(0,0);
lcd.print("ArQ=");
lcd.print(sensorValue,DEC);
lcd.print(" PPM");
lcd.println("       "); 
lcd.print("  ");
delay(100);

float valorADC;
float rNTC, temperature;
//Valor lleigt al Arduino
valorADC = analogRead(NTCPin);
Serial.println();
Serial.print("ADC:");
Serial.print(valorADC);
Serial.print(" ");
// Valor de ressistencia NTC calculat
// PULL-DOWN CONFIGURATION
rNTC = rSerie * ((1023/valorADC)-1);
// Valor de ressistencia NTC calculat
// PULL-UP CONFIGURATION
// rNTC = rSerie / ((1023/valorADC)-1);
Serial.print("Rntc:");
Serial.print(rNTC);
Serial.print(" ");
// STEIN - REINHART EQUATION
temperature = 1/((1/tNominal)+(log(rNTC/rNominal)/coeficientB));
temperature -= 273.15;
Serial.print("Temperature:");
Serial.print(temperature);
Serial.print("C");
delay(5000);



}


  
