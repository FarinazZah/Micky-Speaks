#include <WiFi.h>
#include <HTTPClient.h>

// WiFi Network credentials
const char* ssid = "GTvisitor";
const char* password = "";

// Phone Number & API Keyfrom CallMeBot
const String phoneNumber = "+14787146278";
const String apiKey = "4896755";          

// Pin assignments

// Yellow LED & IR Sensor
const int irSensorPinYellow = D8;  
const int yellowLEDPin = D7;       

// Red LED & IR Sensor
const int irSensorPinRed = A5;     
const int redLEDPin = A6;          

// State Information - Used for Red/Yellow Combo
bool prevYellowState = false;
bool prevRedState = false;

// Red is triggered from Red/Yellow Combo first
unsigned long redTriggeredTime = 0;   // Timestamp

// Red/Yellow Combo has to be within 5 seconds
const unsigned long comboTimeout = 5000;

void setup() {
    pinMode(irSensorPinYellow, INPUT);
    pinMode(yellowLEDPin, OUTPUT);
    digitalWrite(yellowLEDPin, LOW);

    pinMode(irSensorPinRed, INPUT);
    pinMode(redLEDPin, OUTPUT);
    digitalWrite(redLEDPin, LOW);

    Serial.begin(115200);
    Serial1.begin(9600);  // Text-to-Speech Module Communication

    connectToWiFi();
}

void loop() {
    // Red LED logic
    bool irValueRed = (digitalRead(irSensorPinRed) == LOW);
    if (irValueRed && !prevRedState) {
        digitalWrite(redLEDPin, HIGH);
        redTriggeredTime = millis();  // Store red trigger time
        prevRedState = true;
    } else if (!irValueRed && prevRedState) {
        digitalWrite(redLEDPin, LOW);
        prevRedState = false;
    }

    // Yellow LED logic with combo check
    bool irValueYellow = (digitalRead(irSensorPinYellow) == LOW);
    if (irValueYellow && !prevYellowState) {
        digitalWrite(yellowLEDPin, HIGH);

        // Check if red was triggered within the last 5 seconds
        if (millis() - redTriggeredTime <= comboTimeout) {
            String message = "Red Yellow Combo!";
            sendWhatsAppMessage(message);
            sendSpeechCommand(message);
            redTriggeredTime = 0;  // Reset red trigger time to avoid multiple combos
        } else {
            String message = "Yellow LED has been triggered!";
            sendWhatsAppMessage(message);
            sendSpeechCommand(message);
        }

        prevYellowState = true;
    } else if (!irValueYellow && prevYellowState) {
        digitalWrite(yellowLEDPin, LOW);
        prevYellowState = false;
    }

    delay(100);
}

void connectToWiFi() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi.");
}

void sendWhatsAppMessage(String message) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber +
                     "&text=" + urlEncode(message) + "&apikey=" + apiKey;

        http.begin(url);
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            Serial.println("Message sent: " + message);
        } else {
            Serial.println("Error sending message: " + String(httpResponseCode));
        }
        http.end();
    } else {
        Serial.println("WiFi not connected.");
    }
}

void sendSpeechCommand(String text) {
    if (Serial1) {  // Check if Serial1 is ready
        Serial1.print(text + "\n");  // Send text with newline to SYN6658
        Serial.println("Text sent to SYN6658: " + text);  // Debug output
    } else {
        Serial.println("SYN6658 module is not ready to receive data.");
    }
}

String urlEncode(const String &msg) {
    String encodedMsg = "";
    char c;
    for (int i = 0; i < msg.length(); i++) {
        c = msg[i];
        if (isalnum(c)) {
            encodedMsg += c;
        } else {
            encodedMsg += '%' + String((int)c, HEX);
        }
    }
    return encodedMsg;
}

