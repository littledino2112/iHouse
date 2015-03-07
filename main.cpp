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
const uint8_t eeprom_address = 1;
const uint8_t chip_select = A2;
uint16_t image_count;
//uint8_t folder_num;

// Forward declaration
int findDevice(String command);
int HM_IOControl(String);
int checkIOState(String);
int takePicture(String);
void signalDoneConfig();
void setTimeStamp(uint16_t* date, uint16_t* time);

void setup()
{
	/* Initial serial communication setup */
	Serial.begin(115200); // For Debug purposes
	Spark.syncTime();
	pinMode(D7, OUTPUT);
	digitalWrite(D7, LOW);
    Serial1.begin(9600);	// Somehow, the Serial1 needs to be initialized before Serial2!
    Serial2.begin(115200);	// Doesn't work the other way around
    // Reading Folder info from EEPROM
//    folder_num = EEPROM.read(eeprom_address);

    while (!Serial.available());	// This is for debugging purpose only
	Serial.println("Starting!");

    Serial.println("Initialize SD card...");
	if (!SD.begin(chip_select)){
		Serial.println("SD card failed to initialized!");
		return ;
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
    Spark.function("takePicture", takePicture);
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

/* @brief	Take a picture once commanded by the Spark cloud
 * @detail	None
 * @param[in]	cmd	A string sent from Spark Cloud
 * @return		int	1 if picture taken succesfully, 0 otherwise
 */
int takePicture(String cmd){
	digitalWrite(D7, HIGH);	// Indicate photo is being taken

	uint8_t day_to_compare = EEPROM.read(1);	// day is stored in Address 1
	uint8_t month_to_compare = EEPROM.read(2);	// month is stored in Address 2
	Serial.print("Day stored is ");
	Serial.println(day_to_compare);
	Serial.print("Month stored is ");
	Serial.println(month_to_compare);
	if ((day_to_compare!=Time.day())&&(month_to_compare!=Time.month())){
		Serial.println("Day and month are not matched.");
		image_count = 1;
		EEPROM.write(1,Time.day());
		EEPROM.write(2,Time.month());
	}
	else {
		Serial.println("Day and month are matched");
		image_count = EEPROM.read(3);
		Serial.print("Image count is ");
		Serial.println(image_count);
	}

	for (int count=1;count<=7;count++){
		// If takePicture() is ever called, the folder_num will be incremented and saved to EEPROM
//		if (image_count==1){
//			EEPROM.write(eeprom_address,++folder_num);
//		}
		Serial.println("Taking picture!");
        if(myCamera.resumeVideo()) Serial.println("Resume video.");
        delay(500);	// a delay is needed btw resumeVideo() and takePicture() in order to get a proper snapshot
        if(myCamera.takePicture()) Serial.println("Snapped!");

        // Open an image on SD card to write
//        char image_name[11]="img";//follow 8.3 DOS file name format
//        char image_count_str[5];
//        sprintf(image_count_str,"%d",image_count);
//        strcat(image_name,image_count_str);
//        char extension[]=".jpg";
//        strcat(image_name,extension);
//
//        char image_path[20]="FOLDER";
//        char folder_number_char[3];	// Maxium is 255
//        sprintf(folder_number_char, "%d", folder_num);
//        strcat(image_path,folder_number_char);
//        Serial.print("Making directory ");
//        Serial.println(image_path);
//		Serial.println(SD.mkdir(image_path));
//
//        strcat(image_path,"/");
//        strcat(image_path,image_name);
//        Serial.println(image_path);
//
//        myImage = SD.open(image_path, FILE_WRITE);
        char image_path[12]="";
        char temp[2]="";
        sprintf(temp,"%.2d", Time.day());
        strcat(image_path,temp);
        sprintf(temp,"%.2d", Time.month());
        strcat(image_path,temp);
        strcat(image_path,"_");
        sprintf(temp,"%d",image_count);
        strcat(image_path,temp);
        strcat(image_path,".jpg");
        Serial.print("Image path is ");
        Serial.println(image_path);
        Serial.print("Size of image path is ");
        Serial.println(sizeof(image_path));
        myImage = SD.open(image_path, FILE_WRITE);

        if (myImage){
            int32_t image_size = myCamera.getImageLength();
            Serial.println(image_size);
            uint8_t bytes_to_read = 128;
            unsigned long time_begin = millis();
            while (image_size>0){
                if (image_size<bytes_to_read){
                    bytes_to_read=image_size;
                }
                uint8_t* stuffed_partial_image=myCamera.readPictureData(bytes_to_read);
                for (int i=5; i<bytes_to_read+5;i++){
                    myImage.write(*(stuffed_partial_image+i));
                }
                image_size=image_size-bytes_to_read;
                Serial.print(".");
            }
            myImage.close();
            unsigned long time_taken = millis()-time_begin;
            Serial.println("Image saved!");
            Serial.print("Time to take 1 image(ms): ");
            Serial.println(time_taken);
            image_count++;	//increase image count
        }
        else {
            Serial.println("Failed to open file");
            return 0;
        }
	}
	EEPROM.write(3,image_count);
	digitalWrite(D7,LOW);
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

