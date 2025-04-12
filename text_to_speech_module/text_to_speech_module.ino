void setup() {
    Serial.begin(9600);
    delay(1000);
    Serial.println("Initializing SYN6658 Speech Synthesis...");

    Serial1.begin(9600);  // Ensure the SYN6658 baud rate matches
    delay(500);

    // Test with a built-in sound command
    Serial1.print("sound113[d]\n");
    delay(1000);

    // Test with simplified text
    sendSpeechCommand("你好，欢迎使用语音合成模块");  // Chinese test
    delay(1000);
    sendSpeechCommand("Hello SYN6658");  // English test
}

void loop() {
    delay(5000);  // Wait between tests
}

void sendSpeechCommand(String text) {
    Serial.println("sendSpeechCommand called.");

    if (Serial1) {  // Check if Serial1 is ready
        Serial1.print("[m3][f1][t5][v10]" + text + "[d]\n");
        Serial.println("Text sent to SYN6658: " + text);
    } else {
        Serial.println("SYN6658 module is not ready to receive data.");
    }
}
