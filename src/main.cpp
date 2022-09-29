#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <USBKeyboard.h>


#define VL53L0X_I2C_ADDR1 0x30
#define VL53L0X_I2C_ADDR2 0x31

const int PICO_SDA1 = 26; // GP26
const int PICO_SCL1 = 27; // GP27

const int VL51_XSHUT1 = 16; // GP16
const int VL51_XSHUT2 = 21; // GP21
const int VL51_INT1 = 17; // GP17
const int VL51_INT2 = 22; // GP22

USBKeyboard key;

arduino::MbedI2C Wire1(PICO_SDA1, PICO_SCL1);// = TwoWire(PICO_SDA1, PICO_SCL1); // SDA, SCL

Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();

void setIDs( void )
{
  Serial.println("Adafruit VL53L0X test.");
  
  pinMode(VL51_XSHUT1,OUTPUT);
  pinMode(VL51_XSHUT2,OUTPUT);

  Serial.println(F("Shutdown pins inited..."));
  
  digitalWrite(VL51_XSHUT1, LOW);
  digitalWrite(VL51_XSHUT2, LOW);
  
  delay(10);

  digitalWrite(VL51_XSHUT1, HIGH);
  digitalWrite(VL51_XSHUT2, HIGH);
  
  digitalWrite(VL51_XSHUT1, HIGH);
  digitalWrite(VL51_XSHUT2, LOW);

  delay(10);

  Serial.println(F("Both in reset mode...(pins are low)"));

  if (!lox1.begin(VL53L0X_I2C_ADDR1, false, &Wire1)) 
  {
    while(1)
      Serial.println(F("Failed to boot VL53L0X #1"));
  }
  
  lox1.startRangeContinuous();
  
  Serial.println(F("VL53L0X #1 init done\n\n"));
  
  delay(10);
 
  digitalWrite(VL51_XSHUT2, HIGH);
 
  delay(10);

  if (!lox2.begin(VL53L0X_I2C_ADDR2, false, &Wire1)) 
  {
    while(1)
      Serial.println(F("Failed to boot VL53L0X #2"));
  }
  lox2.startRangeContinuous();
  Serial.println(F("VL53L0X #2 init done\n\n"));
}

void setup() {
  Serial.begin(115200);

  delay(1000);
  Serial.println("Start");

  key.printf("start"); // Print start from "keyboard" ;)

  Serial.println("KBD done");

  setIDs();

  Serial.println("Set ID done");
}

void loop() 
{
  uint16_t distance_s1=0, distance_s2=0;
  static bool sent = false;

  if (lox1.isRangeComplete())
  {
    distance_s1 = lox1.readRange();
    Serial.print("Distance1 in mm: ");
    Serial.println(distance_s1);
  }
  else if (lox2.isRangeComplete())
  {
    distance_s2 = lox2.readRange();
    Serial.print("Distance2 in mm: ");
    Serial.println(distance_s1);
  }
/*
  if()
    key.key_code(RIGHT_ARROW);
  else if()
    key.key_code(LEFT_ARROW); 
*/
}