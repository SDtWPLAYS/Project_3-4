#include <MFRC522.h>
#include <SPI.h>
#include <Keypad.h>
#include <Wire.h>

// Devil bank
// Met Devil bills

#define SS_PIN 10
#define RST_PIN 9

String content;
String code;

byte error = 0;

const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {11, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2};

bool invoeren;

MFRC522 mfrc522(SS_PIN, RST_PIN);
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  SPI.begin();
  Wire.begin();
  mfrc522.PCD_Init();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!mfrc522.PICC_IsNewCardPresent()){ 
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()){
    return;
  }
  String content= "";
  //Show UID on serial monitor
  Serial.print("UID tag: ");
  for (byte teller = 0; teller < mfrc522.uid.size; teller++) 
  {
     Serial.print(mfrc522.uid.uidByte[teller], HEX);
     content.concat(String(mfrc522.uid.uidByte[teller], HEX));
  }
  Serial.println();

  if(content == "8a1cdbb"){
    keypadLogic();
    Wire.beginTransmission(666);
    Wire.write(2);
    Wire.endTransmission();
    Wire.requestFrom(666, 1);
    bool correct = Wire.read();
    if(error > 2){
      Wire.beginTransmission(666);
      Wire.write(4);
      Wire.endTransmission();
    }
    else if(correct){
      error = 0;
      Serial.println("Congratulations (victorysounds)");
    }
    else{
      error++;
    }
  }
   if(content == "fb7dd81a"){
     keypadLogic();
     Wire.beginTransmission(666);
     Wire.write(3);
     Wire.endTransmission();
     Wire.requestFrom(666, 1);
     bool correct = Wire.read();
     if(error > 2){
      Wire.beginTransmission(666);
      Wire.write(5);
      Wire.endTransmission();
    }
    else if(correct){
      error = 0;
      Serial.println("Congratulations (victorysounds)");
    }
    else{
      error++;
    }
  }
}

void keypadLogic(){
  long prevMillis = millis();
  char customKey;
  invoeren = true;
  Wire.beginTransmission(666);
  Wire.write(1);
  while(invoeren == true){
    customKey = customKeypad.getKey();
    if (customKey == 'A') {
      Wire.write(customKey);
      prevMillis = millis();
    }
    else if (customKey == 'B') {
      Wire.write(customKey);
      prevMillis = millis();
    }
    else if (customKey == 'C') {
      Wire.write(customKey);
      prevMillis = millis();
    }
    else if (customKey == 'D') {
      Wire.write(customKey);
      prevMillis = millis();
    }
    else if(customKey == '*'){
      code = "";
      Wire.endTransmission();
      Wire.beginTransmission(666);
      //Serial.println(customKey);
      customKey = NO_KEY;
      prevMillis = millis();
    }
    else if(customKey == '#'){
      invoeren = false;
      Wire.endTransmission();
      prevMillis = millis();
    }
    else if(customKey){
      Wire.write(customKey);
      prevMillis = millis();
    } 
    if(customKey != NO_KEY){
      Serial.print(customKey);
    }
    if(millis() - prevMillis > 10000){
      Wire.endTransmission();
      Serial.println();
      Serial.println("Timeout.");
      return;
    }
  }
  Serial.println();
}
