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

#define USE_GPIO

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

		bool enterCommandMode();
		bool exitCommandMode();
		
		//Set Commands
		bool setS7(bool en);
		bool setAuthenticationMode(uint8_t authMode);
		bool setBreak(uint8_t breakVal);
		bool setServiceClass(uint8_t hex);
		bool setDeviceClass(uint8_t hex);
		bool setUUID(char hex[32]);
		bool restoreFactoryDefaults();
		bool setHIDRegister(uint8_t hex);
		bool setInquiryScanWindow(uint8_t hex);
		bool setPageScanWindow(uint8_t hex);
		bool setUARTParity(char parity);
		bool setMode(unsigned int mode);
		bool setDeviceName(char name[20]);
		bool setExtendedStatusString(char name[8]);
		bool setPinCode(char pin[4]);
		bool setMask(uint8_t mask);
		bool setRemoteAddress(char address[12]);
		bool eraseRemoteAddress();

		bool setRemoteAddressLastObserved();
		bool setServiceName(char name[20]);

		bool setConfigTimer(uint8_t value);
		bool setUARTBaud(uint8_t baud);
		bool setSniff(uint8_t hex);
		bool setBonding(bool en);
		bool setTransmitPower(uint8_t hex);
		bool setNonStandardBaud(uint8_t multi);
		bool setProfile(uint8_t value);
		bool setSerializedFriendlyName(char name[15]);
		bool setRoleSwitch(bool en);
		bool setConfigChar(char c);
		bool setLPConnectMode(uint8_t hex);

		//Get Commands
		char *getBasicSettings();
		char *getExtendedSettings();
		char *getBluetoothAddress();
		char *getConnectedRemoteAddress();
		bool getConnectionStatus();
		char *getStoredRemoteAddress();
		char *getGPIOStatus();

		//Action Commands
		char *displayDipwitchValues();
		bool connectToStoredAddress();
		bool connectToAddress(char address[13]);
		bool connectToAddressFast(char address[13]);
		bool connectToLastFoundAddressFast();
		bool connectToStoredRemoteAddressFast();
		bool connectToAddressTimed(char address[13], uint8_t time);
		bool fastDataMode();
		char *help();
		char *performInquiryScan(uint8_t time);
		char *performInquiryScan(uint8_t time, uint8_t cod);
		char *performInquiryScanNN(uint8_t time);
		char *performInquiryScanNN(uint8_t time, uint8_t cod);
		char *scanRSSI();
		char *performRovingInquiryScan(uint8_t time);
		char *performCableReplaceInquiryScan(uint8_t time);
		bool hidePIN();
		bool killConnection();
		char *linkQuality();
		char *remoteModemSignalStatus();
		char *otherSettings();
		void passMessage(char mes[32]);
		bool quietMode();
		bool passThrough(bool en);
		bool uartChangeTemp(char baud[4], char parity);
		char *getFirmwareVersion();
		bool enableDiscoveryConnection();
		void sleep();

		//GPIO Commands

#ifdef USE_GPIO
		bool pinMode(uint8_t pin, uint8_t dir);
		bool digitalWrite(uint8_t pin, uint8_t val);
		bool pinModePowerUp(uint8_t pin, uint8_t dir);
		bool digitalWritePowerUp(uint8_t pin, uint8_t val);
#endif // USE_GPIO

		//Message Mode
		bool sendMessage(char message[32]);
		char *recieveMessage();

	private:

		HardwareSerial &serial;

		//Vaiables
		char recvBuf[16];
		bool _commandMode;
		unsigned long _baud;
		char _configChar[1];

#ifdef USE_GPIO
		byte gpioSet = 0;
		byte gpioDir = 0;
		byte gpioVal = 0;
		byte gpio811 = 0;
		byte gpioSetPowerUp = 0;
		byte gpioDirPowerUp = 0;
		byte gpioValPowerUp = 0;
#endif // USE_GPIO

		//Private Commands
		void setupIO();
		char *buildHexSString(const PROGMEM char* cmd);
		char *buildSString(const PROGMEM char* cmd, bool isString);

		//Send Data
		char *getString();
		char *getString(char *buffer);
		bool isAOK();

		const PROGMEM char S[2] = { 'S','\0' };
		const PROGMEM char commaPercent[3] = { ',','%','\0' };
		const PROGMEM char comma[2] = { ',','\0' };
		const PROGMEM char str[2] = { 's','\0' };
		const PROGMEM char dec[2] = { 'd','\0' };
		const PROGMEM char pgmHex[4] = { '0','4','X','\0' };
		const PROGMEM char connected[12] = { 'C','o','n','n','e','c','t','e','d','\r','\n','\0' };
	};

#endif

