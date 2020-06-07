
// Devil bank
// Met als currency Sins

#include <MFRC522.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

#define SS_PIN D0
#define RST_PIN D1

const char* ssid = "Driessen";
const char* passwd = "fjdkaljfda";

IPAddress server_addr(192,168,178,45);  // IP of the MySQL *server* here
char user[] = "Dank";              // MySQL user login username
char password[] = "This_is_A_Very_secure_psw!;)";        // MySQL user login password

WiFiClient client;
MySQL_Connection conn((Client *)&client);
// Create an instance of the cursor passing in the connection
MySQL_Cursor cur = MySQL_Cursor(&conn);

String content;
String code;
String money;

const byte ROWS = 4; 
const byte COLS = 4; 

String acc_name;
String card_pin;
String user_balance;

bool notTimeOut = true;
bool notRemoved;
bool quit;
bool hoofdmenu = false;
bool enough;

int error = 0;

int fiveSinBill = 0;
int tenSinBill = 0;
int twentySinBill = 0;
int fiftySinBill = 0;

MFRC522 mfrc522(SS_PIN, RST_PIN);

bool localUser;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); 
  SPI.begin();
  WiFi.hostname("DevilBank");
  WiFi.begin();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
  if (conn.connect(server_addr, 3306, user, password)) {
    delay(1000);
  }
  else Serial.println("Connection failed.");
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



  char query[100] = "SELECT acc_name, card_pin, balance FROM devilbank.atm_user WHERE card_no = ";
  query[75] = '\'';
  query[76] = content[0];
  query[77] = content[1];
  query[78] = content[2];
  query[79] = content[3];
  query[80] = content[4];
  query[81] = content[5];
  query[82] = content[6];
  query[83] = '\'';
  conn.connect(server_addr, 3306, user, password);
  row_values *row = NULL;

  //delay(1000);

  //Serial.println("1) Demonstrating using a cursor dynamically allocated.");
  // Initiate the query class instance
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  // Execute the query
  cur_mem->execute(query);
  // Fetch the columns (required) but we don't use them.
  column_names *columns = cur_mem->get_columns();

  // Read the row (we are only expecting the one)
  do {
    row = cur_mem->get_next_row();
    if (row != NULL) {
      acc_name = row->values[0];
      card_pin = row->values[1];
      user_balance = row->values[2];
      localUser = true;
    }
    else{
      localUser = false;
      // communiceer naar de noob!
    }
  } while (row != NULL);
  // Deleting the cursor also frees up memory used
  delete cur_mem;




  while(true){
    if(error == 3){
      Serial.println("Pinpas geblokkeerd!");
      break;
    }
    keypadLogic();

    if(card_pin == code && error < 3){
      while(true){
        hoofdmenu = false;
        error = 0;
        balanceLogic();
        if(enough && !quit){          
          int afschrijven = money.toInt();
          Serial.print(money);
          Serial.println(" sins gepint!");
          balance = String(int(balance) - afschrijven);


          if(localUser){
            char UPDATE_SQL[150] = "UPDATE `atm_user` SET `balance` = '"; // 45 characters long
            int index = 45
            int i = 0;
            for(index; index < index + balance.length(); index++){
              UPDATE_SQL[index] = balance[i];
              i++;
            }
            String lastPart = "' WHERE `atm_user`.`user_id` = '"; // 33 characters long
            i = 0;
            for(index; index < index + lastPart.length(); index++){
              UPDATE_SQL[index] = lastPart[i];
              i++;
            }
            i = 0;
            for(index; index < index + acc_name.length(); index++){
              UPDATE_SQL[index] = acc_name[i];
              i++;
            }
            index++;
            UPDATE_SQL[index] = '\'';
            cursor->execute(INSERT_SQL);
          }
          else{
            // communiceer naar de noob om het geld af te schrijven
          }



          
          quit = true;
          //removeKeycard();
        }
        else if(!notTimeOut || quit){
          notTimeOut = true;
          //removeKeycard();
          break;        
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
     else if(notTimeOut){
       error2++;
       Serial.print("Aantal verkeerde pogingen: ");
       Serial.println(error2);
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
Serial.println("Vul nu je pincode in.");
  long prevMillis = millis();
  char customKey;
  int number = 0;
  code = "";
  bool invoeren = true;
  while(invoeren == true){
    customKey = keypadKey();
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
      prevMillis = millis();
    }
    else if(customKey == '*'){
      code = "";
      number = 0;
      Serial.println(customKey);
      customKey = 'n';
      prevMillis = millis();
    }
    else if(customKey == '#'){
      invoeren = false;
      prevMillis = millis();
      number++;
    }else if(customKey == 'n'){
      
    }
    else if(customKey && number < 4){
      code += customKey;
      Serial.print("*");
      prevMillis = millis();
      number++;
    } 
    if(millis() - prevMillis > 20000){
      Serial.println();
      Serial.println("Timeout.");
      notTimeOut = false;
      return;
    }
  }
  Serial.println();
}


void balanceLogic(){
  int balance = int(user_balance);
  long prevMillis = millis();
  char customKey;
  money = "";
  bool invoeren = true;
  bool handmatig = true;
  Serial.println("Wil je je saldo zien druk dan op B.");
  Serial.println("Druk op C om snel 70 te pinnen in de vorm van 50 en 20");
  while(invoeren == true){
    customKey = keypadKey();
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
      break;
    }
    else if (customKey == 'D') {
      hoofdmenu = true;
      prevMillis = millis();
    }
    else if(customKey == '*'){
      money = "";
      Serial.println(customKey);
      customKey = 'n';
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
  bool back;
  enough = authMoney(money, balance1);
  if(enough && handmatig){
    back = chooseBills(money);
  }
  else if(enough){
    twentySinBill = 1;
    fiftySinBill = 1;
  }
  if(!back){
    enough = false;
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

bool chooseBills(String money){
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
    customKey = keypadKey();
    if (customKey == 'A') {
      quit = true;
      return false;
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
        Serial.print(customKey);
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
        Serial.print(customKey);
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
        Serial.print(customKey);
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
        Serial.print(customKey);
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
      customKey = 'n';
      prevMillis = millis();
    }
    else if(customKey == '#'){
      prevMillis = millis();
    }
    else if(customKey){
      prevMillis = millis();
    } 
    if(millis() - prevMillis > 20000){
      Serial.println();
      Serial.println("Timeout.");
      notTimeOut = false;
      return false;
    }
    if(hoofdmenu) return false;
  }
  return true;
}
