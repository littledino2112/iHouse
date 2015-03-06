/*
 * VC0706_camera.cpp
 *
 *  Created on: Feb 28, 2015
 *      Author: littledino2112
 */

#include "VC0706_camera.h"
#include "Serial2.h"

VC0706::VC0706():frame_pointer(0), buffer_counter(0){
	//Initilize Serial2 port used to communicate with the camera
//	Serial2.begin(115200);
}

/*	@brief Send basic commands to the Camera. This normally doesn't require any extra data to be sent.
 *	@detail Basic commands only require the following format: 0x56 0x00 [command] 0x00
 *	@param[in] code   belongs to enum COMMAND_CODES, this is the command to be sent
 */
void VC0706::sendCmd(COMMAND_CODES code, uint8_t args[], uint8_t args_num){
	send(0x56);
	send(0x00);
	send(code);
	for (int i=0; i<args_num; i++){
		send(args[i]);
	}
}

/* @brief Read serial response from the Camera's UART
 * @detail
 * @param[in] data_length	Provided data length to read
 * @param[in] timeout_ms	Timeout in ms. Function will return fail after timeout
 * @return byte_read	Return number of bytes read in this period
 */
uint8_t VC0706::readResponse(uint8_t data_length, uint16_t timeout_ms){
	uint16_t timeout_counter = 0;
	buffer_counter = 0;

	while((timeout_counter!=timeout_ms)&&(buffer_counter!=data_length)){
		if (!Serial2.available()){
			timeout_counter++;
			delay(1);
			continue;
		}
		else{
			camera_buffer[buffer_counter++]=Serial2.read();
		}
	}
	return buffer_counter;
}

/* @brief	Retrieve camera DSP version
 * @detail	This is done by sending 0x11 command to camera
 * @param[in]
 * @return	Version of camera's DSP
 */
char* VC0706::getCameraVersion(void){
	uint8_t command_args[] = {0x00};
	sendCmd(VC0706::GET_FIRMWARE_VERSION, command_args, sizeof(command_args));
	if (!readResponse(100, 200)) return (char*) "Failed to read";
	else {
		for (int i=0,j=0; j<buffer_counter;j++){	//i is index for camera_version, j is for camera_buffer
			if (j>=5){								//only copy from the 5th byte of camera_buffer
				camera_version[i] = camera_buffer[j];
				i++;
			}
		}
		return camera_version;
	}
}

/*	@brief	Command camera to take one image
 *	@detail Send a [stop current frame] command to Frame Buffer Control register
 *	@param[in]	none
 *	@return		uint8_t	0x01 if responded bytes match {0x76,0x00,0x36,0x00,0x00}; return 0x00 if they don't
 */
uint8_t VC0706::takePicture(){
	uint8_t command_args[]={0x01,FBUF_STOP_CURRENT_FRAME};
	sendCmd(FBUF_CTRL, command_args, sizeof(command_args));
	readResponse(20,200);	//Read 20 response bytes to camera_buffer variables
	uint8_t expected_result[]={0x76,0x00,0x36,0x00,0x00};
	if (memcmp(expected_result, camera_buffer, 5*sizeof(uint8_t))==0) {
		frame_pointer=0;	// Reset frame pointer to 0
		return 0x01;
	}
	else return 0x00;
}

/*	@brief	Get image length of current frame in the buffer
 * 	@detail
 * 	@param[in]	none
 * 	@return	uint16_t image length stored in buffer
 */
uint16_t VC0706::getImageLength(){
	uint8_t command_args[]={0x01, FBUF_CURRENT_FRAME};
	sendCmd(GET_FBUF_LEN, command_args, sizeof(command_args));
	readResponse(20,200);
	uint8_t expected_result[]={0x76,0x00,0x34,0x00,0x04};
	uint8_t image_length_bytes[4];
	if (memcmp(expected_result,camera_buffer,5*sizeof(uint8_t))==0){
		memcpy(image_length_bytes,camera_buffer+5,4*sizeof(uint8_t));
		uint32_t temp;
		temp = (image_length_bytes[0]<<24)+(image_length_bytes[1]<<16)+(image_length_bytes[2]<<8)+image_length_bytes[3];
		return temp;
	}
	else return 0;
}

/*	@brief	Read part of picture data. Number of bytes to read is indicated by the argument
 *	@detail
 *	@param[in]	bytes_to_read	indicate the number of bytes that one wants to read from the camera buffer's memory
 *	@return		pointer to the object's [camera_buffer]
 */
uint8_t* VC0706::readPictureData(uint8_t bytes_to_read){
	uint8_t command_args[]={0x0C,0x00,0x0A,
							0x00,0x00,frame_pointer>>8,frame_pointer&0xFF,
							0x00,0x00,0x00,bytes_to_read,
							CAMERA_DELAY>>8, CAMERA_DELAY&0xFF};
	sendCmd(READ_FBUF,command_args,sizeof(command_args));
	readResponse(bytes_to_read+10,200);
	frame_pointer+=bytes_to_read;
	return camera_buffer;
}

/*	@brief Resume camera back to normal camera mode
 * 	@detail	During takePicture() call, the camera is commanded to stop at the current frame, to get
 * 	latest snapshot, the camera needs to be back to normal camera mode. This function should be called
 * 	before takePicture().
 * 	@param[in]	none
 * 	@return	0x01 if responded bytes match {0x76,0x00,0x36,0x00,0x00). Return 0x00 otherwise.
 */
uint8_t VC0706::resumeVideo(){
	uint8_t command_args[]={0x01, FBUF_RESUME_FRAME};
	sendCmd(FBUF_CTRL,command_args,sizeof(command_args));
	readResponse(20,200);
	uint8_t expected_result[] = {0x76,0x00,0x36,0x00,0x00};
	if (memcmp(expected_result,camera_buffer,5*sizeof(uint8_t))==0) return 0x01;
	else return 0x00;
}


