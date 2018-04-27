                          ////////
//  Weather station using Arduino( LCD display and update data to ThinkSpeak )
                          ////////
#include <Wire.h>
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
#include "DHT.h"
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#define DHTTYPE DHT11
#define IP "184.106.153.149" //ThinkSpeak ip.
String GET = "GET /update?key=202PWE8WL2WKTS4O"; // api_key of ThinkSpeak
DHT dht(A0, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
/*
LCD D7 ­> 2 
LCD D6 –> 3 
LCD D5 –> 4 
LCD D4 –> 5 
LCD E  –> 11 
LCD RS –> 12 
LCD VDD –> (+)  
LCD A –> (+)  
LCD VSS –> (-­) 
LCD K –>(­-) 
LCD RW –> (-­) 
LCD VO –> Potentiometer Middle Pin
*/
SoftwareSerial esp8266Module(6,7);
void setup(){
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  if (!bmp.begin()) 
  {
    Serial.println("BMP180 sensor not found");
    while (1) {}
  }
  Serial.println("Serial Started 9600");
  esp8266Module.begin(115200);
  bool a=true;
  while(a){
    esp8266Module.write("AT\r\n");
    delay(250);
    if(esp8266Module.find("OK")){
      Serial.println("Got the OK reply");
      a=false;
    }else Serial.println("ESP Not connected");
    delay(1000);
  }
  bool b=true;
  lcd.begin(16, 2);
}
void loop(){
  dht.read(A0);
  updateData(dht.readTemperature(),dht.readHumidity(),analogRead(A1),analogRead(A2),analogRead(A3),bmp.readPressure());
  updateLcd(dht.readTemperature(),dht.readHumidity(),analogRead(A1),bmp.readPressure());
  Serial.print(dht.readTemperature());
  Serial.print(" | ");
  Serial.print(dht.readHumidity());
  Serial.print(" | ");
  Serial.print(analogRead(A1));
  Serial.print(" | ");
  Serial.print(analogRead(A2));
  Serial.print(" | ");
  Serial.print(analogRead(A3));
  Serial.print(" | ");
  Serial.println(bmp.readPressure());
  if (esp8266Module.available()) {
    Serial.write(esp8266Module.read());
  }
  for(int i=0;i<10;i++)
  { 
    updateLcd(dht.readTemperature(),dht.readHumidity(),analogRead(A1),bmp.readPressure());
    delay(1000);
  }
}
void updateLcd(int a1,int a2,int a3,int a4){
  lcd.setCursor(0,0);
  lcd.print(a1);
  lcd.print(" C   ");
  lcd.print(a2);
  lcd.print(" %   ");
  lcd.setCursor(0,1);
  lcd.print("SM:");
  lcd.print(a3);
  lcd.print("  ");
  lcd.print(a4);
  lcd.print("Pa");
}

void updateData(int v1,int v2,int v3,int v4,int v5,int v6)
{  
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  esp8266Module.println(cmd);
  delay(5000);
  if(esp8266Module.find("Error")){
    Serial.println("Error");
  }
  Serial.println(v6);
  cmd = GET + "&field1=" + v1 + "&field2=" + v2 + "&field3=" + v3 + "&field4=" + v4 + "&field5=" + v5 + "&field6=" + v6 + "\r\n";
  esp8266Module.print("AT+CIPSEND=");
  esp8266Module.println(cmd.length());
  Serial.println(cmd.length());
  Serial.println(cmd);
  delay(500);
  if(esp8266Module.find("OK")){
    delay(1000);
    Serial.println("Sending");
    esp8266Module.println(cmd);
    Serial.println("Data sent");
    esp8266Module.println("AT+CIPCLOSE");
  }else{
    esp8266Module.println("AT+CIPCLOSE");
    Serial.println("Connection closed");
  }
}
