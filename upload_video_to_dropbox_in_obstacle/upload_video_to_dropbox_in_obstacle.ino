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
#include <Process.h>

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
bool isRecorded = false;


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

  // mount to sda
  process.runShellCommand("mkdir -p " + path); /* mkdir -p /mnt/sda/ */
  while (process.running());
  process.runShellCommand("mount /dev/sda " + path);  /* mount /dev/sda /mnt/sda/ */
  while (process.running());
  process.runShellCommand("mjpg_streamer -i 'input_uvc.so -y -n -f 30 -r 280x175' -o 'output_http.so -p 8080 -n -w /www/webcam' &");  /* start streaming */
  while (process.running());
}


void loop() {
  monitorUltraSonicSensor(80);
  delay(500);
}


void recordVideo() {
  Console.println("Recording video...");
  
  // Generate filename with timestamp
  filename = "";
  process.runShellCommand("date +%s");
  while (process.running());

  while (process.available() > 0) {
    char c = process.read();
    filename += c;
  }
  filename.trim();
  filename += ".avi";

  // record video: ffmpeg -f mjpeg -r 5 -i 'http://localhost:8080/?action=stream' -t 5 /mnt/sda/1495478139.avi
  process.runShellCommand("ffmpeg -f mjpeg -r 5 -i 'http://localhost:8080/?action=stream' -t 5 " + path + filename);
  while (process.running());

  Console.println("Uploading video...");
  
  // Upload to Dropbox: python /mnt/sda/upload_picture.py /mnt/sda/1495478139.png
  process.runShellCommand("python " + path + "upload_file_to_dropbox.py " + path + filename);
  while (process.running());

  Console.println("Recorded and uploaded :)");

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
    if(isRecorded == false){
      isRecorded = true;
      recordVideo();
    }
  }
  else {
    digitalWrite(led, LOW);
    digitalWrite(led2, HIGH);
    isRecorded = false;
  }

  if (distance >= 200 || distance <= 0) {
    Console.println("Out of range");
  }
  else {
    Console.print(distance);
    Console.println(" cm");
  }
}

