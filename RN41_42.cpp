/*
 Name:		RN41.cpp
 Created:	6/7/2017 9:46:16 PM
 Author:	Chris
 Editor:	http://www.visualmicro.com
*/

#include "RN41_42.h"



RN41_42::RN41_42(HardwareSerial &_serial) : serial(_serial) {
	_commandMode = false;
	_configChar[0] = '$';
	setupIO();
}

RN41_42::RN41_42(HardwareSerial &_serial, char configChar) : serial(_serial) {
	_commandMode = false;
	_configChar[0] = configChar;
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

//Dipswitch/GPIO Functions

#ifdef RN41_42_GPIO4
void RN41_42::factoryReset()
{
	digitalWrite(RN41_42_GPIO4, HIGH);
	reset();
	delay(500);
	digitalWrite(RN41_42_GPIO4, LOW);
	delay(1000);
	digitalWrite(RN41_42_GPIO4, HIGH);
	digitalWrite(RN41_42_GPIO4, LOW);
	delay(1000);
	digitalWrite(RN41_42_GPIO4, HIGH);
	digitalWrite(RN41_42_GPIO4, LOW);
	reset();
}
#endif // RN41_42_GPIO4

#ifdef RN41_42_GPIO3
void RN41_42::autoDiscoveryPairing(bool en)
{
	digitalWrite(RN41_42_GPIO3, en);
}
#endif // RN41_42_GPIO3

#ifdef RN41_42_GPIO6
void RN41_42::autoConnect(bool en)
{
	digitalWrite(RN41_42_GPIO6, en);
}
#endif // RN41_42_GPIO6

#ifdef RN41_42_GPIO7
void RN41_42::setBaudRate9600(bool en)
{
	digitalWrite(RN41_42_GPIO7, en);
}
#endif // RN41_42_GPIO7

//SET COMMANDS

//Set 7-bit Data Mode
//S7,<flag>
bool RN41_42::setS7(bool en)
{
	if (!_commandMode) { return false; }
	char buffer[5];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("7"), false), en);
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
bool RN41_42::setAuthenticationMode(int authMode)
{
	if (!(authMode >= 0 && authMode <= 4 && authMode != 3) || !_commandMode) { return false; }
	char buffer[5];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("A"), false), authMode);
	serial.println(buffer);
	return isAOK();
}

//Set Break
//SB,<value>
bool RN41_42::setBreak(int breakVal)
{
	if (!(breakVal >= 1 && breakVal <= 6) || !_commandMode) { return false; }
	char buffer[5];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("B"), false), breakVal);
	serial.println(buffer);
	return isAOK();
}

//Set Service Class
//SC,<value>
bool RN41_42::setServiceClass(int hex)
{
	if (!_commandMode) { return false; }
	char buffer[8];
	snprintf(buffer, sizeof(buffer), buildHexSString(PSTR("C")), hex);
	serial.println(buffer);
	return isAOK();
}


//Set Device Class
//SD,<value>
bool RN41_42::setDeviceClass(int hex)
{
	if (!_commandMode) { return false; }
	char buffer[8];
	snprintf(buffer, sizeof(buffer), buildHexSString(PSTR("D")), hex);
	serial.println(buffer);
	return isAOK();
}

//Set UUID
//SE,<value>
bool RN41_42::setUUID(char hex[32])
{
	if (!_commandMode) { return false; }
	char buffer[35];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("E"), true), hex);
	serial.println(buffer);
	return isAOK();
}

//Restore Factory Defaults
//SF,1
bool RN41_42::restoreFactoryDefaults()
{
	if (!_commandMode) { return false; }
	char buffer[5];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("F"), false), 1);
	serial.println(buffer);
	return isAOK();
}

//Set HID Flag Register
//SH,<value>
bool RN41_42::setHIDRegister(int hex)
{
	if (!_commandMode) { return false; }
	char buffer[8];
	snprintf(buffer, sizeof(buffer), buildHexSString(PSTR("H")), hex);
	serial.println(buffer);
	return isAOK();
}

//Set Inquiry Scan Window
//SI,<value>
bool RN41_42::setInquiryScanWindow(int hex)
{
	if (!_commandMode) { return false; }
	char buffer[8];
	snprintf(buffer, sizeof(buffer), buildHexSString(PSTR("I")), hex);
	serial.println(buffer);
	return isAOK();
}

//Set Page Scan Window
//SJ,<value>
bool RN41_42::setPageScanWindow(int hex)
{
	if (!_commandMode) { return false; }
	char buffer[8];
	snprintf(buffer, sizeof(buffer), buildHexSString(PSTR("J")), hex);
	serial.println(buffer);
	return isAOK();
}

//Set UART Parity
//SL,<value>
bool RN41_42::setUARTParity(char parity)
{
	if (!(parity == 'E' || parity == 'O' || parity == 'N') || !_commandMode) { return false; }
	char buffer[5];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("L"), true), parity);
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
	if (!(mode <= 6) || !_commandMode) { return false; }
	char buffer[5];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("M"), false), mode);
	serial.println(buffer);
	return isAOK();
}

//Sets Device Name
//SN,<string>
//Maximum 20 characters
bool RN41_42::setDeviceName(char name[20])
{
	if (!_commandMode) { return false; }
	char buffer[24];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("N"), true), name);
	serial.println(buffer);
	return isAOK();
}

//Sets Extended Status String
//SO,<string>
//Maximum 8 characters
bool RN41_42::setExtendedStatusString(char name[8])
{
	if (!_commandMode) { return false; }
	char buffer[12];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("O"), true), name);
	serial.println(buffer);
	return isAOK();
}

//Sets the Security Pin
//SP,<string>
bool RN41_42::setPinCode(char pin[4])
{
	if (!_commandMode) { return false; }
	char buffer[8];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("P"), true), pin);
	serial.println(buffer);
	return isAOK();
}

//Sets the Mask
//SQ,<string>
bool RN41_42::setMask(unsigned int mask)
{
	if (!_commandMode) { return false; }
	char buffer[7];
	switch (mask)
	{
	case 0:
	case 4:
	case 8:
	case 16:
	case 128:
	case 256:
		snprintf(buffer, sizeof(buffer), buildSString(PSTR("Q"), false), mask);
		serial.println(buffer);
		return isAOK();
	default:
		return false;
	}
}

//Set Remote Address
//SR,<hex value>
bool RN41_42::setRemoteAddress(char address[12])
{
	if (!_commandMode) { return false; }
	char buffer[16];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("R"), true), address);
	serial.println(buffer);
	return isAOK();
}

//Erase Remote Address
//SR,Z
bool RN41_42::eraseRemoteAddress()
{
	if (!_commandMode) { return false; }
	char buffer[5];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("R"), true), "Z");
	serial.println(buffer);
	return isAOK();
}

//Set Remote Address as Last Observed Address
//SR,I
bool RN41_42::setRemoteAddressLastObserved()
{
	if (!_commandMode) { return false; }
	char buffer[5];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("R"), true), "I");
	serial.println(buffer);
	return isAOK();
}

//Set Service Name
//SS,<string>
bool RN41_42::setServiceName(char name[20])
{
	if (!_commandMode) { return false; }
	char buffer[24];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("S"), true), name);
	serial.println(buffer);
	return isAOK();
}

//Set Remote Config Timer
//ST,<string>
bool RN41_42::setConfigTimer(byte value)
{
	if (!_commandMode) { return false; }
	char buffer[7];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("T"), false), value);
	serial.println(buffer);
	return isAOK();
}

//Set UART Baud
//SU,<value>
bool RN41_42::setUARTBaud(unsigned int baud)
{
	if (!_commandMode) { return false; }
	char buffer[6];
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
		snprintf(buffer, sizeof(buffer), buildSString(PSTR("U"), false), baud);
		serial.println(buffer);
		return isAOK();
	default:
		return false;
	}
}

//Set Sniff Mode
//SW,<value>
bool RN41_42::setSniff(int hex)
{
	if (!_commandMode) { return false; }
	char buffer[8];
	snprintf(buffer, sizeof(buffer), buildHexSString(PSTR("W")), hex);
	serial.println(buffer);
	return isAOK();
}

//Set Bonding
//SX,<flag>
bool RN41_42::setBonding(bool en)
{
	if (!_commandMode) { return false; }
	char buffer[5];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("X"), false), (int)en);
	serial.println(buffer);
	return isAOK();
}

//Set Transmit Power
//SY,<hex value>
bool RN41_42::setTransmitPower(int hex)
{
	if (!_commandMode) { return false; }
	char buffer[8];
	snprintf(buffer, sizeof(buffer), buildHexSString(PSTR("Y")), hex);
	serial.println(buffer);
	return isAOK();
}

//Set Non-Standard Baud
//SZ,<value>
bool RN41_42::setNonStandardBaud(unsigned int multi)
{
	if (!_commandMode) { return false; }
	char buffer[6];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("Z"), false), multi);
	serial.println(buffer);
	return isAOK();
}

//Set Profile
//S~,<value>
bool RN41_42::setProfile(int value) {
	if (!(value >= 0 && value <= 6) || !_commandMode) { return false; }
		char buffer[5];
		enterCommandMode();
		snprintf(buffer, sizeof(buffer), buildSString(PSTR("~"), false), value);
		serial.println(buffer);
		return isAOK();
}

//Set Serialized Friendly Name
//S-,<string>
bool RN41_42::setSerializedFriendlyName(char name[15])
{
	if (!_commandMode) { return false; }
	char buffer[19];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("-"), true), name);
	serial.println(buffer);
	return isAOK();
}

//Set Roll Switch
//S?,<flag>
bool RN41_42::setRoleSwitch(bool en)
{
	if (!_commandMode) { return false; }
	char buffer[5];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("?"), false), en);
	serial.println(buffer);
	return isAOK();
}

//Set Configuration Character
//S$,<char>
bool RN41_42::setConfigChar(char c)
{
	if (!_commandMode) { return false; }
	char buffer[5];
	snprintf(buffer, sizeof(buffer), buildSString(PSTR("$"), true), c);
	serial.println(buffer);
	if (isAOK()) { _configChar[0] = c; return true; }
	return false;
}

//Set Low-Power Connect Mode
//S|,<value>
bool RN41_42::setLPConnectMode(int hex)
{
	if (!_commandMode) { return false; }
	char buffer[8];
	snprintf(buffer, sizeof(buffer), buildHexSString(PSTR("|")), hex);
	serial.println(buffer);
	return isAOK();
}

//Returns the basic settings
//D
char *RN41_42::getBasicSettings()
{
	if (!_commandMode) { return false; }
	serial.println(PSTR("D"));
	return getString();
}

//Returns the extended settings
//E
char *RN41_42::getExtendedSettings()
{
	if (!_commandMode) { return false; }
	serial.println(PSTR("E"));
	return getString();
}

//Returns the Bluetooth Address
//GB
char *RN41_42::getBluetoothAddress()
{
	if (!_commandMode) { return false; }
	serial.println(PSTR("GB"));
	return getString();
}

//Returns the Connected Device Bluetooth Address
//GF
char *RN41_42::getConnectedRemoteAddress()
{
	if (!_commandMode) { return false; }
	serial.println(PSTR("GF"));
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
	serial.println(PSTR("GK"));
	return strncmp_P(getString(), PSTR("1,0,0\r\n"), 8) == 0 ? true : false;
#endif // RN41_42_CONN_PIN
}

//Returns the Stored Remote Bluetooth Address
//GR
char  *RN41_42::getStoredRemoteAddress()
{
	if (!_commandMode) { return false; }
	serial.println(PSTR("GR"));
	return getString();
}

//Return the GPIO Bitmask
//G&
char *RN41_42::getGPIOStatus()
{
	if (!_commandMode) { return false; }
	serial.println(PSTR("G&"));
	return getString();
}

char * RN41_42::displayDipwitchValues()
{
	if (!_commandMode) { return false; }
	serial.println(PSTR("&"));
	return getString();
}

bool RN41_42::connectToStoredAddress()
{
	if (!_commandMode) { return false; }
	serial.println(PSTR("C"));
	return strncmp_P(getString(), connected, 12) == 0 ? true : false;
}

bool RN41_42::connectToAddress(char address[13])
{
	if (!_commandMode) { return false; }
	char buffer[15];
	snprintf_P(buffer, sizeof(buffer), PSTR("C,%S"), address);
	serial.println(buffer);
	return strncmp_P(getString(), connected, 12) == 0 ? true : false;
}

bool RN41_42::connectToAddressFast(char address[13])
{
	if (!_commandMode) { return false; }
	char buffer[16];
	snprintf_P(buffer, sizeof(buffer), PSTR("CF,%S"), address);
	serial.println(buffer);
	return strncmp_P(getString(), connected, 12) == 0 ? true : false;
}

bool RN41_42::connectToLastFoundAddressFast()
{
	if (!_commandMode) { return false; }
	serial.println(PSTR("CFI"));
	return strncmp_P(getString(), connected, 12) == 0 ? true : false;
}

bool RN41_42::connectToStoredRemoteAddressFast()
{
	if (!_commandMode) { return false; }
	serial.println(PSTR("CFR"));
	return strncmp_P(getString(), connected, 12) == 0 ? true : false;
}

bool RN41_42::connectToAddressTimed(char address[13], byte time)
{
	if (!_commandMode) { return false; }
	char buffer[19];
	snprintf_P(buffer, sizeof(buffer), PSTR("CT,%S,$d"), address, time);
	serial.println(buffer);
	return strncmp_P(getString(), connected, 12) == 0 ? true : false;
}

bool RN41_42::fastDataMode()
{
	if (!_commandMode) { return false; }
	serial.println(PSTR("F,1"));
	_commandMode = false;
	return true;
}

char * RN41_42::help()
{
	if (!_commandMode) { return false; }
	serial.println(PSTR("H"));
	return getString();
}

bool RN41_42::killConnection()
{
	if (!_commandMode) { return false; }
	serial.println(PSTR("K,"));
	return strncmp_P(getString(), PSTR("KILL\r\n"), 7) == 0 ? true : false;
}

void RN41_42::passMessage(char mes[32])
{
	char buffer[34];
	strncpy_P(buffer, PSTR("P,"), 3);
	strncat(buffer, mes, 34);
	serial.println(buffer);
}

bool RN41_42::quietMode()
{
	if (!_commandMode) { return false; }
	serial.println(PSTR("Q"));
	return strncmp_P(getString(), PSTR("QUIET\r\n"), 8) == 0 ? true : false;
}

//Get The Device's Firmware Version
char *RN41_42::getFirmwareVersion()
{
	if (!_commandMode) { return false; }
	serial.println(PSTR("V"));
	return getString();
}

void RN41_42::sleep()
{
	serial.println(PSTR("Z"));
	_commandMode = false;
}

//Resets The Device
//Also Exits Command Mode
//Uses GPIO over serial commands if available
//Device Must Be Reset After A Config Change To Take Effect
bool RN41_42::reset()
{
#ifdef RN41_42_RESET
	digitalWrite(RN41_42_RESET, LOW);
	delay(100);
	digitalWrite(RN41_42_RESET, HIGH);
	return true;
#else
	if (!_commandMode) { return false; }
	serial.println(PSTR("R,1"));
	if (strcmp_P(getString(), PSTR("Reboot!\r\n")) == 0) { _commandMode = false; return true; }
	return false;

#endif // RN41_42_RESET_PIN
}

bool RN41_42::sendMessage(char message[32])
{
	if (_commandMode) { return false; }
	serial.println(message);
	return true;
}

char *RN41_42::recieveMessage()
{
	if (_commandMode) { return "ERR"; }
	return getString();
}

//Private Functions Below

//Enter Command Mode
bool RN41_42::enterCommandMode()
{
	if (_commandMode) { return true; }
	for (int i = 0; i < 3; i++) { serial.print(_configChar); }
	char buffer[6];
	if (strncmp_P(getString(buffer), PSTR("CMD\r\n"), 6) == 0) { _commandMode = true; return true; }
	return false;
}

//Exit Command Mode
bool RN41_42::exitCommandMode()
{
	if (!_commandMode) { return true; }
	char buffer[6];
	serial.println(PSTR("---"));
	if (strncmp_P(getString(buffer), PSTR("END\r\n"), 6) == 0) { _commandMode = false; return true; }
	return false;
}

char *RN41_42::getString() {
	int i = 0;
	while (serial.available()) {
		recvBuf[i] = serial.read();
		i++;
	}
	recvBuf[i] = { '\0' };
	return recvBuf;
}

char * RN41_42::getString(char * buffer)
{
	int i = 0;
	while (serial.available()) {
		buffer[i] = serial.read();
		i++;
	}
	buffer[i] = { '\0' };
	return buffer;
}

void RN41_42::setupIO()
{
#ifdef RN41_42_RESET
	pinMode(RN41_42_RESET, OUTPUT);
	digitalWrite(RN41_42_RESET, HIGH);
#endif // RN41_42_RESET_PIN

#ifdef RN41_42_GPIO2
	pinMode(RN41_42_GPIO2, INPUT);
	digitalWrite(RN41_42_GPIO2, HIGH);
#endif // RN41_42_CONN_PIN

#ifdef RN41_42_GPIO4
	pinMode(RN41_42_GPIO4, OUTPUT);
	digitalWrite(RN41_42_GPIO4, LOW);
#endif // RN41_42_GPIO4

#ifdef RN41_42_GPIO3
	pinMode(RN41_42_GPIO3, OUTPUT);
	digitalWrite(RN41_42_GPIO3, LOW);
#endif // RN41_42_GPIO3

#ifdef RN41_42_GPIO6
	pinMode(RN41_42_GPIO6, OUTPUT);
	digitalWrite(RN41_42_GPIO6, LOW);
#endif // RN41_42_GPIO6

#ifdef RN41_42_GPIO7
	pinMode(RN41_42_GPIO7, OUTPUT);
	digitalWrite(RN41_42_GPIO7, LOW);
#endif // RN41_42_GPIO
}

char * RN41_42::buildHexSString(const PROGMEM char* cmd)
{
	char sBuf[10];
	strncpy_P(sBuf, S, 2);
	strncat_P(sBuf, cmd, 2);
	strncat_P(sBuf, pgmHex, 4);
	return sBuf;
}

char *RN41_42::buildSString(const PROGMEM char* cmd, bool isString)
{
	char sBuf[8];
	strncpy_P(sBuf, S, 2);
	strncat_P(sBuf, cmd, 2);
	strncat_P(sBuf, commaPercent, 3);
	if (isString) {
		strncat_P(sBuf, str, 2);
	}
	else
	{
		strncat_P(sBuf, dec, 2);
	}
	return sBuf;
}

bool RN41_42::isAOK()
{
	return strncmp_P(getString(), PSTR("AOK\r\n"), 6) == 0 ? true : false;
}

