
#include <SoftwareSerial.h> 
#include <dht.h>
#define dht_apin A1 // Analog Pin sensor is connected to
dht DHT;

const long eventTime_1_LDR =120000 ; //2 min in ms
const long eventTime_2_temp = 60000; //in ms
const long eventTime_3_soil = 15000; //in ms
const long eventTime_4_rain = 10000; //in ms


/* When did they start the race? */
unsigned long previousTime_1 = 0;
unsigned long previousTime_2 = 0;
unsigned long previousTime_3 = 0;
unsigned long previousTime_4 = 0;

int rainAnalog=A5;//ANALOG FOR RainFall pins
//#define sensorPower 7 //ANALOG FOR SOIL MOISTURE PINS
int  rainAnalogVal;

int sensor_pin = A0;  //ANALOG FOR SOIL MOISTURE PINS
int output_value ;

int sensorPin_LDR = 2; ////ANALOG fOR LIGHT INTENSITY PINS
int value = 0; 

String agAdi = "DESK";                 // We write the name of our network here.    
String agSifresi = "2609@1999";           // We write the password of our network here.

int rxPin = 10;                                               //ESP8266 RX pini
int txPin = 11;                                               //ESP8266 TX pini

String ip = "184.106.153.149";                                //Thingspeak ip adresi
float temp,hum;

SoftwareSerial esp(rxPin, txPin);                             // We make serial communication pin settings.

void setup() {  
  Serial.begin(9600);  // We are starting our communication with the serial port.
  Serial.println("Started");
  esp.begin(115200);                                          // We are starting serial communication with ESP8266.
  esp.println("AT");                                         // We do the module control with the AT command.
  Serial.println("AT  sent ");
  while(!esp.find("OK")){                                     // We wait until the module is ready.
    esp.println("AT");
    Serial.println("ESP8266 Not Find.");
  }
  Serial.println("OK Command Received");
  esp.println("AT+CWMODE=1");                                 // We set the ESP8266 module as a client.
  while(!esp.find("OK")){                                     // We wait until the setting is done.
    esp.println("AT+CWMODE=1");
    Serial.println("Setting is ....");
  }
  Serial.println("Set as client");
  Serial.println("Connecting to the Network ...");
  esp.println("AT+CWJAP=\""+agAdi+"\",\""+agSifresi+"\"");    // We are connecting to our network.
  while(!esp.find("OK"));                                     // We wait until it is connected to the network.
  Serial.println("connected to the network.");
  delay(1000);
   Serial.println("Lecture et envoie tout les paramètres...");

       lectureAndSend();
}
int getLdr(){
   value = analogRead(sensorPin_LDR); // Read LDR
   value = map(value,1016,34,34,1016);
   return value;
  }
  float getTemp(){DHT.read11(dht_apin); 
  temp=DHT.temperature;
  return temp;}  
  float getHum(){ DHT.read11(dht_apin);hum=DHT.humidity;
  return hum;}
int getRain(){
   rainAnalogVal = analogRead(rainAnalog);
   rainAnalogVal =map(rainAnalogVal,666,210,0,100);
   return rainAnalogVal;}

int getSoil(){
   output_value= analogRead(sensor_pin);// Read Soil moisture
   output_value = map(output_value,691,155,0,100);
   return output_value;}
void lectureAndSend(){ 
    esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");           // We connect to Thingspeak.
  if(esp.find("Error")){                                      // We check the connection error.
    Serial.println("AT+CIPSTART Error");}
    
   String veri = "GET https://api.thingspeak.com/update?api_key=MSIATUOC0DLEFJS2";   // Thingspeak command. We write our own api key in the key part.          
   veri += "&field1=";
   veri += String(getTemp());// The Temp  variable we will send
   veri += "&field2=";
   veri += String(getHum());// The Hum  variable we will 
   veri += "&field3=";
   veri += String(getSoil());// The Soil  variable we will 
   veri += "&field4=";
   veri += String(getLdr());// The Ldr  variable we will 
   veri += "&field5=";
   veri += String(getRain());// The Rain  variable we will 
   veri += "\r\n\r\n";
  esp.print("AT+CIPSEND=");                                   // We give the length of data that we will send to ESP.
  esp.println(veri.length()+2);
  delay(2000);
  if(esp.find(">")){                                          // The commands in it are running when ESP8266 is ready..
    esp.print(veri);                                          // We send the data.
    Serial.println(veri);
    Serial.println("Data sent.");
    }
Serial.println("Connection Closed."); // we close the link
esp.println("AT+CIPCLOSE"); 
delay(1000);                           // We wait 1 minute for sending new data

  }

void loop() {
  unsigned long currentTime = millis();   

    esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");           // We connect to Thingspeak.
  if(esp.find("Error")){                                      // We check the connection error.
    Serial.println("AT+CIPSTART Error");}

    if( currentTime - previousTime_2 >=eventTime_2_temp){
         String veri1 = "GET https://api.thingspeak.com/update?api_key=MSIATUOC0DLEFJS2";   // Thingspeak command. We write our own api key in the key part.          
         veri1 += "&field1=";
         veri1 += String(getTemp());// The temperature variable we will send
         veri1 += "&field2=";
         veri1 += String(getHum());// The humidity variable we will send
         veri1 += "\r\n\r\n";
  esp.print("AT+CIPSEND=");                                   // We give the length of data that we will send to ESP.
  esp.println(veri1.length()+2);
  delay(2000);
  if(esp.find(">")){                                          // The commands in it are running when ESP8266 is ready..
    esp.print(veri1);                                          // We send the data.
    Serial.println(veri1);
    Serial.println("Data sent.");
  //  delay(1000);
    }
    previousTime_2 = currentTime;

    }
 if( currentTime - previousTime_3 >= eventTime_3_soil){
        String veri2 = "GET https://api.thingspeak.com/update?api_key=MSIATUOC0DLEFJS2";   // Thingspeak command. We write our own api key in the key part.          
        veri2 += "&field3=";
        veri2 += String(getSoil());// The soil moisture variable we will send
        veri2 += "\r\n\r\n";
  esp.print("AT+CIPSEND=");                                   // We give the length of data that we will send to ESP.
  esp.println(veri2.length()+2);
  delay(2000);
  if(esp.find(">")){                                          // The commands in it are running when ESP8266 is ready..
    esp.print(veri2);                                          // We send the data.
    Serial.println(veri2);
    Serial.println("Data sent.");
   // delay(1000);
    }
    previousTime_3 = currentTime;

 }
 if( currentTime - previousTime_1 >= eventTime_1_LDR){
        String veri = "GET https://api.thingspeak.com/update?api_key=MSIATUOC0DLEFJS2";   // Thingspeak command. We write our own api key in the key part.          
        veri += "&field4=";
        veri += String(getLdr());// The LDR  variable we will send
        veri += "\r\n\r\n";
  esp.print("AT+CIPSEND=");                                   // We give the length of data that we will send to ESP.
  esp.println(veri.length()+2);
  delay(2000);
  if(esp.find(">")){                                          // The commands in it are running when ESP8266 is ready..
    esp.print(veri);                                          // We send the data.
    Serial.println(veri);
    Serial.println("Data sent.");
  //  delay(1000);
    }
    previousTime_4 = currentTime;                                         
}
if(currentTime - previousTime_4 >= eventTime_4_rain){    
        String veri3 = "GET https://api.thingspeak.com/update?api_key=MSIATUOC0DLEFJS2";   // Thingspeak command. We write our own api key in the key part.          
        veri3 += "&field5=";
        veri3 += String(getRain());// The RainFall  variable we will send
        veri3 += "\r\n\r\n";
  esp.print("AT+CIPSEND=");                                   // We give the length of data that we will send to ESP.
  esp.println(veri3.length()+2);
  delay(2000);
  if(esp.find(">")){                                          // The commands in it are running when ESP8266 is ready..
    esp.print(veri3);                                          // We send the data.
    Serial.println(veri3);
    Serial.println("Data sent.");
  //  delay(1000);
    }
    previousTime_4 = currentTime;

 }
Serial.println("Connection Closed."); // we close the link
esp.println("AT+CIPCLOSE"); 
delay(1000);                           // We wait 1 minute for sending new data

}
