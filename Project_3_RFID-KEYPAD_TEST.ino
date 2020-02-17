#include <MFRC522.h>
#include <SPI.h>
#include <Keypad.h>

#define SS_PIN 10
#define RST_PIN 9

String content;
String code;

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
     if(code == "22884646BA"){
      Serial.println("Congratulations (victorysounds)");
     }
   }
   if(content == "fb7dd81a"){
    keypadLogic();
     if(code == "AB64648822"){
      Serial.println("Congratulations (victorysounds)");
     }
    }
}

void keypadLogic(){
  long prevMillis = millis();
  char customKey;
  code = "";
  invoeren = true;
  while(invoeren == true){
    customKey = customKeypad.getKey();
    if (customKey == 'A') {
      code += customKey;
      prevMillis = millis();
    }
    else if (customKey == 'B') {
      code += customKey;
      prevMillis = millis();
    }
    else if (customKey == 'C') {
      code += customKey;
      prevMillis = millis();
    }
    else if (customKey == 'D') {
      code += customKey;
      prevMillis = millis();
    }
    else if(customKey == '*'){
      code = "";
      Serial.println(customKey);
      customKey = NO_KEY;
      prevMillis = millis();
    }
    else if(customKey == '#'){
      invoeren = false;
      prevMillis = millis();
    }
    else if(customKey){
      code += customKey;
      prevMillis = millis();
    } 
    if(customKey){
      Serial.print(customKey);
    }
    if(millis() - prevMillis > 10000){
      Serial.println();
      Serial.println("Timeout.");
      return;
    }
  }
  Serial.println();
  Serial.println(code);
}
