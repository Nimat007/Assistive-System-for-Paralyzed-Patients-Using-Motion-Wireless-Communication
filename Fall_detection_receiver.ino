#include <SPI.h>
#include <RH_ASK.h>       // Include the RadioHead ASK module
#include <Wire.h>         // Include the Wire library for I2C communication
#include <LiquidCrystal_I2C.h>  // Include the LiquidCrystal_I2C library

// Create an instance of the RH_ASK object
RH_ASK driver;

// Initialize the LCD with I2C address (default is 0x27 or 0x3F, adjust as needed)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address, 16 columns, 2 rows

void setup() {
  Serial.begin(9600);
  pinMode(A0, OUTPUT);

  // Initialize the LCD
  lcd.init();          // Initialize the LCD
  lcd.backlight();     // Turn on the LCD backlight
  lcd.clear(); 
  lcd.print("Waiting...");  // Display a waiting message
  digitalWrite(A0, HIGH);
  delay(1000);
  digitalWrite(A0, LOW);
  
delay(1000);
  // Initialize the RadioHead driver
  if (!driver.init()) {
    Serial.println("RadioHead initialization failed!");
    lcd.clear();
    lcd.print("RF Init Failed");
    delay(1000);
    while (1);  // Stop if initialization fails
  }

  Serial.println("RadioHead receiver initialized");
  lcd.clear();
  lcd.print("RF Ready...");
  delay(1000);
}

void loop() {
  uint8_t buf[15];  // Buffer to store the received message
  uint8_t len = sizeof(buf);

  // Check if a message is available
  if (driver.recv(buf, &len)) {
    buf[len] = '\0';  // Null-terminate the string
    String message = String((char*)buf);  // Convert buffer to a String for processing

    // Print the message on Serial Monitor
    Serial.print("Received message: ");
    Serial.println(message);

    // Display the message on the LCD
    lcd.clear();            // Clear the LCD screen
    lcd.setCursor(0, 0);    // Set the cursor to the first line
    lcd.print("Status:"); // Display a label
    lcd.setCursor(0, 1);    // Move the cursor to the second line
    lcd.print(message);     // Display the message

    if(message == "Emergency" || message == "Fall Detected") {
        digitalWrite(A0, HIGH); // Activate external alert (e.g., buzzer)
        delay(1000);
        digitalWrite(A0, LOW);
    }
  }
}
