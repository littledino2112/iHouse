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
#ifndef __MASTER_H
#define __MASTER_H

 class Master
 {
 public:
 	Master();
 	// Confiure the Master module, reset it, put it in ROLE1, IMME1 and NOTI1 modes
 	uint32_t config();

 	uint32_t discoverDevices();

 	String readResponse(String ends="");
 public:
 	uint32_t iNumSlave;
 	String Slave[10]; // store discovered Slave's addresses
 };

 #endif
