#include <LiquidCrystal.h>

// LCD pin setup: RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int gasSensor = A0;   // MQ-2 sensor analog pin
int buzzer = 8;       // Buzzer connected to D8
int gasValue = 0;
int threshold = 3;  // Adjust as needed

void setup() {
  lcd.begin(16, 2);
  pinMode(buzzer, OUTPUT);
  lcd.print("Gas Detector");
  delay(2000);
  lcd.clear();
}

void loop() {
  gasValue = analogRead(gasSensor);

  lcd.setCursor(0, 0);
  lcd.print("Gas Value: ");
  lcd.print(gasValue);
  
  lcd.setCursor(0, 1);
  if (gasValue > threshold) {
    lcd.print("!! GAS DETECTED !!");
    digitalWrite(buzzer, HIGH);  // Turn on buzzer
  } else {
    lcd.print("Safe              ");
    digitalWrite(buzzer, LOW);   // Turn off buzzer
  }

  delay(1000);
}
