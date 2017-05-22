/*
  UploadToDropbox
  
  Demonstrates uploading a file to Dropbox using the Temboo Arduino Yun SDK.
  
  This example code is in the public domain.
*/

#include <Console.h>
#include <Bridge.h>
#include <TembooYunShield.h>
//#include "TembooAccount.h" // contains Temboo account information, as described below

#define TEMBOO_ACCOUNT "ratulahmed"  // your Temboo account name 
#define TEMBOO_APP_KEY_NAME "myFirstApp"  // your Temboo app key name
#define TEMBOO_APP_KEY  "EooH6NGW7ZAYe9iqsxnNAhkV7jPehjgW"  // your Temboo app key

/*** SUBSTITUTE YOUR ACCESS TOKEN BELOW: ***/

// Note that for additional security and reusability, you could
// use #define statements to specify your Access Token in a .h file.

// Your Dropbox access token. For your own account, this can be 
// found in the Dropbox App console. For other accounts, the 
// access token is returned by the FinalizeOAuth Choreo
const String DROPBOX_ACCESS_TOKEN = "RyzY2OBYynAAAAAAAAABhY5nYfF1q1dmQ3RAH0tvUMxW3vSuN48vFpE2L2yKg7YB";

boolean success = false; // a flag to indicate whether we've uploaded the file yet

void setup() {
  Bridge.begin();
  Console.begin();
}

void loop() {

  Console.println("Loop started wait a moment...");
  delay(5000);
  
  if (!success) {
    Console.println("Uploading data to Dropbox...");
    
    TembooYunShieldChoreo UploadChoreo;

    // Invoke the Temboo client
    UploadChoreo.begin();

    // Set Temboo account credentials
    UploadChoreo.setAccountName(TEMBOO_ACCOUNT);
    UploadChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    UploadChoreo.setAppKey(TEMBOO_APP_KEY);
    
    // Set the path and filename of the new file
    UploadChoreo.addInput("Path", "/ArduinoTest.txt");

    // Specify the file content to upload. We're creating a plain text file,
    // but any other type of file must be Base-64 encoded before upload
    UploadChoreo.addInput("FileContent", "Hello, Arduino!");

    // Set the content type as plain text. Any other type of file must be
    // Base-64 encoded before upload.
    UploadChoreo.addInput("ContentType", "text/plain");

    // finally, the Dropbox Access Token defined above
    UploadChoreo.addInput("AccessToken", DROPBOX_ACCESS_TOKEN);
    
    // Identify the Choreo to run
    UploadChoreo.setChoreo("/Library/Dropbox/Files/Upload");
    
    // tell the Process to run and wait for the results. The 
    // return code (returnCode) will tell us whether the Temboo client 
    // was able to send our request to the Temboo servers
    unsigned int returnCode = UploadChoreo.run();

    // a return code of zero (0) means everything worked
    if (returnCode == 0) {
        Console.println("Success! File uploaded!");
        success = true;
    } else {
      // a non-zero return code means there was an error
      Console.println("Uh-oh! Something went wrong!");
    }
    
    // print out the full response to the serial monitor in all
    // cases, just for debugging
    while(UploadChoreo.available()) {
      char c = UploadChoreo.read();
      Console.print(c);
    }
    UploadChoreo.close();
  }

  Console.println("Waiting...");
  delay(30000); // wait 30 seconds between Upload attempts
}
