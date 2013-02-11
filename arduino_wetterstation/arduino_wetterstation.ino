#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Cosm.h>
//#include <DHT.h>
#include <Adafruit_BMP085.h>
#include <Wire.h>

// MAC address for your ethernet shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

//you cosm API-key
char cosmKey[] = "INSERT_YOUR_COSM_KEY"; // replace INSERT_YOUR_COSM_KEY

// define Arduino PIN (digital) and define sensor
//#define DHTPIN 2
//#define DHTTYPE DHT22

//DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;

// Define the strings for our datastream IDs
/*char sensorId[] = "Temperature";
char bufferId[] = "Humidity";
char dewId[] = "DewPoint";*/
char pressureId[] = "Luftdruck";
char altitudeId[] = "Hoehe";
char bmpTempId[] = "Temperatur";

 CosmDatastream datastreams[] = {
// CosmDatastream(sensorId, strlen(sensorId), DATASTREAM_FLOAT),
// CosmDatastream(bufferId, strlen(bufferId), DATASTREAM_FLOAT),
// CosmDatastream(dewId, strlen(dewId), DATASTREAM_FLOAT),
 CosmDatastream(pressureId, strlen(pressureId), DATASTREAM_FLOAT),
 CosmDatastream(altitudeId, strlen(altitudeId), DATASTREAM_FLOAT),
 CosmDatastream(bmpTempId, strlen(bmpTempId), DATASTREAM_FLOAT),
};

// Finally, wrap the datastreams into a feed
CosmFeed feed(INSERT_OUR_FEED_ID, datastreams, 3 /* number of datastreams */); // replace INSERT_OUR_FEED_ID with your feed id

EthernetClient client;
CosmClient cosmclient(client);

void setup() {

 Serial.begin(9600);

 //dht.begin();
 Serial.println("Getting IP from DHCP...");
 while (Ethernet.begin(mac) != 1)
   {
   Serial.println("Error getting IP address via DHCP, trying again...");
   delay(15000);
 }

 Serial.println("Starting multiple datastream upload to Cosm...");
 Serial.println();



 if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
 }

}

void loop() {
/*  
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


// calculate DewPoint; put to datastream and serial-monitor 
 float dewPoint = (237.3 * sensorValue) / (17.271 - sensorValue) / 100 * (-1);
 datastreams[2].setFloat(dewPoint);
 Serial.print("DewPoint is ");
 Serial.println(datastreams[2].getFloat());
*/ 


// get temperature from BMP085; put to datastream and serial-monitor
 float sensorValue0 = bmp.readPressure();
 datastreams[0].setFloat(sensorValue0 / 100);
 Serial.print("Read sensor value (Pressure BMP) ");
 Serial.println(datastreams[0].getFloat());


// get pressure from bmp085; put to datastream and serial-monitor
 float sensorValue1 = bmp.readAltitude();
 datastreams[1].setFloat(sensorValue1);
 Serial.print("Read sensor (Altitude BMP): ");
 Serial.println(datastreams[1].getFloat());  

// get altitude from bmp085; put to datastream and serial-monitor
 float sensorValue2 = bmp.readTemperature();
 datastreams[2].setFloat(sensorValue2);
 Serial.print("Read sensor (Temperature BMP): ");
 Serial.println(datastreams[2].getFloat());  


// upload data
 Serial.println("Uploading it to Cosm");
 int ret = cosmclient.put(feed, cosmKey);
 Serial.print("cosmclient.put returned ");
 Serial.println(ret);

 Serial.println();

//wait 60000 ms 
 delay(60000);
}
