#include <SPI.h>
#include <Ethernet.h>

// Temperature (thermistor) sensor http://www.circuitbasics.com/arduino-thermistor-temperature-sensor-tutorial/
const int thermistorPin = 5;
String temperatureSensor() {
  //the calculating formula of temperature
  float resistor = (1023.0*10000)/analogRead(thermistorPin)-10000;
  float tempC = (3435.0/(log(resistor/10000)+(3435.0/(273.15+25)))) - 273.15;
  return String(tempC);
}

// Light sensor ///https://www.digikey.com/en/maker/projects/design-a-luxmeter-with-an-ldr-and-an-arduino/623aeee0f93e427bb57e02c4592567d1
const int photocellPin = 0;
int photocellReading;
int LEDbrightness; 
String lightSensor() {
  photocellReading = analogRead(photocellPin);
  return String(photocellReading);
}

// Distance sensor https://howtomechatronics.com/tutorials/arduino/ultrasonic-sensor-hc-sr04/
const int trigPin = 3;
const int echoPin = 2;
long duration;
int distance;
String distanceSensor() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance in cm
  distance= duration*0.034/2 + 2; //seems to give 2cm less
  return String(distance);
}

// ETHERNET
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0F, 0x46, 0x77
};
IPAddress ip(192, 168, 137, 92); //192.168.137.66
EthernetClient client;

char server[] = "ec2-18-188-222-133.us-east-2.compute.amazonaws.com"; 

String json;
void sendJSON(String sensor, String measure) {
  json = "";
  json += "{";
  json += "  \"value\": "+measure+",";
  json += "  \"sensor\": \""+sensor+"\",";
  json += "  \"deviceOwner\": \"pacoard@gmail.com\"";
  json += "}";

  Serial.println(json);

  // Make a HTTP POST message:
  client.println("POST /api/SensorReading HTTP/1.1");
  client.println("Host: yo");
  //client.println("User-Agent: Arduino/1.0");
  client.println("Connection: keep-alive");
  client.println("Content-Type: application/json;charset=utf-8");
  client.println("Accept: application/json");
  client.print("Content-Length: ");
  client.println(json.length());
  client.println();
  client.println(json);
  
}

void setup() {
  //Distance sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // start the Ethernet connection and the client:
  Serial.println("if (Ethernet.begin(mac) == 0) {...}");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to configure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  Serial.print("client is at ");
  Serial.println(Ethernet.localIP());
  delay(1000); // wait for initialization*/
}

void loop() {
  // Connect to the blockchain server
  if (client.connect(server, 3000)) {
    sendJSON("temp", temperatureSensor());
    delay(5000);
    sendJSON("light", lightSensor());
    delay(5000);
    sendJSON("distance", distanceSensor());
    delay(5000);
    client.stop();
  } else {
    Serial.println("connection failed");
  }
  delay(30000);
}
