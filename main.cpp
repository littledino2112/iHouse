/*  This Arduino module is used to control a Spark Core which in turn controls a HM-10 module
to set up a star network formed by multiple slave HM10 modules.

    Written by Hoang Nguyen
*/
#include "Master.h"
#include "application.h"

bool DEBUG = true;
Master myMaster;
void setup()
{
	/* Initial serial communication setup */
	Serial.begin(9600); // For Debug purposes
	pinMode(D7, OUTPUT);
	digitalWrite(D7, HIGH);
	delay(5000);
    Serial1.begin(9600);

    /* Following 2 lines are used to stabilize the UART interface
    // After reset, TX line stay low until something is written into TX buffer
    // After writing something to TX buffer, a delay is needed for the HM10 to interpret the AT command
    */
    Serial1.println("Starting!");
    delay(100);
    myMaster.config();
    myMaster.discoverDevices();
    myMaster.connectDevice(myMaster.Slave[0]);

    // Spark.function("control", HM10_Control);


 //    if (DEBUG) {
 //        delay(5000);
 //        Serial1.print("AT+CONB4994C715302");
 //        delay(1000);
 //        Serial1.print("AT+PIO31");
 //        delay(5000);
 //        Serial1.print("AT"); // Will disconnect from slave with this command 
 //    }
}

void loop()
{
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
