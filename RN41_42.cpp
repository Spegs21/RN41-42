/*
 Name:		RN41.cpp
 Created:	6/7/2017 9:46:16 PM
 Author:	Chris
 Editor:	http://www.visualmicro.com
*/

#include "RN41_42.h"

uint8_t gpioSetDir = 0U;
uint8_t gpioDir = 0U;
uint8_t gpioSetVal = 0U;
uint8_t gpioVal = 0U;
uint8_t gpio811 = 0U;
uint8_t gpioSetDirPowerUp = 0U;
uint8_t gpioDirPowerUp = 0U;
uint8_t gpioSetValPowerUp = 0U;
uint8_t gpioValPowerUp = 0U;

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
  serial.begin(baudrate);
}

void RN41_42::begin()
{
  serial.begin(115200);
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
//Enter Command Mode
bool RN41_42::enterCommandMode()
{
  if (_commandMode) { return true; }
  for (int i = 0; i < 3; i++) { serial.print(_configChar); }
  serial.flush();
  if (strncmp_P(getString(), PSTR("CMD\r\n"), 6) == 0) { _commandMode = true; return true; }
  return false;
}

//Exit Command Mode
bool RN41_42::exitCommandMode()
{
  if (!_commandMode) { return true; }
  serial.println(F("---"));
  serial.flush();
  if (strncmp_P(getString(), PSTR("END\r\n"), 6) == 0) { _commandMode = false; return true; }
  return false;
}

int RN41_42::sendMessage(char message[32])
{
  if (_commandMode) { return -1; }
  serial.println(message);
  serial.flush();
  return sizeof(message);
}

int RN41_42::sendChar(char c)
{
    if (_commandMode) { return -1; }
    serial.print(c);
    serial.flush();
    return sizeof(c);
}

char *RN41_42::recieveMessage()
{
  if (_commandMode) { return "ERR"; }
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
  if (!_commandMode) { return false; }
  static char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("S7,%d"), en);
  serial.println(buffer);
  serial.flush();
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
  if (!(authMode >= 0 && authMode <= 4 && authMode != 3) || !_commandMode) { return false; }
  static char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("SA,%d"), authMode);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Set Break
//SB,<value>
bool RN41_42::setBreak(uint8_t breakVal)
{
  if (!(breakVal >= 1 && breakVal <= 6) || !_commandMode) { return false; }
  static char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("SB,%d"), breakVal);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Set Service Class
//SC,<value>
bool RN41_42::setServiceClass(uint16_t hex)
{
  if (!_commandMode) { return false; }
  static char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("SC,%04X"), hex);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}


//Set Device Class
//SD,<value>
bool RN41_42::setDeviceClass(uint16_t hex)
{
  if (!_commandMode) { return false; }
  static char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("SD,%04X"), hex);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Set UUID
//SE,<value>
bool RN41_42::setUUID(char hex[32])
{
  if (!_commandMode) { return false; }
  static char buffer[35];
  snprintf_P(buffer, sizeof(buffer), PSTR("SE,%s"), hex);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Restore Factory Defaults
//SF,1
bool RN41_42::restoreFactoryDefaults()
{
  if (!_commandMode) { return false; }
  serial.println(PSTR("SF,1"));
  serial.flush();
  return isAOK();
}

//Set HID Flag Register
//SH,<value>
bool RN41_42::setHIDRegister(uint16_t hex)
{
  if (!_commandMode) { return false; }
  static char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("SH,%04X"), hex);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Set Inquiry Scan Window
//SI,<value>
bool RN41_42::setInquiryScanWindow(uint16_t hex)
{
  if (!_commandMode) { return false; }
  static char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("SI,%04X"), hex);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Set Page Scan Window
//SJ,<value>
bool RN41_42::setPageScanWindow(uint16_t hex)
{
  if (!_commandMode) { return false; }
  static char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("SJ,%04X"), hex);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Set UART Parity
//SL,<value>
bool RN41_42::setUARTParity(char parity)
{
  if (!(parity == 'E' || parity == 'O' || parity == 'N') || !_commandMode) { return false; }
  static char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("SL,%s"), parity);
  serial.println(buffer);
  serial.flush();
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
  if (!(mode <= 6) || !_commandMode) { return false; }
  static char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("SM,%d"), mode);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Sets Device Name
//SN,<string>
//Maximum 20 characters
bool RN41_42::setDeviceName(char name[21])
{
  if (!_commandMode) { return false; }
  static char buffer[24];
  snprintf_P(buffer, sizeof(buffer), PSTR("SN,%s"), name);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Sets Extended Status String
//SO,<string>
//Maximum 8 characters
bool RN41_42::setExtendedStatusString(char name[9])
{
  if (!_commandMode) { return false; }
  static char buffer[12];
  snprintf_P(buffer, sizeof(buffer), PSTR("SO,%s"), name);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Sets the Security Pin
//SP,<string>
bool RN41_42::setPinCode(char pin[5])
{
  if (!_commandMode) { return false; }
  static char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("SP,%s"), pin);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Sets the Mask
//SQ,<string>
bool RN41_42::setMask(uint8_t mask)
{
  if (!_commandMode) { return false; }
  static char buffer[7];
  switch (mask)
  {
  case 0:
  case 4:
  case 8:
  case 16:
  case 128:
  case 256:
    snprintf_P(buffer, sizeof(buffer), PSTR("SQ,%d"), mask);
    serial.println(buffer);
    serial.flush();
    return isAOK();
  default:
    return false;
  }
}

//Set Remote Address
//SR,<hex value>
bool RN41_42::setRemoteAddress(char address[13])
{
  if (!_commandMode) { return false; }
  static char buffer[16];
  snprintf_P(buffer, sizeof(buffer), PSTR("SR,%s"), address);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Erase Remote Address
//SR,Z
bool RN41_42::eraseRemoteAddress()
{
  if (!_commandMode) { return false; }
  serial.println(PSTR("SR,Z"));
  serial.flush();
  return isAOK();
}

//Set Remote Address as Last Observed Address
//SR,I
bool RN41_42::setRemoteAddressLastObserved()
{
  if (!_commandMode) { return false; }
  serial.println(PSTR("SR,I"));
  serial.flush();
  return isAOK();
}

//Set Service Name
//SS,<string>
bool RN41_42::setServiceName(char name[21])
{
  if (!_commandMode) { return false; }
  static char buffer[24];
  snprintf_P(buffer, sizeof(buffer), PSTR("SS,%s"), name);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Set Remote Config Timer
//ST,<string>
bool RN41_42::setConfigTimer(uint8_t value)
{
  if (!_commandMode) { return false; }
  static char buffer[7];
  snprintf_P(buffer, sizeof(buffer), PSTR("ST,%d"), value);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Set UART Baud
//SU,<value>
bool RN41_42::setUARTBaud(uint8_t baud)
{
  if (!_commandMode) { return false; }
  static char buffer[6];
  switch (baud) {
  case 12:
  case 24:
  case 48:
  case 96:
  case 19:
  case 28:
  case 38:
  case 57:
  case 11:
  case 23:
  case 46:
  case 92:
    snprintf_P(buffer, sizeof(buffer), PSTR("SU,%d"), baud);
    serial.println(buffer);
    serial.flush();
    return isAOK();
  default:
    return false;
  }
}

//Set Sniff Mode
//SW,<value>
bool RN41_42::setSniff(uint16_t hex)
{
  if (!_commandMode) { return false; }
  static char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("SW,%04X"), hex);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Set Bonding
//SX,<flag>
bool RN41_42::setBonding(bool en)
{
  if (!_commandMode) { return false; }
  static char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("SX,%d"), en);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Set Transmit Power
//SY,<hex value>
bool RN41_42::setTransmitPower(uint16_t hex)
{
  if (!_commandMode) { return false; }
  static char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("SY,%04X"), hex);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Set Non-Standard Baud
//SZ,<value>
bool RN41_42::setNonStandardBaud(uint8_t multi)
{
  if (!_commandMode) { return false; }
  static char buffer[6];
  snprintf_P(buffer, sizeof(buffer), PSTR("SZ,%d"), multi);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Set Profile
//S~,<value>
bool RN41_42::setProfile(uint8_t value) {
  if (!(value >= 0 && value <= 6) || !_commandMode) { return false; }
  static char buffer[5];
  enterCommandMode();
  snprintf_P(buffer, sizeof(buffer), PSTR("S~,%d"), value);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Set Serialized Friendly Name
//S-,<string>
bool RN41_42::setSerializedFriendlyName(char name[16])
{
  if (!_commandMode) { return false; }
  static char buffer[19];
  snprintf_P(buffer, sizeof(buffer), PSTR("S-,%s"), name);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Set Roll Switch
//S?,<flag>
bool RN41_42::setRoleSwitch(bool en)
{
  if (!_commandMode) { return false; }
  static char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("S?,%d"), en);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Set Configuration Character
//S$,<char>
bool RN41_42::setConfigChar(char c)
{
  if (!_commandMode) { return false; }
  static char buffer[5];
  snprintf(buffer, sizeof(buffer), PSTR("S$,%s"), c);
  serial.println(buffer);
  serial.flush();
  return isAOK();;
}

//Set Low-Power Connect Mode
//S|,<value>
bool RN41_42::setLPConnectMode(uint8_t hex)
{
  if (!_commandMode) { return false; }
  static char buffer[8];
  snprintf(buffer, sizeof(buffer), PSTR("S|,%04X"), hex);
  serial.println(buffer);
  serial.flush();
  return isAOK();
}

//Returns the basic settings
//D
char *RN41_42::getBasicSettings()
{
  if (!_commandMode) { return false; }
  serial.println(F("D"));
  serial.flush();
  return getString();
}

//Returns the extended settings
//E
char *RN41_42::getExtendedSettings()
{
  if (!_commandMode) { return false; }
  serial.println(F("E"));
  serial.flush();
  return getString();
}

//Returns the Bluetooth Address
//GB
char *RN41_42::getBluetoothAddress()
{
  if (!_commandMode) { return false; }
  serial.println(F("GB"));
  serial.flush();
  return getString();
}

//Returns the Connected Device Bluetooth Address
//GF
char *RN41_42::getConnectedRemoteAddress()
{
  if (!_commandMode) { return false; }
  serial.println(F("GF"));
  serial.flush();
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
  if (!_commandMode) { return false; }
  serial.println(F("GK"));
  serial.flush();
  return strncmp_P(getString(), PSTR("1\r\n"), 8) == 0 ? true : false;
#endif // RN41_42_CONN_PIN
}

//Returns the Stored Remote Bluetooth Address
//GR
char  *RN41_42::getStoredRemoteAddress()
{
  if (!_commandMode) { return false; }
  serial.println(F("GR"));
  serial.flush();
  return getString();
}

//Return the GPIO Bitmask
//G&
char *RN41_42::getGPIOStatus()
{
  if (!_commandMode) { return false; }
  serial.println(F("G&"));
  serial.flush();
  return getString();
}

char * RN41_42::displayDipwitchValues()
{
  if (!_commandMode) { return false; }
  serial.print(F("&"));
  serial.flush();
  return getString();
}

bool RN41_42::connectToStoredAddress()
{
  if (!_commandMode) { return false; }
  serial.println(F("C"));
  serial.flush();
  return  true;
}

bool RN41_42::connectToAddress(char address[13])
{
  if (!_commandMode) { return false; }
  static char buffer[15];
  snprintf_P(buffer, sizeof(buffer), PSTR("C,%S"), address);
  serial.println(buffer);
  serial.flush();
  return true;
}

bool RN41_42::connectToAddressFast(char address[13])
{
  if (!_commandMode) { return false; }
  static char buffer[16];
  snprintf_P(buffer, sizeof(buffer), PSTR("CF,%S"), address);
  serial.println(buffer);
  serial.flush();
  return true;
}

bool RN41_42::connectToLastFoundAddressFast()
{
  if (!_commandMode) { return false; }
  serial.println(F("CFI"));
  serial.flush();
  return true;
}

bool RN41_42::connectToStoredRemoteAddressFast()
{
  if (!_commandMode) { return false; }
  serial.println(F("CFR"));
  serial.flush();
  return true;
}

bool RN41_42::connectToAddressTimed(char address[13], uint8_t time)
{
  if (!_commandMode) { return false; }
  static char buffer[19];
  snprintf_P(buffer, sizeof(buffer), PSTR("CT,%S,$d"), address, time);
  serial.println(buffer);
  serial.flush();
  return true;
}

bool RN41_42::fastDataMode()
{
  if (!_commandMode) { return false; }
  serial.println(F("F,1"));
  _commandMode = false;
  serial.flush();
  return true;
}

char * RN41_42::help()
{
  if (!_commandMode) { return false; }
  serial.println(F("H"));
  serial.flush();
  return getString();
}

char * RN41_42::performInquiryScan(uint8_t time)
{
  if (!(time <= 48) || !_commandMode) { return false; }
  static char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("I,$d"), time);
  serial.println(buffer);
  serial.flush();
  return getString();
}

char * RN41_42::performInquiryScan(uint8_t time, uint32_t cod)
{
  if (!(time <= 48) || !_commandMode) { return false; }
  static char buffer[11];
  snprintf_P(buffer, sizeof(buffer), PSTR("I,$d,%06X"), time, cod);
  serial.println(buffer);
  serial.flush();
  return getString();
}

char * RN41_42::performInquiryScanNN(uint8_t time)
{
  if (!(time <= 48) || !_commandMode) { return false; }
  static char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("IN$d"), time);
  serial.println(buffer);
  serial.flush();
  return getString();
}

char * RN41_42::performInquiryScanNN(uint8_t time, uint32_t cod)
{
  if (!(time <= 48) || !_commandMode) { return false; }
  static char buffer[11];
  snprintf_P(buffer, sizeof(buffer), PSTR("IN$d,%06X"), time, cod);
  serial.println(buffer);
  serial.flush();
  return getString();
}

char * RN41_42::scanRSSI()
{
  if (!_commandMode) { return false; }
  serial.println(F("IQ"));
  serial.flush();
  return getString();
}

char * RN41_42::performRovingInquiryScan(uint8_t time)
{
  if (!_commandMode) { return false; }
  static char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("IS$d"), time);
  serial.println(buffer);
  serial.flush();
  return getString();
}

char * RN41_42::performCableReplaceInquiryScan(uint8_t time)
{
  if (!_commandMode) { return false; }
  static char buffer[5];
  snprintf_P(buffer, sizeof(buffer), PSTR("IR$d"), time);
  serial.println(buffer);
  serial.flush();
  return getString();
}

bool RN41_42::hidePIN()
{
  if (!_commandMode) { return false; }
  serial.println(F("J"));
  serial.flush();
  return true;
}

bool RN41_42::killConnection()
{
  if (!_commandMode) { return false; }
  serial.println(F("K,"));
  serial.flush();
  return strncmp_P(getString(), PSTR("KILL\r\n"), 7) == 0 ? true : false;
}

char * RN41_42::linkQuality()
{
  if (!_commandMode) { return false; }
  serial.println(F("L"));
  serial.flush();
  return getString();
}

char * RN41_42::remoteModemSignalStatus()
{
  if (!_commandMode) { return false; }
  serial.println(F("M"));
  serial.flush();
  return getString();
}

char * RN41_42::otherSettings()
{
  if (!_commandMode) { return false; }
  serial.println(F("O"));
  serial.flush();
  return getString();
}

void RN41_42::passMessage(char mes[32])
{
  static char buffer[34];
  strncpy_P(buffer, PSTR("P,"), 3);
  strncat(buffer, mes, sizeof(buffer));
  serial.println(buffer);
  serial.flush();
}

bool RN41_42::quietMode()
{
  if (!_commandMode) { return false; }
  serial.println(F("Q"));
  serial.flush();
  return strncmp_P(getString(), quiet, sizeof(quiet)) == 0 ? true : false;
}

bool RN41_42::quietMode(uint8_t mode)
{
  if (!(mode >= 0 && mode <= 2) || !_commandMode) { return false; }
  static char buffer[4];
  snprintf_P(buffer, sizeof(buffer), PSTR("Q,$d"), mode);
  serial.println(buffer);
  serial.flush();
  return strncmp_P(getString(), quiet, sizeof(quiet)) == 0 ? true : false;
}

uint8_t RN41_42::quietStatus()
{
  if (!_commandMode) { return false; }
  serial.println(F("Q,?"));
  serial.flush();
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
  delay(100);
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
  if (!_commandMode) { return false; }
  static char buffer[4];
  snprintf_P(buffer, sizeof(buffer), PSTR("T,$d"), en);
  serial.println(buffer);
  serial.flush();
  return true;
}

bool RN41_42::uartChangeTemp(char baud[5], char parity)
{
  if (!_commandMode) { return false; }
  static char buffer[9];
  snprintf_P(buffer, sizeof(buffer), PSTR("U,%S,%S"), baud, parity);
  serial.println(buffer);
  serial.flush();
  if (isAOK()) { _commandMode = false; return true; }
  return false;
}

//Get the firmware version of the RN42/RN42
//V
char *RN41_42::getFirmwareVersion()
{
  if (!_commandMode) { return false; }
  serial.println(F("V"));
  serial.flush();
  return getString();
}

bool RN41_42::enableDiscoveryConnection()
{
  if (!_commandMode) { return false; }
  serial.println(F("W"));
  serial.flush();
  return true;
}

//Puts the RN41/RN42 to sleep.
//Z
void RN41_42::sleep()
{
  if (!_commandMode) { return; }
  serial.println(F("Z"));
  serial.flush();
  _commandMode = false;
}

//GPIO Commands

//Sets the pin mode for GPIO 0 through 7.
//S@
bool RN41_42::pinMode(uint8_t pin, uint8_t dir)
{

  if (!(pin >= 0 && pin <= 7) || !(dir == 0 || dir == 1) || !_commandMode) { return false; }
  bitSet(gpioSetDir, pin);
  bitWrite(gpioDir, pin, dir);
  static char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("S@,%02X%02X"), gpioSetDir, gpioDir);
  serial.println(buffer);
  serial.flush();
  return true;
}

//Writes to GPIO 0 through 11.
//S& or S*
bool RN41_42::digitalWrite(uint8_t pin, uint8_t val)
{

  if (!(pin >= 0 && pin <= 11) || !(val == 0 || val == 1) || !_commandMode) { return false; }
  static char buffer[8];
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
  serial.flush();
  return true;
}

//Set the pin mode on power up for GPIO 0 through 7.
//S%
bool RN41_42::pinModePowerUp(uint8_t pin, uint8_t dir)
{
  if (!(pin >= 0 && pin <= 7) || !(dir == 0 || dir == 1) || !_commandMode) { return false; }
  bitSet(gpioSetDirPowerUp, pin);
  bitWrite(gpioDirPowerUp, pin, dir);
  static char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("S%,%02X%02X"), gpioSetDirPowerUp, gpioDirPowerUp);
  serial.println(buffer);
  serial.flush();
  return true;
}

//Set the pin value on power up for GPIO 0 through 7.
//S^
bool RN41_42::digitalWritePowerUp(uint8_t pin, uint8_t val)
{
  if (!(pin >= 0 && pin <= 7) || !(val == 0 || val == 1) || !_commandMode) { return false; }
  bitSet(gpioSetValPowerUp, pin);
  bitWrite(gpioValPowerUp, pin, val);
  static char buffer[8];
  snprintf_P(buffer, sizeof(buffer), PSTR("S^,%02X%02X"), gpioSetValPowerUp, gpioValPowerUp);
  serial.println(buffer);
  serial.flush();
  return true;
}

//Private Functions Below

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
  //Serial.print(recvBuf);
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