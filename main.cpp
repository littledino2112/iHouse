/*  This Arduino module is used to control a Spark Core which in turn controls a HM-10 module
to set up a star network formed by multiple slave HM10 modules.

    Written by Hoang Nguyen
*/
#include "Master.h"

bool DEBUG = true;
void setup()
{
    Serial1.begin(9600);
    Master myMaster;
    myMaster.config();
    myMaster.discoverDevices();

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
