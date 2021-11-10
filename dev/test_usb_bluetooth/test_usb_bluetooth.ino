#include <SoftwareSerial.h>

#define RX 11 // not used (TX module bluetooth)
#define TX 12 // MISO on ISCP (RX module bluetooth)
SoftwareSerial BTserial(RX, TX); // RX not connected 
// Connect the HC-05 TX to Arduino pin 11 RX (MOSI). 
// Connect the HC-05 RX to Arduino pin 12 TX (MISO).
#define USB_SPEED 115200 //define serial transmision speed
#define BLUETOOTH_SPEED 9600 //bluetooth speed (9600 by default)
#define FREQUENCY 10 // freq output in Hz
#define USB_MODE // uncomment for usb mode
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
    
    unsigned int checksum_end, ai, bi;                                               // Calculating checksum for data string
    for (checksum_end = 0, ai = 0; ai < str_out.length(); ai++)
    {
      bi = (unsigned char)str_out[ai];
      checksum_end ^= bi;
    }
    #ifdef USB_MODE
    Serial.print("$");       
    Serial.print(str_out);
    Serial.print("*");
    Serial.println(checksum_end, HEX);
    #endif
    #ifdef BLUETOOTH_MODE
    BTserial.print("$");                     //print first sign of NMEA protocol
    BTserial.print(str_out);                 //print data string
    BTserial.print("*");                     //end of protocol string
    BTserial.println(checksum_end, HEX);
    #endif

  }
}
