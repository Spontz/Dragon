/*
	efxGlow.c: glow effect
*/

#include "../interface/demo.h"

// ******************************************************************

typedef struct {
	tExpression tParameters;
} efxGlow_section;

static efxGlow_section *local;

// ******************************************************************

void preload_efxGlow () {
}

// ******************************************************************

void load_efxGlow () {
	char *equation;
	size_t equationSize;
	
	// script validation
	if (mySection->stringNum != 5) {
		section_error("5 strings needed");
		return;
	}

	local = malloc(sizeof(efxGlow_section));
	mySection->vars = (void *) local;

	equationSize  = strlen(mySection->strings[0]);
	equationSize += strlen(mySection->strings[1]);
	equationSize += strlen(mySection->strings[2]);
	equationSize += strlen(mySection->strings[3]);
	equationSize += strlen(mySection->strings[4]);
	
	equation = calloc(equationSize+1, sizeof(char));

	strcpy(equation, (char *) mySection->strings[0]);
	strcat(equation, (char *) mySection->strings[1]);
	strcat(equation, (char *) mySection->strings[2]);
	strcat(equation, (char *) mySection->strings[3]);
	strcat(equation, (char *) mySection->strings[4]);
	
	local->tParameters.equation = equation;
	
	// Init the expression evaluation library
	initExpression(&local->tParameters);
	mySection->loaded=1;
}

// ******************************************************************

void init_efxGlow () {
}

// ******************************************************************

void render_efxGlow () {
	double red, green, blue;
	double accumulation, radius;
	
	local = (efxGlow_section *) mySection->vars;

	// capture current color buffer
	tex_bind (demoSystem.backup);
	gldrv_copyColorBuffer ();

	// Insert the default section values into the size equation
	insertSectionVariables(&local->tParameters);

	// Evaluate the equations
	local->tParameters.err = exprEval(local->tParameters.o, &local->tParameters.result);

	// Check for errors
	if(local->tParameters.err != EXPR_ERROR_NOERROR) section_error("efxGlow: render_efxGlow: Expression evaluation Error (%d): %s", local->tParameters.err, local->tParameters.equation);

	// Retrieve the values and assign them
	exprValListGet(local->tParameters.v,          "red", &red  );
	exprValListGet(local->tParameters.v,        "green", &green);
	exprValListGet(local->tParameters.v,         "blue", &blue );
	exprValListGet(local->tParameters.v, "accumulation", &accumulation);
	exprValListGet(local->tParameters.v,       "radius", &radius);

	// set render state
	glDisable (GL_DEPTH_TEST);
	glEnable (GL_BLEND);
	camera_set2d ();

	// highpass filter
	render_highpassfilter_ext (demoSystem.rtt, (float)red, (float)green, (float)blue, (int)accumulation);

	
	// box blur
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	render_boxblur (demoSystem.rtt, (float)radius);
		
	// restore original color buffer
	tex_bind (demoSystem.backup);
	
	if (mySection->hasBlend) {
		glEnable(GL_BLEND);
		glBlendFunc(mySection->sfactor, mySection->dfactor);
	} else {
		glBlendFunc (GL_ONE, GL_ONE);
	}
	
	glColor4f (1,1,1,1);
	gldrv_texscreenquad ();

	// clear depth buffer
	glClear (GL_DEPTH_BUFFER_BIT);

	// unset render state
	camera_restore ();
	glDisable (GL_BLEND);
	glEnable (GL_DEPTH_TEST);
}

// ******************************************************************

void end_efxGlow () {
}
