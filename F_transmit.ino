#include <SPI.h>
#include <RH_ASK.h>  // RadioHead ASK module for RF communication
#include "I2Cdev.h"
#include "MPU6050.h"

// Create an instance of the MPU6050 sensor
MPU6050 mpu;

// Create an instance of the RH_ASK object for RF communication
RH_ASK driver;

int16_t ax, ay, az, gx, gy, gz;
int flex;
bool blinkState;

void setup() {
  // Initialize Serial communication for debugging
  Serial.begin(9600);
  Serial.println("Initializing MPU...");
  pinMode(A0, INPUT);
  delay(1000);
  // Initialize RF Transmitter (RadioHead)
  if (!driver.init()) {
    Serial.println("RadioHead initialization failed!");
    while (true); // Stop if RadioHead initialization fails
  }

  Serial.println("RadioHead init successful");
  driver.send((uint8_t *)"RH init", strlen("RH init"));
  delay(2000);

  // Start I2C communication
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin(); 
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif
  delay(1000);

  // Initialize MPU6050 sensor
  mpu.initialize();
  Serial.println("MPU6050 conn");
  driver.send((uint8_t *)"MPU conn", strlen("MPU conn"));
  delay(2000);

  if (!mpu.testConnection()) {
    Serial.println("MPU failed!");
    driver.send((uint8_t *)"MPU failed!", strlen("MPU failed!"));
    while (true); // Stay here if MPU6050 connection fails
  } else {
    Serial.println("MPU6050 connection successful");
    driver.send((uint8_t *)"MPU OK", strlen("MPU OK"));
  }
 delay(2000);
  // Set up the sensor offsets (optional)
  mpu.setXAccelOffset(0);
  mpu.setYAccelOffset(0);
  mpu.setZAccelOffset(0);
  mpu.setXGyroOffset(0);
  mpu.setYGyroOffset(0);
  mpu.setZGyroOffset(0);

  pinMode(LED_BUILTIN, OUTPUT);  // Set up LED for activity indication
}

void loop() {
  // Continuously read the accelerometer and gyroscope data
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  //  read flex
  flex = analogRead(A0);

  // Debugging: Print sensor data to Serial Monitor
  Serial.print("\n");
   Serial.print("\tFlex: "); Serial.println(flex);
  Serial.print("AX: "); Serial.print(ax);
  Serial.print("\tAY: "); Serial.print(ay);
  Serial.print("\tAZ: "); Serial.print(az);
  Serial.print("\tGX: "); Serial.print(gx);
  Serial.print("\tGY: "); Serial.print(gy);
  Serial.print("\tGZ: "); Serial.println(gz);

  // Default command
  char command[15] = "no need";

  // Determine the command based on sensor readings
  if (ax > 15000) {
    strcpy(command, "Fall Detected"); // Need Water
  } else if (ax < 6000) {
    strcpy(command, "feeling sick"); // Feeling Sick
  } else if (ay > -6000) {
    strcpy(command, "Washroom"); // Washroom
  } else if (ay < -16000) {
    strcpy(command, "need food"); // Need Food
  }else if (flex > 600) {
    strcpy(command, "Emergency"); // Need Food
  }

  // Send the message over RF
  if (driver.send((uint8_t *)command, strlen(command))) {
    Serial.println(String(command));
  } else {
    Serial.println("Failed to send message");
  }

  // Blink LED to indicate activity
  blinkState = !blinkState;
  digitalWrite(LED_BUILTIN, blinkState);

  // Delay to make sure the loop doesn't run too fast
  delay(1000); // Delay for 1000ms (1 second)
}
