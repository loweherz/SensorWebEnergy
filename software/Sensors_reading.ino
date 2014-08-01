#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1;                   // Create an instance

void init_sensors(){
  //CT SENSOR
  emon1.current(CT_PIN, 111.1);
  delay(1000);
  //CT-Setup phase
  emon1.calcIrms(1480);
  emon1.calcIrms(1480);
  emon1.calcIrms(1480);
  
  pinMode(FLUSSO, INPUT);
  pinMode(CALDAIA, INPUT);
  pinMode(CENERI, INPUT);
  
  //PIN 2 - FLUSSOMETRO
  attachInterrupt(0, interruptFlusso, RISING);
  //PIN 3 - VANO CALDAIA
  attachInterrupt(1, interruptCaldaia, RISING);
}

void interruptCaldaia()
{
  _caldaia++;  
}

void interruptFlusso()
{
  _flusso++;
}

//DHT
#include <dht.h>
dht DHT; //dichiaro una variabile globale per la classe del sensore


void readDHT(){

  if(DHT.read22(DHT22_PIN)==0){  //valore di ritorno : 0 -> Lettura andata a buon fine
    _temp = DHT.temperature;
    _hum = DHT.humidity;
  }
  else{    
    _temp = -1000;
    _hum = -1000;
  }
}




#define ERROR_VALUE -1000

float readTempWater(byte type){
 OneWire ds(type);

 //returns the temperature from one DS18S20 in DEG Celsius
 byte data[12];
 byte addr[8];

 if ( !ds.search(addr)) {
   //no more sensors on chain, reset search
   ds.reset_search();
   //check_sensor_error = true; 
   return ERROR_VALUE;
 }

 if ( OneWire::crc8( addr, 7) != addr[7]) {
   showString(PSTR("CRC is not valid!\n"));
   //check_sensor_error = true; 
   return ERROR_VALUE;
 }

 if ( addr[0] != 0x10 && addr[0] != 0x28) {
   showString(PSTR("Device is not recognized\n"));
   //check_sensor_error = true; 
   return ERROR_VALUE;
 }

 ds.reset();
 ds.select(addr);
 ds.write(0x44,1); // start conversion, with parasite power on at the end

 byte present = ds.reset();
 ds.select(addr);  
 ds.write(0xBE); // Read Scratchpad

 
 for (int i = 0; i < 9; i++) { // we need 9 bytes
  data[i] = ds.read();
 }
 
 ds.reset_search();
 
 byte MSB = data[1];
 byte LSB = data[0];

 float tempRead = ((MSB << 8) | LSB); //using two's compliment
 
 float TemperatureSum = tempRead / 16;

 return TemperatureSum;  
}






float readFlusso(){   
   return _flusso;
}






SoftwareSerial sonar(RS232_SONAR, 10, true);

float readSonar_data(){  
  sonar.begin(9600);

  delay(500);

  char *buffer;
  char x;
  char array[3];  
  float inch, cm;

  //delay(50);  
  while(sonar.available()>0)
  {
     x = sonar.read();//Bytes(buffer,1);
     //delay(50);
     if(x=='R'){ 
       x= sonar.read();//Bytes(buffer,1);       
       array[0]=x; 
       x= sonar.read();//Bytes(buffer,1);       
       array[1]=x;//*buffer; 
       x= sonar.read();//Bytes(buffer,1);       
       array[2]=x;//*buffer;
     }     
  }  
  
  inch = (array[0]-48)*100 + (array[1]-48)*10 +(array[2]-48) ;
  cm = inch*2.54;
  
  while(sonar.available()>0){
    sonar.read();
    delay(2);
  }
  
  sonar.end();
  
  
  //Serial.print("->");
  //Serial.println(cm*0.42);
  delay(10);
  
  
  if(cm<0){
    readSonar_data();
    //delay(10);
  }
  
  
  return cm*(0.42); 
}



void readSonar(){
  byte count = 10;
  
  do{
    float tmp = readSonar_data();
    if(tmp>0){
      _sonar = tmp;      
      break;
    }
    else
      count--;
  }while(count>0);
  
  if(_sonar<0)
    _sonar = ERROR_VALUE;
  
}



void readEnergy(){

  double Irms;
 
  Irms = emon1.calcIrms(1480);  // Calculate Irms only
  
//  Serial.println(Irms*230.0);	       // Apparent power
//  Serial.print(" ");
  Serial.println(Irms);	
  _power=(Irms*143.3)-236.7;
  // coef conv (G1*146.5)-300.9
//  if( (Irms/3)>3 ) {
//    _power = ( (Irms/3) - 3 );    
//  }else{
//    _power = 0;
//  }
  Serial.println(_power);	
}






bool vanoCeneriAperto(){
  if(digitalRead(CENERI)==HIGH)
    return true;
  else
    return false;
}




bool vanoCaldaiaAperto(){
  if(digitalRead(CALDAIA)==HIGH)
    return true;
  else
    return false;
}

void readBattery(){
  #define referenceVolts 5 // the default reference on a 5-volt board

  #define R1 100000 // value for a maximum voltage of 20 volts
  #define R2 33000
  // determine by voltage divider resistors, see text
  //const int resistorFactor = 255 / (R2/(R1 + R2)); 

  float volts = (analogRead(BATTERY_PIN) / (255 / (R2/(R1 + R2)))) * referenceVolts ; // calculate the ratio
}

