/*
 * 
 * Documentation: https://drive.google.com/drive/folders/0B6uNNXJ2z4CxaFVzZEZZVTR5Snc
  */

#include <SoftwareSerial.h>

#define TRIGGER 2
#define PULSE 3
#define LED 13

#define CMD_SEL_DEV 0X35
#define DEV_TF 0X01
#define CMD_PLAY_FOLDER_FILE 0X42
#define CMD_PLAY_WITH_VOLUME 0x31

// song definitions
#define CREEPY_BACKGROUND 2 //creepy background sound, suitable in-between actions
#define MANIACAL_LAUGH 1


#define MP3_RX 5  //should connect to TX of the Serial MP3 Player module
#define MP3_TX 6  //connect to RX of the module

SoftwareSerial mp3(MP3_RX, MP3_TX);

static uint8_t ansbuf[10] = {0}; // Buffer for the answers

void setup() {
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);
  pinMode(TRIGGER, OUTPUT);
  pinMode(PULSE, INPUT);
  digitalWrite(TRIGGER, LOW);
  mp3.begin(9600);
  Serial.begin(9600);
  delay(1000);
  //initialisation command
  sendCommand(CMD_SEL_DEV, DEV_TF);
  delay(200);
  while (mp3.available()) {
    Serial.println(decodeMP3Answer());
  }
}

void loop() {

  digitalWrite(TRIGGER, HIGH);
  delay(1);
  digitalWrite(TRIGGER, LOW);
  
  unsigned long pulseWidth = pulseIn(PULSE, HIGH); 
  unsigned long distance;

  while (mp3.available()) {
    Serial.println(decodeMP3Answer());
  }

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
  sendCommand(CMD_PLAY_WITH_VOLUME, 20, song);
  delay(10);
  while (mp3.available()) {
    Serial.println(decodeMP3Answer());
  }
  //TODO: we could wait for a response from the MP3 player, for when the playback has finished?
  switch (song) {
    case CREEPY_BACKGROUND:
      delay(65000);
     case MANIACAL_LAUGH:
      delay(5000);
  }
}

void sendCommand(byte command, byte data1) {
  static int8_t Send_buf[5] = { 0x7E, 0x03, 0x00, 0x00, 0xEF };
  Send_buf[2] = command;
  Send_buf[3] = data1;

  for (uint8_t i = 0; i < 5; i++)
  {
    mp3.write(Send_buf[i]) ;
    Serial.print(sbyte2hex(Send_buf[i]));
  }
  mp3.flush();
  Serial.println();
}

void sendCommand(byte command, byte data1, byte data2) {
  static int8_t Send_buf[6] = { 0x7E, 0x04, 0x00, 0x00, 0x00, 0xEF };
  Send_buf[2] = command;
  Send_buf[3] = data1;
  Send_buf[4] = data2;

  for (uint8_t i = 0; i < 6; i++)
  {
    mp3.write(Send_buf[i]) ;
    Serial.print(sbyte2hex(Send_buf[i]));
  }
  mp3.flush();
  Serial.println();
}

String sbyte2hex(uint8_t b)
{
  String shex;

  shex = "0X";

  if (b < 16) shex += "0";
  shex += String(b, HEX);
  shex += " ";
  return shex;
}

//this is not right.. can't find the documentation though
String decodeMP3Answer() {
  String decodedMP3Answer = "";

  decodedMP3Answer += sanswer();

  switch (ansbuf[3]) {
    case 0x3A:
      decodedMP3Answer += " -> Memory card inserted.";
      break;

    case 0x3D:
      decodedMP3Answer += " -> Completed play num " + String(ansbuf[6], DEC);
      break;

    case 0x40:
      decodedMP3Answer += " -> Error";
      break;

    case 0x41:
      decodedMP3Answer += " -> Data recived correctly. ";
      break;

    case 0x42:
      decodedMP3Answer += " -> Status playing: " + String(ansbuf[6], DEC);
      break;

    case 0x48:
      decodedMP3Answer += " -> File count: " + String(ansbuf[6], DEC);
      break;

    case 0x4C:
      decodedMP3Answer += " -> Playing: " + String(ansbuf[6], DEC);
      break;

    case 0x4E:
      decodedMP3Answer += " -> Folder file count: " + String(ansbuf[6], DEC);
      break;

    case 0x4F:
      decodedMP3Answer += " -> Folder count: " + String(ansbuf[6], DEC);
      break;
  }

  return decodedMP3Answer;
}

String sanswer(void)
{
  uint8_t i = 0;
  String mp3answer = "";

  // Get only 10 Bytes
  while (mp3.available() && (i < 10))
  {
    uint8_t b = mp3.read();
    ansbuf[i] = b;
    i++;

    mp3answer += sbyte2hex(b);
  }

  // if the answer format is correct.
  if ((ansbuf[0] == 0x7E) && (ansbuf[9] == 0xEF))
  {
    return mp3answer;
  }

  return "???: " + mp3answer;
}
