
// Simple code upload the tempeature and humidity data using thingspeak.com
// Hardware: NodeMCU,DHT11

#include <DHT.h>  // Including library for dht
//#include "HX711.h"
#include <ESP8266WiFi.h>
 
String apiKey = "K5XKLJ3Y5DJ5TWWO";     //  Enter your Write API key from ThingSpeak

const char *ssid =  "pula";     // replace with your wifi ssid and wpa2 key
const char *pass =  "pulsara145";
const char* server = "api.thingspeak.com";

#define DHTPIN D4          //pin where the dht11 is connected
//#define DOUT 13 //D7
//#define CLK 15  //D8

DHT dht(DHTPIN, DHT11);
//HX711 scale;

WiFiClient client;
 
void setup() 
{
      Serial.begin(115200);
     // Serial.println("Press T to tare");
      ////scale.begin(DOUT,CLK);
    //  scale.set_scale(-96650); //Calibration Factor obtained from first sketch
    //  scale.tare(); //Reset the scale to 0
       delay(10);
       dht.begin();
 
       Serial.println("Connecting to ");
       Serial.println(ssid);
 
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
 
}
void loop() 
{
      //Serial.print("Weight: ");
      //Serial.print(scale.get_units(), 5); //Up to 5 decimal points
     //Serial.println(" kg"); //Change this to kg and re-adjust the calibration factor if you follow lbs

      float h = dht.readHumidity();
      float t = dht.readTemperature();
    //  if(Serial.available())
//{
//char temp = Serial.read();
//if(temp == 't' || temp == 'T')
//scale.tare(); //Reset the scale to zero
//}
//float w = scale.get_units();
//w = w + 10; //0.1 kg is the fly trap net weight

              if (isnan(h) || isnan(t)) 
                 {
                     Serial.println("Failed to read from sensors!");
                      return;
                 }
                         if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             //postStr +="&field3=";
                            // postStr += String(w);
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
                             
                             Serial.print("Temperature: ");
                             Serial.print(t);
                             Serial.print(" degrees Celcius, Humidity: ");
                             Serial.print(h);
                             Serial.println("%. Send to Thingspeak.");
                             //Serial.print("Weight: ");
                             //Serial.print(w);
                             //Serial.println(" kg. Send to Thingspeak.");
                        }
          client.stop();
 
          Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  delay(5000);
}
