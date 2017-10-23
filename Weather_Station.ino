                          ////////
//  Weather station using Arduino( LCD display and update data to ThinkSpeak )
                          ////////
#include "DHT.h"
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#define dht_dpin A0
#define DHTTYPE DHT11
#define IP "184.106.153.149" //ThinkSpeak ip.
String GET = "GET /update?key=................."; // api_key of ThinkSpeak
DHT dht(dht_dpin, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

SoftwareSerial esp8266Module(6,7);

void setup(){
Serial.begin(9600);
  while (!Serial) {
    ;
  }
  Serial.println("Started");
  esp8266Module.begin(115200);
  esp8266Module.write("AT\r\n");
  delay(1500);
  if(esp8266Module.find("OK")){
    Serial.println("OK");
  }
  delay(500);
  lcd.begin(16, 2);
  lcd.print("TEMP HUMIDITY"); 
  Serial.begin(9600); 
}
void loop(){
  lcd.setCursor(1,3);
  dht.read(dht_dpin);
  updateTemp(dht.readTemperature(),dht.readHumidity());
  lcd.print(dht.readTemperature());
  lcd.print("C  "); 
  lcd.print(dht.readHumidity());
  lcd.print("%   ");
  Serial.println(dht.readTemperature());
  Serial.println(dht.readHumidity());
  if (esp8266Module.available()) {
    Serial.write(esp8266Module.read());
  }
  delay(5000);
}


void updateTemp(int v1,int v2)
{  
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  esp8266Module.println(cmd);
  delay(5000);
  if(esp8266Module.find("Error")){
    Serial.println("Error");
  }
  cmd = GET + "&field1=" + v1 + "&field2=" + v2 + "\r\n";
  esp8266Module.print("AT+CIPSEND=");
  esp8266Module.println(cmd.length());
  delay(3000);
  if(esp8266Module.find(">"))
  {
    esp8266Module.print(cmd);
    Serial.println("Data sent");
  }else{
    esp8266Module.println("AT+CIPCLOSE");
    Serial.println("Connection closed");
  }
}


