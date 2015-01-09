#include "Master.h"

Master::Master():iNumSlave(0){
	// Need to call Serial.begin() first in setup()
}

uint32_t Master::config(){
	String responseOK="Set:1";
	String responseRenew = "RENEW";
	Serial1.print("AT+RENEW");
    Serial.print(readResponse(responseRenew));
    Serial1.print("AT+ROLE1");  // Set the module as master, default after factory reset is ROLE0
    Serial.print(readResponse(responseOK));
    Serial1.print("AT+IMME1");  // After restart, the module just sit there waiting for AT+START or AT+CON commands
    Serial.print(readResponse(responseOK));
    Serial1.print("AT+NOTI1");
    Serial.print(readResponse(responseOK));

    return 1;
}

 uint32_t Master::discoverDevices(){
	String endWith = "DISCE";
	delay(1000);
 	Serial1.print("AT+DISC?");
    Serial.print(readResponse(endWith));

 	return 1;
 }

String Master::readResponse(String ends){
	// Check if a String have ending characters match with the given argument. If it matches, end polling for new characters
	String tempString;
	char tempChar;
	while (1)	{
        if (Serial1.available()>0) {
        	tempChar = Serial1.read();
            tempString += tempChar;
//            Serial.print(tempChar);
        }
        else if (tempString.endsWith(ends)){
        	break;
        }
	}

	return tempString;
}
