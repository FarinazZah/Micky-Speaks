#include <WiFi.h>
#include <HTTPClient.h>

// WiFi Network credentials
const char* ssid = "Kamran's iPhone";
const char* password = "kamhotspot786";

// Phone Number & API Key from CallMeBot
const String phoneNumber = "+14707868007";
const String apiKey = "6497409";          

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
unsigned long redTriggeredTime = 0;   // Timestamp for red LED
unsigned long yellowTriggeredTime = 0;  // Timestamp for yellow LED
bool redComboAttempted = false;       // Track if combo is checked for Red

// Timeout configurations
const unsigned long comboTimeout = 5000; // 5 seconds for combo
const unsigned long ledTimeout = 3000;   // 3 seconds for LEDs

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
    unsigned long currentMillis = millis();

    // Red LED logic
    bool irValueRed = (digitalRead(irSensorPinRed) == LOW);
    if (irValueRed && !prevRedState) {
        digitalWrite(redLEDPin, HIGH);
        redTriggeredTime = currentMillis;  // Store red trigger time
        redComboAttempted = false;         // Reset combo attempt flag
        prevRedState = true;
    } else if (!irValueRed && prevRedState) {
        if (currentMillis - redTriggeredTime > ledTimeout) {  // Red LED timeout
            digitalWrite(redLEDPin, LOW);
            prevRedState = false;
        }
    }

    // Check if combo timeout has passed without a yellow trigger
    if (redTriggeredTime != 0 && !redComboAttempted &&
        currentMillis - redTriggeredTime > comboTimeout) {
        String message = "Micky is hungry!";
        sendWhatsAppMessage(message);  // Send WhatsApp message
        sendSpeechCommand(message);   // Send speech command
        redComboAttempted = true;     // Mark combo check as attempted
        redTriggeredTime = 0;         // Reset red trigger time
    }

    // Yellow LED logic with timeout
    bool irValueYellow = (digitalRead(irSensorPinYellow) == LOW);
    if (irValueYellow && !prevYellowState) {
        digitalWrite(yellowLEDPin, HIGH);
        yellowTriggeredTime = currentMillis;  // Store yellow trigger time
        prevYellowState = true;

        // Check if red was triggered within the last 5 seconds
        if (currentMillis - redTriggeredTime <= comboTimeout) {
            String message = "Micky wants a parent! <3";
            sendWhatsAppMessage(message);  // Send WhatsApp message
            sendSpeechCommand(message);   // Send speech command
            redTriggeredTime = 0;         // Reset red trigger time to avoid multiple combos
            redComboAttempted = true;
        } else {
            String message = "Micky is thirsty!";
            sendWhatsAppMessage(message);  // Send WhatsApp message
            sendSpeechCommand(message);   // Send speech command
        }
    } else if (!irValueYellow && prevYellowState) {
        if (currentMillis - yellowTriggeredTime > ledTimeout) {  // Yellow LED timeout
            digitalWrite(yellowLEDPin, LOW);
            prevYellowState = false;
        }
    }

    delay(100);
}

void connectToWiFi() {
    Serial.println("Connecting to WiFi...");
    WiFi.mode(WIFI_STA); // Set to Station Mode
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nConnected to WiFi.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
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

