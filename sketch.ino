/*

  Automated Defense system simulation using ESP32
  Author: OBR
  link: https://wokwi.com/projects/426251415580474369 
*/


#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// define pins
#define SERVO_PIN 14
#define LED_PIN 18
#define TRIG_PIN 32
#define ECHO_PIN 34

// variables
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo;
bool stopFlag = false;
int detectionZone= 150;  // Object detected within 150 cm
int currentAngle = 0;
int direction = 1;  // 1 = forward, -1 = backward


void setup() {
// initialise serial communication
  Serial.begin(115200);

// initiale the servo motor
  servo.attach(SERVO_PIN);
  servo.write(0);

//set the lcd
  Wire.begin(21, 22);
  lcd.begin(16, 2);
  lcd.backlight() ;
  lcd.setCursor(0, 0);
  lcd_message("system ready");

  //configure pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  delay(1000);
 lcd_message("scanning...");
}


void loop() {
  if (!stopFlag) {  // scanning scenario
    float distance= moveServo();
    if (distance > 0 && distance <= detectionZone) { 
      stopFlag = true;  }
    } else { // an object is detected
      objectDetected();
    }
}




void lcd_message (char* msg){
  lcd.clear();
  lcd.print(msg);
  delay(200);
}


void objectDetected() {
    digitalWrite(LED_PIN, HIGH);  // Turn on LED when object detected
    lcd_message("THREAT DETECTED!");
    delay(1000);
    Serial.printf("\n--- PAUSED at Angle: %d degrees ---\n", currentAngle);
    simulateFiring() ;
      Serial.println("--- Resuming... ---");
      digitalWrite(LED_PIN, LOW);
      lcd_message("scanning...");
      stopFlag = false;  // Reset flag
  } 


float moveServo(){
    servo.write(currentAngle);
    delay(10);
    float distance = calculateDistance();
    distance=random(130,1500); // delete this line in case of real hardware, not simulator
    Serial.printf("Angle: %d degrees, Distance: %.2f cm\n", currentAngle, distance);
    currentAngle += direction;
    if (currentAngle >= 180 || currentAngle <= 0) direction *= -1;  // Reverse direction
  return distance;
}

float calculateDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); // send wave
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
// capture the wave sent back 
  long duration = pulseIn(ECHO_PIN, HIGH); 
// find distance in cm 
  return duration / 58.2;  
}


void simulateFiring() {
  Serial.println("  >> FIRE! >>");
  lcd_message("STARTING FIRE..");
  for(int i=0; i<2; i++) {
    Serial.println("    *PEW*");
    delay(500);
  }
}
