#include "Master.h"

Master::Master():numSlave(0){
	// Need to call Serial.begin() first in setup()
}

uint32_t Master::config(){
	String responseOK="Set:1";
	String responseRenew = "RENEW";
	Serial1.print("AT");
	Serial.println(readResponse("OK"));
	Serial1.print("AT+RENEW");
    Serial.println(readResponse(responseRenew));
    Serial1.print("AT+ROLE1");  // Set the module as master, default after factory reset is ROLE0
    Serial.println(readResponse(responseOK));
    Serial1.print("AT+IMME1");  // After restart, the module just sit there waiting for AT+START or AT+CON commands
    Serial.println(readResponse(responseOK));
    Serial1.print("AT+NOTI1");
    Serial.println(readResponse(responseOK));

    return 1;
}

 uint32_t Master::discoverDevices(){
	String endWith = "DISCE";
	delay(1000);
 	Serial1.print("AT+DISC?");
    // Read discovered MAC addresses to see how many devices are found
    String result = readResponse(endWith);
    Serial.println(result);

    numSlave = (result.length() - 8 - 8)/20; //This formula bases on the format of response from HM10
    Serial.print("Length of the string: ");
    Serial.println(result.length());
    Serial.print("Number of slaves: ");
    Serial.println(numSlave);

    for (int i=0; i<numSlave; i++){
    	Slave[i] = result.substring(16+20*i, 16+20*i+12);
    	Serial.print("Discovered device: ");
    	Serial.println(Slave[i]);
    }

 	return numSlave;
 }

uint32_t Master::connectDevice(String& address){
	String command = "AT+CON" + address;
	Serial1.print(command);
	delay(500);
	String connectionRsp = readResponse("", false);
	if (connectionRsp.substring(11)=="CONN"){
		Serial.println("Successfully connected!");
		return 1;
	}
	else
		Serial.println("Connection failed!");
		return -1;
}

String Master::readResponse(String ends, bool endWith){
	// Check if a String have ending characters match with the given argument. If it matches, end polling for new characters
	String tempString;
	char tempChar;
	while (1)	{
        if (Serial1.available()>0) {
        	tempChar = Serial1.read();
            tempString += tempChar;
        }
        else if (endWith){
        	if (tempString.endsWith(ends)){
        		break;
        	}
        }
        else 	// endWidth = false, no need to check ending of responses
       		break;
	}

	return tempString;
}
