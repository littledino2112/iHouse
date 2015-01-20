/*  This Arduino module is used to control a Spark Core which in turn controls a HM-10 module
to set up a star network formed by multiple slave HM10 modules.

    Written by Hoang Nguyen
*/
#include "Master.h"
#include "application.h"
#include <string.h>

const bool DEBUG=1;
Master myMaster;
int findDevice(String command); //Forward declaration
void setup()
{
	/* Initial serial communication setup */
	Serial.begin(9600); // For Debug purposes
	pinMode(D7, OUTPUT);
	digitalWrite(D7, HIGH);
	delay(5000);	// wait for 5s so one can connect to serial monitor for watching results!
    Serial1.begin(9600);
	Serial.println("Starting!");

    /* Following 2 lines are used to stabilize the UART interface
    // After reset, TX line stay low until something is written into TX buffer
    // After writing something to TX buffer, a delay is needed for the HM10 to interpret the AT command
    */
    Serial1.println("Starting!");
    delay(100);
    myMaster.config();
//     Spark.function("control", HM10_Control);
    Spark.function("findDevice", findDevice);
    Spark.variable("numSlave",&myMaster.numSlave, INT);
    Spark.variable("foundDevices", &myMaster.SlaveArray, STRING);
//    for (uint8_t i=0; i<myMaster.numSlave; i++){
//    	char temp[2];
//    	char tempCommand[10]="slave";
//    	sprintf(temp, "%d", i);
//    	strcat(tempCommand, temp);
//    	Spark.variable(tempCommand, myMaster.Slave[i], STRING);
//    	if (DEBUG){
//    		Serial.println(tempCommand);
//    	}
//    }
    if (DEBUG){
    	Serial.println("Done discovery!");
    }
}

void loop()
{
//	myMaster.connectDevice(myMaster.Slave[1]);
//	myMaster.setIODevice(3, Master::LO);
//	myMaster.disconnect();
//	delay(1000);
}

int HM10_Control(String command){
    if (command=="1"){
    	// Send AT command: AT+PIO31 to turn attached LED ON
    	Serial1.print("AT+PIO31");
    }
    else if (command=="0"){
    	// Send AT command: AT+PIO30 to turn attached LED OFF
    	Serial1.print("AT+PIO30");
    }
    return 1;
}

int findDevice(String command){
    myMaster.discoverDevices();
   	return 1;
}
