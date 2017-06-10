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

		//Public Commands

		void begin(unsigned long baudrate);
		bool reset();

		//set commands
		bool setDeviceName(String name);
		bool setAuthenticationMode(int authMode);
		bool setMode(int mode); //done

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

		//Private Commands
		bool enterCommandMode();
		bool exitCommandMode();

		//Send Data
		void sendString(String msg);
		bool readable();
		String getString();
		String getString(char terminationChar);
	};

#endif

