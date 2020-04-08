// Devil bank
// Met als currency Sins

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
bool notTimeOut = true;
bool notRemoved;
bool quit;

int error1 = 0;
int error2 = 0;

MFRC522 mfrc522(SS_PIN, RST_PIN);
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  SPI.begin();
  mfrc522.PCD_Init();
  start();
}

void start(){
  while(!Serial){}
  Serial.println("Welkom bij de pinautomaat van de Devilbank. Je kan hier sins pinnen.");
  Serial.println("Hou je pas voor de scanner en voer daarna je pincode in.");
  Serial.println("Als je de goede code hebt ingevoerd druk dan het hekje in om het te bevestigen.");
  Serial.println("Als je 3 keer een verkeerde code in hebt gevoerd dan word je pas geblokkeerd.");
  Serial.println("Wil je terug druk dan op A. We hopen dat het een duivelse ervaring wordt!");
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
  for (byte teller = 0; teller < mfrc522.uid.size; teller++){
     Serial.print(mfrc522.uid.uidByte[teller], HEX);
     content.concat(String(mfrc522.uid.uidByte[teller], HEX));
  }
  Serial.println();

  if(content == "8a1cdbb"){
    while(true){
    if(error1 == 3){
      Serial.println("Pinpas geblokkeerd!");
      return;
    }
     //notRemoved = true;
     keypadLogic();
     if(code == "1234" && error1 < 3){
      Serial.println("Money everywhere the eye can see!");
      error1 = 0;
      //removeKeycard();
      break;
     }
     else if(!notTimeOut || quit){
        quit = false;
        notTimeOut = true;
        //removeKeycard(); 
        break;       
     }
     else if(notTimeOut){
      error1++;
      Serial.print("Aantal verkeerde pogingen: ");
      Serial.println(error1);
     }
    }
   }
   else if(content == "fb7dd81a"){
    while(true){
    if(error2 == 3){
      Serial.println("Pinpas geblokkeerd!");
      break;
    }
    keypadLogic();
     if(code == "4321" && error2 < 3){
      Serial.println("Money everywhere the eye can see!");
      error2 = 0;
      //removeKeycard();
      break;
     }
     else if(!notTimeOut || quit){
        quit = false;
        notTimeOut = true;
        //removeKeycard();
        break;        
     }
     else if(notTimeOut){
      error2++;
      Serial.print("Aantal verkeerde pogingen: ");
      Serial.println(error2);
     }
    }
   }
}

void removeKeycard(){ // werkt niet
  String temp;
  content = "";
  while(notRemoved){
        if (!mfrc522.PICC_IsNewCardPresent()){
          return;
        }
        else{
          Serial.println("Please remove card");
          delay(1000);
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
      invoeren = false;
      quit = true;
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
    if(millis() - prevMillis > 20000){
      Serial.println();
      Serial.println("Timeout.");
      notTimeOut = false;
      return;
    }
  }
  Serial.println();
  Serial.println(code);
}
