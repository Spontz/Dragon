/*
	bassdriver.c: bass interface
*/

#include "../../main.h"
#include <bass.h>
#include "bassdriver.h"


// ******************************************************************

void sound_init() {
	if (!BASS_Init(-1, 44100, 0, 0, NULL)) {
		dkernel_error("bassdriver: Sound cannot be initialized, error in sound_init(), line BASS_Init()\n");
		return;
	}
}

// ******************************************************************

void sound_play() {
	BASS_Start();
}

// ******************************************************************

void sound_update() {
	BASS_Update(200);
}

// ******************************************************************

void sound_pause() {
	BASS_Pause();
}

// ******************************************************************

void sound_stop() {
	BASS_Stop();
}

// ******************************************************************

void sound_end() {
	BASS_Free();
}

// ******************************************************************

float sound_cpu() {
	return BASS_GetCPU();
}

// ******************************************************************

int sound_channels() {
	// TODO: To be implemented in BASS, as it was in FMOD
	//return FSOUND_GetChannelsPlaying();
	return 0;	
}