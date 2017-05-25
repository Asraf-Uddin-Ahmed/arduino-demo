/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>
#include <Console.h>
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>

// Listen to the default port 5555, the Yún webserver
// will forward there all the HTTP requests you send
BridgeServer server;

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  Bridge.begin();
  Console.begin();
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();
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
  
  if (Console.available() > 0) {
    pos = Console.parseInt();
    Console.parseInt();
    Console.println(pos);
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
  }
}

void servoCommand(BridgeClient client) {
  int value, currentPos;
  value = client.parseInt();
  currentPos = value + pos;
  pos = currentPos > 0 && currentPos <= 180 ? currentPos : pos;
  myservo.write(pos);
}

void digitalCommand(BridgeClient client) {
  Console.println("digital command");
}

