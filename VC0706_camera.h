/*
 * VC0706_camera.h
 *
 *  Created on: Feb 28, 2015
 *      Author: littledino2112
 */

#ifndef VC0706_CAMERA_H_
#define VC0706_CAMERA_H_

#include "application.h"

class VC0706
{
public:
	enum COMMAND_CODES {
		GET_FIRMWARE_VERSION 	= 0x11,	// Get firmware version
		SYSTEM_RESET 			= 0x26,	// System reset
		READ_DATA 				= 0x30,	// Read DATA register
		WRITE_DATA 				= 0x31,	// Write DATA register
		READ_FBUF				= 0x32,	// Read Buffer register
		WRITE_FBUF				= 0x33, // Write Buffer register
		FBUF_CTRL				= 0x36, // Control Buffer register
		GET_FBUF_LEN			= 0x34	// Used to get image length stored in buffer
	} ;

	enum CONTROL_CODES {
		FBUF_STOP_CURRENT_FRAME 	= 0x00,
		FBUF_STOP_NEXT_FRAME		= 0x01,
		FBUF_RESUME_FRAME			= 0x03,	// The document of VC0706 got this one wrong
		FBUF_STEP_FRAME				= 0x02,
		FBUF_CURRENT_FRAME			= 0x00,
		FBUF_NEXT_FRAME				= 0x01
	};

public:
	VC0706();
	/*	@brief Send basic commands to the Camera. This normally doesn't require any extra data to be sent.
	 *	@detail Basic commands only require the following format: 0x56 0x00 [command] 0x00
	 *	@param[in] code   belongs to enum COMMAND_CODES, this is the command to be sent
	 */
	void sendCmd(COMMAND_CODES code, uint8_t args[], uint8_t args_num);
	char* getCameraVersion(void);
	uint8_t readResponse(uint8_t data_length, uint16_t timeout_ms);
	uint8_t resumeVideo();
	uint8_t takePicture();
	uint16_t getImageLength();
	uint8_t* readPictureData(uint8_t bytes_to_read);

	inline void send(uint8_t byte) {Serial2.write(byte);}
	inline uint8_t getBufferCounter(){return buffer_counter;}

private:
	static const uint8_t BUFFERLENGTH = 200;
	const uint8_t CAMERA_DELAY = 10;
	uint16_t frame_pointer;
	uint8_t camera_buffer[BUFFERLENGTH+1];
	uint8_t buffer_counter;

public:
    char camera_version[20];
};




#endif /* FIRMWARE_FIRMWARE_SPARK_CORE_APPLICATIONS_CAMERA_TEST_VC0706_CAMERA_H_ */
