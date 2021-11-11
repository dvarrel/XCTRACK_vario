 /*
VARIO XCTRACK
arduino nano
sensor ms5611

damien varrel 07/2020 

Based on Arduino Vario by Benjamin PERRIN 2017 / Vari'Up
Based on Arduino Vario by Jaros, 2012 and vario DFelix 2013

https://github.com/LK8000/LK8000/blob/master/Docs/LK8EX1.txt
$LK8EX1,pressure,altitude,vario,temperature,battery,*checksum

test sentence PRS, shorter but no checksum
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
// #define BLUETOOTH_MODE // uncomment this line for bluetooth mode
#define USE_LK8000 //comment this line to use PRS sentence
// normally you do not need USB_MODE if you use BLUETOOTH_MODE
/*
 these 2 sentences are available for xctrack : 
 LK8000 sentence : 32 chars to send
 $LK8EX1,pressure,altitude,vario,temperature,battery,*checksum

 PRS sentence : 10 chars to send
 PRS pressure in pascal in hexadecimal format
 
 bluetooth in 9600 bauds -> 1.25ms for a char
 LK8000 : 1.25*32=40ms
 PRS : 1.25*10=12.5ms
 USB in 115200 bauds -> 0.08ms for a char
 LK8000 : 0.08*32=2.5ms
 PRS : 0.08*10=0.8ms

 PRS is faster but LK8000 has checksum !
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
  //Serial.print(Pressure);
  //Serial.print(",");
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
    +String(Pressure)+String(",0,9999,")
    +String(Temp)+String(",999,");
    
    // Calculating checksum for data string
    uint16_t checksum = 0, bi;
    for (uint8_t ai = 0; ai < str_out.length(); ai++)
    {
      bi = (uint8_t)str_out[ai];
      checksum ^= bi;
    }
    str_out = "$"+str_out+"*"+String(checksum, HEX);

    #else // use PRS sentence
    String str_out = String("PRS ")+ String(Pressure, HEX);
    #endif
    
    #ifdef USB_MODE      
    Serial.println(str_out);
    #endif
    #ifdef BLUETOOTH_MODE
    BTserial.println(str_out);
    #endif
  }
}
