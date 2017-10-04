/*
 Name:		RN41.cpp
 Created:	6/7/2017 9:46:16 PM
 Author:	Chris
 Editor:	http://www.visualmicro.com
*/

#include "RN41_42.h"

RN41_42::RN41_42(HardwareSerial& _serial) : serial(_serial) {
  _commandMode = false;
  _configChar[0] = '$';
  _configChar[1] = '\0';
  setupIO();
}

RN41_42::RN41_42(HardwareSerial& _serial, char configChar) : serial(_serial) {
  _commandMode = false;
  _configChar[0] = configChar;
  _configChar[1] = '\0';
  setupIO();
}

void RN41_42::begin(unsigned long baudrate)
{
  _baud = baudrate;
  serial.begin(_baud);
}

void RN41_42::begin()
{
  _baud = 115200;
  serial.begin(_baud);
}

void RN41_42::end()
{
    serial.end();
}

int RN41_42::available()
{
    return serial.available();
}

char RN41_42::read()
{
    return serial.read();
}

void RN41_42::flush()
{
    return serial.flush();
}

//Enter Command Mode
bool RN41_42::enterCommandMode()
{
  if (_commandMode) { return true; }
  for (int i = 0; i < 3; i++) { serial.print(_configChar);}
  if (strncmp_P(getString(), PSTR("CMD\r\n"), 6) == 0) { _commandMode = true; return true; }
  return false;
}

//Exit Command Mode
bool RN41_42::exitCommandMode()
{
  if (!_commandMode) { return true; }
  serial.println(F("---"));
  if (strncmp_P(getString(), PSTR("END\r\n"), 6) == 0) { _commandMode = false; return true; }
  return false;
}

void RN41_42::sendMessage(char message[32])
{
  serial.print(message);
}

void RN41_42::sendChar(char c)
{
    serial.write(c);
}

char *RN41_42::recieveMessage()
{
  return getString();
}

//Dipswitch/GPIO Functions

#ifdef RN41_42_GPIO4
void RN41_42::factoryReset()
{
  ::digitalWrite(RN41_42_GPIO4, HIGH);
  reset();
  delay(500);
  ::digitalWrite(RN41_42_GPIO4, LOW);
  delay(1000);
  ::digitalWrite(RN41_42_GPIO4, HIGH);
  ::digitalWrite(RN41_42_GPIO4, LOW);
  delay(1000);
  ::digitalWrite(RN41_42_GPIO4, HIGH);
  ::digitalWrite(RN41_42_GPIO4, LOW);
  reset();
}
#endif // RN41_42_GPIO4

#ifdef RN41_42_GPIO3
void RN41_42::autoDiscoveryPairing(bool en)
{
  ::digitalWrite(RN41_42_GPIO3, en);
}
#endif // RN41_42_GPIO3

#ifdef RN41_42_GPIO6
void RN41_42::autoConnect(bool en)
{
  ::digitalWrite(RN41_42_GPIO6, en);
}
#endif // RN41_42_GPIO6

#ifdef RN41_42_GPIO7
void RN41_42::setBaudRate9600(bool en)
{
  ::digitalWrite(RN41_42_GPIO7, en);
}
#endif // RN41_42_GPIO7

//SET COMMANDS

//Set 7-bit Data Mode
//S7,<flag>
bool RN41_42::setS7(bool en)
{
  char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("S7,%d"), en);
  serial.println(buffer);
  return isAOK();
}

//Set Authentication Mode
//SA,<value>
//Available Modes:
//0 - Open Mode
//1 - SSP Keyboard I/O Mode
//2 - SSP "Just Works" Mode
//4 - Pin Code
//Default Mode: 1
bool RN41_42::setAuthenticationMode(uint8_t authMode)
{
  char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("SA,%d"), authMode);
  serial.println(buffer);
  return isAOK();
}

//Set Break
//SB,<value>
bool RN41_42::setBreak(uint8_t breakVal)
{
  char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("SB,%d"), breakVal);
  serial.println(buffer);
  return isAOK();
}

//Set Service Class
//SC,<value>
bool RN41_42::setServiceClass(uint16_t hex)
{
  char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("SC,%04X"), hex);
  serial.println(buffer);
  return isAOK();
}


//Set Device Class
//SD,<value>
bool RN41_42::setDeviceClass(uint16_t hex)
{
  char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("SD,%04X"), hex);
  serial.println(buffer);
  return isAOK();
}

//Set UUID
//SE,<value>
bool RN41_42::setUUID(char hex[32])
{
  char buffer[35];
  snprintf_P(buffer, sizeof(buffer), PSTR("SE,%s"), hex);
  serial.println(buffer);
  return isAOK();
}

//Restore Factory Defaults
//SF,1
bool RN41_42::restoreFactoryDefaults()
{
  serial.println(PSTR("SF,1"));
  return isAOK();
}

//Set HID Flag Register
//SH,<value>
bool RN41_42::setHIDRegister(uint16_t hex)
{
  char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("SH,%04X"), hex);
  serial.println(buffer);
  return isAOK();
}

//Set Inquiry Scan Window
//SI,<value>
bool RN41_42::setInquiryScanWindow(uint16_t hex)
{
  char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("SI,%04X"), hex);
  serial.println(buffer);
  return isAOK();
}

//Set Page Scan Window
//SJ,<value>
bool RN41_42::setPageScanWindow(uint16_t hex)
{
  char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("SJ,%04X"), hex);
  serial.println(buffer);
  return isAOK();
}

//Set UART Parity
//SL,<value>
bool RN41_42::setUARTParity(char parity)
{
  char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("SL,%s"), parity);
  serial.println(buffer);
  return isAOK();
}

//Sets Device Mode
//SM,<value>
//Available Modes:
//0 - Slave Mode
//1 - Master Mode
//2 - Trigger Mode
//3 - Auto-Connect Master Mode
//4 - Auto-Connect DTR Mode
//5 - Auto-Connect Any Mode
//6 - Pairing Mode
//Default Mode: 4
bool RN41_42::setMode(unsigned int mode)
{
  char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("SM,%d"), mode);
  serial.println(buffer);
  return isAOK();
}

//Sets Device Name
//SN,<string>
//Maximum 20 characters
bool RN41_42::setDeviceName(char name[21])
{
  char buffer[24];
  snprintf_P(buffer, sizeof(buffer), PSTR("SN,%s"), name);
  serial.println(buffer);
  return isAOK();
}

//Sets Extended Status String
//SO,<string>
//Maximum 8 characters
bool RN41_42::setExtendedStatusString(char name[9])
{
  char buffer[12];
  snprintf_P(buffer, sizeof(buffer), PSTR("SO,%s"), name);
  serial.println(buffer);
  return isAOK();
}

//Sets the Security Pin
//SP,<string>
bool RN41_42::setPinCode(char pin[5])
{
  char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("SP,%s"), pin);
  serial.println(buffer);
  return isAOK();
}

//Sets the Mask
//SQ,<string>
bool RN41_42::setMask(uint8_t mask)
{
  char buffer[7];
  snprintf_P(buffer, sizeof(buffer), PSTR("SQ,%d"), mask);
  serial.println(buffer);
  return isAOK();
}

//Set Remote Address
//SR,<hex value>
bool RN41_42::setRemoteAddress(char address[13])
{
  char buffer[16];
  snprintf_P(buffer, sizeof(buffer), PSTR("SR,%s"), address);
  serial.println(buffer);
  return isAOK();
}

//Erase Remote Address
//SR,Z
bool RN41_42::eraseRemoteAddress()
{
  serial.println(F("SR,Z"));
  return isAOK();
}

//Set Remote Address as Last Observed Address
//SR,I
bool RN41_42::setRemoteAddressLastObserved()
{
  serial.println(F("SR,I"));
  return isAOK();
}

//Set Service Name
//SS,<string>
bool RN41_42::setServiceName(char name[21])
{
  char buffer[24];
  snprintf_P(buffer, sizeof(buffer), PSTR("SS,%s"), name);
  serial.println(buffer);
  return isAOK();
}

//Set Remote Config Timer
//ST,<string>
bool RN41_42::setConfigTimer(uint8_t value)
{
  char buffer[7];
  snprintf_P(buffer, sizeof(buffer), PSTR("ST,%d"), value);
  serial.println(buffer);
  return isAOK();
}

//Set UART Baud
//SU,<value>
bool RN41_42::setUARTBaud(uint8_t baud)
{
  char buffer[6];
    snprintf_P(buffer, sizeof(buffer), PSTR("SU,%d"), baud);
    serial.println(buffer);
    return isAOK();
}

//Set Sniff Mode
//SW,<value>
bool RN41_42::setSniff(uint16_t hex)
{
  char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("SW,%04X"), hex);
  serial.println(buffer);
  return isAOK();
}

//Set Bonding
//SX,<flag>
bool RN41_42::setBonding(bool en)
{
  char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("SX,%d"), en);
  serial.println(buffer);
  return isAOK();
}

//Set Transmit Power
//SY,<hex value>
bool RN41_42::setTransmitPower(uint16_t hex)
{
  char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("SY,%04X"), hex);
  serial.println(buffer);
  return isAOK();
}

//Set Non-Standard Baud
//SZ,<value>
bool RN41_42::setNonStandardBaud(uint8_t multi)
{
  char buffer[6];
  snprintf_P(buffer, sizeof(buffer), PSTR("SZ,%d"), multi);
  serial.println(buffer);
  return isAOK();
}

//Set Profile
//S~,<value>
bool RN41_42::setProfile(uint8_t value) {
  char buffer[5];
  enterCommandMode();
  snprintf_P(buffer, sizeof(buffer), PSTR("S~,%d"), value);
  serial.println(buffer);
  return isAOK();
}

//Set Serialized Friendly Name
//S-,<string>
bool RN41_42::setSerializedFriendlyName(char name[16])
{
  char buffer[19];
  snprintf_P(buffer, sizeof(buffer), PSTR("S-,%s"), name);
  serial.println(buffer);
  return isAOK();
}

//Set Roll Switch
//S?,<flag>
bool RN41_42::setRoleSwitch(bool en)
{
  char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("S?,%d"), en);
  serial.println(buffer);
  return isAOK();
}

//Set Configuration Character
//S$,<char>
bool RN41_42::setConfigChar(char c)
{
  char buffer[5];
  snprintf(buffer, sizeof(buffer), PSTR("S$,%s"), c);
  serial.println(buffer);
  return isAOK();
}

//Set Low-Power Connect Mode
//S|,<value>
bool RN41_42::setLPConnectMode(uint8_t hex)
{
  char buffer[8];
  snprintf(buffer, sizeof(buffer), PSTR("S|,%04X"), hex);
  serial.println(buffer);
  return isAOK();
}

//Returns the basic settings
//D
char *RN41_42::getBasicSettings()
{
  serial.println(F("D"));
  return getString();
}

//Returns the extended settings
//E
char *RN41_42::getExtendedSettings()
{
  serial.println(F("E"));
  return getString();
}

//Returns the Bluetooth Address
//GB
char *RN41_42::getBluetoothAddress()
{
  serial.println(F("GB"));
  return getString();
}

//Returns the Connected Device Bluetooth Address
//GF
char *RN41_42::getConnectedRemoteAddress()
{
  serial.println(F("GF"));
  return getString();
}

//Gets Connection Status
//GK
//0,0,0 = Not Connected
//1,0,0 = Connected
//Uses GPIO over serial commands if available
bool RN41_42::getConnectionStatus()
{
#ifdef RN41_42_GPIO2
  return digitalRead(RN41_42_GPIO2);
#else
  serial.println(F("GK"));
  return strncmp_P(getString(), PSTR("1\r\n"), 8) == 0 ? true : false;
#endif // RN41_42_CONN_PIN
}

//Returns the Stored Remote Bluetooth Address
//GR
char  *RN41_42::getStoredRemoteAddress()
{
  serial.println(F("GR"));
  return getString();
}

//Return the GPIO Bitmask
//G&
char *RN41_42::getGPIOStatus()
{
  serial.println(F("G&"));
  return getString();
}

char * RN41_42::displayDipwitchValues()
{
  serial.print(F("&"));
  return getString();
}

bool RN41_42::connectToStoredAddress()
{
  serial.println(F("C"));
  return true;// strncmp_P(getString(), PSTR("TRYING\r\n"), 9) == 0 ? true : false;
}

bool RN41_42::connectToAddress(char address[13])
{
  char buffer[15];
  snprintf_P(buffer, sizeof(buffer), PSTR("C,%S"), address);
  serial.println(buffer);
  return strncmp_P(getString(), connected, sizeof(connected)) == 0 ? true : false;
}

bool RN41_42::connectToAddressFast(char address[13])
{
  char buffer[16];
  snprintf_P(buffer, sizeof(buffer), PSTR("CF,%S"), address);
  serial.println(buffer);
  return strncmp_P(getString(), connected, sizeof(connected)) == 0 ? true : false;
}

bool RN41_42::connectToLastFoundAddressFast()
{
  serial.println(F("CFI"));
  return strncmp_P(getString(), connected, sizeof(connected)) == 0 ? true : false;
}

bool RN41_42::connectToStoredRemoteAddressFast()
{
  serial.println(F("CFR"));
  return strncmp_P(getString(), connected, sizeof(connected)) == 0 ? true : false;
}

bool RN41_42::connectToAddressTimed(char address[13], uint8_t time)
{
  char buffer[19];
  snprintf_P(buffer, sizeof(buffer), PSTR("CT,%S,%d"), address, time);
  serial.println(buffer);
  return strncmp_P(getString(), connected, sizeof(connected)) == 0 ? true : false;
}

bool RN41_42::fastDataMode()
{
  serial.println(F("F,1"));
  _commandMode = false;
  return true;
}

char * RN41_42::help()
{
  serial.println(F("H"));
  return getString();
}

char * RN41_42::performInquiryScan(uint8_t time)
{
  char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("I,%d"), time);
  serial.println(buffer);
  return getString();
}

char * RN41_42::performInquiryScan(uint8_t time, uint32_t cod)
{
  char buffer[11];
  snprintf_P(buffer, sizeof(buffer), PSTR("I,%d,%06X"), time, cod);
  serial.println(buffer);
  return getString();
}

char * RN41_42::performInquiryScanNN(uint8_t time)
{
  char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("IN%d"), time);
  serial.println(buffer);
  return getString();
}

char * RN41_42::performInquiryScanNN(uint8_t time, uint32_t cod)
{
  char buffer[12];
  snprintf_P(buffer, sizeof(buffer), PSTR("IN%d,%06X"), time, cod);
  serial.println(buffer);
  Serial.println(buffer);
  return getString();
}

char * RN41_42::scanRSSI()
{
  serial.println(F("IQ"));
  return getString();
}

char * RN41_42::performRovingInquiryScan(uint8_t time)
{
  char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("IS%d"), time);
  serial.println(buffer);
  return getString();
}

char * RN41_42::performCableReplaceInquiryScan(uint8_t time)
{
  char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("IR%d"), time);
  serial.println(buffer);
  return getString();
}

bool RN41_42::hidePIN()
{
  serial.println(F("J"));
  return isAOK();
}

bool RN41_42::killConnection()
{
  serial.println(F("K,"));
  return strncmp_P(getString(), PSTR("KILL\r\n"), 7) == 0 ? true : false;
}

char * RN41_42::linkQuality()
{
  serial.println(F("L"));
  return getString();
}

char * RN41_42::remoteModemSignalStatus()
{
  serial.println(F("M"));
  return getString();
}

char * RN41_42::otherSettings()
{
  serial.println(F("O"));
  return getString();
}

void RN41_42::passMessage(char mes[32])
{
  char buffer[34];
  strncpy_P(buffer, PSTR("P,"), 3);
  strncat(buffer, mes, sizeof(buffer));
  serial.println(buffer);
}

bool RN41_42::quietMode()
{
  serial.println(F("Q"));
  return strncmp_P(getString(), quiet, sizeof(quiet)) == 0 ? true : false;
}

bool RN41_42::quietMode(uint8_t mode)
{
  char buffer[4];
  snprintf_P(buffer, sizeof(buffer), PSTR("Q,%d"), mode);
  serial.println(buffer);
  return strncmp_P(getString(), quiet, sizeof(quiet)) == 0 ? true : false;
}

uint8_t RN41_42::quietStatus()
{
  serial.println(F("Q,?"));
  return atoi(getString());
}

//Resets The Device
//Also Exits Command Mode
//Uses GPIO over serial commands if available
//Device Must Be Reset After A Config Change To Take Effect
bool RN41_42::reset()
{
#ifdef RN41_42_RESET
  ::digitalWrite(RN41_42_RESET, LOW);
  delay(50);
  ::digitalWrite(RN41_42_RESET, HIGH);
  return true;
#else
  if (!_commandMode) { return false; }
  serial.println(F("R,1"));
  serial.flush();
  if (strcmp_P(getString(), PSTR("Reboot!\r\n")) == 0) { _commandMode = false; return true; }
  return false;

#endif // RN41_42_RESET_PIN
}

bool RN41_42::passThrough(bool en)
{
  char buffer[4];
  snprintf_P(buffer, sizeof(buffer), PSTR("T,%d"), en);
  serial.println(buffer);
  return true;
}

bool RN41_42::uartChangeTemp(char baud[5], char parity)
{
  char buffer[9];
  snprintf_P(buffer, sizeof(buffer), PSTR("U,%S,%S"), baud, parity);
  serial.println(buffer);
  return isAOK();
}

//Get the firmware version of the RN42/RN42
//V
char *RN41_42::getFirmwareVersion()
{
  serial.println(F("V"));
  return getString();
}

bool RN41_42::enableDiscoveryConnection()
{
  serial.println(F("W"));
  return true;
}

//Puts the RN41/RN42 to sleep.
//Z
void RN41_42::sleep()
{
  serial.println(F("Z"));
  _commandMode = false;
}

//GPIO Commands

//Sets the pin mode for GPIO 0 through 7.
//S@
bool RN41_42::pinMode(uint8_t pin, uint8_t dir)
{
  bitSet(gpioSetDir, pin);
  bitWrite(gpioDir, pin, dir);
  char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("S@,%02X%02X"), gpioSetDir, gpioDir);
  serial.println(buffer);
  return isAOK();
}

//Writes to GPIO 0 through 11.
//S& or S*
bool RN41_42::digitalWrite(uint8_t pin, uint8_t val)
{
  char buffer[8];
  if (pin <= 7)
  {
    bitSet(gpioSetVal, pin);
    bitWrite(gpioVal, pin, val);
    snprintf_P(buffer, sizeof(buffer), PSTR("S&,%02X%02X"), gpioSetVal, gpioVal);
  }
  else
  {
    bitSet(gpio811, (pin - 4));
    bitWrite(gpio811, (pin - 8), val);
    snprintf_P(buffer, sizeof(buffer), PSTR("S*,%02X%02X"), (gpio811 >> 4), (gpio811 & 0xF));
  }
  serial.println(buffer);
  return isAOK();
}

//Set the pin mode on power up for GPIO 0 through 7.
//S%
bool RN41_42::pinModePowerUp(uint8_t pin, uint8_t dir)
{
  bitSet(gpioSetDirPowerUp, pin);
  bitWrite(gpioDirPowerUp, pin, dir);
  char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("S%,%02X%02X"), gpioSetDirPowerUp, gpioDirPowerUp);
  serial.println(buffer);
  return isAOK();
}

//Set the pin value on power up for GPIO 0 through 7.
//S^
bool RN41_42::digitalWritePowerUp(uint8_t pin, uint8_t val)
{
  bitSet(gpioSetValPowerUp, pin);
  bitWrite(gpioValPowerUp, pin, val);
  char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("S^,%02X%02X"), gpioSetValPowerUp, gpioValPowerUp);
  serial.println(buffer);
  return isAOK();
}

//Private Functions

char *RN41_42::getString()
{
  delay(50);
  int i = 0;
  while (serial.available() > 0)
  {
    recvBuf[i] = serial.read();
    i++;
  }
  recvBuf[i] = { '\0' };
  Serial.print(recvBuf);
  return recvBuf;
}

void RN41_42::setupIO()
{
#ifdef RN41_42_RESET
  ::pinMode(RN41_42_RESET, OUTPUT);
  ::digitalWrite(RN41_42_RESET, HIGH);
#endif // RN41_42_RESET_PIN

#ifdef RN41_42_GPIO2
  ::pinMode(RN41_42_GPIO2, INPUT);
  //::digitalWrite(RN41_42_GPIO2, HIGH);
#endif // RN41_42_CONN_PIN

#ifdef RN41_42_GPIO4
  ::pinMode(RN41_42_GPIO4, OUTPUT);
  ::digitalWrite(RN41_42_GPIO4, LOW);
#endif // RN41_42_GPIO4

#ifdef RN41_42_GPIO3
  ::pinMode(RN41_42_GPIO3, OUTPUT);
  ::digitalWrite(RN41_42_GPIO3, LOW);
#endif // RN41_42_GPIO3

#ifdef RN41_42_GPIO6
  ::pinMode(RN41_42_GPIO6, OUTPUT);
  ::digitalWrite(RN41_42_GPIO6, LOW);
#endif // RN41_42_GPIO6

#ifdef RN41_42_GPIO7
  ::pinMode(RN41_42_GPIO7, OUTPUT);
  ::digitalWrite(RN41_42_GPIO7, LOW);
#endif // RN41_42_GPIO
}

bool RN41_42::isAOK()
{
  return strncmp_P(getString(), PSTR("AOK\r\n"), 6) == 0 ? true : false;
}

#ifdef RN41_42_RESET
void RN41_42::wakeup()
{
    reset();
    delay(500);
}
#endif