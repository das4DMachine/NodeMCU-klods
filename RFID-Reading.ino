#include <SoftwareSerial.h>

#define lightPort A0
#define lightConstant 300 //Depending on the surroundings!

SoftwareSerial RFID(2, 3); // RX and TX

int data;
String result = "";
String lastResult;
char c;

void setup()
{
  RFID.begin(9600);    // start serial to RFID reader
  Serial.begin(9600);  // start serial to PC 
}
 
void loop()
{
  checkUpdateAndPrint();
  if(somethingOnTop()) postID(lastResult);
}

void postID(String ID) 
{
  ID = ID.substring(1, 13);
  Serial.println(ID);
}

void checkUpdateAndPrint() {

  while(RFID.available()>0) {
    c=RFID.read();
    result += c;
    
    if(c == 0x03) break;
  }
  
  if(c == 0x03) {
    //Serial.println("result" + result);
    //we have a reading
    lastResult = result;
    result = "";   
  }

}

boolean somethingOnTop() {
    int value = analogRead(lightPort);
    Serial.println(value);
    return value < lightConstant;
}
