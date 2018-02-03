/*
	main.c: main demo code !!!
*/

#include "main.h"

// ******************************************************************

int main(int argc, char *argv[]) {
	dkernel_trace("Dragon: Starting up...\n");

	dkernel_getArguments(argc, argv);
	dkernel_trace("Dragon: Got arguments\n");
	
	// Remove a previous camera.cam file in order to start capturing the camera from zero
	// In the future, the captured camera values will be sent to the demo editor instead of being written to a file
	remove("camera.cam");
	dkernel_trace("Dragon: Cleaning old camera files");
	
	dkernel_loadScripts();
	dkernel_trace("Dragon: Scripts Loaded\n");
	
	if (demoSystem.slaveMode) {
		dkernel_trace("Dragon: Running in network slave mode\n");
	} else {
		dkernel_trace("Dragon: Running in standalone mode\n");
	}
		
	dkernel_initDemo();
	dkernel_trace("Dragon: Engine inited successfully\n");

	dkernel_mainLoop();
	dkernel_trace("Dragon: Main loop ended successfully\n");

	dkernel_closeDemo();
	dkernel_trace("Dragon: Engine closed\n");

	return EXIT_SUCCESS;
}
