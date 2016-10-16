//引用函式庫
#include <SoftwareSerial.h>

//設定RX/TX腳位
int RX_PIN = 2;
int TX_PIN = 3;

SoftwareSerial bluetooth(RX_PIN, TX_PIN);

// Send and Receive buffers
String bluetooth_rx_buffer = "";
String bluetooth_tx_buffer = "";

// Delimiter used to separate messages
char DELIMITER = '\n';

void setup() {
  // Start USB Communication
  Serial.begin(9600);
  
  // Uncomment this only once, when configuring a new BlueSMiRF
  //set_bluetooth_baudrate();
  
  // Start bluetooth communication
  bluetooth.begin(9600);
  pinMode(13, OUTPUT); 
  Serial.println("Initialized.");
}

void set_bluetooth_baudrate() {
  
  // Open BlueSMiRF at default baudrate
  bluetooth.begin(115200);
  
  // Enter command mode
  bluetooth.print("$$$");  
  
  // Short delay, wait for the Mate to send back CMD
  delay(100);
  
  // Change the baudrate to 19200 bps
  // 115200 is usually too fast for SoftwareSerial to read data properly
  bluetooth.println("SU,19");
}

void gotMessage(String message) {
  
  
  Serial.println("[RECV] " + message);
  if( message == "On")
    digitalWrite(13,HIGH);
  if( message == "Off")
    digitalWrite(13,LOW);
  // Do something!
}

void parseReadBuffer() {
  
  // Find the first delimiter in the buffer
  int inx = bluetooth_rx_buffer.indexOf(DELIMITER);
  // If there is none, exit
  if (inx == -1) return;
  // Get the complete message, minus the delimiter
  String s = bluetooth_rx_buffer.substring(0, inx);
  // Remove the message from the buffer
  bluetooth_rx_buffer = bluetooth_rx_buffer.substring(inx + 1);
  
  // Process the message
  gotMessage(s);
  
  // Look for more complete messages
  parseReadBuffer();
}

void parseWriteBuffer() {
  
  // Find the first delimiter in the buffer
  int inx = bluetooth_tx_buffer.indexOf(DELIMITER);
  // If there is none, exit
  if (inx == -1) return;
  
  // Get the complete message, including the delimiter
  String message = bluetooth_tx_buffer.substring(0, inx + 1);
  
  // Remove the message from the buffer
  bluetooth_tx_buffer = bluetooth_tx_buffer.substring(inx + 1);
  
  // Send off the message
  bluetooth.print(message);
  Serial.print("[SENT] " + message);
  if( message == "On\n")
    digitalWrite(13,HIGH);
  if( message == "Off\n")
    digitalWrite(13,LOW);
  
  // Look for more
  parseWriteBuffer();
}

String incomingByte ;  
void loop() {
  //Serial傳送訊息
  if (Serial.available() > 0) {
    
    bluetooth_tx_buffer = Serial.readString() + "\n";
    //印出傳送訊息
    parseWriteBuffer();
  }
  
  //藍牙接收訊息
  if(bluetooth.available()) {
    while(bluetooth.available()) {
      bluetooth_rx_buffer += (char)bluetooth.read();
    }
    //印出讀取訊息
    parseReadBuffer();
  }
}
