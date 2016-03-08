/*
 * NOTES: 
 * Sign up to openweathermap.org
 * change your SSID,password
 * enter the Weather stations name, Altitude & GPS coordinates
 * 
 * It will post data every 20 mins and include rainfall once an hour.
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <Wire.h>  


#define server "openweathermap.org" //Do not change!

//Enter you Wifi SSID and Password
#define SSID ""
#define password ""

//Enter here the BASE64 encoded Credentials in the form <Username>:<Password>, www.base64encode.org   eg: encode bob:builder == Ym9iOmJ1aWxkZXI=
#define CredBase64 "" 



//retrieve your altitude & GPS coordinates from http://www.gps-coordinates.net/
const String StationName = "Wio-01"; //Enter the Station Name how it will be displayed on Openweathermap.org
const String lat = "-27.938179"; //Latitude goes here
const String lng = "153.323263"; //Longitude goes here
const String alt = "32"; //Enter your Altitude in meters without decimals

long updateTime = 20 * 60000;  //Every 20 minutes
unsigned long timeNow = 0;

int rainCount = 0;
int rainCountPerHour = 0;

/********Pin Defines***********/
#define rainGauge 2 
#define windDir  12    //pulseIn
#define windSpeedP 14
#define dhtPin 13          
#define pwr 15      //9
//4 & 5 = I2C   bmp180


/********Instances***********/
Adafruit_BMP085 bmp;
DHT dht22(dhtPin, DHT22, 20);  //3 for 8mHz, 6 for 16mHz, 16-30 for ESP 80Mhz


String cmd = "";
String packet = "";

void setup() 
{
  rainCountPerHour = 3600000 / updateTime; //Set how many time we check the rain gauge per hour
  pinMode(pwr, OUTPUT);
  digitalWrite(pwr, HIGH);
  Serial.begin(115200);
  bmpInit();
  dhtInit();
  
  pinMode(rainGauge, INPUT);
  attachInterrupt(rainGauge,rainGageClick,FALLING); 
  
  timeNow = millis();
  
  WiFi.begin(SSID, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


}

void loop() 
{

  if (millis()-timeNow > updateTime){
    Serial.print("connecting to ");
    Serial.println(server);
  
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(server, httpPort)) {
      Serial.println("connection failed");
      return;
    }
  
    buildPacket();   
    buildCmd();
    Serial.println(cmd);
  
    client.println(cmd);
    delay(500);
    
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    client.stop(); //Stopping client 
  
    timeNow = millis();
  }
}


void buildCmd()
{
    //Build up POST request
  cmd = "POST /data/post HTTP/1.1\n";
  cmd += "Host: ";
  cmd += server;
  cmd += "\n";
  cmd += "Content-Type: application/x-www-form-urlencoded\n";
  cmd += "Authorization: Basic ";
  cmd += CredBase64;
  cmd += "\n";
  cmd += "Content-Length: ";
  cmd += packet.length();
  cmd += "\n";
  cmd += "Connection: close\n\n";
  cmd += packet;
  cmd += "\r\n\r\n";
}


void buildPacket()
{  
  double p = bmp.readPressure()/100.00;
  double h = dht22.readHumidity();
  double t = getTemp();
  packet = "temp=";
  packet += t, 2;
  packet += "&humidity=";
  packet += h;
  packet += "&pressure=";
  packet += p, 2;

  //Only send rain data once per hour
  if (rainCount == rainCountPerHour){
    packet += "&rain_1hr=";
    packet += calcUnitRain(), 2;
  } else {
    rainCount++;
  }
  
  packet += "&lat=";
  packet += lat;
  packet += "&long=";
  packet += lng;
  packet += "&alt=";
  packet += alt;
  packet += "&name=";
  packet += StationName;
}

