#include <Arduino.h>
#include "Adafruit_TinyUSB.h"
#include <Wire.h>
#include "Adafruit_VL53L0X.h"

#define VL53L0X_I2C_ADDR1 0x30
#define VL53L0X_I2C_ADDR2 0x31

const int PICO_SDA1 = 26; // GP26
const int PICO_SCL1 = 27; // GP27

const int VL51_XSHUT1 = 16; // GP16
const int VL51_XSHUT2 = 21; // GP21
const int VL51_INT1 = 17; // GP17
const int VL51_INT2 = 22; // GP22

// HID report descriptor using TinyUSB's template
// Single Report (no ID) descriptor
uint8_t const desc_hid_report[] =
{
  TUD_HID_REPORT_DESC_KEYBOARD()
};

// USB HID object. For ESP32 these values cannot be changed after this declaration
// desc report, desc len, protocol, interval, use out endpoint
Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_KEYBOARD, 2, false);

//TwoWire Wire(PICO_SDA1, PICO_SCL1);// = TwoWire(PICO_SDA1, PICO_SCL1); // SDA, SCL
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

  Serial.println(F("VL53L0X #1 init done\n\n"));
  
  delay(10);
 
  digitalWrite(VL51_XSHUT2, HIGH);
 
  delay(10);

  if (!lox2.begin(VL53L0X_I2C_ADDR2, false, &Wire1)) 
  {
    while(1)
      Serial.println(F("Failed to boot VL53L0X #2"));
  }
  Serial.println(F("VL53L0X #2 init done\n\n"));
}

void setup() {
  Serial.begin(115200);

  Wire1.begin(); //(PICO_SDA1, PICO_SCL1);

  #if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
    // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
    TinyUSB_Device_Init(0);
  #endif

  usb_hid.begin();
  
  //setSDA(PICO_SDA1);
  //setSCL(PICO_SCL1);
  Wire.begin();

  setIDs();
}

void loop() {
  // used to avoid send multiple consecutive zero report for keyboard
  static bool keyPressedPreviously = false;

  uint8_t count=0;
  uint8_t keycode[6] = { 0 };
  
  delay(2);


  if ( TinyUSBDevice.suspended() && count )
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    TinyUSBDevice.remoteWakeup();
  }
  // Need logic here to select LEFT/RIGHT
  // TBD
  /*
  
  keycode[count++] = HID_KEY_ARROW_RIGHT;

  keycode[count++] = HID_KEY_ARROW_LEFT;
  
  */
  // skip if hid is not ready e.g still transferring previous report
  if ( !usb_hid.ready() ) return;

  if ( count )
  {
    // Send report if there is key pressed
    uint8_t const report_id = 0;
    uint8_t const modifier = 0;

    keyPressedPreviously = true;
    usb_hid.keyboardReport(report_id, modifier, keycode);
  } 
  else
  {
    // Send All-zero report to indicate there is no keys pressed
    // Most of the time, it is, though we don't need to send zero report
    // every loop(), only a key is pressed in previous loop()
    if ( keyPressedPreviously )
    {
      keyPressedPreviously = false;
      usb_hid.keyboardRelease(0);
    }
  }

  if (lox1.isRangeComplete())
  {
    Serial.print("Distance1 in mm: ");
    Serial.println(lox1.readRange());
  }

  else if (lox2.isRangeComplete())
  {
    Serial.print("Distance2 in mm: ");
    Serial.println(lox2.readRange());
  }
  else
    Serial.print("No data");

}