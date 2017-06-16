/*
 Name:		RN41.cpp
 Created:	6/7/2017 9:46:16 PM
 Author:	Chris
 Editor:	http://www.visualmicro.com
*/

#include "RN41_42.h"

RN41_42::RN41_42(HardwareSerial &_serial) : serial(_serial) {
	_commandMode = false;
	_configChar = '$';
	setupIO();
}

RN41_42::RN41_42(HardwareSerial &_serial, char configChar) : serial(_serial) {
	_commandMode = false;
	_configChar = configChar;
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
	enterCommandMode();
	if (en == true) {
		sendString("S7,1\r\n");
	}
	else {
		sendString("S7,0\r\n");
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
		char buf[1];
		sprintf(buf, "SA,%d\r\n", authMode);
		String msg = buf;
		sendString(msg);
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
		char buf[1];
		sprintf(buf, "SB,%d\r\n", breakVal);
		String msg = buf;
		sendString(msg);
		return isAOK();
	}
	else
	{
		return false;
	}
}

//Set Service Class
//SC,<value>
bool RN41_42::setServiceClass(String hex) {
	enterCommandMode();
	sendString("SC," + hex + "\r\n");
	return isAOK();
}

//Set Device Class
//SD,<value>
bool RN41_42::setDeviceClass(String hex) {
	enterCommandMode();
	sendString("SD," + hex + "\r\n");
	return isAOK();
}

//Set UUID
//SE,<value>
bool RN41_42::setUUID(String hex) {
	enterCommandMode();
	sendString("SE," + hex + "\r\n");
	return isAOK();
}

//Restore Factory Defaults
//SF,1
bool RN41_42::restoreFactoryDefaults() {
	enterCommandMode();
	sendString("SF,1");
	return isAOK();
}

//Set HID Flag Register
//SH,<value>
bool RN41_42::setHIDRegister(String hex) {
	enterCommandMode();
	sendString("SH," + hex + "\r\n");
	return isAOK();
}

//Set Inquiry Scan Window
//SI,<value>
bool RN41_42::setInquiryScanWindow(String hex) {
	enterCommandMode();
	sendString("SI," + hex + "\r\n");
	return isAOK();
}

//Set Page Scan Window
//SJ,<value>
bool RN41_42::setPageScanWindow(String hex) {
	enterCommandMode();
	sendString("SJ," + hex + "\r\n");
	return isAOK();
}

//Set UART Parity
//SL,<value>
bool RN41_42::setUARTParity(char parity) {
	if (parity == 'E' || parity == 'O' || parity == 'N') {
		enterCommandMode();
		sendString("SL," + String(parity) + "\r\n");
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
		char buf[1];
		sprintf(buf, "SM,%d\r\n", mode);
		String msg = buf;
		sendString(msg);
		return isAOK();
	}
	else {
		return false;
	}
	
}

//Sets Device Name
//SN,<string>
//Maximum 20 characters
bool RN41_42::setDeviceName(String name) {
	if (name.length() >= 1 && name.length() <= 20) {
		enterCommandMode();
		sendString("SN," + name + "\r\n");
		return isAOK();
	}
	else {
		return false;
	}
}

//Sets Extended Status String
//SO,<string>
//Maximum 8 characters
bool RN41_42::setExtendedStatusString(String string) {
	if (string.length() >= 1 && string.length() <= 8) {
		enterCommandMode();
		sendString("SO," + string + "\r\n");
		return isAOK();
	}
	else {
		return false;
	}
}

//Sets the Security Pin
//SP,<string>
bool RN41_42::setPinCode(String pin) {
	enterCommandMode();
	sendString("SP," + pin + "\r\n");
	return isAOK();
}

//Sets the Mask
//SQ,<string>
bool RN41_42::setMask(unsigned int mask) {
	switch (mask) {
	case 0:
	case 4:
	case 8:
	case 16:
	case 128:
	case 256:
		enterCommandMode();
		sendString("SQ," + String(mask) + "\r\n");
		return isAOK();
	default:
		return false;
	}
}

//Set Remote Address
//SR,<hex value>
bool RN41_42::setRemoteAddress(String address) {
	enterCommandMode();
	sendString("SR," + address + "\r\n");
	return isAOK();
}

//Erase Remote Address
//SR,Z
bool RN41_42::eraseRemoteAddress() {
	enterCommandMode();
	sendString("SR,Z\r\n");
	return isAOK();
}

//Set Remote Address as Last Observed Address
//SR,I
bool RN41_42::setRemoteAddressLastObserved() {
	enterCommandMode();
	sendString("SR,I\r\n");
	return isAOK();
}

//Set Service Name
//SS,<string>
bool RN41_42::setServiceName(String name) {
	if (name.length() >= 1 && name.length() <= 20) {
		enterCommandMode();
		sendString("SS," + name + "\r\n");
		return isAOK();
	}
	else {
		return false;
	}
}

//Set Remote Config Timer
//ST,<string>
bool RN41_42::setConfigTimer(unsigned int value) {
	if (value >= 0 && value <= 255) {
		enterCommandMode();
		sendString("ST," + String(value) + "\r\n");
		return isAOK();
	}
	else {
		return false;
	}
}

//Set UART Baud
//SU,<value>
//Full baudrate required so it can be changed on the microcontroller side too.
bool RN41_42::setUARTBaud(unsigned int baud) {
	unsigned int currentBaud = _baud;
	switch (baud) {
	case 1200:
	case 2400:
	case 4800:
	case 9600:
	case 19200:
	case 28800:
	case 38400:
	case 57600:
	case 115000:
	case 230000:
	case 460000:
	case 921000:
		enterCommandMode();
		sendString("SU," + String(baud).substring(0, 1) + "\r\n");
		serial.end();
		begin(baud);
		if (isAOK()) {
			return true;
		}
		else {
			serial.end();
			begin(currentBaud);
			return false;
		}
	default:
		return false;
	}
}

//Set Sniff Mode
//SW,<value>
bool RN41_42::setSniff(String hex) {
	enterCommandMode();
	sendString("SW," + hex + "\r\n");
	return isAOK();
}

//Set Bonding
//SX,<flag>
bool RN41_42::setBonding(bool en) {
	enterCommandMode();
	if (en == true) {
		sendString("SX,1\r\n");
	}
	else {
		sendString("SX,0\r\n");
	}
	return	isAOK();
}

//Set Transmit Power
//SY,<hex value>
bool RN41_42::setTransmitPower(String hex) {
	if (hex == "0010" || hex == "000C" || hex == "0008" || hex == "0004" || hex == "0000" || hex == "FFFC" || hex == "FFF8" || hex == "FFF4") {
		enterCommandMode();
		sendString("SY," + hex + "\r\n");
		return isAOK();
	}
	else {
		return false;
	}
}

//Set Non-Standard Baud
//SZ,<value>
//Takes effect on reboot
bool RN41_42::setNonStandardBaud(unsigned int multi) {
	enterCommandMode();
	sendString("SZ," + String(multi) + "\r\n");
	if (isAOK()) {
		serial.end();
		begin((unsigned long)(multi / 0.004096)); //This probably won't work well
		return true;
	}
	else {
		return false;
	}
}

//Set Profile
//S~,<value>
bool RN41_42::setProfile(int value) {
	if (value >= 0 && value <= 6) {
		enterCommandMode();
		sendString("S~," + String(value) + "\r\n");
		return isAOK();
	}
	else {
		return false;
	}
}

//Set Serialized Friendly Name
//S-,<string>
bool RN41_42::setSerializedFriendlyName(String name) {
	if (name.length() >= 1 && name.length() <= 15) {
		enterCommandMode();
		sendString("S-," + name + "\r\n");
		return isAOK();
	}
	else {
		return false;
	}
}

//Set Roll Switch
//S?,<flag>
bool RN41_42::setRoleSwitch(bool en) {
	enterCommandMode();
	if (en == true) {
		sendString("S?,1\r\n");
	}
	else {
		sendString("S?,0\r\n");
	}
	return	isAOK();
}

//Set Configuration Character
//S$,<char>
bool RN41_42::setConfigChar(char c)
{
	enterCommandMode();
	sendString("S$," + String(c) + "\r\n");
	if (isAOK()) {
		_configChar = c;
		return true;
	}
	else {
		return false;
	}

}

//Set Low-Power Connect Mode
//S|,<value>
bool RN41_42::setLPConnectMode(String hex)\
{
	enterCommandMode();
	sendString("S|," + hex + "\r\n");
	return isAOK();
}

//Returns the basic settings
//D
String RN41_42::getBasicSettings()
{
	enterCommandMode();
	sendString("D\r\n");
	String res = getString();
	exitCommandMode();
	return res;
}

//Returns the extended settings
//E
String RN41_42::getExtendedSettings()
{
	enterCommandMode();
	sendString("E\r\n");
	String res = getString();
	exitCommandMode();
	return res;
}

//Returns the Bluetooth Address
//GB
String RN41_42::getBluetoothAddress()
{
	enterCommandMode();
	sendString("GB\r\n");
	String res = getString();
	res = res.substring(0, res.length() - 2);
	exitCommandMode();
	return res;
}

//Returns the Connected Device Bluetooth Address
//GF
String RN41_42::getConnectedRemoteAddress()
{
	enterCommandMode();
	sendString("GF\r\n");
	String res = getString();
	res = res.substring(0, res.length() - 2);
	exitCommandMode();
	return res;
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
		sendString("GK\r\n");
		if (getString() == "1,0,0\r\n") {
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
String RN41_42::getStoredRemoteAddress()
{
	enterCommandMode();
	sendString("GR\r\n");
	String res = getString();
	res = res.substring(0, res.length() - 2);
	exitCommandMode();
	return res;
}

//Return the GPIO Bitmask
//G&
String RN41_42::getGPIOStatus()
{
	enterCommandMode();
	sendString("G&\r\n");
	String res = getString();
	exitCommandMode();
	return res;
}

//Get The Device's Firmware Version
String RN41_42::getFirmwareVersion() {
	enterCommandMode();
	sendString("V\r\n");
	String version = getString();
	exitCommandMode();
	return version;
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
		sendString("R,1\r\n");
		if (getString() == "Reboot!\r\n") {
			_commandMode = false;
			return true;
		}
		else {
			return false;
		}
	#endif // RN41_42_RESET_PIN
}

bool RN41_42::connectToAddress(String address)
{
	return false;
}

bool RN41_42::sendMessage(String message, char terminationChar) {
	if (!_commandMode) {
		String msg = message + terminationChar;
		serial.write(printf("%s", msg.c_str()));
		return true;
	}
	else {
		return false;
	}
}

String RN41_42::recieveMessage(char terminationChar) {
	if (!_commandMode) {
		return getString(terminationChar);
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
			sendString(String(_configChar));
		}
		if (getString() == "CMD\r\n") {
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
		sendString("---\r\n");
		if (getString() == "END\r\n") {
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



void RN41_42::sendString(String msg) {
	serial.write(printf("%s", msg.c_str()));
}

String RN41_42::getString() {
	String msg = "";
//	char prev;
	char curr;
	while (serial.available()) {
//		prev = curr;
		curr = serial.read();
		msg += curr;
//		if (prev == '\r' && curr == '\n') {
//			break;
//		}
	}
	return msg;
}

String RN41_42::getString(char terminationChar) {
	String msg = "";
	char curr;
	while (serial.available()) {
		curr = serial.read();
		msg += curr;
		if (curr == terminationChar) {
			break;
		}
	}
	return msg;
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

bool RN41_42::isAOK() {
	if (getString() == "AOK\r\n") {
		exitCommandMode();
		return true;
	}
	else
	{
		exitCommandMode();
		return false;
	}
}