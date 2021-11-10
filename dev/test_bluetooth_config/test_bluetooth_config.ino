 /*
Programme de reception ou config Bluetooth

AT : check the connection
AT+NAME: Change name. No space between name and command.
AT+BAUD: change baud rate, x is baud rate code, no space between command and code.
AT+PIN: change pin, xxxx is the pin, again, no space.
AT+VERSION

HC03 HC05 LF+CR
HC04 HC06 no LF+CR
BLE CC2541 without crystal send with LF + CR
HM-10 (CC2541 with crystal) no LF+CR
AT+HELP list of command
http://www.jnhuamao.cn/bluetooth.asp?id=1
*/

#define RX 11 //RX is MOSI (TX module bluetooth)
#define TX 12 //TX is MISO (RX module bluetooth)
 
#include <SoftwareSerial.h>
SoftwareSerial BTserial(RX,TX);
byte serialA; // variable de reception de donnée via RX

void setup()
{
  Serial.begin(9600);
  while (!Serial) ; // Needed for native USB port only
  Serial.println("Hello !");
  Serial.println("Enter AT commands:");
  // set the data rate for the SoftwareSerial port
  BTserial.begin(9600);
}

void loop() {
  if (BTserial.available())
  {
    serialA = BTserial.read();
    Serial.write(serialA);
  }
  if (Serial.available())
    BTserial.write(Serial.read());

}
