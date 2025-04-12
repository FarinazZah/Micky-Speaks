// Define the pin where the LED is connected
const int ledPin = A6;  // Pin 13 is commonly used, but you can change this to any other digital pin

void setup() {
  // Initialize the LED pin as an output
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Turn the LED on
  digitalWrite(ledPin, HIGH);  // Pin 13 goes HIGH, powering the LED
  
  delay(1000);  // (Optional) Wait for 1 second before repeating any actions
}

