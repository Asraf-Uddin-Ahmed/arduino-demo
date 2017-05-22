/*
100 ohm in ground
buzzer pin:
  +ve (long) with piezoPin
  -ve (small) with ground
*/

//Specify digital pin on the Arduino that the positive lead of piezo buzzer is attached.
int piezoPin = 13;
 
void setup() {
 
}//close setup
 
void loop() {
 
  /*Tone needs 2 arguments, but can take three
    1) Pin#
    2) Frequency - this is in hertz (cycles per second) which determines the pitch of the noise made
    3) Duration - how long teh tone plays
  */
//  digitalWrite(piezoPin, 1);
//  tone(piezoPin, 1000, 500);
 
  tone(piezoPin, 1000, 500);
  delay(1000);
 
}
