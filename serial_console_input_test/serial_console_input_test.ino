/*
For wifi network connction use -> Console
Otherwise use -> Serial

Give value on input box (top of the Serail Monitor):
  0 => led off
  1 => led on
*/

#include <Console.h>
#include <Bridge.h>

int led = 9;

void setup() {
  Bridge.begin();
  Serial.begin(9600);
  Console.begin();
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
}

void loop() {
  Console.println("looping...");
  if (Console.available() > 0) {
    char ledState = Console.read();
    Console.println(ledState);
    if (ledState == '1') {
      digitalWrite(led, HIGH);
    }
    if (ledState == '0') {
      digitalWrite(led, LOW);
    }
  }
  delay(500);
}
