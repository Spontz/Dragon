/*
	main.c: main demo code !!!
*/

#include "main.h"

// ******************************************************************

int main(int argc, char *argv[]) {
	dkernel_trace("Valpurgis: Starting up...\n");

	dkernel_getArguments(argc, argv);
	dkernel_trace("Valpurgis: Got arguments\n");
	
	// Remove a previous camera.cam file in order to start capturing the camera from zero
	// In the future, the captured camera values will be sent to the demo editor instead of being written to a file
	remove("camera.cam");
	dkernel_trace("Valpurgis: Cleaning old camera files");
	
	dkernel_loadScripts();
	dkernel_trace("Valpurgis: Scripts Loaded\n");
	
	if (demoSystem.slaveMode) {
		dkernel_trace("Valpurgis: Running in network slave mode\n");
	} else {
		dkernel_trace("Valpurgis: Running in standalone mode\n");
	}
		
	dkernel_configDialog();
	dkernel_trace("Valpurgis: Config Dialog executed\n");
	
	dkernel_initDemo();
	dkernel_trace("Valpurgis: Engine inited successfully\n");

	dkernel_mainLoop();
	dkernel_trace("Valpurgis: Main loop ended successfully\n");

	dkernel_closeDemo();
	dkernel_trace("Valpurgis: Engine closed\n");

	return EXIT_SUCCESS;
}
