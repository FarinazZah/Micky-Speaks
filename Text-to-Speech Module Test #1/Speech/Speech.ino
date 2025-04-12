void setup() {
  // Initialize Serial for communication with SYN6658
  Serial.begin(4800);
  delay(1000); // Allow time for initialization
  
  Serial.println("SYN6658 Speech Module Test");
  
  // Example text-to-speech command
  sendSpeechCommand("Hello");
}

void loop() {
  // Add additional command logic if needed
}

void sendSpeechCommand(String text) {
  if (Serial) {  // Check if Serial is ready
    Serial.print(text);  // Send text command to SYN6658
    Serial.println("Text Sent: " + text);  // Debug output
  } else {
    Serial.println("SYN6658 module is not ready to receive data.");
  }
}
