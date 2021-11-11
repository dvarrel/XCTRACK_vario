#include <SoftwareSerial.h>

#define RX 11 // not used (TX module bluetooth)
#define TX 12 // MISO on ISCP (RX module bluetooth)
SoftwareSerial BTserial(RX, TX); // RX not connected 
// Connect the HC-05 TX to Arduino pin 11 RX (MOSI). 
// Connect the HC-05 RX to Arduino pin 12 TX (MISO).
#define USB_SPEED 115200 //define serial transmision speed
#define BLUETOOTH_SPEED 9600 //bluetooth speed (9600 by default)
#define FREQUENCY 10 // freq output in Hz
//#define USB_MODE // uncomment for usb mode
#define BLUETOOTH_MODE // uncomment for bluetooth mode

void setup()                // setup() function to setup all necessary parameters before we go to endless loop() function
{
  #ifdef USB_MODE
  Serial.begin(USB_SPEED);       // set up arduino serial port
  #endif
  #ifdef BLUETOOTH_MODE
  BTserial.begin(BLUETOOTH_SPEED);     // start communication with the HC-05 using 38400
  #endif
}

uint32_t Pressure=91000;
uint32_t get_time = millis();

void loop(void) {
  if (millis() >= (get_time + (1000/FREQUENCY)))
  {
    get_time = millis();
    Pressure -= 1;
    if (Pressure<90000) Pressure = 91000;
    String str_out = String("LK8EX1,")
    +String(Pressure)
    +String(",0,9999,0,999,");
    
    uint16_t checksum = 0, bi;
    for (uint8_t ai = 0; ai < str_out.length(); ai++)
    {
      bi = (uint8_t)str_out[ai];
      checksum ^= bi;
    }
    str_out = "$"+str_out+"*"+String(checksum, HEX);
    
    #ifdef USB_MODE      
    Serial.println(str_out);
    #endif
    #ifdef BLUETOOTH_MODE
    BTserial.println(str_out);
    #endif

  }
}
