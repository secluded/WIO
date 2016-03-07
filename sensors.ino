void bmpInit()
{
  while(!bmp.begin()) {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    delay(100);
  }
}

float getBarometric(){ //float *temp){
    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
    
    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    Serial.println(" meters");

    Serial.print("Pressure at sealevel (calculated) = ");
    Serial.print(bmp.readSealevelPressure());
    Serial.println(" Pa");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    Serial.print("Real altitude = ");
    Serial.print(bmp.readAltitude(101500));
    Serial.println(" meters");
    
    Serial.println();
    delay(500);
}

/*
void get_bmp()
{
    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
    
    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    Serial.println(" meters");

    Serial.print("Pressure at sealevel (calculated) = ");
    Serial.print(bmp.readSealevelPressure());
    Serial.println(" Pa");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    Serial.print("Real altitude = ");
    Serial.print(bmp.readAltitude(101500));
    Serial.println(" meters");
    
    Serial.println();
    delay(500);
} */

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

void gettemperature() {
    // Reading temperature for humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    float hum = dht22.readHumidity();          // Read humidity (percent)
    float temp = dht22.readTemperature();     // Read temperature
    // Check if any reads failed and exit early (to try again).
    if (isnan(hum) || isnan(temp)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    Serial.println(hum);
    Serial.println(temp);
  
}

/*
float getHumidity(float *temp, float *heatIndex)
{
  float h, f;
  //dhtInit();
  
  // Check if any reads failed and exit early (to try again).
  do {
    Serial.println("Reading DHT sensor!");
    //dht22.begin();
    delay(100);
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    h = dht22.readHumidity();
    // Read temperature as Celsius
    *temp = dht22.readTemperature();
    // Read temperature as Fahrenheit
    f = dht22.readTemperature(true);    
    //Serial.println("Failed to read from DHT sensor!");
    //return 0.0;
  } while (isnan(h) || isnan(*temp) || isnan(f));

  // Compute heat index
  // Must send in temp in Fahrenheit!
  float hi = dht22.computeHeatIndex(f, h);

    //
  //dataToSend = buildString(h,5,"2=");

  Serial.print("Humidity: "); 
  Serial.print(h);
  //espSerial.print(h);
  //espSerial.print(',');
  Serial.print(" %\t");
//  Serial.print("Temperature: "); 
//  Serial.print(t);
//  Serial.print(" *C ");
//  Serial.print(f);
//  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  *heatIndex = ((hi-32)*5)/9;
  Serial.print(*heatIndex);
  //espSerial.print(heatIndex);
  //espSerial.print(',');
  Serial.println(" *C");
 
  
  //Add Humidity to String
  //dataToSend += "Humidity," + String(h) + "\r\n";  
  
  return h;
} */

//****************************RAIN*****************************

#define RAIN_FACTOR 0.2794

volatile float rainTipper=0.0;
volatile unsigned long rainLastTip=0;
 
void calcUnitRain(){ 
  
  float unitRain = rainTipper * RAIN_FACTOR;
  
  if(rainCount < rainCountPerHour) {  //360 samples per hour
    rainTotal += unitRain;  //add them all together
    rainCount++;
  } else {  //If we are over 1 hour
    rainHourly[rainHour] = rainTotal;  //Store the total for that hour
    rainTotal = 0.0;           //reset total
    rainHour++;                //move to next hour
    rainHourly[rainHour] = 0.0; //Clear previous data
    if(rainHour == 24) {       //start again
      rainHour = 0; 
    }
    rainCount = 0;        
  }
  
  rainTipper=0.0;  
  
  Serial.print("Rain Fall :");
  Serial.print(unitRain);
  Serial.println("mm");
}
 
void rainGageClick(){
  if(millis()-rainLastTip>400) {  //Stops debounce of the reed switch
    rainTipper += 1.0;
    rainLastTip=millis();
  }
}
