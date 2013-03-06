#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Cosm.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561.h>

// MAC address for your ethernet shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// your Cosm key and FEED_ID to upload your data
char cosmKey[] = "pXk-qzhIN5qhHRojPUq6y7-8Uc6SAKx2eVdjb05ld3ZFMD0g"; 							// replace INSERT_YOUR_COSM_KEY with your key
//int FEED_ID = 101375; 									//replace INSERT_OUR_FEED_ID with your feed id

#define DHTPIN 2
#define DHTPIN_2 3

#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
DHT dht_2(DHTPIN_2, DHTTYPE);

Adafruit_BMP085 bmp;
Adafruit_TSL2561 tsl = Adafruit_TSL2561(TSL2561_ADDR_FLOAT, 12345);

// Define the strings for our datastream IDs
char sensorId[] = "innenTemperatur";
char bufferId[] = "innenLuftfeuchte";
//char dewId[] = "DewPoint";
char outTempId[] = "aussenTemperatur";
char outHumidityId[] = "aussenLuftfeuchte";
char pressureId[] = "Luftdruck";
char lightId[] = "Helligkeit";

  CosmDatastream datastreams[] = {
  CosmDatastream(sensorId, strlen(sensorId), DATASTREAM_FLOAT),
  CosmDatastream(bufferId, strlen(bufferId), DATASTREAM_FLOAT),
 // CosmDatastream(dewId, strlen(dewId), DATASTREAM_FLOAT),
  CosmDatastream(outTempId, strlen(outTempId), DATASTREAM_FLOAT),
  CosmDatastream(outHumidityId, strlen(outHumidityId), DATASTREAM_FLOAT),
  CosmDatastream(pressureId, strlen(pressureId), DATASTREAM_FLOAT),
  CosmDatastream(lightId, strlen(lightId), DATASTREAM_FLOAT),
};

// Finally, wrap the datastreams into a feed
CosmFeed feed(101375, datastreams, 6 /* number of datastreams */); // replace INSERT_OUR_FEED_ID with your feed id

EthernetClient client;
CosmClient cosmclient(client);

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" lux");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" lux");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" lux");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2561
*/
/**************************************************************************/
void configureSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoGain(true);          /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */  
  Serial.println("------------------------------------");
  Serial.print  ("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("13 ms");
  Serial.println("------------------------------------");
}

void setup() {
   
  Serial.begin(9600);
  
  dht.begin();
  //dht_2.begin();
  
  Serial.println("Starting multiple datastream upload to Cosm...");
  Serial.println();

  while (Ethernet.begin(mac) != 1)
  {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(15000);
  }
  
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
  
  Serial.println("Light Sensor Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!tsl.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  /* Display some basic information on this sensor */
  displaySensorDetails();
  
  /* Setup the sensor gain and integration time */
  configureSensor();
  
  /* We're ready to go! */
  Serial.println("");
  
}

void loop() {
  
  
   sensors_event_t event;
  tsl.getEvent(&event);
 
  /* Display the results (light is measured in lux) */
  if (event.light)
  {
    Serial.print(event.light); Serial.println(" lux");
  }
  else
  {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    Serial.println("Sensor overload");
  }
  
// get temperature from dht22; put to datastream and serial-monitor
  float sensorValue = dht.readTemperature();
  datastreams[0].setFloat(sensorValue);
  Serial.print("Read sensor value ");
  Serial.println(datastreams[0].getFloat());


// get humidity from dht22; put to datastream and serial-monitor
  float sensorValue2 = dht.readHumidity();
  datastreams[1].setFloat(sensorValue2);
  Serial.print("Read sensor value ");
  Serial.println(datastreams[1].getFloat());

/*
// calculate DewPoint; put to datastream and serial-monitor 
  float dewPoint = (237.3 * sensorValue) / (17.271 - sensorValue) / 100 * (-1);
  datastreams[2].setFloat(dewPoint);
  Serial.print("DewPoint is ");
  Serial.println(datastreams[2].getFloat());
*/ 
 
 
// get outside temperature from dht22; put to datastream and serial-monitor
  float sensorValue3 = dht_2.readTemperature();
  datastreams[2].setFloat(sensorValue3);
  Serial.print("Read sensor value ");
  Serial.println(datastreams[2].getFloat());


// get outside humidity from dht22; put to datastream and serial-monitor
  float sensorValue4 = dht_2.readHumidity();
  datastreams[3].setFloat(sensorValue4);
  Serial.print("Read sensor value ");
  Serial.println(datastreams[3].getFloat());  
  
// airpressure and correct value from bmp085; put to datastream and serial-monitor
  float expo = 0.9607413; // calculatet exp(-höhe/7330m)
  float pressureNN = (bmp.readPressure() / 100 ) / expo;
  float sensorValue5 = pressureNN;
  datastreams[4].setFloat(sensorValue5);
  Serial.print("Read sensor value ");
  Serial.println(datastreams[4].getFloat());  
  
// get temperature from dht22; put to datastream and serial-monitor
  float sensorValue6 = event.light;
  datastreams[5].setFloat(sensorValue6);
  Serial.print("Read sensor value ");
  Serial.println(datastreams[5].getFloat());
    


// upload data
  Serial.println("Uploading it to Cosm");
  int ret = cosmclient.put(feed, cosmKey);
  Serial.print("cosmclient.put returned ");
  Serial.println(ret);

  Serial.println();
 
//wait 60000 ms 
  delay(60000);
}

