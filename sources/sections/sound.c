#pragma comment( lib, "bass" )

#include "../interface/demo.h"
#include <bass.h>

#define BUFFER_SAMPLES	512
#define	DEFAULT_ENERGY	1.0f
//#define	BEAT_RATIO		1.4f
//#define FADE_OUT		4.0f


typedef struct {
	HSTREAM str;	// Music Stream
	
	// Beat parameters
	float					energy[BUFFER_SAMPLES];
	float					beat_ratio;
	float					fade_out;
	float					intensity;
	int						position;
} sound_section;

static sound_section* local;

// ******************************************************************

void preload_sound(){
}

void load_sound(){
	if (!demoSystem.sound) {
		return;
	}

	if (mySection->stringNum != 1){
		section_error("1 string needed");
		return;
	}
	local = malloc(sizeof(sound_section));
	mySection->vars = (void *)local;


	local->str = BASS_StreamCreateFile(FALSE, mySection->strings[0], 0, 0, BASS_STREAM_PRESCAN);
	if (local->str == 0)
		section_error("BASS cannot read file: %s", mySection->strings[0]);
	else
		mySection->loaded = 1;
}

void init_sound(){
	if (!demoSystem.sound)
		return;

	if (demoSystem.state != DEMO_PLAY)
		return;

	local = (sound_section *)mySection->vars;

	// Beat detection - Init variables
	local->beat_ratio = demoSystem.beat_ratio;
	local->fade_out = demoSystem.beat_fadeout;

	// Clean variables
	for (auto i = 0; i<BUFFER_SAMPLES; i++) {
		local->energy[i] = DEFAULT_ENERGY;
	}
	local->intensity = 0;
	local->position = 1;


	QWORD bytes = BASS_ChannelSeconds2Bytes(local->str, mySection->runTime); // convert seconds to bytes
	BASS_ChannelSetPosition(local->str, bytes, BASS_POS_BYTE); // seek there
	
	BOOL r = BASS_ChannelPlay(local->str, FALSE);
	if (r != TRUE)
		section_error("BASS_ChannelPlay returned error: %i", BASS_ErrorGetCode());
}

void render_sound() {
	

	float instant, avg;	// Instant energy
	int i;
	float fft[BUFFER_SAMPLES]; // 512 samples, because we have used "BASS_DATA_FFT1024", and this returns 512 values
	
	local = (sound_section *)mySection->vars;

	// Update local values with the ones defined by the demosystem
	local->beat_ratio = demoSystem.beat_ratio;
	local->fade_out = demoSystem.beat_fadeout;

	BOOL r = BASS_ChannelGetData(local->str, fft, BASS_DATA_FFT1024); // get the FFT data
	if (r == -1)
		section_error("BASS_ChannelGetData returned error: %i", BASS_ErrorGetCode());

	instant = 0;
	for (i=0; i<(int)BUFFER_SAMPLES; i++)
		instant += fft[i];
	
	// calculate average energy in last samples
	avg = 0;
	for (i = 0; i<BUFFER_SAMPLES; i++) {
		avg += local->energy[i];
	}
	avg /= (float)local->position;

	// instant sample is a beat?
	if ((instant / avg) > local->beat_ratio) {
		local->intensity = 1.0f;
	}
	else if (local->intensity > 0) {
		local->intensity -= local->fade_out * demoSystem.frameTime;
		if (local->intensity < 0) local->intensity = 0;
	}

	// updated kernel shared variable
	// to be used by kernel itself or another sections
	demoSystem.beat = local->intensity;

	// update energy buffer
	if (local->position < BUFFER_SAMPLES) {
		local->energy[local->position-1] = instant;
		local->position++;
	}
	else {
		for (i=1; i<BUFFER_SAMPLES; i++) {
			local->energy[i-1] = local->energy[i];
		}
		local->energy[BUFFER_SAMPLES-1] = instant;
	}

	// Spectrum drawing (Only in debug mode, when the timing information is also being displayed)
	if ((demoSystem.debug) && (demoSystem.drawSound)) {
		glDisable(GL_DEPTH_TEST);

		if (mySection->hasBlend) {
			glEnable(GL_BLEND);
			glBlendFunc(mySection->sfactor, mySection->dfactor);
		}
		camera_set2d();
		glDisable(GL_TEXTURE_2D);
		glColor4f(1, 1, 1, 1);
		glBegin(GL_LINES);
		for (i = 0; i < BUFFER_SAMPLES; i++) {
			glVertex2f(i / 640.0f, fft[i]);
			glVertex2f(i / 640.0f, 0);
		}
		glEnd();
		
		// Draws a quad when a beat is detected
		if (local->intensity > 0) {
			glColor4f(local->intensity, local->intensity, local->intensity, local->intensity);
			//tex_bind(local->texture);
			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex2f(0.9f,0.9f);

			glTexCoord2f(1,0);
			glVertex2f(1,0.9f);

			glTexCoord2f(1,1);
			glVertex2f(1,1);

			glTexCoord2f(0,1);
			glVertex2f(0.9f,1);
			glEnd();
		}
		camera_restore();

		if (mySection->hasBlend) glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}

}

void end_sound() {
	if (!demoSystem.sound)
		return;
	BOOL r = BASS_ChannelStop(local->str);
	if (r != TRUE)
		section_error("BASS_ChannelStop returned error: %i", BASS_ErrorGetCode());
}