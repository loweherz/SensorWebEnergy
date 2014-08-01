/*
  *********************************
      Sensor Web Energy Station 
                  -
     sistema di monitoraggio a 
     basso costo degli apparati 
     di produzione energia/calore
  *********************************
  
  Last update: 04/12/2013 - 16:30
  Project by: Alessandro Zaldei (CNR - IBIMET)
  Review by: Mirko Mancin (UNIPR - WASNLAB)
  Last updater: Mirko Mancin (UNIPR - WASNLAB)
  Version: sensor_web_energy_0.3


  * PINOUT * 

  A0- SONAR         A3- DHT22
  A1- CT            A4
  A2- BATTERY       A5
  
  2- CONTA_LT       10- SPORT. CENERI
  3- SPORT. CALDAIA 11- SONAR_PWM
  4-                12- 
  5- Liquido MAND   13
  6- Liquido RIT    
  7- RX GSM         
  8- TX GSM         
  9- RS232 SONAR
*/

#define SONAR_AN A0
#define CT_PIN A1  
#define BATTERY_PIN A2
#define DHT22_PIN A3     
      
#define CENERI 10
#define CALDAIA 3
#define MANDATA 5
#define RITORNO 6
#define RS232_SONAR 9
#define FLUSSO 2



#include <SoftwareSerial.h>
#include <Wire.h>
#include <OneWire.h>
#include "LowPower.h"

/* VARIABILE PER IL TEMPO */
unsigned int time = millis();

/*************************
  DEFINE PER IMPOSTAZIONI
  
  * TOGLIERE COMMENTI 
    PER DEFINIRE LE VARIABILI    
*************************/
#define SERIAL_DEBUG 0
//#define INIT_TIME 0


/************
  WAIT
*************/
#define DEFINE_WAIT 0
int SECONDS_TO_SLEEP = DEFINE_WAIT;

/**********
  RTC
***********/
#include <RTClib.h>
RTC_DS1307 RTC;

/**************
  GLOBAL
**************/
float   _temp=0.0;
float   _hum=0.0;
float   _mandata=0.0;
float   _ritorno=0.0;
float   _sonar=0.0;
double  _power=0.0;
int     _flusso=0;
int     _caldaia=0;


//Dichiarazione di funzione che punta all'indirizzo zero
void(* Reset)(void) = 0;



void setup()
{  
  Wire.begin();
  //#ifdef SERIAL_DEBUG
    Serial.begin(9600);// inizializza seriale
  //#endif

  // RTC Setup
  RTC.begin();
  
  #ifdef INIT_TIME
    RTC.adjust(DateTime(__DATE__, __TIME__));  // definisci il realtime
  #endif
 
  init_sensors();
  
  // GSM Setup
  initGSM_Serial();
  delay(1000); 
    
     
  #ifdef SERIAL_DEBUG
    showString(PSTR("\n*********\nInitialising...\n\n"));
    delay(100); //Allow for serial print to complete.
  #endif  
  
  time = millis();
} 





void loop(){
  if( (millis() - time) > (DEFINE_WAIT*1000)){
    sensingAndSend();
    time = millis();
  } 
}



void sensingAndSend(){   
    
    #ifdef SERIAL_DEBUG
      showString(PSTR("Reading DHT...\n"));
      delay(10); //Allow for serial print to complete.
    #endif    
    /* Sensors_reading - LETTURA DHT */
    readDHT();
    
    #ifdef SERIAL_DEBUG
      showString(PSTR("Reading Mandata...\n"));
      delay(10); //Allow for serial print to complete.
    #endif    
    /* Sensors_reading - LETTURA TEMP MANDATA */
    _mandata = readTempWater(MANDATA);
    
    #ifdef SERIAL_DEBUG
      showString(PSTR("Reading Ritorno...\n"));
      delay(10); //Allow for serial print to complete.
    #endif    
    /* Sensors_reading - LETTURA TEMP RITORNO */
    _ritorno = readTempWater(RITORNO);
    
    #ifdef SERIAL_DEBUG
      showString(PSTR("Reading Flusso...\n"));
      delay(10); //Allow for serial print to complete.
    #endif    
    /* Sensors_reading - LETTURA FLUSSO */
    _flusso = readFlusso();
    
    #ifdef SERIAL_DEBUG
      showString(PSTR("Reading Sonar...\n"));
      delay(10); //Allow for serial print to complete.
    #endif    
    /* Sensors_reading - LETTURA SONAR */
    readSonar();
    
    #ifdef SERIAL_DEBUG
      showString(PSTR("Reading Energy...\n"));
      delay(10); //Allow for serial print to complete.
    #endif    
    /* Sensors_reading - LETTURA ENERGY */
    readEnergy();
    
       
    
    #ifdef SERIAL_DEBUG
      showString(PSTR("\nSensing complete!\n"));
      delay(100); //Allow for serial print to complete.
    #endif
  
    
    
    // Accendo e imposto il modulo GSM per funzionare con l'APN corretto
    //GSM
    turnONGSM();
    initialSetup();   
  
    delay(100);
    #ifdef SERIAL_DEBUG
      showString(PSTR("\nInitialising complete!\n"));
      delay(100); //Allow for serial print to complete.
    #endif
    
    
    //INVIO AL SERVER - arg1 = Server, arg2 = File dove inviare i dati
    sendGSMData("/*URL*//sensorwebhub/j_set_data?dato_in=");
    
    delay(500);
    //GSM - SPENGO
    turnOFFGSM();       
    
    //delay(1000*DEFINE_WAIT);
    //Reset();
    
    #ifdef SERIAL_DEBUG
      showString(PSTR("\nMemory test results: "));
      Serial.print(memoryTest(),DEC);
      showString(PSTR(" bytes free\n"));
      
      delay(500);
    #endif
    
    /*
    Serial.print("SONAR: ");
    Serial.println(_sonar);
    Serial.print("FLUSSO: ");
    Serial.println(_flusso);
    Serial.print("CALDAIA: ");
    Serial.println(_caldaia);
    */
    
    // RESET VARIABILI 
    _temp=0;
    _hum=0;
    _mandata=0;
    _ritorno=0;
    _sonar=0;
    _power=0;
    _flusso=0;
    _caldaia=0;
    
    Reset();
    
}

/*
void sleepForSeconds(int time){
  for(int i=0; i<(int)time/8; i++){
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}
*/

#ifdef SERIAL_DEBUG
    // this function will return the number of bytes currently free in RAM
    int memoryTest() {
      int byteCounter = 0; // initialize a counter
      byte *byteArray; // create a pointer to a byte array
      // More on pointers here: http://en.wikipedia.org/wiki/Pointer#C_pointers
    
      // use the malloc function to repeatedly attempt allocating a certain number of bytes to memory
      while ( (byteArray = (byte*) malloc (byteCounter * sizeof(byte))) != NULL ) {
        byteCounter++; // if allocation was successful, then up the count for the next try
        free(byteArray); // free memory after allocating it
      }
      
      free(byteArray); // also free memory after the function finishes
      return byteCounter; // send back the highest number of bytes successfully allocated
    }
#endif
