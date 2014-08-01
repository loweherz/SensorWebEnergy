/************
  MODULO GSM
************/
SoftwareSerial GSM_Serial(7,8);

void initGSM_Serial(){
  GSM_Serial.begin(9600);
}

//DEFINISCO MACRO PER LA STAMPA DELLE STRINGHE
#define STRING_ERROR 0
#define STRING_OK 1
#define STRING_BAD 2
#define STRING_200OK 3
#define STRING_200BAD 4

void turnONGSM(){
  // ACCENSIONE MODEM GPRS
  pinMode(9, OUTPUT); 
  digitalWrite(9,LOW);
  delay(1000);
  digitalWrite(9,HIGH);
  delay(2000);
  digitalWrite(9,LOW);
  delay(2000);
}

void turnOFFGSM(){
  // SPEGNIMENTO MODEM GPRS
  pinMode(9, OUTPUT); 
  digitalWrite(9,LOW);
  delay(1000);
  digitalWrite(9,HIGH);
  delay(2500);
  digitalWrite(9,LOW);
  delay(2000);
}

void initialSetup(){
  GSM_Serial.begin(9600); 
  
  #ifdef SERIAL_DEBUG
    showString(PSTR("Config SIM900...\n")); 
  #endif
  delay(2000); 
  #ifdef SERIAL_DEBUG
    showString(PSTR("Done!...\n")); 
  #endif
  
  GSM_Serial.flush(); 
  Serial.flush();

  //devo attendere 10s che si colleghi alla cella
  delay(10000);
  // attach or detach from GPRS service
  
  showStringGSM(PSTR("AT+CGATT?")) ; 
  delay(500); 
  ShowSerialData();
  /*
  if(!checkAttachedGSM("AT+CGATT?")){
    #ifdef SERIAL_DEBUG
      printString(STRING_ERROR);
    #endif
    turnOFFGSM();
    Reset();
  }*/
  
  // bearer settings
  
  showStringGSM(PSTR("AT+SAPBR=3,1 ,\"CONTYPE\",\"GPRS\"")); 
  delay(2000); 
  ShowSerialData();
  /*
  if(!checkOK("CONTYPE GPRS")){
    #ifdef SERIAL_DEBUG
      printString(STRING_ERROR);
    #endif
    turnOFFGSM();
    Reset();
  }*/
  
  // bearer settings

  showStringGSM(PSTR("AT+SAPBR=3,1 ,\"APN\",\"ibox.tim.it\"")); 
  delay(2000); 
  ShowSerialData();
  /*
  if(!checkOK("APN ibox.tim.it")){
    #ifdef SERIAL_DEBUG
      printString(STRING_ERROR);
    #endif
    turnOFFGSM();
    Reset();
  }*/

  // bearer settings

  showStringGSM(PSTR("AT+SAPBR=1,1 ")); 
  delay(2000); 
  ShowSerialData();
  /*
  if(!checkOK("AT+SAPBR=1,1")){
    #ifdef SERIAL_DEBUG
      printString(STRING_ERROR);
    #endif
    turnOFFGSM();
    Reset();
  }*/
  
  GSM_Serial.flush();
}

void sendGSMData(char* path){
  //GSM_Serial.begin(19200);
  //delay(3000);
  
  DateTime now = RTC.now();
  
  showStringGSM(PSTR("AT+HTTPINIT" )); 
  delay(2000); 
  ShowSerialData();
  /*
  if(!checkOK("HTTPINIT")){
    #ifdef SERIAL_DEBUG
      printString(STRING_ERROR);
    #endif
    turnOFFGSM();
    Reset();
  }*/
  // set http param value

  GSM_Serial.print("AT+HTTPPARA= \"URL\",\"");
  GSM_Serial.print(path);
  GSM_Serial.print(now.year(), DEC);
  GSM_Serial.print(",");
  GSM_Serial.print(now.month(), DEC);
  GSM_Serial.print(",");
  GSM_Serial.print(now.day(), DEC);
  GSM_Serial.print(",");
  GSM_Serial.print(now.hour(), DEC);
  GSM_Serial.print(",");
  GSM_Serial.print(now.minute(), DEC);
  GSM_Serial.print(",");
  GSM_Serial.print("SWE1,");
  GSM_Serial.print(_temp);
  GSM_Serial.print(",");
  GSM_Serial.print(_hum);
  GSM_Serial.print(",");
  GSM_Serial.print(_mandata);
  GSM_Serial.print(",");
  GSM_Serial.print(_ritorno);
  GSM_Serial.print(",");
  GSM_Serial.print(_flusso);
  GSM_Serial.print(",");
  GSM_Serial.print(vanoCaldaiaAperto());
  GSM_Serial.print(",");
  GSM_Serial.print(vanoCeneriAperto());
  GSM_Serial.print(",");
  GSM_Serial.print(_power);
  GSM_Serial.print(",");
  GSM_Serial.print(_sonar);
  GSM_Serial.println("\"");
  
  
    delay(2000); 
  ShowSerialData();
  #ifdef SERIAL_DEBUG
  showString(PSTR("\n"));
  #endif
  // set http action type 0 = GET, 1 = POST, 2 = HEAD

  showStringGSM(PSTR("AT+HTTPACTIO N=0")); 
  delay(5000); 
  ShowSerialData();
  /*
  if(!check200OK("HTTPACTIO")){
    #ifdef SERIAL_DEBUG
      printString(STRING_ERROR);
    #endif
    turnOFFGSM();
    Reset();
  } */
  
  // read server response

  showStringGSM(PSTR("AT+HTTPREAD" )); 
  delay(2000); 
  //ReadACK();
  ShowSerialData();
  showStringGSM(PSTR(""));


  showStringGSM(PSTR("AT+HTTPTERM" )); 
  ShowSerialData();
  delay(1000);

  showStringGSM(PSTR("")); 
  delay(5000);
  //GSM_Serial.flush();
  
}

void ShowSerialData()
{
    while(GSM_Serial.available()!=0){
      char c = GSM_Serial.read();
      #ifdef SERIAL_DEBUG
        Serial.write(c);
      #endif
    }
}

/*
//Controlla se ho ricevuto OK dopo comando GSM
boolean checkAttachedGSM(char* show){
  char c, tmp[30];
  byte i = 0;
  #ifdef SERIAL_DEBUG
    Serial.print(show);
    showString(PSTR("..."));
  #endif
  
  //if(GSM_Serial.available()!=0){      
      
      time = millis();
      //SALTO IL COMANDO DATO E LO SPAZIO
      while( GSM_Serial.read() != '\n')
          if(millis()-time>10000) { //ERRORE
           turnOFFGSM();
           Reset();
        }
      while( GSM_Serial.read() != '\n')
          if(millis()-time>10000) { //ERRORE
           turnOFFGSM();
           Reset();
        }
      while( GSM_Serial.read() != '\n')
          if(millis()-time>10000) { //ERRORE
           turnOFFGSM();
           Reset();
        }
      while( GSM_Serial.read() != '\n')
          if(millis()-time>10000){ //ERRORE
           turnOFFGSM();
           Reset();
        }
          
      time = millis();
      while( GSM_Serial.available()!=0 ){
        c=GSM_Serial.read();
        tmp[i] = c;
        i++;
        if(c=='\n') break;
        
        if(millis()-time>10000){ //ERRORE
           turnOFFGSM();
           Reset();
        }
      }
      tmp[i]=0;
      delay(10);      
  //}   
  
  //RICEVUTO +CGATT: 1
  if((tmp[8] == '1')){
    #ifdef SERIAL_DEBUG
      printString(STRING_OK);
    #endif    
  }
  else{ 
    #ifdef SERIAL_DEBUG
      printString(STRING_BAD);
    #endif    
  }
  
  tmp[0]=0;
  i=0;
  time = millis();
  while( GSM_Serial.read() != '\n')
        if(millis()-time>10000){ //ERRORE
           turnOFFGSM();
           Reset();
        }
  
  time = millis();
  while( GSM_Serial.available()!=0 ){
    c=GSM_Serial.read();
    tmp[i] = c;
    i++;
    if(c=='\n') break;
    
    if(millis()-time>10000){ //ERRORE
           turnOFFGSM();
           Reset();
        }       
  }
  
  tmp[i]=0;
  
  if((tmp[0] == 'O') && (tmp[1] = 'K')){
    #ifdef SERIAL_DEBUG
      printString(STRING_OK);
    #endif
    
    return true;
  }
  else{ 
    #ifdef SERIAL_DEBUG
      printString(STRING_BAD);
    #endif
    
    return false;    
  }
}



//Controlla se ho ricevuto OK dopo comando GSM
boolean checkOK(char* show){
  char c, tmp[30];
  
  #ifdef SERIAL_DEBUG
    Serial.print(show);
    showString(PSTR("..."));
  #endif
  
  //if(GSM_Serial.available()!=0){
      byte i = 0;
      
      time = millis();
      //SALTO IL COMANDO DATO E LO SPAZIO
      while( GSM_Serial.read() != '\n')
        if(millis()-time>10000){ //ERRORE
           turnOFFGSM();
           Reset();
        }
      while( GSM_Serial.read() != '\n')
        if(millis()-time>10000){ //ERRORE
           turnOFFGSM();
           Reset();
        }
      
      time = millis();
      while( GSM_Serial.available()!=0 ){
        c=GSM_Serial.read();
        tmp[i] = c;
        i++;
        if(c=='\n') break;
        
        if(millis()-time>10000){ //ERRORE
           turnOFFGSM();
           Reset();
        }
      }
      tmp[i]=0;
      delay(10);      
  //}   
   
  if((tmp[0] == 'O') && (tmp[1] = 'K')){
    #ifdef SERIAL_DEBUG
      printString(STRING_OK);
    #endif
    
    return true;
  }
  else{ 
    #ifdef SERIAL_DEBUG
      printString(STRING_BAD);
    #endif
    
    return false;    
  }
}

//Controlla se ho ricevuto 200 OK dopo GET HTTP
boolean check200OK(char* show){
  char c, tmp[30];
  byte i = 0;
  
  #ifdef SERIAL_DEBUG
    Serial.print(show);
    showString(PSTR("..."));
  #endif
  
  //if(GSM_Serial.available()!=0){
      
      //SALTO IL COMANDO E LO SPAZIO
      time = millis();
      while( GSM_Serial.read() != '\n')
        if(millis()-time>10000){ //ERRORE
           turnOFFGSM();
           Reset();
        }
      while( GSM_Serial.read() != '\n')
        if(millis()-time>10000){ //ERRORE
           turnOFFGSM();
           Reset();
        }
      
      time = millis();
      while( GSM_Serial.available()!=0 ){
        c=GSM_Serial.read();
        tmp[i] = c;
        i++;
        if(c=='\n') break;
        
        if(millis()-time>10000){ //ERRORE
           turnOFFGSM();
           Reset();
        }
      }
      tmp[i]=0;
      //Serial.print(tmp);
      delay(50);
      //tmp[0] = 0;      
  //}   
  
  //RICEVUTO OK
  if((tmp[0] == 'O') && (tmp[1] = 'K')){
    #ifdef SERIAL_DEBUG
      printString(STRING_OK);
    #endif
  }
  else{ 
    #ifdef SERIAL_DEBUG
      printString(STRING_BAD);
    #endif
    return false;
  }
  
  tmp[0]=0;
  i=0;
  time = millis();
  while( GSM_Serial.read() != '\n')
        if(millis()-time>10000){ //ERRORE
           turnOFFGSM();
           Reset();
        }
  
  time = millis();
  while( GSM_Serial.available()!=0 ){
    c=GSM_Serial.read();
    tmp[i] = c;
    i++;
    if(c=='\n') break;
    
    if(millis()-time>10000){ //ERRORE
           turnOFFGSM();
           Reset();
        }
  }
  
  //RICEVUTO +HTTPACTION:n,200,n
  if((tmp[14] == '2') && (tmp[15] = '0') && (tmp[16] = '0')){
    #ifdef SERIAL_DEBUG
      printString(STRING_200OK);
    #endif    
    
    return true;
  }
  else{ 
    #ifdef SERIAL_DEBUG
      printString(STRING_200BAD);
    #endif
    
    return false;
  }
}

//LEGGO ACK DI RISPOSTA DAL SERVER
void ReadACK()
{
  char tmp[12];
  byte ii=0;
  
  time = millis();
  while( GSM_Serial.read() != '\n')
        if(millis()-time>10000){ //ERRORE
           turnOFFGSM();
           Reset();
        }
  while( GSM_Serial.read() != '\n')
        if(millis()-time>10000){ //ERRORE
           turnOFFGSM();
           Reset();
        }
  while( GSM_Serial.read() != '\n')
        if(millis()-time>10000){ //ERRORE
           turnOFFGSM();
           Reset();
        }
  
  delay(1000);
  
  time = millis();
  while(GSM_Serial.available()!=0){
    tmp[ii++] = GSM_Serial.read();
    if(tmp[ii-1] == '\n') break;
    
    if(millis()-time>10000){ //ERRORE
           turnOFFGSM();
           Reset();
        }
  }
  //GSM_Serial.flush();
  tmp[--ii] = '\0';  
  delay(1000);
  ii--;
  
  int value = 0, m=1;
  for(int i=ii-1; i>=0; i--){
    value += (tmp[i] - '0') * m;
    m *= 10;
  }

  if(value>0)
    SECONDS_TO_SLEEP = value;
  else
    SECONDS_TO_SLEEP = DEFINE_WAIT;
  
  #ifdef SERIAL_DEBUG
    showString(PSTR("Sleeping for "));    
    Serial.print(SECONDS_TO_SLEEP);
    showString(PSTR(" seconds!\n"));
  #endif
  
  
}
*/
