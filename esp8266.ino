/**
 * Example for reading temperature and humidity
 * using the DHT22 and ESP8266 and send to Influxdb v2
 *
 * Copyright (c) 2016 Losant IoT. All rights reserved.
 * https://orangeunit.com
 */

#include "DHT.h"
#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

const char* ssid     = "Your Wifi";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "wifi pass";     // The password of the Wi-Fi network
// InfluxDB v2 server
#define INFLUXDB_URL "https://influx.com:8086"
// InfluxDB v2 server or cloud API token (Use: InfluxDB UI -> Data -> API Tokens -> <select token>)
#define INFLUXDB_TOKEN "token"
// InfluxDB v2 organization id (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_ORG "org"
// InfluxDB v2 bucket name (Use: InfluxDB UI ->  Data -> Buckets)
#define INFLUXDB_BUCKET "bucket"

#define DHTPIN 4     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors

DHT dht(DHTPIN, DHTTYPE);

WiFiClientSecure wifiClient;

#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point sensor("sensors");
void setup() {
  Serial.begin(9600);
  Serial.setTimeout(2000);        // Start the Serial communication to send messages to the computer
  delay(10);
    // Wait for serial to initialize.
  while(!Serial) { }
  Serial.println('\n');
  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  Serial.println("Running DHT!");
  Serial.println("-------------------------------------");
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());  
  dht.begin();       // Send the IP address of the ESP8266 to the computer
  
  sensor.addTag("sensors", "temp_input");
  //sensor.addTag("SSID", DHTTYPE);

  // Accurate time is necessary for certificate validation and writing in batches
  // For the fastest time sync find NTP servers in your area: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void loop() {
  // Clear fields for reusing the point. Tags will remain untouched
  sensor.clearFields();
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
    //  timeSinceLastRead = 0;
      return;
    }

    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");

  // Store measured value into point
  // Report RSSI of currently connected network
  sensor.addField("temperature", t);
  sensor.addField("humidity", h);
  // Print what are we exactly writing
  Serial.print("Writing: ");
  Serial.println(sensor.toLineProtocol());


  // Write point
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  //Wait 10s
  Serial.println("Wait 3min");
  delay(180000);
}
