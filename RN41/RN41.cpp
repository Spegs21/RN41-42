/*
 Name:		RN41.cpp
 Created:	6/7/2017 9:46:16 PM
 Author:	Chris
 Editor:	http://www.visualmicro.com
*/

#include "RN41.h"

RN41::RN41(HardwareSerial &_serial): serial(_serial) {
	_commandMode = false;
}

void RN41::begin(unsigned long baudrate)
{
	serial.begin(baudrate);
}

//Resets The Device
//Also Exits Command Mode
//Device Must Be Reset After A Config Change To Take Effect
bool RN41::reset() {
	enterCommandMode();
	sendString("R,1\r\n");
	if (getString() == "Reboot!\r\n") {
		_commandMode = false;
		return true;
	}
	else {
		return false;
	}
}

//Sets Device Name
//Maximum 20 characters
bool RN41::setDeviceName(String name) {
	enterCommandMode();
	sendString("SN," + name + "\r\n");
	if (getString() == "AOK\r\n") {
		reset();
		return true;
	}
	else {
		return false;
	}
}

//Set Authentication Mode
//Available Modes:
//0 - Open Mode
//1 - SSP Keyboard I/O Mode
//2 - SSP "Just Works" Mode
//4 - Pin Code
//Default Mode: 1
bool RN41::setAuthenticationMode(int authMode) {
	enterCommandMode();
	if (authMode < 0 || authMode > 4 || authMode == 3) {
		return false;
	}
	char buf[1];
	sprintf(buf, "SA,%d\r\n", authMode);
	String msg = buf;
	sendString(msg);
	if (getString() == "AOK\r\n") {
		reset();
		return true;
	}
	else {
		return false;
	}
}

//Sets Device Mode
//Available Modes:
//0 - Slave Mode
//1 - Master Mode
//2 - Trigger Mode
//3 - Auto-Connect Master Mode
//4 - Auto-Connect DTR Mode
//5 - Auto-Connect Any Mode
//6 - Pairing Mode
//Default Mode: 4
bool RN41::setMode(int mode) {
	enterCommandMode();
	if (mode < 0 || mode > 6) {
		return false;
	}
	char buf[1];
	sprintf(buf, "SM,%d\r\n", mode);
	String msg = buf;
	sendString(msg);
	if (getString() == "AOK\r\n") {
		reset();
		return true;
	}
	else {
		return false;
	}
}

//Gets Bluetooth Address
//returns the 12 didgite MAC ID
String RN41::getBluetoothAddress() {
	enterCommandMode();
	sendString("GB\r\n");
	String result = getString();
	String address = result.substring(0, result.length() - 2);
	exitCommandMode();
	return address;
}

//Gets Connection Status
//0,0,0 = Not Connected
//1,0,0 = Connected
bool RN41::getConnectionStatus() {
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
}

//Get The Device's Firmware Version
String RN41::getFirmwareVersion() {
	enterCommandMode();
	sendString("V\r\n");
	String version = getString();
	exitCommandMode();
	return version;
}

bool RN41::connectToAddress(String address)
{
	return false;
}

bool RN41::sendMessage(String message, char terminationChar) {
	if (_commandMode) { return false; }
	String msg = message + terminationChar;
	serial.write(printf("%s", msg.c_str()));
	return true;
}

String RN41::recieveMessage(char terminationChar) {
	return getString(terminationChar);
	if (_commandMode) { return "*ERROR*"; }
}

//Private Functions Below

//Enter Command Mode
bool RN41::enterCommandMode() {
	if (_commandMode == true) {
		return true;
	}
	sendString("$$$");
	if (getString() == "CMD\r\n") {
		_commandMode = true;
		return true;
	}
	else {
		return false;
	}
}

//Exit Command Mode
bool RN41::exitCommandMode() {
	if (_commandMode == false) {
		return true;
	}
	sendString("---\r\n");
	if (getString() == "END\r\n") {
		_commandMode = false;
		return true;
	}
	else {
		return false;
	}
}

void RN41::sendString(String msg) {
	serial.write(printf("%s", msg.c_str()));
}

bool RN41::readable() {
	return serial.available();
}

String RN41::getString() {
	String msg = "";
	char prev = ' ';
	char curr = ' ';
	while (1) {
		if (serial.available()) {
			prev = curr;
			curr = serial.read();
			msg += curr;
			if (prev == '\r' && curr == '\n') {
				break;
			}
		}
	}

	return msg;
}

String RN41::getString(char terminationChar) {
	String msg = "";
	char curr = ' ';
	while (1) {
		if (serial.available()) {
			curr = serial.read();
			msg += curr;
			if (curr == terminationChar) {
				break;
			}
		}
	}
	return msg;
}