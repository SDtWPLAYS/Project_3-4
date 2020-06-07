#include <ESP8266WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <MFRC522.h>
#include <SPI.h>

const char* ssid = "Driessen";
const char* passwd = "dfasdf";

IPAddress server_addr(192,168,178,45);  // IP of the MySQL *server* here
char user[] = "Dank";              // MySQL user login username
char password[] = "This_is_A_Very_secure_psw!;)";        // MySQL user login password

// Sample query

WiFiClient client;
MySQL_Connection conn((Client *)&client);
// Create an instance of the cursor passing in the connection
MySQL_Cursor cur = MySQL_Cursor(&conn);

int i = 0;
String number = "1234";

#define SS_PIN D0
#define RST_PIN D1
MFRC522 mfrc522(SS_PIN, RST_PIN);
String content;

void setup() {
  Serial.begin(115200);
  while (!Serial); // wait for serial port to connect
  Serial.println("Connecting...");
  WiFi.hostname("BankingSystem");
  WiFi.begin(ssid, passwd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
  if (conn.connect(server_addr, 3306, user, password)) {
    delay(1000);
  }
  else
    Serial.println("Connection failed.");
}


void loop() {
  char query[100] = "SELECT acc_name, card_pin, balance FROM devilbank.atm_user WHERE user_id = ";
  query[75] = number[i];
//  query[76] = number[1];
//  query[77] = number[2];
//  query[78] = number[3];
//  query[79] = number[4];
//  query[80] = number[5];
  //query[81] = number[6];
  conn.connect(server_addr, 3306, user, password);
  row_values *row = NULL;
  String returnValue;
  String returnValue2;
  String returnValue3;

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
      returnValue = row->values[0];
      returnValue2 = row->values[1];
      returnValue3 = row->values[2];
    }
  } while (row != NULL);
  // Deleting the cursor also frees up memory used
  delete cur_mem;

  // Show the result
  Serial.print("Resultaat = ");
  Serial.print(returnValue);
  Serial.print(" - ");
  Serial.print(returnValue2);
  Serial.print(" - ");
  Serial.println(returnValue3);

  //delay(500);
  i++;
  if(i == 4) i = 0;
}
