#include <Wire.h>

String correctCode1;
String correctCode2;
int balance;
int pas;
bool blocked1 = false;
bool blocked2 = false;

String code;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin(666);
  Wire.onRequest(request);
  Wire.onReceive(receive);
  correctCode1 = "22884646BA";
  correctCode2 = "ÄB64648822";
}

void loop() {
  // put your main code here, to run repeatedly:

}

void request(){
  if(pas == 1){
    if(code == correctCode1){
      Wire.write(true);
    }
    else{
      Wire.write(false);
    }
  }
  else if(pas == 2){
    if(code == correctCode2){
      Wire.write(true);
    }
    else{
      Wire.write(false);
    }
  }
  pas = 0;
}

void receive(){
  int i = Wire.read();
  if(i == 1){
    readCode();
  }
  else if(i == 2){
    pas = 1;
  }
  else if(i == 3){
    pas = 2;
  }
  else if(i == 4){
    blocked1 = true;
  }
  else if(i == 5){
    blocked2 = true;
  }
}

void readCode(){
  code = "";
    while(Wire.available()){
      code += Wire.read();
    }
}
