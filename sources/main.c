/*
	main.c: main demo code !!!
*/

#include "main.h"

// ******************************************************************

int main(int argc, char *argv[]) {
	dkernel_trace("Dragon Engine: Starting up...");

	dkernel_getArguments(argc, argv);
	
	// Remove a previous camera.cam file in order to start capturing the camera from zero
	// In the future, the captured camera values will be sent to the demo editor instead of being written to a file
	remove("camera.cam");
	dkernel_trace("Cleaning old camera files");
	
	dkernel_loadScripts();
	dkernel_trace("Scripts Loaded");
	
	if (demoSystem.slaveMode) {
		dkernel_trace("Running in network slave mode");
	} else {
		dkernel_trace("Running in standalone mode");
	}
		
	dkernel_initDemo();
	dkernel_trace("Engine inited successfully\n");

	dkernel_mainLoop();
	dkernel_trace("Main loop ended successfully\n");

	dkernel_closeDemo();
	dkernel_trace("Engine closed\n");

	return EXIT_SUCCESS;
}
