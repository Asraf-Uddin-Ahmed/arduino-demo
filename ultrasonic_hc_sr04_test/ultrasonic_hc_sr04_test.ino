/*
HC-SR04 Ping distance sensor
  VCC to arduino 5v 
  GND to arduino GND
  Echo to Arduino pin 12 
  Trig to Arduino pin 13

Response led
  Red POS to Arduino pin 11
  Green POS to Arduino pin 10
 560 ohm resistor to both LED NEG and GRD power rail

Piezo Buzzer
  +ve -> arduino pin 9
  -ve -> arduino GND

 More info at: http://goo.gl/kJ8Gl
 Original code improvements to the Ping sketch sourced from Trollmaker.com
 Some code and wiring inspired by http://en.wikiversity.org/wiki/User:Dstaub/robotcar
 */

#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>

// Listen to the default port 5555, the Yún webserver
// will forward there all the HTTP requests you send
BridgeServer server;


#define trigPin 13
#define echoPin 12
#define led 11    // obstacle response pin
#define led2 10   // free response pin
#define piezoPin 9


void setup() {
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
  pinMode(led2, OUTPUT);

  // Bridge
  pinMode(piezoPin, OUTPUT);
  digitalWrite(piezoPin, LOW);
  Bridge.begin();
  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();
}

void loop() {
  // Get clients coming from server
  BridgeClient client = server.accept();
  // There is a new client?
  if (client) {
    // Process request
    digitalCommand(client);
    // Close connection and free resources.
    client.stop();
  }

  monitorUltraSonicSensor(80);
  
  delay(500);
}


void digitalCommand(BridgeClient client) {
  int pin, value;

  // Read pin number
  pin = client.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/digital/13/1"
  if (client.read() == '/') {
    value = client.parseInt();
    digitalWrite(pin, value);
  } else {
    value = digitalRead(pin);
  }

  // Send feedback to client
  client.print(F("Pin D"));
  client.print(pin);
  client.print(F(" set to "));
  client.println(value);

  // Update datastore key with the current pin value
  String key = "D";
  key += pin;
  Bridge.put(key, String(value));
}

void monitorUltraSonicSensor(int obstacleDistance) {
  long duration, distance;
 
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  if (distance < obstacleDistance) {  // This is where the LED On/Off happens
    digitalWrite(led,HIGH); // When the Red condition is met, the Green LED should turn off
    digitalWrite(led2,LOW);
//    tone(piezoPin, 2000);
    digitalWrite(piezoPin, HIGH);
  }
  else {
    digitalWrite(led,LOW);
    digitalWrite(led2,HIGH);
  }
  
  if (distance >= 200 || distance <= 0){
    Serial.println("Out of range");
  }
  else {
    Serial.print(distance);
    Serial.println(" cm");
  }
}

