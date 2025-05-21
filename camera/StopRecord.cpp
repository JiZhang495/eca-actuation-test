#include <cstdio>
//#include <windows.h>
#include "EDSDK.h"
#include "EDSDKErrors.h"
#include "EDSDKTypes.h"

EdsError getCamera(EdsCameraRef *camera) {
	// Function to get the camera
	EdsError err;
	EdsCameraListRef camList;
	EdsUInt32 count = 0;

	err = EdsGetCameraList(&camList);
	if (err == EDS_ERR_OK) {
		err = EdsGetChildCount(camList,&count);
		if(count == 0) {
			err = EDS_ERR_DEVICE_NOT_FOUND;
			puts("No cameras found");
		}
	}
	printf("Number of cameras: %i\n", count);
	if(err == EDS_ERR_OK) {
		err = EdsGetChildAtIndex(camList,0,camera);
	}
	if(camList != NULL) {
		EdsRelease(camList);
		camList = NULL;
	}
	return err;
}

EdsError startLiveView(EdsCameraRef camera) {
	// Function to start the live view on the camera
	EdsError err;
	EdsUInt32 device = 0;
	err =	EdsGetPropertyData(camera,kEdsPropID_Evf_OutputDevice,0,sizeof(device),&device);
	if(err == EDS_ERR_OK) {
		device|= kEdsEvfOutputDevice_PC;
		err = EdsSetPropertyData(camera,kEdsPropID_Evf_OutputDevice,0,sizeof(device),&device);
	}
	return err;
}

EdsError startRecording(EdsCameraRef camera) {
	// Function to start recording video
	EdsError err;
	EdsUInt32 record_start = 4; // Begin movie shooting 
	err = EdsSetPropertyData(camera, kEdsPropID_Record, 0, sizeof(record_start), &record_start); 
	return err;
}

EdsError stopRecording(EdsCameraRef camera) {
	//Function to stop recording video
	EdsError err;
	EdsUInt32 record_stop = 0; // End movie shooting 
	err = EdsSetPropertyData(camera, kEdsPropID_Record, 0, sizeof(record_stop), &record_stop);
	return err;
}

EdsError setCameraAsDestination(EdsCameraRef camera) {
	//Function to set the camera's sd as save target
	EdsError err;
	EdsUInt32 saveTo = kEdsSaveTo_Camera;
	err = EdsSetPropertyData(camera,kEdsPropID_SaveTo,0,sizeof(saveTo),&saveTo);
	return err;
}

//Main

EdsError initialize(EdsCameraRef *camera) {
	EdsError err = EDS_ERR_OK;
	err = EdsInitializeSDK();
	if(err == EDS_ERR_OK)
		puts("SDK initialized");
	else {
		puts("SDK not initialized");
		getchar();
		exit(-1);
	}
	if(err == EDS_ERR_OK)
		err = getCamera(camera);
	if(err == EDS_ERR_OK) 
		err = EdsOpenSession(*camera);
	return err;
}

void exitProperly(EdsCameraRef *camera) {
	EdsError err;
	err = EdsCloseSession(*camera);
	if(camera != NULL) 
		EdsRelease(*camera);
	EdsTerminateSDK();
	if(err != EDS_ERR_OK)
		puts("ERROR");
	return;
}

int main() {
	EdsError err;
	EdsCameraRef camera;
	err = initialize(&camera);
	err = stopRecording(camera);
	if(err != EDS_ERR_OK) {
		puts("ERROR: Not able to stop recording! Switch off the camera manually");
		getchar();
		exit(-1);
	}
	return 0;
}