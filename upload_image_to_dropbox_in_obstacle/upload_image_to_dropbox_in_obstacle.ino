// Sketch to upload pictures to Dropbox when motion is detected
#include <Bridge.h>
#include <Process.h>
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

  More info at: http://goo.gl/kJ8Gl
  Original code improvements to the Ping sketch sourced from Trollmaker.com
  Some code and wiring inspired by http://en.wikiversity.org/wiki/User:Dstaub/robotcar
*/

#include <Bridge.h>
#include <BridgeServer.h>
#include <Console.h>

// Listen to the default port 5555, the Yún webserver
// will forward there all the HTTP requests you send
BridgeServer server;


#define trigPin 13
#define echoPin 12
#define led 11    // obstacle response pin
#define led2 10   // free response pin

// Picture process
Process process;
String filename;
String path = "/mnt/sda/";
bool isCaptured = false;


void setup() {
  Bridge.begin();
  Console.begin();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
  pinMode(led2, OUTPUT);

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();
}


void loop() {
  monitorUltraSonicSensor(80);
  delay(500);
}


void captureImage() {
  Console.println("Capturing image...");
  
  // Generate filename with timestamp
  filename = "";
  process.runShellCommand("date +%s");
  while (process.running());

  while (process.available() > 0) {
    char c = process.read();
    filename += c;
  }
  filename.trim();
  filename += ".png";

  // Take picture: fswebcam /mnt/sda/1495478139.png  -r 320x240
  process.runShellCommand("fswebcam " + path + filename + " -r 320x240");
  while (process.running());

  Console.println("Uploading image...");
  
  // Upload to Dropbox: python /mnt/sda/upload_picture.py /mnt/sda/1495478139.png
  process.runShellCommand("python " + path + "upload_file_to_dropbox.py " + path + filename);
  while (process.running());

  Console.println("Captured and uploaded :)");

  process.runShellCommand("rm " + path + filename);
  while (process.running());
}

void monitorUltraSonicSensor(int obstacleDistance) {
  long duration, distance;

  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;

  if (distance < obstacleDistance) {  // This is where the LED On/Off happens
    digitalWrite(led, HIGH); // When the Red condition is met, the Green LED should turn off
    digitalWrite(led2, LOW);
    if(isCaptured == false){
      isCaptured = true;
      captureImage();
    }
  }
  else {
    digitalWrite(led, LOW);
    digitalWrite(led2, HIGH);
    isCaptured = false;
  }

  if (distance >= 200 || distance <= 0) {
    Console.println("Out of range");
  }
  else {
    Console.print(distance);
    Console.println(" cm");
  }
}

