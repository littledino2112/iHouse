/*  This Arduino module is used to control a Spark Core which in turn controls a HM-10 module
to set up a star network formed by multiple slave HM10 modules.

    Written by Hoang Nguyen
*/
#include "Master.h"
#include "application.h"
#include <string.h>
#include "VC0706_camera.h"
#include "SD.h"
#include "SdFat.h"

Master myMaster;
VC0706 myCamera;
File myImage;

const bool DEBUG=1;
const uint8_t chip_select = A2;
uint16_t image_count = 1;

// Forward declaration
int findDevice(String command);
int HM_IOControl(String);
int checkIOState(String);
void signalDoneConfig();
void setTimeStamp(uint16_t* date, uint16_t* time);

void setup()
{
	/* Initial serial communication setup */
	Serial.begin(115200); // For Debug purposes
	Spark.syncTime();
	pinMode(D7, OUTPUT);
    Serial1.begin(9600);
    while (!Serial.available());
	Serial.println("Starting!");

	// Initialize SD card
	Serial.println("Initialize SD card...");
	if (!SD.begin(chip_select)){
		Serial.println("SD card failed to initialized!");
		return;
	}

	// Initialize Camera VC0706
	Serial.println("Initialize Camera...");
	if (strcmp(myCamera.getCameraVersion(),"")==0){
		Serial.println("Camera not found!");
		return;
	}
	else {
		Serial.println("Camera found!");
		Serial.print("Camera version: ");
		Serial.println(myCamera.camera_version);
		SdFile::dateTimeCallback(setTimeStamp);
	}

    /* Following 2 lines are used to stabilize the UART interface
    // After reset, TX line stay low until something is written into TX buffer
    // After writing something to TX buffer, a delay is needed for the HM10 to interpret the AT command
    */
	Serial.println("Initialize HM10 master...");
    Serial1.println("Starting!");
    delay(100);
    myMaster.config();
    signalDoneConfig();
    Spark.function("findDevice", findDevice);
    Spark.variable("numSlave",&myMaster.numSlave, INT);
    Spark.variable("foundDevices", &myMaster.SlaveArray, STRING);
    Spark.function("io_control", HM_IOControl);
    Spark.function("checkIO",checkIOState);
}

void loop()
{
//	myMaster.connectDevice(myMaster.Slave[1]);
//	myMaster.setIODevice(3, Master::LO);
//	myMaster.disconnect();
//	delay(1000);
}

/* Spark function HM_IOControl: control GPIO (2 or 3) of the connected slave device
 * Argument: [String] command in the following format: [mac address: 12 characters],[state: 0 or 1]
 */
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

/* Spark function findDevice: trigger discovery of slave devices
 * Argument: String command
 */
int findDevice(String command){
    myMaster.discoverDevices();
   	return 1;
}

/* Spark function checkIOState: trigger checking of GPIO state of slave device
 * Argument: String command: consists of device's mac address + gpio that want to check (2 or 3)
 * command in the following format: [mac address],[2 or 3]
 * Output: 0 for OFF, 1 for ON
 */
int checkIOState(String command){
	if (DEBUG){
		Serial.println("Checking IO state: " + command);
	}
	char slaveAddress[15];
	char gpio[2];
	command.substring(0,12).toCharArray(slaveAddress, sizeof(slaveAddress), 0);
	command.substring(13).toCharArray(gpio, sizeof(gpio), 0);
	myMaster.connectDevice(slaveAddress);
	int result;
	if (strcmp(gpio, "2")==0){
		result = myMaster.checkIOState(2);
		myMaster.disconnect();
		return result;
	}
	else if (strcmp(gpio, "3")==0){
		result = myMaster.checkIOState(3);
		myMaster.disconnect();
		return result;
	}

	else {
		myMaster.disconnect();
		return -1;
	}


}

void signalDoneConfig(){
	for (int i=0; i<5; i++){
		digitalWrite(D7, HIGH);
		delay(100);
		digitalWrite(D7, LOW);
		delay(100);
	}
}

void setTimeStamp(uint16_t* date, uint16_t* time){
	uint16_t year;
	uint8_t month, day, hour, minute, second;

	year = Time.year();
	month = Time.month();
	day = Time.day();
	hour = Time.hour();
	minute = Time.minute();
	second = Time.second();

	*date = FAT_DATE(year, month, day);
	*time = FAT_TIME(hour, minute, second);
}

