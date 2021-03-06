/* This class represents the Master HM10 module
Class members:
uint32_t iNumSlave: number of discovered slaves
String Slave[50]: store Slave's addresses

Class methods:
connectSlave(String address):  Connect to a slave with provided address
config(): Self-config the master module: RENEW -> ROLE1 -> IMME1
String getSlaveResponse():	Get reponses from the connected slave
 */

#include "application.h"
#include <string.h>
#ifndef __MASTER_H
#define __MASTER_H

class Master
{
private:
 	String responseSet = "Set:1";
 	static uint8_t const MAX_DEVICES = 10;

public:
 	Master();

 	enum GPIOState {
 	 		LO = 0,
 			HI = 1
 	 	};

 	// Configure the Master module, reset it, put it in ROLE1, IMME1 and NOTI1 modes.
 	uint32_t config();

 	/* Discover other HM-10 modules in slave modes (advertising and connectable).
 	 * Output: numSlave (number of discovered Slaves).
 	 */
 	uint32_t discoverDevices();

 	/* Connect to a device described by its MAC address. After establishing connection,
 	 * set connected slave to MODE 1. In this mode, the Master can control GPIO 2&3 and their state
 	 * are saved even if the connection is lost.
 	 * Inputs: MAC address of the device want to connect to.
 	 * Output: 1 if successfully connected, -1 if fail to connect.
 	 */
 	uint32_t connectDevice(const char* address);

 	/* Disconnect to a device
 	 *
 	 */
 	uint32_t disconnect();

 	/* Read AT responses from the HM10 master module
 	 * Inputs: String ends, bool endWidth. If String [ends] is provided, the func will keep reading
 	 * the Serial port until it finds the indicated String.
 	 * If [endWidth] is provided with False. The function will read the Serial port until there's no data
 	 * available in the buffer.
 	 */
 	String readResponse(String ends="", bool endWith=true);

 	/* Set GPIO 2 or 3 of the connected device to 1
 	 *
 	 */
 	uint32_t setIODevice(uint8_t pin, GPIOState state);

 	uint32_t checkIOState(uint8_t pin);

public:
 	uint32_t numSlave;
// 	String Slave[50]; // store discovered Slave's addresses
 	char Slave[10][50];
 	char SlaveArray[MAX_DEVICES*12];
 	bool DEBUG=true;
};

 #endif
