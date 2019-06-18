#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

String Line_token = ""; // token line notify
#define btnpin1 5
#define ledpin1 4

int ledstate = HIGH;
int buttonstate;
int lastbuttonstate = LOW;

const char* ssid = ""; // access point name
const char* password = ""; // password wifi access point
const char fingerprint[] PROGMEM = ""; // finger print

void setup() {
  // put your setup code here, to run once:
   pinMode(ledpin1, OUTPUT);
   pinMode(btnpin1, INPUT);

   WiFi.begin(ssid, password);
   WiFi.mode (WIFI_STA);
   Serial.begin(115200);
   while (WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print("."); 
   }

   Serial.println("");
   Serial.println("WiFi Connected");
   Serial.println("IP Address : ");
   Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  int reading = digitalRead(btnpin1);

  if(reading != buttonstate){
     buttonstate = reading;
     //Serial.println(buttonstate);

     if(buttonstate == LOW){
        ledstate = !ledstate;
        Line_Notify(Line_token, "push button"); 
        delay(100);
     }
  }
  digitalWrite(ledpin1, ledstate);
}

void Line_Notify(String Line_token, String msg){
   //Serial.println(Line_token);
   String m = String("message=") + msg;
   //Serial.println(m);

   WiFiClientSecure client;
   client.setFingerprint(fingerprint);
   client.setTimeout(15000);
   
   if(!client.connect("notify-api.line.me", 443)){
      Serial.println("connection failed");
      return;
   }

   String request = "";
   request += "POST /api/notify HTTP/1.1 \r\n";
   request += "Host: notify-api.line.me\r\n";
   request += "Content-Type: application/x-www-form-urlencoded\r\n";
   request += "Authorization: Bearer " + Line_token + "\r\n";
   request += "Content-Length: " + String(m.length()) + "\r\n";
   request += "\r\n";
   request += m;
   
   client.print(request);

   unsigned long timeout = millis();
   while (client.available() == 0){
      if(millis() - timeout > 5000){
         Serial.println("Client Timeout!");
         client.stop();
         return;
      }
   }

   while (client.connected()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
   }

   Serial.println();
   Serial.println("closing connection");
   
}   
