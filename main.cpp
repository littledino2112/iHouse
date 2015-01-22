/*  This Arduino module is used to control a Spark Core which in turn controls a HM-10 module
to set up a star network formed by multiple slave HM10 modules.

    Written by Hoang Nguyen
*/
#include "Master.h"
#include "application.h"
#include <string.h>

const bool DEBUG=1;
Master myMaster;

// Forward declaration
int findDevice(String command);
int HM_IOControl(String);
void signalDoneConfig();

void setup()
{
	/* Initial serial communication setup */
	Serial.begin(9600); // For Debug purposes
	pinMode(D7, OUTPUT);
//	digitalWrite(D7, LOW);
//	delay(5000);	// wait for 5s so one can connect to serial monitor for watching results!
    Serial1.begin(9600);
	Serial.println("Starting!");

    /* Following 2 lines are used to stabilize the UART interface
    // After reset, TX line stay low until something is written into TX buffer
    // After writing something to TX buffer, a delay is needed for the HM10 to interpret the AT command
    */
    Serial1.println("Starting!");
    delay(100);
    myMaster.config();
    signalDoneConfig();
    Spark.function("findDevice", findDevice);
    Spark.variable("numSlave",&myMaster.numSlave, INT);
    Spark.variable("foundDevices", &myMaster.SlaveArray, STRING);
    Spark.function("io_control", HM_IOControl);
}

void loop()
{
//	myMaster.connectDevice(myMaster.Slave[1]);
//	myMaster.setIODevice(3, Master::LO);
//	myMaster.disconnect();
//	delay(1000);
}

int HM_IOControl(String command){
	char slaveAddress[15];
	char state[2];
	command.substring(0,12).toCharArray(slaveAddress, sizeof(slaveAddress), 0);
	command.substring(13).toCharArray(state, sizeof(state), 0);
	myMaster.connectDevice(slaveAddress);
	if (strcmp(state,"1")==0)
		myMaster.setIODevice(3, Master::HI);
	else if (strcmp(state,"0")==0)
		myMaster.setIODevice(3, Master::LO);
	myMaster.disconnect();
	if (DEBUG){
		Serial.println(command);
		Serial.println(slaveAddress);
		Serial.println(state);
	}
    return 1;
}

int findDevice(String command){
    myMaster.discoverDevices();
   	return 1;
}

void signalDoneConfig(){
	for (int i=0; i<5; i++){
		digitalWrite(D7, HIGH);
		delay(100);
		digitalWrite(D7, LOW);
		delay(100);
	}
}
