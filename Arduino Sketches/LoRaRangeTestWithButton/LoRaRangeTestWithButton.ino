/*
  Lora Send And Receive
  This sketch demonstrates how to send and receive data with the MKR WAN 1300/1310 LoRa module.
  This example code is in the public domain.
*/

#include <MKRWAN.h>

LoRaModem modem;

// Uncomment if using the Murata chip as a module
// LoRaModem modem(Serial1);

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;

const int buttonPin = 2; // the number of the pushbutton pin
const int waitingLED = 3;
const int sendFailLED = 4;
const int sendSucceedLED = 5;
int buttonState = 0; 
int counter = 0;
bool useSerial = false;

void setup() {
  
  //Button setup
  pinMode(buttonPin, INPUT);
  
  pinMode(waitingLED, OUTPUT);
  pinMode(sendFailLED, OUTPUT);
  pinMode(sendSucceedLED, OUTPUT);

  // put your setup code here, to run once:
  if(useSerial)
  {
    Serial.begin(115200);
  }
  
  // change this to your regional band (eg. US915, AS923, ...)
  if (!modem.begin(EU868)) {
    
    if(useSerial) {Serial.println("Failed to start module");}
   
    while (1) {
      digitalWrite(sendFailLED, HIGH);
        delay(100);
        digitalWrite(sendFailLED, LOW);
        delay(100);}
  };
  if(useSerial)
  {
    Serial.print("Your module version is: ");
    Serial.println(modem.version());
    Serial.print("Your device EUI is: ");
    Serial.println(modem.deviceEUI());
  }
  
  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected) {
    if(useSerial) {Serial.println("Something went wrong; are you indoor? Move near a window and retry");
  }
    while (1) 
    { 
        digitalWrite(sendFailLED, HIGH);
        delay(100);
        digitalWrite(sendFailLED, LOW);
        delay(100);
    }
  }

  // Set poll interval to 60 secs.
  modem.minPollInterval(60);
  // NOTE: independently by this setting the modem will
  // not allow to send more than one message every 2 minutes,
  // this is enforced by firmware and can not be changed.
}

void loop() {
 if(useSerial) 
 {
  Serial.println();
  Serial.println("Enter a message to send to network");
  Serial.println("Awaiting button press. Currently at" + counter);
 }
 
  digitalWrite(waitingLED, HIGH);
  delay(20);
  digitalWrite(waitingLED, LOW);
  

   // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(waitingLED, HIGH);
    String msg = "This is a test button press" + counter;

    if(useSerial) 
    {
      Serial.println();
      Serial.print("Sending: " + msg + " - ");  
      for (unsigned int i = 0; i < msg.length(); i++) 
      {
        Serial.print(msg[i] >> 4, HEX);
        Serial.print(msg[i] & 0xF, HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
  
    int err;
    modem.beginPacket();
    modem.print(msg);
    err = modem.endPacket(true);
    if (err > 0) 
    {
      if(useSerial)
      {
        Serial.println("Message sent correctly!");
      }

      for (unsigned int j = 0; j < 5; j++) 
      {
        digitalWrite(sendSucceedLED, HIGH);
        delay(200);
        digitalWrite(sendSucceedLED, LOW);
        delay(200);
      }
    }
    else 
    {
      if(useSerial)
      {
         Serial.println("Error sending message :(");
         Serial.println("(you may send a limited amount of messages per minute, depending on the signal strength");
         Serial.println("it may vary from 1 message every couple of seconds to 1 message every minute)");
      }

      for (unsigned int j = 0; j < 10; j++) 
      {
        digitalWrite(sendFailLED, HIGH);
        delay(100);
        digitalWrite(sendFailLED, LOW);
        delay(100);
      }
    }
    
    delay(1000);
    if (!modem.available()) 
    {
      if(useSerial)
      {
        Serial.println("No downlink message received at this time.");
      }
      return;
    }
    delay(2);

  } else {
    // turn LED off:
    digitalWrite(waitingLED, LOW);
  }
  delay(1000);
}
