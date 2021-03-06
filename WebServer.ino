/*
 WiFiEsp example: WebServer

 A simple web server that shows the value of the analog input 
 pins via a web page using an ESP8266 module.
 This sketch will print the IP address of your ESP8266 module (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to display the web page.
 The web page will be automatically refreshed each 20 seconds.

 For more details see: http://yaab-arduino.blogspot.com/p/wifiesp.html
*/

#include "WiFiEsp.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "./secret.h"

char ssid[] = WIFI_SSID;            // your network SSID (name)
char pass[] = WIFI_PASSWORD;        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
int reqCount = 0;                // number of requests received

WiFiEspServer server(80);
String http_string = "";

//*******************************************************************************
#define DHTPIN            22         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT22      // DHT 22 (AM2302)
#define DUSTADC           15         // VO
#define DUSTLED           53         // LED on/off

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
String ret_dht = "";
//*******************************************************************************

void setup()
{
  // dust sensor setting
  pinMode(DUSTLED,OUTPUT);
  pinMode(DUSTADC, INPUT); 
  
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(115200);
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");
  printWifiStatus();
  
  //*******************************************************************************
  dht.begin();

  Serial.println("DHTxx Unified Sensor Example");
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");  
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Min Dealy:    "); Serial.print(delayMS); Serial.println("ms");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");  
  Serial.println("------------------------------------");
  Serial.println("Example output:");
  Serial.println("{\"dht22\":{\"status\":\"error\",\"message\":\"Error reading temperature!\"}}");
  Serial.println("{\"dht22\":{\"status\":\"succeed\",\"message\":{\"temperature\":\"19.20C\",\"humidity\":\"17.20%\"}}}");
  Serial.println("{\"dust\":{\"status\":\"error\",\"message\":\"error reading dust sensor\"}}");
  Serial.println("{\"dust\":{\"status\":\"succeed\",\"message\":186.81}}");
  Serial.println("------------------------------------");
  //*******************************************************************************

  read_dust();
  
  // start the web server on port 80
  server.begin();
}


void loop()
{
  ret_dht = "";
  // listen for incoming clients
  WiFiEspClient client = server.available();
  if (client) {
    // Serial.println("New client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (http_string.length() < 100) http_string += c;
        
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          http_string.toUpperCase();
          if (http_string.startsWith("GET /DHT22 HTTP")){
            client.print(
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"  // the connection will be closed after completion of the response
            "\r\n");
            ret_dht = read_dht22();
            client.print(ret_dht);
            http_string = String("");
            break;
          }
          if (http_string.startsWith("GET /DUST HTTP")){
            client.print(
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"  // the connection will be closed after completion of the response
            "\r\n");
            ret_dht = read_dust();
            client.print(ret_dht);
            http_string = String("");
            break;
          }
          client.print(
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"  // the connection will be closed after completion of the response
            "\r\n");
          http_string = String("");
          break;
        }
        
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(10);
    Serial.println(ret_dht);
    // Delay between measurements.
    delay(delayMS);
    // close the connection:
    client.stop();

  }
}


void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  
  // print where to go in the browser
  Serial.println();
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
  Serial.println();
}

String read_dust() {
  String ret;
  float dustVal = 0;
  int i;
  
  for(i=0;i<100;i++) dustVal += read_dust_once();
  dustVal /= 100.0;
  //dustVal = (dustVal * (5.0 / 1024.0) * 0.17 - 0.1) * 1000;
  //if (dustVal < 0)  dustVal = 0;
  ret = String("{\"dust\":{\"status\":\"succeed\",\"message\":" + String(dustVal) +"}}\r\n");
  /*
  if (dustVal>36.455) {
    //dustVal = (float(dustVal/1024)-0.0356)*120000*0.035;
    ret = String("{\"dust\":{\"status\":\"succeed\",\"message\":" + String(dustVal) +"\"}}\r\n");
  }
  else  {
    ret = String("{\"dust\":{\"status\":\"error\",\"message\":\"error reading dust sensor\"}}\r\n");
  }*/
  return ret;
}

float read_dust_once() {
  float dustVal = 0;
  digitalWrite(DUSTLED,LOW); 
  delayMicroseconds(280);
  dustVal=analogRead(DUSTADC); 
  delayMicroseconds(40);
  digitalWrite(DUSTLED,HIGH); 
  delayMicroseconds(9680);
  return dustVal;
}

String read_dht22() {
  // Get temperature event and print its value.
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  String json_status = "error";
  String json_message = "";
  String temp = "-273";

  String ret = String("{\"dht22\":{\"status\":\"");
  if (isnan(event.temperature)) {
    json_status = "error";
    json_message = "Error reading temperature!";
    return (ret + json_status + "\",\"message\":\"" + json_message + "\"}}\r\n");
  }
  else {
    temp = event.temperature;
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    json_status = "error";
    json_message = "Error reading humidity!";
    return (ret + json_status + "\",\"message\":\"" + json_message + "\"}}\r\n");
  }
  else {
    json_status = "succeed";
    json_message = "{\"temperature\":\"" + temp + "C\",\"humidity\":\"" + event.relative_humidity + "%\"}";
    return (ret + json_status + "\",\"message\":" + json_message + "}}\r\n");
  }
}
