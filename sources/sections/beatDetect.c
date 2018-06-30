#include "../interface/demo.h"


typedef struct {
	float					beat_ratio;
	float					fade_out;
} beatDetect_section;

static beatDetect_section* local;

// ******************************************************************

void preload_beatDetect(){
}

void load_beatDetect(){
	
	if (mySection->paramNum != 2){
		section_error("2 parameters are needed: ratio (default: 1.4f) and fadeout (default: 4.0f)");
		return;
	}
	local = malloc(sizeof(beatDetect_section));
	mySection->vars = (void *)local;

	mySection->loaded = 1;
}

void init_beatDetect(){
	//local = (beatDetect_section*)mySection->vars;
	demoSystem.beat_ratio = mySection->param[0];	// Set the beat_ratio value
	demoSystem.beat_fadeout = mySection->param[1];	// Set the beat_facdeout value
}

void render_beatDetect() {
	//local = (beatDetect_section*)mySection->vars;
	demoSystem.beat_ratio = mySection->param[0];	// Set the beat_ratio value
	demoSystem.beat_fadeout = mySection->param[1];	// Set the beat_facdeout value	
}

void end_beatDetect() {
}