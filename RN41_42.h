/*
 Name:		RN41.h
 Created:	6/7/2017 9:46:16 PM
 Author:	Chris
 Editor:	http://www.visualmicro.com
*/



#ifndef _RN41_42_h
#define _RN41_42_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class RN41_42 {

	public:

		RN41_42(HardwareSerial &_serial);
		void begin(unsigned long baudrate);
		void begin();
		bool reset();

		//set commands
		bool setS7(bool en);
		bool setAuthenticationMode(int authMode);
		bool setBreak(int breakVal);
		bool setServiceClass(String hex);
		bool setDeviceClass(String hex);
		bool setUUID(String hex);
		bool restoreFactoryDefaults();
		bool setHIDRegister(String hex);
		bool setInquiryScanWindow(String hex);
		bool setPageScanWindow(String hex);
		bool setUARTParity(String parity);
		bool setDeviceName(String name);
		bool setExtendedStatusString(String string);
		bool setPinCode(unsigned int pin);
		bool setMask(unsigned int mask);
		bool setRemoteAddress(String address);
		bool setServiceName(String name);
		bool setConfigTimer(unsigned int value);
		bool setUARTBaud(unsigned int baud);
		bool setMode(int mode);
		bool eraseRemoteAddress();
		bool setRemoteAddressLastObserved();

		

		//get commands
		String getBluetoothAddress();
		bool getConnectionStatus();
		String getFirmwareVersion();

		//action commands
		bool connectToAddress(String address);

		//Message Mode
		bool sendMessage(String message, char terminationChar);
		String recieveMessage(char terminationChar);

	private:

		HardwareSerial &serial;

		//Vaiables
		bool _commandMode;
		unsigned long _baud;

		//Private Commands
		bool enterCommandMode();
		bool exitCommandMode();

		//Send Data
		void sendString(String msg);
		bool readable();
		String getString();
		String getString(char terminationChar);
		bool isAOK();
	};

#endif

