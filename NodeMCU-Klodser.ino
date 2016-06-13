#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define lightPort A0
#define lightConstant 250 //Depending on the surroundings!
#define updateRate 500

const String ID = "4"; //SHould be assigned by reader initialisation can be preserved here

double timestamp = 0;

//WiFi informationer
const char* ssid     = "Mark's iPhone";
const char* password = "LOLl0l69";


SoftwareSerial RFID(2, 3); // RX and TX

int data;
String result = "";
String lastResult;
char c;

HTTPClient http;


void setup()
{
  RFID.begin(9600);    // start serial to RFID reader
  Serial.begin(9600);  // start serial to PC 
  delay(10);
  wifiConnect();

}
 
void loop()
{
  wifiCheck(); //Maintain wifi connection
  yield(); //Let the ESPcore handle background tasks
  
  checkUpdateAndPrint();
 
  
  if(somethingOnTop()) post("klods_id=" + ID + "&stacked_rfid=" + lastResult.substring(1, 13), "/3dserver/arduino/stack");
  else post("klods_id=" + ID, "/3dserver/arduino/unstack");

}

void checkUpdateAndPrint() {

  while(RFID.available() > 0) {
    c=RFID.read();
    result += c;
    
    if(c == 0x03) break;
  }
  
  if(c == 0x03) {
    Serial.println("reading: " + result);
    //we have a reading
    if(result != "") lastResult = result; //Defencive string reading can be strange
    result = ""; 
    c = 0; // Reset so we dont end up here egain  
  }

}

boolean somethingOnTop() {
    int value = analogRead(lightPort);
   // Serial.println(value);
    return value < lightConstant;
}




void wifiConnect() {
  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void wifiCheck()
{
  if (WiFi.status() != WL_CONNECTED) //if wifi is connected: do nothing.
  {
    int tickCount = 0;
    Serial.println("Wifi dropped. Retry in 60 seconds.");
    delay(60000); //wait 60 seconds
    Serial.println("Connecting");
    WiFi.begin(ssid, password); //reconnect

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println(".");
      tickCount++;
      if (tickCount > 100) //after awaiting reconnection for 50 seconds
      {
        Serial.println("Wifi fail...");
        //This is where you end up if the connection was lost and unrecoverable
        while (1); //Endless loop...
      }
    }

    //This is the place to do something in case the connection was lost but fixed.

  }
}



void post(String payload, String url) {

    if( millis() - timestamp > updateRate) {
      timestamp = millis();
  
      Serial.println("[HTTP] begin...");
      // configure traged server and url
      http.begin("graungaard.com", 80, url); //HTTP
      
      Serial.println("payload: " + payload);
      Serial.println("url: " + url);
  
      int response = http.POST(payload);
  
      Serial.println(response);

    }
}


