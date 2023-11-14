/*
  Web client

  Find the full UNO R4 WiFi Network documentation here:
  https://docs.arduino.cc/tutorials/uno-r4-wifi/wifi-examples#wi-fi-web-client
 */


#include "WiFiS3.h"


#include "arduino_secrets.h" 

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "BELL856";        // your network SSID (name)
char pass[] = "7C3E456CC496";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "data-gateway.akenza.io";    // name address for Google (using DNS)

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

/* -------------------------------------------------------------------------- */
void setup() {
/* -------------------------------------------------------------------------- */  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
     
    // wait 10 seconds for connection:
    delay(10000);
  }
  
  printWifiStatus();
 
}

/* just wrap the received data up to 80 columns in the serial print*/
/* -------------------------------------------------------------------------- */
void read_response() {
/* -------------------------------------------------------------------------- */  
  uint32_t received_data_num = 0;
  while (client.available()) {
    /* actual data reception */
    char c = client.read();
    /* print data to serial port */
    Serial.print(c);
    /* wrap data to 80 columns*/
    received_data_num++;
    if(received_data_num % 80 == 0) { 
      Serial.println();
    }
  }  
}


int voltage0Pin = A0; // Voltage sensor < 25V
int voltage0 = 0;  // variable to store the value read
int voltage1Pin = A1; // Voltage sensor < 25V
int voltage1 = 0;  // variable to store the value read

int current0Pin = A0; // Voltage sensor < 25V
int current0 = 0;  // variable to store the value read
int current1Pin = A1; // Voltage sensor < 25V
int current1 = 0;  // variable to store the value read
/* -------------------------------------------------------------------------- */
void loop() {
/* -------------------------------------------------------------------------- */  
  voltage0 = analogRead(voltage0Pin);
  voltage1 = analogRead(voltage1Pin);
  current0 = analogRead(current0Pin);
  current1 = analogRead(current1Pin);

  Serial.println(voltage0);
  Serial.println(voltage1);
  Serial.println(current0);
  Serial.println(current1);

  // if the server's disconnected, run the client:
  if (!client.connected()) {
    Serial.println("\nStarting connection to server...");
    client.connect(server, 80);
  }
  // if you get a connection, report back via serial:
  if (client.connected()) {
    Serial.println("\nConnection to server is up...");
    //String data = "{\"key1\":\"value1\",\"voltage\": 33,\"temperature\": 24}";

    String data = "{\"voltage0\":";
    data = data + voltage0 + ",";
    data = data + "\"voltage1\":" + voltage1 + ",";
    data = data + "\"current0\":" + current0 + ",";
    data = data + "\"current1\":" + current1 + "}";

    Serial.println(data);

    //Serial.println("connected to server");
    // Make a HTTP request:
    client.println("POST /v3/capture?secret=4fikexkwk695w7vx9rd6bjcxooi4hace&deviceId=DD444FC2501A3484&topic=default HTTP/1.1");
    client.println("Host: data-gateway.akenza.io");
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.print  ("Content-Length: ");
    client.println(data.length());
    client.println();
    client.println(data); 
  }
  read_response();

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

    // do nothing forevermore:
    //while (true);
  }
  // wait 12 seconds for connection:
  delay(12000);
}

/* -------------------------------------------------------------------------- */
void printWifiStatus() {
/* -------------------------------------------------------------------------- */  
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
