/*
For wifi network connction use -> Console
Otherwise use -> Serial
*/

#include <Bridge.h>
#include <Console.h>

int a = 0;

void setup() {
  Bridge.begin();
  Console.begin();
}

void loop() {
  Console.println(a);
  a = a + 1;
  delay(1000);
  if(a > 10) {
    a = 0;
  }
}
