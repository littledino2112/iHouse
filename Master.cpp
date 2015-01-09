#include "Master.h"

Master::Master():iNumSlave(0){
	// Need to call Serial.begin() first in setup()
}

uint8_t Master::config(){
	String temp;
	Serial1.print("AT+RENEW");
    delay(3000);    // Wait for 2 secs for the module to stablize
    Serial1.print("AT+ROLE1");  // Set the module as master, default after factory reset is ROLE0
    delay(200);
    Serial1.print("AT+IMME1");  // After restart, the module just sit there waiting for AT+START or AT+CON commands
    delay(200);
    Serial1.print("AT+NOTI1");
    delay(200);
}

 uint8_t Master::discoverDevices(){
 	delay(1000);
 	Serial1.print("AT+DISC?");
 	delay(2000);	// Discovery will take some time to take place, so the delay is needed here
 }

String Master::readResponse(){
	String tempString;
	char tempChar;
	while (Serial1.available()==0){};
	while (Serial1.available()>0)
	{
		tempChar = Serial1.read();
		tempString += tempChar; 
	}
	return tempString;
}