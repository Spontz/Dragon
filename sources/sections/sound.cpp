#pragma comment( lib, "fmod64_vc" )

#include "../interface/demo.h"

#include <fmod.h>
#include <fmod_errors.h>

//

const unsigned int	BUFFER_SAMPLES	= 512;
const float			DEFAULT_ENERGY	= 0.0f;
const float			BEAT_RATIO		= 1.4f;
const float			FADE_OUT		= 4.0f;

//

struct sound_section
	{
	// FMOD
	FMOD_SYSTEM*			m_pFMODSystem;
	FMOD_SOUND*				m_pSound;
	FMOD_CHANNEL*			m_pChannel;
	FMOD_DSP*				m_pDSP;
	FMOD_DSP_PARAMETER_FFT*	m_pFFT;
	
	// Beat
	float					energy[BUFFER_SAMPLES];
	float					default_energy;
	float					beat_ratio;
	float					fade_out;
	float					intensity;
	int						position;
	};

static sound_section* local;

//

void sound_error(FMOD_RESULT const& Value){
	section_error("FMod: %s", FMOD_ErrorString(Value));}

extern "C" void preload_sound(){
}


extern "C" void load_sound(){
	if (!demoSystem.sound)
		return;

	if (mySection->stringNum != 1){
		section_error("1 string needed");
		return;}
	
	local = new sound_section();
	mySection->vars = local;

	FMOD_System_Create(&local->m_pFMODSystem);

	auto p = FMOD_System_Init(
		local->m_pFMODSystem,
		64,
		FMOD_INIT_NORMAL,
		nullptr
	);

	const auto res = FMOD_System_CreateSound(
		local->m_pFMODSystem,
		mySection->strings[0],
		FMOD_CREATESTREAM,
		nullptr,
		&local->m_pSound);

	if (res != FMOD_OK){
		sound_error(FMOD_OK);
		return;}

	if (mySection->paramNum != 0 && mySection->paramNum != 3){
		section_error("sound: 3 params needed");
		return;}

	mySection->loaded = 1;}


extern "C" void init_sound(){
	if (!demoSystem.sound)
		return;

	if (demoSystem.state != DEMO_PLAY)
		return;

	local = reinterpret_cast<sound_section*>(mySection->vars);

	// Seek if the section doesn't start from the begining:
	if (mySection->runTime > 0){
		const auto r = FMOD_Channel_SetPosition(
			local->m_pChannel,
			static_cast<unsigned int>(mySection->runTime*1000.0f),
			FMOD_TIMEUNIT_MS
			);

		if (r != FMOD_OK)
			sound_error(r);
		}

	const auto r = FMOD_System_PlaySound(
		local->m_pFMODSystem,
		local->m_pSound,
		nullptr,
		false,
		&local->m_pChannel);

	if (r != FMOD_OK)
		sound_error(r);

	FMOD_System_CreateDSPByType(local->m_pFMODSystem, FMOD_DSP_TYPE::FMOD_DSP_TYPE_FFT, &local->m_pDSP);
	FMOD_Channel_AddDSP(local->m_pChannel, FMOD_DSP_PARAMETER_DATA_TYPE_FFT, local->m_pDSP);

	// Beat detection specific information
	//unit = FSOUND_DSP_GetFFTUnit();
	//FSOUND_DSP_SetActive(unit, TRUE);

	// get parameters
	if (mySection->paramNum == 0) {
		local->default_energy = DEFAULT_ENERGY;
		local->beat_ratio = BEAT_RATIO;
		local->fade_out = FADE_OUT;
	}
	else {
		local->default_energy = mySection->param[0];
		local->beat_ratio = mySection->param[1];
		local->fade_out = mySection->param[2];
	}

	// clean variables
	for (auto i=0; i<BUFFER_SAMPLES; i++) {
		local->energy[i] = local->default_energy;
	}
	
	local->intensity = 0;
	local->position = 1;
}


extern "C" void render_sound() {
	float *spectrum, instant, avg;
	int i;
			
	// Update the beat information in the demo structure
	local = (sound_section *) mySection->vars;

	FMOD_DSP_GetParameterData(
		local->m_pDSP,
		FMOD_DSP_FFT_SPECTRUMDATA,
		reinterpret_cast<void **>(local->m_pFFT),
		nullptr,
		nullptr,
		0);


	return;

	for (int c = 0; c < local->m_pFFT->numchannels; ++c)
		for (int i = 0; i < local->m_pFFT->length; ++i)
			float val = local->m_pFFT->spectrum[c][i];

	spectrum = &local->m_pFFT->spectrum[0][0];

	// get instant energy
	instant = 0;
	//for (i=0; i<512; i++) {
//		instant += spectrum[i];
//	}

	// calculate average energy in last samples
	avg = 0;
	for (i=0; i<BUFFER_SAMPLES; i++) {
		avg += local->energy[i];
	}
	avg /= (float) local->position;

	// instant sample is a beat?
	if ((instant / avg) > local->beat_ratio) {
		local->intensity = 1.0f;
	} else if (local->intensity > 0) {
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
	if ((demoSystem.debug)) {
		glDisable(GL_DEPTH_TEST);
		
		if (mySection->hasBlend) {
			glEnable(GL_BLEND);
			glBlendFunc(mySection->sfactor, mySection->dfactor);
		}
		
		camera_set2d();

		glDisable(GL_TEXTURE_2D);
			glColor4f(1,1,1,1);
			
			glBegin(GL_LINES);
			
				for (i=0; i<512; i++) {
					glVertex2f(i/640.0f, spectrum[i]);
					glVertex2f(i/640.0f, 0);
				}
			
			glEnd ();
		glEnable(GL_TEXTURE_2D);

		/* if (local->intensity > 0) {
			glColor4f(local->intensity, local->intensity, local->intensity, local->intensity);
			tex_bind(local->texture);
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
		} */

		camera_restore();

		if (mySection->hasBlend) glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
}


extern "C" void end_sound(){
	if (!demoSystem.sound)
		return;

	local = reinterpret_cast<sound_section*>(mySection->vars);

	const auto r = FMOD_Channel_Stop(local->m_pChannel);
	if (r != FMOD_OK)
		sound_error(r);

}
