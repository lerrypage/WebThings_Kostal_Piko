#define LARGE_JSON_BUFFERS 1
#include <ESP8266WiFi.h>
#include <Thing.h>
#include <WebThingAdapter.h>
uint8_t abfrage[] = { 0x62, 0x01, 0x03, 0x01, 0x00, 0x43, 0x56, 0x00 }; // WR-Bus (RS485) Adresse muss 
int i = 0;                                                              // von "255" auf "1" gestellt werden!
byte bufferRX[73];
const char* ssid     = "your SSID";    (edit)
const char* password = "your PASSWORD";(edit)

// Hostname used by mDNS
const String mDNSHostname = "solar_thing";
WebThingAdapter* adapter;
const char* sensorTypes[] = {"SolarpowerSensor", nullptr};
ThingDevice sensor("Solar", "Solarpower", sensorTypes);

ThingProperty sensorPowerOutW("Einspeisen Watt", "", NUMBER, nullptr);
ThingProperty sensorPowerInS1("String 1 Watt", "", NUMBER, nullptr);
ThingProperty sensorPowerInS2("String 2 Watt", "", NUMBER, nullptr);
ThingProperty sensorPowerInS3("String 3 Watt", "", NUMBER, nullptr);
ThingProperty sensorVInS1("String 1 Spannung", "", NUMBER, nullptr);
ThingProperty sensorVInS2("String 2 Spannung", "", NUMBER, nullptr);
ThingProperty sensorVInS3("String 3 Spannung", "", NUMBER, nullptr);

const char* host = "192.168.1.116"; // IP-Adresse vom Kostal Piko Wechselrichter (edit)
const uint16_t port = 81;
unsigned long timeout = 60000;
unsigned long mytimer = 0;

void setup() {
  Serial.begin(115200);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  

  // Create new WebThings connection handle (default port: 80)
  adapter = new WebThingAdapter(mDNSHostname, WiFi.localIP());
  sensorPowerOutW.unit = "Watt";
  sensorPowerInS1.unit = "Watt";
  sensorPowerInS2.unit = "Watt";
  sensorPowerInS3.unit = "Watt";
  sensorVInS1.unit = "Volt";
  sensorVInS2.unit = "Volt";
  sensorVInS3.unit = "Volt";


  // Associate properties with device
  sensor.addProperty(&sensorPowerOutW);
  sensor.addProperty(&sensorPowerInS1);
  sensor.addProperty(&sensorPowerInS2);
  sensor.addProperty(&sensorPowerInS3);
  sensor.addProperty(&sensorVInS1);
  sensor.addProperty(&sensorVInS2);
  sensor.addProperty(&sensorVInS3);

  // Associate device with connection
  adapter->addDevice(&sensor);

  // Start mDNS and HTTP server
  adapter->begin();
  
}

void loop() {
  ThingPropertyValue tpVal;
  //static bool wait = false;
   // Use WiFiClient class to create TCP connections
 //***** Abfrage 1x in der Minute *****
 if (millis() > timeout + mytimer) { 
  //mytimer = millis();
  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  // Use WiFiClient class to create TCP connections
   WiFiClient client; 
   if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }
  // This will send a string to the server
  Serial.println("sending data to server");
  if (client.connected()) {
    client.write(abfrage,8);
    delay(200);
  }
    // Read all the lines of the reply from server and print them to Serial
  Serial.println("receiving from remote server");
  // not testing 'client.connected()' since we do not need to send data here
  while (client.available()) {
    bufferRX[i] = (client.read());
    //Serial.println(" ");
    //Serial.print(bufferRX[i], HEX);
    //Serial.print(" ");
    i++;
  }
   mytimer = millis();
   i = 0;

 }
  int CR = ((uint16_t)bufferRX[1] << 8) + (uint16_t)bufferRX[2];
  unsigned long  spannungS1 = ((uint16_t)bufferRX[6] << 8) + (uint16_t)bufferRX[5];
  unsigned long stromS1 = ((uint16_t)bufferRX[8] << 8) + (uint16_t)bufferRX[7];
  unsigned long  powerS1 = ((uint16_t)bufferRX[10] << 8) + (uint16_t)bufferRX[9];
  unsigned long  spannungS2 = ((uint16_t)bufferRX[16] << 8) + (uint16_t)bufferRX[15];  
  unsigned long  stromS2 = ((uint16_t)bufferRX[18] << 8) + (uint16_t)bufferRX[17];
  unsigned long  powerS2 = ((uint16_t)bufferRX[20] << 8) + (uint16_t)bufferRX[19];
  unsigned long  spannungS3 = ((uint16_t)bufferRX[26] << 8) + (uint16_t)bufferRX[25]; 
  unsigned long  stromS3 = ((uint16_t)bufferRX[28] << 8) + (uint16_t)bufferRX[27];
  unsigned long  powerS3 = ((uint16_t)bufferRX[30] << 8) + (uint16_t)bufferRX[29];
  unsigned long  spannungP1 = ((uint16_t)bufferRX[36] << 8) + (uint16_t)bufferRX[35];
  unsigned long  spannungP2 = ((uint16_t)bufferRX[44] << 8) + (uint16_t)bufferRX[43];
  unsigned long  spannungP3 = ((uint16_t)bufferRX[52] << 8) + (uint16_t)bufferRX[51];
  unsigned long  powerP1 = ((uint16_t)bufferRX[40] << 8) + (uint16_t)bufferRX[39];
  unsigned long  powerP2 = ((uint16_t)bufferRX[48] << 8) + (uint16_t)bufferRX[47];
  unsigned long  powerP3 = ((uint16_t)bufferRX[56] << 8) + (uint16_t)bufferRX[55];
  unsigned long  stromP1 = ((uint16_t)bufferRX[38] << 8) + (uint16_t)bufferRX[37];
  unsigned long  stromP2 = ((uint16_t)bufferRX[46] << 8) + (uint16_t)bufferRX[45];
  unsigned long  stromP3 = ((uint16_t)bufferRX[54] << 8) + (uint16_t)bufferRX[53];
  unsigned long  powerOutW = (powerP1 + powerP2 + powerP3);
//  Serial.print (spannungS1/10.0);
//  Serial.print(" Volt S1");
//  Serial.print("     ");
//  Serial.print (spannungS2/10.0);
//  Serial.print(" Volt S2");
//  Serial.print("     ");
//  Serial.print (spannungS3/10.0);
//  Serial.println(" Volt S3");
//
//  Serial.print (stromS1/100.0);
//  Serial.print(" Ampere S1");
//  Serial.print("     ");
//  Serial.print (stromS2/100.0);
//  Serial.print(" Ampere S2");
//  Serial.print("     ");
//  Serial.print (stromS3/100.0);
//  Serial.println(" Ampere S3");
//  
//
//  Serial.print (powerS1);
//  Serial.print(" Watt S1");
//  Serial.print("     ");  
//  Serial.print (powerS2);
//  Serial.print(" Watt S2");
//  Serial.print("     ");  
//  Serial.print (powerS3);
//  Serial.println(" Watt S3");
//  Serial.println("     "); 
//  Serial.println("     ");   
//  Serial.print (spannungP1/10.0);
//  Serial.print(" Volt P1");
//  Serial.print("     ");  
//  Serial.print (spannungP2/10.0);
//  Serial.print(" Volt P2");
//  Serial.print("     ");    
//  Serial.print (spannungP3/10.0);
//  Serial.println(" Volt P3");
//  
//  Serial.print (stromP1/100.0);
//  Serial.print(" Strom P1");
//  Serial.print("     ");  
//  Serial.print (stromP2/100.0);
//  Serial.print(" Strom P2");
//  Serial.print("     ");    
//  Serial.print (stromP3/100.0);
//  Serial.println(" Strom P3");
//
//  
//  Serial.print (powerP1);
//  Serial.print(" Watt P1");
//  Serial.print("     ");  
//  Serial.print (powerP2);
//  Serial.print(" Watt P2");
//  Serial.print("     ");    
//  Serial.print (powerP3);
//  Serial.println(" Watt P3");
//  Serial.println("     ");  
//  Serial.print (powerOutW);
//  Serial.println(" Watt am Netz");
  
  //Serial.println("closing connection");
  //client.stop();
  if (CR = 57857){
  // Update device values
  delay(200);
  tpVal.number = powerOutW;
  sensorPowerOutW.setValue(tpVal);
  adapter->update();

  delay(200);
  tpVal.number = powerS1;
  sensorPowerInS1.setValue(tpVal);
  adapter->update();

  delay(200);
  tpVal.number = powerS2;
  sensorPowerInS2.setValue(tpVal);
  adapter->update();
  
  delay(200);
  tpVal.number = powerS3;
  sensorPowerInS3.setValue(tpVal);
  adapter->update();

  delay(200);
  tpVal.number = (spannungS1/10.0);
  sensorVInS1.setValue(tpVal);
  adapter->update();

  delay(200);
  tpVal.number = (spannungS2/10.0);
  sensorVInS2.setValue(tpVal);
  adapter->update();

  delay(200);
  tpVal.number = (spannungS3/10.0);
  sensorVInS3.setValue(tpVal);
  adapter->update();
  } 
}
