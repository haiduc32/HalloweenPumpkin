/*
 * 
 * Documentation: https://drive.google.com/drive/folders/0B6uNNXJ2z4CxaFVzZEZZVTR5Snc
  */
#include "RedMP3.h"

#define TRIGGER 2
#define PULSE 3
#define LED 13

// song definitions
#define CREEPY_BACKGROUND 1 //creepy background sound, suitable in-between actions
#define MANIACAL_LAUGH 2

#define MP3_RX 5  //should connect to TX of the Serial MP3 Player module
#define MP3_TX 6  //connect to RX of the module

MP3 mp3(MP3_RX, MP3_TX);

static uint8_t ansbuf[10] = {0}; // Buffer for the answers

void setup() {
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);
  pinMode(TRIGGER, OUTPUT);
  pinMode(PULSE, INPUT);
  digitalWrite(TRIGGER, LOW);
  Serial.begin(9600);
  delay(1000);
  mp3.begin();
  delay(50);
}

void loop() {
  //trigger the range sensor
  digitalWrite(TRIGGER, HIGH);
  delay(1);
  digitalWrite(TRIGGER, LOW);
  
  unsigned long pulseWidth = pulseIn(PULSE, HIGH); 
  unsigned long distance;
  
  // in millimeters
  if((pulseWidth < 60000) && (pulseWidth > 1)) {   // Pulse effective range (1, 60000).
    // Calculating the distance by the pulse width.   
    distance = (pulseWidth*34/100)/2;           
    // Output to the serial port monitor        
    Serial.print("Present Distance is: ");           
    Serial.print(distance, DEC);                    
    Serial.println("mm");             
    
    if (distance < 1000) {
      //TODO: play random sounds
      playSong(MANIACAL_LAUGH);

      //also we need some cooldown time (15 sec?)
      delay(15000);
    }
   } 
  delay(500);
}

void playSong(byte song) {
  mp3.playWithFileName(0x01, song);
  delay(50);

  //TODO: we could wait for a response from the MP3 player, for when the playback has finished?
  switch (song) {
    case CREEPY_BACKGROUND:
      delay(65000);
     case MANIACAL_LAUGH:
      delay(5000);
  }
}
