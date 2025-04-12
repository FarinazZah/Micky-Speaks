#include <WiFi.h>
 
const char* ssid     = "farinaz";
const char* password = "yesyoumayuseit1";

void setup() {
    Serial.begin(115200);
    delay(100);
    
    Serial.println("Hello, ESP32!");
    Serial.println("Starting WiFi connection...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.println("Connecting...");
        delay(1000);
    }

    Serial.println("Connected!");
    Serial.print("IP is ");
    Serial.println(WiFi.localIP());
}

void loop() {
}
