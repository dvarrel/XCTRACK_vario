 /*
VARIO XCTRACK
arduino nano
sensor ms5611

damien varrel 07/2020 

Based on Arduino Vario by Benjamin PERRIN 2017 / Vari'Up
Based on Arduino Vario by Jaros, 2012 and vario DFelix 2013

https://github.com/LK8000/LK8000/blob/master/Docs/LK8EX1.txt
$LK8EX1,pressure,altitude,vario,temperature,battery,*checksum

test sentence PRS, more shorter
*/

//MS5XXX library by Roman Schmitz
#include <MS5611.h>
#include <avr/wdt.h>
#include <SoftwareSerial.h>

/////////////////////////////////////////
MS5611 sensor(&Wire);
#define RX 11 // not used (TX module bluetooth)
#define TX 12 // MISO on ISCP (RX module bluetooth)
SoftwareSerial BTserial(RX, TX); // RX not connected 
#define USB_SPEED 115200 //serial transmision speed
#define BLUETOOTH_SPEED 9600 //bluetooth speed (9600 by default)
#define FREQUENCY 10 // freq output in Hz
#define USB_MODE // usb by default
// #define BLUETOOTH_MODE // uncomment this line and comment the line above for bluetooth mode
#define USE_LK8000
/*
 * sentences to choice : 
 * $LK8EX1,pressure,altitude,vario,temperature,battery,*checksum
 * PRS pressure in pascal in hexadecimal format
 * LK8000 : 32 chars to send PRS : 10 chars to send
 * bluetooth in 9600bauds -> 1.25ms for a char
*/

const char compile_date[] = "XCTRACK VARIO " __DATE__;
const char mess_check[] = "checking MS5611 sensor...";
const char mess_error[] = "Error connecting MS5611...";
const char mess_reset[] = "Reset in 1s...";

void setup() {
  wdt_disable();
  #ifdef USB_MODE
  Serial.begin(USB_SPEED);
  Serial.println(compile_date);
  Serial.println(mess_check);
  Serial.println(mess_check);
  #endif
  #ifdef BLUETOOTH_MODE
  BTserial.begin(BLUETOOTH_SPEED);     // start communication with the HC-05 using 38400
  BTserial.println(compile_date);
  BTserial.println(mess_check);
  #endif
  wdt_enable(WDTO_1S); //enable the watchdog 1s without reset
  if (sensor.connect()>0) {  
    #ifdef USB_MODE
    Serial.println(mess_error);
    Serial.println(mess_reset);
    #endif
    #ifdef BLUETOOTH_MODE
    BTserial.println(mess_error);
    BTserial.println(mess_reset);
    #endif    
    delay(1200); //for watchdog timeout
  }
  sensor.ReadProm(); //takes about 3ms
}

uint32_t get_time = millis();
uint32_t sum = 0;
uint8_t n = 0;


void loop(void) {
  wdt_reset();
  sensor.Readout(); // with OSR4096 takes about 10ms
  uint32_t Pressure = sensor.GetPres();
  sum += Pressure;
  n += 1;
  if (millis() >= (get_time + (1000/FREQUENCY))) //every 100 milli-seconds send NMEA output over serial port
  {
    get_time = millis();
    Pressure = sum / n ;
    sum=0; n=0;
    uint8_t Temp = sensor.GetTemp()/100;

    //$LK8EX1,pressure,altitude,vario,temperature,battery,*checksum
    //$LK8EX1,pressure,99999,9999,temp,999,*checksum

    #ifdef USE_LK8000
    
    String str_out = String("LK8EX1,")
    +String(Pressure)
    +String(",0,9999,")
    +String(Temp)
    +String(",999,");
  
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
    BTserial.print("$");       
    BTserial.print(str_out);
    BTserial.print("*");
    BTserial.println(checksum_end, HEX);
    #endif

    #else // use PRS sentence
    String str_out = String("PRS ")
    + String(Pressure, HEX);
    #ifdef USB_MODE      
    Serial.println(str_out);
    #endif
    #ifdef BLUETOOTH_MODE
    BTserial.println(str_out);
    #endif
    
    #endif
  }
}
