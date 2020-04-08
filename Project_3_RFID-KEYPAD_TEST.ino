// Devil bank
// Met als currency Sins

#include <MFRC522.h>
#include <SPI.h>
#include <Keypad.h>

#define SS_PIN 10
#define RST_PIN 9

String content;
String code;
String money;

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

bool notTimeOut = true;
bool notRemoved;
bool quit;
bool hoofdmenu = false;
bool enough;

int error1 = 0;
int error2 = 0;

int balance1 = 1920;
int balance2 = 30;

int fiveSinBill = 0;
int tenSinBill = 0;
int twentySinBill = 0;
int fiftySinBill = 0;

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
  Serial.println("Wil je de transactie afbreken druk dan op A. Wil je terug naar het hoofdmenu druk op D.");
  Serial.println("We hopen dat het een duivelse ervaring wordt!");
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
  //Serial.print("UID tag: ");
  for (byte teller = 0; teller < mfrc522.uid.size; teller++){
     //Serial.print(mfrc522.uid.uidByte[teller], HEX);
     content.concat(String(mfrc522.uid.uidByte[teller], HEX));
  }
  //Serial.println();

  if(content == "8a1cdbb"){
    while(true){
      hoofdmenu = false;
      if(error1 == 3){
        Serial.println("Pinpas geblokkeerd!");
        return;
      }
     //notRemoved = true;
     balanceLogic(0);
     if(enough && !quit){
      while(true){
         keypadLogic();
         if(code == "1234" && error1 < 3){
          int afschrijven = money.toInt();
          Serial.print(money);
          Serial.println(" euro gepint!");
          balance1 = balance1 - afschrijven;
          error1 = 0;
          quit = true;
          //removeKeycard();
          break;
         }
         else if(!notTimeOut || quit){
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
     if(!notTimeOut || quit){
       quit = false;
       notTimeOut = true;
       //removeKeycard(); 
       start();
       break;       
       }
    }
   }
   else if(content == "fb7dd81a"){
    while(true){
      hoofdmenu = false;
      if(error2 == 3){
        Serial.println("Pinpas geblokkeerd!");
        break;
      }
      balanceLogic(1);
      if(enough && !quit){
        while(true){
          keypadLogic();
          if(code == "4321" && error2 < 3){
            int afschrijven = money.toInt();
            Serial.print(money);
            Serial.println(" euro gepint!");
            balance2 = balance2 - afschrijven;
            error2 = 0;
            quit = true;
            //removeKeycard();
            break;
          }
          else if(!notTimeOut || quit){
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
     if(!notTimeOut || quit){
       quit = false;
       notTimeOut = true;
       //removeKeycard(); 
       start();
       break;       
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
  Serial.println();
  Serial.println("Vul nu je pincode in.");
  long prevMillis = millis();
  char customKey;
  int number = 0;
  code = "";
  bool invoeren = true;
  while(invoeren == true){
    customKey = customKeypad.getKey();
    if (customKey == 'A') {
      invoeren = false;
      quit = true;
    }
    else if (customKey == 'B') {
      prevMillis = millis();
    }
    else if (customKey == 'C') {
      
      prevMillis = millis();
    }
    else if (customKey == 'D') {
      hoofdmenu = true;
      prevMillis = millis();
    }
    else if(customKey == '*'){
      code = "";
      number = 0;
      Serial.println(customKey);
      customKey = NO_KEY;
      prevMillis = millis();
    }
    else if(customKey == '#'){
      invoeren = false;
      prevMillis = millis();
      number++;
    }
    else if(customKey && number < 4){
      code += customKey;
      Serial.print('*');
      prevMillis = millis();
      number++;
    } 
    if(millis() - prevMillis > 20000){
      Serial.println();
      Serial.println("Timeout.");
      notTimeOut = false;
      return;
    }
    if(hoofdmenu) return;
  }
  Serial.println();
}

void balanceLogic(int accountNr){
  int balance;
  if(accountNr == 0) balance = balance1;
  else if(accountNr == 1) balance = balance2;
  long prevMillis = millis();
  char customKey;
  money = "";
  bool invoeren = true;
  bool handmatig = true;
  Serial.println("Wil je je saldo zien druk dan op B.");
  Serial.println("Druk op C om snel 70 te pinnen in de vorm van 50 en 20");
  while(invoeren == true){
    customKey = customKeypad.getKey();
    if (customKey == 'A') {
      invoeren = false;
      quit = true;
    }
    else if (customKey == 'B') {
      Serial.print("Balance = ");
      Serial.println(balance);
      prevMillis = millis();
    }
    else if (customKey == 'C') {
      handmatig = false;
      money = "70";
      Serial.println();
      break;
    }
    else if (customKey == 'D') {
      hoofdmenu = true;
      prevMillis = millis();
    }
    else if(customKey == '*'){
      money = "";
      Serial.println(customKey);
      customKey = NO_KEY;
      prevMillis = millis();
    }
    else if(customKey == '#'){
      int temp = money.toInt();
      Serial.println();
      if(temp % 5 == 0){
        invoeren = false;
      }
      else{
        Serial.println("Het kleinste biljet is 5 sins en je bedrag dat je wil opnemen kan niet door 5 worden gedeeld.");
        Serial.println("Vul opnieuw een bedrag in.");
        money = "";
      }
      prevMillis = millis();
    }
    else if(customKey){
      money += customKey;
      Serial.print(customKey);
      prevMillis = millis();
    } 
    if(millis() - prevMillis > 20000){
      Serial.println();
      Serial.println("Timeout.");
      quit = true;
      return;
    }
    if(hoofdmenu) return;
  }
  if(accountNr == 0 && !quit){
    enough = authMoney(money, balance1);
    if(enough && handmatig){
      chooseBills(money);
    }
    else if(enough){
      twentySinBill = 1;
      fiftySinBill = 1;
    }
  }
  else if(accountNr == 1 && !quit){
    enough = authMoney(money, balance2);
    if(enough && handmatig){
      chooseBills(money);
    }
    else if(enough){
      twentySinBill = 1;
      fiftySinBill = 1;
    }
  }
}

bool authMoney(String money, int balance){
  int cash = money.toInt();
  if(cash > balance){
    Serial.println("Je hebt niet zoveel saldo op je rekening staan!");
    return false;
  }
  return true;
}

void chooseBills(String money){
  Serial.println();
  Serial.println("Druk op 1 voor een 5 sin briefje.");
  Serial.println("Druk op 2 voor een 10 sin briefje.");
  Serial.println("Druk op 3 voor een 20 sin briefje.");
  Serial.println("Druk op 4 voor een 50 sin briefje.");
  fiveSinBill = 0;
  tenSinBill = 0;
  twentySinBill = 0;
  fiftySinBill = 0;
  int cash = money.toInt();
  char customKey;
  long prevMillis = millis();
  while(cash != 0){
    customKey = customKeypad.getKey();
    if (customKey == 'A') {
      quit = true;
      return;
    }
    else if (customKey == 'B') {
      prevMillis = millis();
    }
    else if (customKey == 'D'){
      hoofdmenu = true;
    }
    else if (customKey == '1') {
      if(cash - 5 >= 0){
        fiveSinBill++;
        cash = cash - 5;
      }
      else {
        Serial.print("Je kan niet zoveel geld pinnen. Nog te pinnen geld: ");
        Serial.println(cash);
      }
      prevMillis = millis();
    }
    else if (customKey == '2') {
      if(cash - 10 >= 0){
        tenSinBill++;
        cash = cash - 10;
      }
      else {
        Serial.print("Je kan niet zoveel geld pinnen. Nog te pinnen geld: ");
        Serial.println(cash);
      }
      prevMillis = millis();
    }
    else if (customKey == '3') {
      if(cash - 20 >= 0){
        twentySinBill++;
        cash = cash - 20;
      }
      else {
        Serial.print("Je kan niet zoveel geld pinnen. Nog te pinnen geld: ");
        Serial.println(cash);
      }
      prevMillis = millis();
    }
    else if (customKey == '4'){
      if(cash - 50 >= 0){
        fiftySinBill++;
        cash = cash - 50;
      }
      else {
        Serial.print("Je kan niet zoveel geld pinnen. Nog te pinnen geld: ");
        Serial.println(cash);
      }
      prevMillis = millis();
    }
    else if(customKey == '*'){
      cash = money.toInt();
      fiveSinBill = 0;
      tenSinBill = 0;
      twentySinBill = 0;
      fiftySinBill = 0;
      Serial.println(customKey);
      customKey = NO_KEY;
      prevMillis = millis();
    }
    else if(customKey == '#'){
      prevMillis = millis();
    }
    else if(customKey){
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
    if(hoofdmenu) return;
  }
}
