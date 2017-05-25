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
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <Servo.h>

#define trigPin 13
#define echoPin 12
#define led 11    // obstacle response pin
#define led2 10   // free response pin
#define piezoPin 9
#define servoPin 8

// Listen to the default port 5555, the Yún webserver
// will forward there all the HTTP requests you send
BridgeServer server;

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 10;    // variable to store the servo position

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

  pinMode(piezoPin, OUTPUT);
  digitalWrite(piezoPin, LOW);

  myservo.attach(servoPin);

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
  BridgeClient client = server.accept();
  if (client) {
    String command = client.readStringUntil('/');
    if (command == "digital") {
      digitalCommand(client);
    }
    else if (command == "servo") {
      servoCommand(client);
    }
    // Close connection and free resources.
    client.stop();
  }

  monitorUltraSonicSensor(80);
  delay(500);
}

void servoCommand(BridgeClient client) {
  int value, currentPos;
  value = client.parseInt();
  currentPos = value + pos;
  pos = currentPos > 0 && currentPos <= 180 ? currentPos : pos;
  myservo.write(pos);
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

  // record video: ffmpeg -f mjpeg -r 5 -i 'http://localhost:8080/?action=stream' -t 15 /mnt/sda/1495478139.avi
  process.runShellCommand("ffmpeg -f mjpeg -r 5 -i 'http://localhost:8080/?action=stream' -t 15 " + path + filename + " &");
  sweepServo(30);
  // while (process.running());
  delay(5000); // for '&' operator in end of the command this delay is used instead of 'while()'
  
  Console.println("Uploading video...");
  
  // Upload to Dropbox: python /mnt/sda/upload_picture.py /mnt/sda/1495478139.png
  process.runShellCommand("python " + path + "upload_file_to_dropbox.py " + path + filename);
  while (process.running());

  Console.println("Recorded and uploaded :)");

  process.runShellCommand("rm " + path + filename);
  while (process.running());
}

int sweepServo(int delayInMilliSec){
  for (int currentPos = 0; currentPos <= 180; currentPos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(currentPos);              // tell servo to go to position in variable 'pos'
    delay(delayInMilliSec);                       // waits 15ms for the servo to reach the position
  }
  for (int currentPos = 180; currentPos >= 0; currentPos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(currentPos);              // tell servo to go to position in variable 'pos'
    delay(delayInMilliSec);                       // waits 15ms for the servo to reach the position
  }
  myservo.write(pos);              // tell servo to go to position in variable 'pos'
  return delayInMilliSec * 360;
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
      digitalWrite(piezoPin, HIGH);
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

