/*
 Name:		RN41.h
 Created:	6/7/2017 9:46:16 PM
 Author:	Chris
 Editor:	http://www.visualmicro.com
*/



#ifndef _RN41_42_h
#define _RN41_42_h

//Define pin to pin connections
#define RN41_42_RESET  PIN_A6     //Reset
//#define RN41_42_GPIO2           //Connection State
//#define RN41_42_GPIO3           //Auto Discovery/Pairing
//#define RN41_42_GPIO4           //Factory Reset
//#define RN41_42_GPIO6           //Auto-Connect
//#define RN41_42_GPIO7           //Baud Rate

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

		//Dipswitch/GPIO Functions
		#ifdef RN41_42_GPIO4
			void factoryReset();
		#endif // RN41_42_GPIO4
		#ifdef RN41_42_GPIO3
			void autoDiscoveryPairing(bool en);
		#endif // RN41_42_GPIO3
		#ifdef RN41_42_GPIO6
			void autoConnect(bool en);
		#endif // RN41_42_GPIO6
		#ifdef RN41_42_GPIO7
			void setBaudRate9600(bool en);
		#endif // RN41_42_GPIO7

		//set commands
		bool setS7(bool en);
		bool setAuthenticationMode(int authMode);
		bool setBreak(int breakVal);
		bool setServiceClass(char hex[4]);
		bool setDeviceClass(char hex[4]);
		bool setUUID(char hex[32]);
		bool restoreFactoryDefaults();
		bool setHIDRegister(char hex[4]);
		bool setInquiryScanWindow(char hex[4]);
		bool setPageScanWindow(char hex[4]);
		bool setUARTParity(char parity);
		bool setMode(int mode);
		bool setDeviceName(char name[20]);
		bool setExtendedStatusString(char name[8]);
		bool setPinCode(char pin[4]);
		bool setMask(unsigned int mask);
		bool setRemoteAddress(char address[12]);
		bool eraseRemoteAddress();

		bool setRemoteAddressLastObserved();
		bool setServiceName(char name[20]);

		bool setConfigTimer(unsigned int value);
		bool setUARTBaud(unsigned int baud);
		bool setSniff(char hex[4]);
		bool setBonding(bool en);
		bool setTransmitPower(char hex[4]);
		bool setNonStandardBaud(unsigned int multi);
		bool setProfile(int value);
		bool setSerializedFriendlyName(char name[15]);
		bool setRoleSwitch(bool en);
		bool setConfigChar(char c);
		bool setLPConnectMode(char hex[4]);

		//get commands
		char *getBasicSettings();
		char *getExtendedSettings();
		char *getBluetoothAddress();
		char *getConnectedRemoteAddress();
		bool getConnectionStatus();
		char *getStoredRemoteAddress();
		char *getGPIOStatus();
		char *getFirmwareVersion();

		//action commands
		char *displayDipwitchValues();
		bool connectToAddress();
		bool connectToAddress(char address[12]);
		bool connectToAddressFast(char address[12]);


		//Message Mode
		bool sendMessage(char message[32], char terminationChar);
		char *recieveMessage();

	private:

		HardwareSerial &serial;

		//Vaiables
		char buf[32];
		bool _commandMode;
		unsigned long _baud;
		char _configChar[1];

		//Private Commands
		bool enterCommandMode();
		bool exitCommandMode();
		void setupIO();
		void emptyBuffer();

		//Send Data
		void sendString(char msg[]);
		void sendString(const __FlashStringHelper* msg);
		char *getString();
		bool isAOK();
	};

#endif

