//****************************RAIN*****************************

#define RAIN_FACTOR 0.2794

volatile float rainTipper=0.0;
volatile unsigned long rainLastTip=0;
 
double calcUnitRain(){ 
  
  double unitRain = rainTipper * RAIN_FACTOR;
  

  rainCount = 0;
  rainTipper=0.0; 
  
  Serial.print("Rain Fall :");
  Serial.print(unitRain);
  Serial.println("mm");
  return unitRain;
}

 
void rainGageClick(){
  if(millis()-rainLastTip>400) {  //Stops debounce of the reed switch
    rainTipper += 1.0;
    rainLastTip=millis();
  }
}

//**********************BMP*****************************************

void bmpInit()
{
  while(!bmp.begin()) {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    delay(100);
  }
}



//**********************DHT*****************************************

void dhtInit()
{
  delay(2000); //Warm Up
  float h, f, temp;
  // Check if any reads failed and exit early (to try again).
  do {
    Serial.println("Connecting to DHT sensor!");
    dht22.begin();
    delay(2000);
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    h = dht22.readHumidity();
    // Read temperature as Celsius  
    temp = dht22.readTemperature();
   
    Serial.println(h);
    Serial.println(temp);
    //Serial.println("Failed to read from DHT sensor!");
    //return 0.0;    
  } while (isnan(h) || isnan(temp));
}


double getTemp()
{
  double t1 = bmp.readTemperature();
  double t2 = dht22.readTemperature();
  return t1 + t2 / 2;
}

