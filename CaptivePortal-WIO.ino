#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <Wire.h>  

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 0, 250);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

int updateTime = 60000;  //Once a minute
long timeNow = -updateTime;

int rainCount = 0;
int rainCountPerHour = 0;
float rainHourly[24];  //store rainfall per hour 
float rainTotal = 0;
byte rainHour = 0; //keep track of wich hour we are up to

/********Pin Defines***********/
#define rainGauge 2 
#define windDir  12    //pulseIn
#define windSpeedP 14
#define dhtPin 13          
#define pwr 15      //9
//4 & 5 = I2C   bmp180
//#define batLevel   ADC


/********Instances***********/
Adafruit_BMP085 bmp;
DHT dht22(dhtPin, DHT22, 20);  //3 for 8mHz, 6 for 16mHz, 30 for ESP 80Mhz


char headerHTML[] = "<!DOCTYPE html><html><head><title>Secluded.io Weather</title></head><body><h1>Real Time weather from Right Here!</h1>";

char tailHTML[] = "<p>Provided by Gold Coast Techspace</p></body></html>";
char responseHTML[350] = "";
char barString[30] = "";
char humString[30] = "";
char tempString[30] = "";
char rainHrString[30] = "";
char testString[18] = "";
char rain24HrString[35] = "";


//Function Prototypes
void bmpInit();
void dhtInit();
void rainGageClick();
void buildResponce();
void gatherData();
void calcUnitRain();

void setup() {
  rainCountPerHour = 3600000 / updateTime; //Set how many time we check the rain gauge per hour
  pinMode(pwr, OUTPUT);
  digitalWrite(pwr, HIGH);
  Serial.begin(115200);
  bmpInit();
  dhtInit();
  
  pinMode(rainGauge, INPUT);
  attachInterrupt(rainGauge,rainGageClick,FALLING); 
  
  timeNow = millis();
  
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Secluded.io Weather");

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  // replay to all requests with same HTML
  buildResponce();
  webServer.onNotFound([]() {
    webServer.send(200, "text/html", responseHTML);
  });
  webServer.begin();
}

void loop() {
  
  if(millis()-timeNow > updateTime){
    buildResponce();
    timeNow = millis();
  }
  dnsServer.processNextRequest();
  webServer.handleClient();
  
}

void addStr(char *data){
  Serial.println(data);
  int len = strlen(responseHTML);
  strcpy(responseHTML+len, data);
}

void buildResponce() {
  calcUnitRain();
  gatherData();
  sprintf(responseHTML, "%s%s%s%s%s%s%s", headerHTML, tempString, barString, humString, rainHrString, rain24HrString, tailHTML);
  Serial.println(responseHTML);
}

void gatherData(){
  float bar = bmp.readPressure();
  //Serial.println(bar);
  //Serial.println(bmp.readPressure());
  char barBuffer[7];
  dtostrf(bar,6,0,barBuffer);
  sprintf(barString, "%s%s%s", "<p>Barometric = ", barBuffer, " Pa</p>");
  Serial.print(barString);
  Serial.print(" :"); Serial.println(strlen(barString));

  float temp = (bmp.readTemperature() + dht22.readTemperature()) / 2.0;
  //Serial.println(temp);
  char tempBuffer[5];
  dtostrf(temp,2,2,tempBuffer);
  sprintf(tempString, "%s%s%s", "<p>Temperature = ", tempBuffer, "*C</p>");
  Serial.print(tempString);
  Serial.print(" :"); Serial.println(strlen(tempString));

  float hum = dht22.readHumidity();
  //Serial.println(hum);
  char humBuffer[5];
  dtostrf(hum,2,2,humBuffer);
  sprintf(humString, "%s%s%s", "<p>Humidity = ", humBuffer, "%</p>");
  Serial.print(humString);
  Serial.print(" :"); Serial.println(strlen(humString));

  float rain = rainTotal;
  char rainBuffer[7];
  dtostrf(rain,3,2,rainBuffer);
  sprintf(rainHrString, "%s%s%s", "<p>Hrly Rainfall = ", rainBuffer, "mm</p>");
  Serial.print(rainHrString);
  Serial.print(" :"); Serial.println(strlen(rainHrString));

  float rain24 = 0.0;
  for(byte i = 0; i < 24; i++) {
    if(i == rainHour) {
      rain24 += rainTotal;
    } else {
      rain24 += rainHourly[i];
    }
  } 
  char rain24Buffer[7] = "24.67";
  dtostrf(rain24,3,2,rain24Buffer);
  sprintf(rain24HrString, "%s%s%s", "<p>24 Hrly Rainfall = ", rain24Buffer, "mm</p>"); 
  //rain24HrString = "<p>24 Hrly Rainfall = 24.67mm</p>";
  Serial.print(rain24HrString);
  Serial.print(" :"); Serial.println(strlen(rain24HrString));
  
}

