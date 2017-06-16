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
bool RN41_42::setS7(bool en) {
	emptyBuffer();
	enterCommandMode();
	if (en == true) {
		strcpy_P(buf, PSTR("S7,1\r\n"));
		sendString(buf);
	}
	else {
		strcpy_P(buf, PSTR("S7,0\r\n"));
		sendString(buf);
	}
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
bool RN41_42::setAuthenticationMode(int authMode) {
	if (authMode >= 0 && authMode <= 4 && authMode != 3) {
		enterCommandMode();
		emptyBuffer();
		sprintf_P(buf, PSTR("SA,%d\r\n"), authMode);
		sendString(buf);
		return isAOK();
	}
	else {
		return false;
	}
	
}

//Set Break
//SB,<value>
bool RN41_42::setBreak(int breakVal) {
	if (breakVal >= 1 && breakVal <= 6) {
		enterCommandMode();
		emptyBuffer();
		sprintf_P(buf, PSTR("SB,%d\r\n"), breakVal);
		sendString(buf);
		return isAOK();
	}
	else
	{
		return false;
	}
}

//Set Service Class
//SC,<value>
bool RN41_42::setServiceClass(char hex[4]) {
	enterCommandMode();
	emptyBuffer();
	sprintf_P(buf, PSTR("SC,%s\r\n"), hex);
	sendString(buf);
	return isAOK();
}

//Set Device Class
//SD,<value>
bool RN41_42::setDeviceClass(char hex[4]) {
	enterCommandMode();
	emptyBuffer();
	sprintf_P(buf, PSTR("SD,%s\r\n"), hex);
	sendString(buf);
	return isAOK();
}

//Set UUID
//SE,<value>
bool RN41_42::setUUID(char hex[32]) {
	enterCommandMode();
	emptyBuffer();
	sprintf_P(buf, PSTR("SE,%s\r\n"), hex);
	sendString(buf);
	return isAOK();
}

//Restore Factory Defaults
//SF,1
bool RN41_42::restoreFactoryDefaults() {
	enterCommandMode();
	emptyBuffer();
	strcpy_P(buf, PSTR("SF,1\r\n"));
	sendString(buf);
	return isAOK();
}

//Set HID Flag Register
//SH,<value>
bool RN41_42::setHIDRegister(char hex[4]) {
	enterCommandMode();
	emptyBuffer();
	sprintf_P(buf, PSTR("SH,%s\r\n"), hex);
	sendString(buf);
	return isAOK();
}

//Set Inquiry Scan Window
//SI,<value>
bool RN41_42::setInquiryScanWindow(char hex[4]) {
	enterCommandMode();
	emptyBuffer();
	sprintf_P(buf, PSTR("SI,%s\r\n"), hex);
	sendString(buf);
	return isAOK();
}

//Set Page Scan Window
//SJ,<value>
bool RN41_42::setPageScanWindow(char hex[4]) {
	enterCommandMode();
	emptyBuffer();
	sprintf_P(buf, PSTR("SJ,%s\r\n"), hex);
	sendString(buf);
	return isAOK();
}

//Set UART Parity
//SL,<value>
bool RN41_42::setUARTParity(char parity) {
	if (parity == 'E' || parity == 'O' || parity == 'N') {
		enterCommandMode();
		emptyBuffer();
		sprintf_P(buf, PSTR("SL,%s\r\n"), parity);
		sendString(buf);
		return isAOK();
	}
	else {
		return false;
	}
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
bool RN41_42::setMode(int mode) {
	if (mode >= 0 && mode <= 6) {
		enterCommandMode();
		emptyBuffer();
		sprintf_P(buf, PSTR("SM,%d\r\n"), mode);
		sendString(buf);
		return isAOK();
	}
	else {
		return false;
	}
	
}

//Sets Device Name
//SN,<string>
//Maximum 20 characters
bool RN41_42::setDeviceName(char name[20]) {
	enterCommandMode();
	emptyBuffer();
	sprintf_P(buf, PSTR("SN,%s\r\n"), name);
	sendString(buf);
	return isAOK();
}

//Sets Extended Status String
//SO,<string>
//Maximum 8 characters
bool RN41_42::setExtendedStatusString(char name[8]) {
	enterCommandMode();
	emptyBuffer();
	sprintf_P(buf, PSTR("SO,%s\r\n"), name);
	sendString(buf);
	return isAOK();
}

//Sets the Security Pin
//SP,<string>
bool RN41_42::setPinCode(char pin[4]) {
	enterCommandMode();
	emptyBuffer();
	sprintf_P(buf, PSTR("SP,%s\r\n"), pin);
	sendString(buf);
	return isAOK();
}

//Sets the Mask
//SQ,<string>
bool RN41_42::setMask(unsigned int mask) {
	emptyBuffer();
	switch (mask) {
	case 0:
	case 4:
	case 8:
	case 16:
	case 128:
	case 256:
		enterCommandMode();
		sprintf_P(buf, PSTR("SQ,%d\r\n"), mask);
		sendString(buf);
		return isAOK();
	default:
		return false;
	}
}

//Set Remote Address
//SR,<hex value>
bool RN41_42::setRemoteAddress(char address[12]) {
	enterCommandMode();
	emptyBuffer();
	sprintf_P(buf, PSTR("SR,%s\r\n"), address);
	sendString(buf);
	return isAOK();
}

//Erase Remote Address
//SR,Z
bool RN41_42::eraseRemoteAddress() {
	enterCommandMode();
	emptyBuffer();
	strcpy_P(buf, PSTR("SR,Z\r\n"));
	sendString(buf);
	return isAOK();
}

//Set Remote Address as Last Observed Address
//SR,I
bool RN41_42::setRemoteAddressLastObserved() {
	enterCommandMode();
	emptyBuffer();
	strcpy_P(buf, PSTR("SR,I\r\n"));
	sendString(buf);
	return isAOK();
}

//Set Service Name
//SS,<string>
bool RN41_42::setServiceName(char name[20]) {
	enterCommandMode();
	emptyBuffer();
	sprintf_P(buf, PSTR("SS,%s\r\n"), name);
	sendString(buf);
	return isAOK();
}

//Set Remote Config Timer
//ST,<string>
bool RN41_42::setConfigTimer(unsigned int value) {
	if (value >= 0 && value <= 255) {
		enterCommandMode();
		emptyBuffer();
		sprintf_P(buf, PSTR("SQ,%d\r\n"), value);
		sendString(buf);
		return isAOK();
	}
	else {
		return false;
	}
}

//Set UART Baud
//SU,<value>
bool RN41_42::setUARTBaud(unsigned int baud) {
	unsigned int currentBaud = _baud;
	emptyBuffer();
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
		enterCommandMode();
		sprintf_P(buf, PSTR("SQ,%d\r\n"), baud);
		sendString(buf);
		return isAOK();
	default:
		return false;
	}
}

//Set Sniff Mode
//SW,<value>
bool RN41_42::setSniff(char hex[4]) {
	enterCommandMode();
	emptyBuffer();
	sprintf_P(buf, PSTR("SW,%s\r\n"), hex);
	sendString(buf);
	return isAOK();
}

//Set Bonding
//SX,<flag>
bool RN41_42::setBonding(bool en) {
	emptyBuffer();
	enterCommandMode();
	if (en == true) {
		strcpy_P(buf, PSTR("SX,1\r\n"));
		sendString(buf);
	}
	else {
		strcpy_P(buf, PSTR("SX,0\r\n"));
		sendString(buf);
	}
	return	isAOK();
}

//Set Transmit Power
//SY,<hex value>
bool RN41_42::setTransmitPower(char hex[4]) {
//	if (hex == "0010" || hex == "000C" || hex == "0008" || hex == "0004" || hex == "0000" || hex == "FFFC" || hex == "FFF8" || hex == "FFF4") {
	enterCommandMode();
	emptyBuffer();
	sprintf_P(buf, PSTR("SY,%s\r\n"), hex);
	sendString(buf);
	return isAOK();
//	}
//	else {
//		return false;
//	}
}

//Set Non-Standard Baud
//SZ,<value>
bool RN41_42::setNonStandardBaud(unsigned int multi) {
	enterCommandMode();
	emptyBuffer();
	sprintf_P(buf, PSTR("SZ,%d\r\n"), multi);
	sendString(buf);
	return isAOK();
}

//Set Profile
//S~,<value>
bool RN41_42::setProfile(int value) {
	if (value >= 0 && value <= 6) {
		enterCommandMode();
		emptyBuffer();
		sprintf_P(buf, PSTR("S~,%d\r\n"), value);
		sendString(buf);
		return isAOK();
	}
	else {
		return false;
	}
}

//Set Serialized Friendly Name
//S-,<string>
bool RN41_42::setSerializedFriendlyName(char name[15]) {
	enterCommandMode();
	emptyBuffer();
	sprintf_P(buf, PSTR("S-,%s\r\n"), name);
	sendString(buf);
	return isAOK();
}

//Set Roll Switch
//S?,<flag>
bool RN41_42::setRoleSwitch(bool en) {
	enterCommandMode();
	emptyBuffer();
	if (en == true) {
		strcpy_P(buf, PSTR("S?,1\r\n"));
		sendString(buf);
	}
	else {
		strcpy_P(buf, PSTR("S?,0\r\n"));
		sendString(buf);
	}
	return	isAOK();
}

//Set Configuration Character
//S$,<char>
bool RN41_42::setConfigChar(char c)
{
	enterCommandMode();
	emptyBuffer();
	sprintf_P(buf, PSTR("SL,%s\r\n"), c);
	sendString(buf);
	if (isAOK()) {
		_configChar[0] = c;
		return true;
	}
	else {
		return false;
	}

}

//Set Low-Power Connect Mode
//S|,<value>
bool RN41_42::setLPConnectMode(char hex[4])
{
	enterCommandMode();
	emptyBuffer();
	sprintf_P(buf, PSTR("S|,%s\r\n"), hex);
	sendString(buf);
	return isAOK();
}

//Returns the basic settings
//D
char *RN41_42::getBasicSettings()
{
	enterCommandMode();
	emptyBuffer();
	strcpy_P(buf, PSTR("D\r\n"));
	sendString(buf);
	getString();
	exitCommandMode();
	return buf;
}

//Returns the extended settings
//E
char *RN41_42::getExtendedSettings()
{
	enterCommandMode();
	emptyBuffer();
	strcpy_P(buf, PSTR("E\r\n"));
	sendString(buf);
	getString();
	exitCommandMode();
	return buf;
}

//Returns the Bluetooth Address
//GB
char *RN41_42::getBluetoothAddress()
{
	enterCommandMode();
	emptyBuffer();
	strcpy_P(buf, PSTR("GB\r\n"));
	sendString(buf);
	getString();
	exitCommandMode();
	return buf;
}

//Returns the Connected Device Bluetooth Address
//GF
char *RN41_42::getConnectedRemoteAddress()
{
	enterCommandMode();
	emptyBuffer();
	strcpy_P(buf, PSTR("GF\r\n"));
	sendString(buf);
	getString();
	exitCommandMode();
	return buf;
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
		enterCommandMode();
		emptyBuffer();
		strcpy_P(buf, PSTR("GK\r\n"));
		sendString(buf);
		if (strcmp_P(getString(), PSTR("1,0,0\r\n"))==0) {
			exitCommandMode();
			return true;
		}
		else {
			exitCommandMode();
			return false;
		}
	#endif // RN41_42_CONN_PIN
}

//Returns the Stored Remote Bluetooth Address
//GR
char  *RN41_42::getStoredRemoteAddress()
{
	enterCommandMode();
	emptyBuffer();
	strcpy_P(buf, PSTR("GR\r\n"));
	sendString(buf);
	getString();
	exitCommandMode();
	return buf;
}

//Return the GPIO Bitmask
//G&
char *RN41_42::getGPIOStatus()
{
	enterCommandMode();
	emptyBuffer();
	strcpy_P(buf, PSTR("G&\r\n"));
	sendString(buf);
	getString();
	exitCommandMode();
	return buf;
}

//Get The Device's Firmware Version
char *RN41_42::getFirmwareVersion() {
	enterCommandMode();
	emptyBuffer();
	strcpy_P(buf, PSTR("V\r\n"));
	sendString(buf);
	getString();
	exitCommandMode();
	return buf;
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
		enterCommandMode();
		emptyBuffer();
		strcpy_P(buf, PSTR("R,1\r\n"));
		sendString(buf);
		if (strcmp_P(getString(), PSTR("Reboot!\r\n"))==0) {
			_commandMode = false;
			return true;
		}
		else {
			return false;
		}
	#endif // RN41_42_RESET_PIN
}

bool RN41_42::connectToAddress(char address[12])
{
	return false;
}

bool RN41_42::sendMessage(char message[32], char terminationChar) {
	if (!_commandMode) {
		emptyBuffer();
		sprintf_P(buf, PSTR("%s%s"), message, terminationChar);
		serial.write(buf);
		return true;
	}
	else {
		return false;
	}
}

char *RN41_42::recieveMessage() {
	if (!_commandMode) {
		return getString();
	}
	else {
		return "*ERROR*";
	}
}

//Private Functions Below

//Enter Command Mode
bool RN41_42::enterCommandMode() {
	if (!_commandMode) {
		for (int i = 0; i < 3; i++) {
			sendString(_configChar);
		}
		if (strcmp_P(getString(), PSTR("CMD\r\n"))==0) {
			_commandMode = true;
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return true;
	}
}

//Exit Command Mode
bool RN41_42::exitCommandMode() {
	if (_commandMode) {
		emptyBuffer();
		strcpy_P(buf, PSTR("---\r\n"));
		sendString(buf);
		if (strcmp_P(getString(), PSTR("END\r\n"))==0) {
			_commandMode = false;
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return true;
	}
}

void RN41_42::sendString(char * msg) {
	serial.print(msg);
}

char *RN41_42::getString() {
	emptyBuffer();
	int i = 0;
	while (serial.available()) {
		buf[i] = serial.read();
		i++;
	}
	return buf;
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

void RN41_42::emptyBuffer()
{
	for (int i = 0; i <= 38; i++)
	{
		buf[i] = { '\0' };
	}
}

bool RN41_42::isAOK() {
	if (strcmp_P(getString(), PSTR("AOK\r\n"))==0) {
		exitCommandMode();
		return true;
	}
	else
	{
		exitCommandMode();
		return false;
	}
}