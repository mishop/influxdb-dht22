# influxdb-dht22
Sned data from dht22 to influxdb using ESP8266

## Connect dht22 to ESP8266
![Tux, the Linux mascot](https://www.losant.com/hs-fs/hubfs/Blog/dht22/dht22_esp8266_wiring.png?width=1280&name=dht22_esp8266_wiring.png)

## Set parametars
Setup your wifi ssid and password.

```
const char* ssid     = "wifi";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "pass";     // The password of the Wi-Fi network

#define INFLUXDB_URL "url"
// InfluxDB v2 server or cloud API token (Use: InfluxDB UI -> Data -> API Tokens -> <select token>)
#define INFLUXDB_TOKEN "token"
// InfluxDB v2 organization id (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_ORG "org"
// InfluxDB v2 bucket name (Use: InfluxDB UI ->  Data -> Buckets)
#define INFLUXDB_BUCKET "bucket"

```
