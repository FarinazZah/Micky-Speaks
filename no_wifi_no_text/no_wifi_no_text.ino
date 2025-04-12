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
bool redComboAttempted = false;       // Track if combo is checked for Red

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
        digitalWrite(redLEDPin, LOW);
        prevRedState = false;
    }

    // Check if combo timeout has passed without a yellow trigger
    if (redTriggeredTime != 0 && !redComboAttempted &&
        currentMillis - redTriggeredTime > comboTimeout) {
        sendSpeechCommand("Red LED has been triggered!"); // Send speech after 5 seconds
        redComboAttempted = true;  // Mark combo check as attempted
        redTriggeredTime = 0;      // Reset red trigger time
    }

    // Yellow LED logic with combo check
    bool irValueYellow = (digitalRead(irSensorPinYellow) == LOW);
    if (irValueYellow && !prevYellowState) {
        digitalWrite(yellowLEDPin, HIGH);

        // Check if red was triggered within the last 5 seconds
        if (currentMillis - redTriggeredTime <= comboTimeout) {
            String message = "Red Yellow Combo!";
            sendSpeechCommand(message); // Send speech for red-yellow combo
            redTriggeredTime = 0;  // Reset red trigger time to avoid multiple combos
            redComboAttempted = true;  // Mark combo check as attempted
        } else {
            String message = "Yellow LED has been triggered!";
            sendSpeechCommand(message); // Send speech for yellow LED trigger
        }

        prevYellowState = true;
    } else if (!irValueYellow && prevYellowState) {
        digitalWrite(yellowLEDPin, LOW);
        prevYellowState = false;
    }

    delay(100);
}

void sendSpeechCommand(String text) {
    if (Serial1) {  // Check if Serial1 is ready
        Serial1.print(text + "\n");  // Send text with newline to SYN6658
        Serial.println("Text sent to SYN6658: " + text);  // Debug output
    } else {
        Serial.println("SYN6658 module is not ready to receive data.");
    }
}

