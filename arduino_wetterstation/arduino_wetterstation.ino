#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Cosm.h>
#include <DHT.h>

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

// Define the strings for our datastream IDs
char sensorId[] = "Temperature";
char bufferId[] = "Humidity";
char dewId[] = "DewPoint";
char outTempId[] = "outTemperature";
char outHumidityId[] = "outHumidity";

  CosmDatastream datastreams[] = {
  CosmDatastream(sensorId, strlen(sensorId), DATASTREAM_FLOAT),
  CosmDatastream(bufferId, strlen(bufferId), DATASTREAM_FLOAT),
  CosmDatastream(dewId, strlen(dewId), DATASTREAM_FLOAT),
  CosmDatastream(outTempId, strlen(outTempId), DATASTREAM_FLOAT),
  CosmDatastream(outHumidityId, strlen(outHumidityId), DATASTREAM_FLOAT),
};

// Finally, wrap the datastreams into a feed
CosmFeed feed(101375, datastreams, 5 /* number of datastreams */); // replace INSERT_OUR_FEED_ID with your feed id

EthernetClient client;
CosmClient cosmclient(client);

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
}

void loop() {
  
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
 
 
 
// get outside temperature from dht22; put to datastream and serial-monitor
  float sensorValue3 = dht_2.readTemperature();
  datastreams[3].setFloat(sensorValue3);
  Serial.print("Read sensor value ");
  Serial.println(datastreams[3].getFloat());


// get outside humidity from dht22; put to datastream and serial-monitor
  float sensorValue4 = dht_2.readHumidity();
  datastreams[4].setFloat(sensorValue4);
  Serial.print("Read sensor value ");
  Serial.println(datastreams[4].getFloat());  



// upload data
  Serial.println("Uploading it to Cosm");
  int ret = cosmclient.put(feed, cosmKey);
  Serial.print("cosmclient.put returned ");
  Serial.println(ret);

  Serial.println();
 
//wait 60000 ms 
  delay(10000);
}

