/*
 Name:		RN41.h
 Created:	6/7/2017 9:46:16 PM
 Author:	Chris
 Editor:	http://www.visualmicro.com
*/



#ifndef _RN41_42_h
#define _RN41_42_h

//#define RN41_42_RESET_PIN
//#define RN41_42_CONN_PIN

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class RN41_42 {

	public:

		RN41_42(HardwareSerial &_serial);
		RN41_42(HardwareSerial & _serial, char configChar);
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
		bool setUARTParity(char parity);
		bool setMode(int mode);
		bool setDeviceName(String name);
		bool setExtendedStatusString(String string);
		bool setPinCode(String pin);
		bool setMask(unsigned int mask);
		bool setRemoteAddress(String address);
		bool eraseRemoteAddress();

		bool setRemoteAddressLastObserved();
		bool setServiceName(String name);

		bool setConfigTimer(unsigned int value);
		bool setUARTBaud(unsigned int baud);
		bool setSniff(String hex);
		bool setBonding(bool en);
		bool setTransmitPower(String hex);
		bool setNonStandardBaud(unsigned int multi);
		bool setProfile(int value);
		bool setSerializedFriendlyName(String name);
		bool setRoleSwitch(bool en);
		bool setConfigChar(char c);
		bool setLPConnectMode(String hex);

		//get commands
		String getBasicSettings();
		String getExtendedSettings();
		String getBluetoothAddress();
		String getConnectedRemoteAddress();
		bool getConnectionStatus();
		String getStoredRemoteAddress();
		String getGPIOStatus();
		String getFirmwareVersion();

		//action commands
		String displayDipwitchValues();
		bool connectToAddress();
		bool connectToAddress(String address);
		bool connectToAddressFast(String address);



		//Message Mode
		bool sendMessage(String message, char terminationChar);
		String recieveMessage(char terminationChar);

	private:

		HardwareSerial &serial;

		//Vaiables

		bool _commandMode;
		unsigned long _baud;
		char _configChar;

		//Private Commands
		bool enterCommandMode();
		bool exitCommandMode();

		//Send Data
		void sendString(String msg);
		String getString();
		String getString(char terminationChar);
		bool isAOK();
	};

#endif

