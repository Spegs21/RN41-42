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
  char recvBuf[32];
  RN41_42(HardwareSerial& _serial);
  RN41_42(HardwareSerial& _serial, char configChar);
  void begin(unsigned long baudrate);
  void begin();
  bool enterCommandMode();
  bool exitCommandMode();
  bool sendMessage(char message[33]);
  char *recieveMessage();

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

  //Set Commands
  bool setS7(bool en);							//S7
  bool setAuthenticationMode(uint8_t authMode);	//SA
  bool setBreak(uint8_t breakVal);				//SB
  bool setServiceClass(uint16_t hex);			//SC
  bool setDeviceClass(uint16_t hex);			//SD
  bool setUUID(char hex[32]);					//SE
  bool restoreFactoryDefaults();				//SF,1
  bool setHIDRegister(uint16_t hex);			//SH
  bool setInquiryScanWindow(uint16_t hex);		//SI
  bool setPageScanWindow(uint16_t hex);			//SJ
  bool setUARTParity(char parity);				//SL
  bool setMode(unsigned int mode);				//SM
  bool setDeviceName(char name[21]);			//SN
  bool setExtendedStatusString(char name[9]);	//SO
  bool setPinCode(char pin[5]);					//SP
  bool setMask(uint8_t mask);					//SQ
  bool setRemoteAddress(char address[13]);		//SR
  bool eraseRemoteAddress();					//SR,Z
  bool setRemoteAddressLastObserved();			//SR,I
  bool setServiceName(char name[21]);			//SS
  bool setConfigTimer(uint8_t value);			//ST
  bool setUARTBaud(uint8_t baud);				//SU
  bool setSniff(uint16_t hex);					//SW
  bool setBonding(bool en);						//SX
  bool setTransmitPower(uint16_t hex);			//SY
  bool setNonStandardBaud(uint8_t multi);		//SZ
  bool setProfile(uint8_t value);				//S~
  bool setSerializedFriendlyName(char name[16]);//S-
  bool setRoleSwitch(bool en);					//S?
  bool setConfigChar(char c);					//S$
  bool setLPConnectMode(uint8_t hex);			//S|

  //Get Commands
  char *getBasicSettings();			//D
  char *getExtendedSettings();		//E
  char *getBluetoothAddress();		//GB
  char *getConnectedRemoteAddress();//GF
  bool getConnectionStatus();		//GK
  char *getStoredRemoteAddress();	//GR
  char *getGPIOStatus();			//G&

  //Action Commands
  char *displayDipwitchValues();								//&
  bool connectToStoredAddress();								//C
  bool connectToAddress(char address[13]);						//C,<address>
  bool connectToAddressFast(char address[13]);					//CF
  bool connectToLastFoundAddressFast();							//CFI
  bool connectToStoredRemoteAddressFast();						//CFR
  bool connectToAddressTimed(char address[13], uint8_t time);	//CT
  bool fastDataMode();									    	//F,1
  char *help();													//H
  char *performInquiryScan(uint8_t time);						//I,<value 1>
  char *performInquiryScan(uint8_t time, uint32_t cod);			//I,<value 1>,<value 2>
  char *performInquiryScanNN(uint8_t time);						//IN,<value 1>
  char *performInquiryScanNN(uint8_t time, uint32_t cod);		//IN,<value 1>,<value 2>
  char *scanRSSI();												//IQ
  char *performRovingInquiryScan(uint8_t time);					//IS
  char *performCableReplaceInquiryScan(uint8_t time);			//IR
  bool hidePIN();												//J
  bool killConnection();										//K,
  char *linkQuality();											//L
  char *remoteModemSignalStatus();								//M
  char *otherSettings();										//O
  void passMessage(char mes[33]);								//P
  bool quietMode();												//Q
  bool quietMode(uint8_t mode);									//Q,0 or Q,1 or Q,2
  uint8_t quietStatus();										//Q,?
  bool reset();													//R,1 or GPIO
  bool passThrough(bool en);									//T
  bool uartChangeTemp(char baud[5], char parity);				//U
  char *getFirmwareVersion();									//V
  bool enableDiscoveryConnection();								//W
  void sleep();													//Z

  //GPIO Commands
  bool pinMode(uint8_t pin, uint8_t dir);				//S@
  bool digitalWrite(uint8_t pin, uint8_t val);		    //S& and S*
  bool pinModePowerUp(uint8_t pin, uint8_t dir);		//S%
  bool digitalWritePowerUp(uint8_t pin, uint8_t val);	//S^

private:

  //Vaiables
  HardwareSerial& serial;
  bool _commandMode;
  unsigned long _baud;
  char _configChar[2];
  char sBuf[10];

  //GPIO Bitmasks
  uint8_t gpioSetDir = 0U;
  uint8_t gpioDir = 0U;
  uint8_t gpioSetVal = 0U;
  uint8_t gpioVal = 0U;
  uint8_t gpio811 = 0U;
  uint8_t gpioSetDirPowerUp = 0U;
  uint8_t gpioDirPowerUp = 0U;
  uint8_t gpioSetValPowerUp = 0U;
  uint8_t gpioValPowerUp = 0U;

  //Private Commands
  void setupIO();
  char *buildHexSString(const PROGMEM char* cmd);
  char *buildSString(const PROGMEM char* cmd, bool isString);

  //Recieve Data
  char *getString();

  //Response Check
  bool isAOK();

  //Constants
  const PROGMEM char S[2] = { 'S','\0' };
  const PROGMEM char commaPercent[3] = { ',','%','\0' };
  const PROGMEM char comma[2] = { ',','\0' };
  const PROGMEM char str[2] = { 's','\0' };
  const PROGMEM char dec[2] = { 'd','\0' };
  const PROGMEM char pgmHex[4] = { '0','4','X','\0' };
  const PROGMEM char connected[12] = { 'C','o','n','n','e','c','t','e','d','\r','\n','\0' };
  const PROGMEM char quiet[8] = { 'Q','u','i','e','t','\r','\n','\0' };
};

#endif

