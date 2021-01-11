/*
  Library for the Sensirion SGP30 Indoor Air Quality Sensor
  By: Ciara Jekel
  SparkFun Electronics
  Date: June 28th, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  SGP30 Datasheet: https://cdn.sparkfun.com/assets/c/0/a/2/e/Sensirion_Gas_Sensors_SGP30_Datasheet.pdf

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14813

  This example reads the sensors calculated CO2 and TVOC values
*/

#include "SparkFun_SGP30_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_SGP30
#include <Wire.h>
#include <shtc1.h>
#include <stdlib.h>

SGP30 mySensor; //create an object of the SGP30 class
SHTC1 shtc1;

int compareFloat(const void*, const void*);
float get_median(const float*, uint8_t);

int compareFloat(const void* a, const void* b)
{
    int aNum = *(float*)a;
    int bNum = *(float*)b;

    if( aNum < bNum ){
        return -1;
    }
    else if( aNum > bNum ){
        return 1;
    }
    return 0;
}

float get_median(const float* array, uint8_t size)
{
    float median;

    // array のコピーを作って、ソートする
    float* array_copy = malloc(sizeof(float) * size);
    memcpy(array_copy, array, sizeof(float) * size);
    qsort(array_copy, size, sizeof(float), compareFloat);
    median = array_copy[size / 2];
    free(array_copy);
    return median;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  //Initialize sensor
  if (mySensor.begin() == false) {
    Serial.println("No SGP30 Detected. Check connections.");
    while (1);
  }
  //Initializes sensor for air quality readings
  //measureAirQuality should be called in one second increments after a call to initAirQuality
  mySensor.initAirQuality();
}

void loop() {
  static float co2_arry[5]  = {0.0};
  static float tvoc_arry[5] = {0.0};
  static float temp_arry[5] = {0.0};
  static float hum_arry[5]  = {0.0};

  float co2,tvoc,temp,hum;
  uint8_t i;

  
  //First fifteen readings will be
  //CO2: 400 ppm  TVOC: 0 ppb
  //measure CO2 and TVOC levels
  mySensor.measureAirQuality();
  //measure TEMP and HUM levels
  shtc1.readSample();

  co2  = float(mySensor.CO2);
  tvoc = float(mySensor.TVOC);
  temp = shtc1.getTemperature();
  hum  = shtc1.getHumidity();

  for(i = 1 ; i < 5; i++){
    co2_arry[i-1]  = co2_arry[i];
    tvoc_arry[i-1] = tvoc_arry[i];
    temp_arry[i-1] = temp_arry[i];
    hum_arry[i-1]  = hum_arry[i];
  }
  co2_arry[4]  = co2;
  tvoc_arry[4] = tvoc;
  temp_arry[4] = temp;
  hum_arry[4]  = hum;
   
  Serial.print("CO2: ");
  Serial.print(get_median(co2_arry,5),0);
  Serial.print("ppm  tTVOC: ");
  Serial.print(get_median(tvoc_arry,5),0);
  Serial.print("ppb");

  Serial.print("  RH: ");
  Serial.print(get_median(hum_arry,5), 2);
  Serial.print("%  TEMP: ");
  Serial.print(get_median(temp_arry,5), 2);
  Serial.println("℃");

  delay(1000); //Wait 1 second

}
