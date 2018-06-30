#pragma once

// ******************************************************************

#ifdef _WIN32
	#include <Windows.h>
#endif

#include "camera.h"
#include "spline.h"
#include <SDL.h>
#include "math3d.h"

// ******************************************************************

extern char *log_separator;

// ******************************************************************

typedef struct {

	int spline;
	int param;
	int mode;

} Modifier;

// ******************************************************************

#define SECTION_PARAMS		32
#define SECTION_STRINGS		32
#define SECTION_SPLINES		32
#define SECTION_MODIFIERS	32

#define RENDERING_BUFFERS	10
#define FBO_BUFFERS			25
// Warning! 'FBO_BUFFERS' should not be less than 25, if you want to set less than 25,
// please check the variable 'scriptCommand' contents in 'dkernel.c' file.

// ******************************************************************

extern void SpzMessageBox( const char* title, const char* message );

typedef struct tDemoSection tDemoSection;
struct tDemoSection
	{

	// queue links
	tDemoSection*		next;
	tDemoSection*		prior;
	tDemoSection*		nextRdy;
	tDemoSection*		priorRdy;
	tDemoSection*		nextRun;
	tDemoSection*		priorRun;
	
	// section data
	char*		identifier[64];						// Unique section identifier generated by the demo editor
	int			staticSectionIndex;					// index to static section (to get generic functions)
	float		startTime;							// section start time
	float		endTime;							// section end time
	float		duration;							// total section seconds
	int			layer;								// layer where sections run (priority)
	int			enabled;							// 1: Section enabled; 0: Section disabled
	int			loaded;								// 1: Section loaded; 0: Section not loaded
	float		runTime;							// seconds elapsed since section start
	int			hasBlend;							// has blending function parameters?
	int			sfactor;							// default source blending factor
	int			dfactor;							// default dest blending factor
	int			hasAlpha;							// has alpha function parameters?
	int			alphaFunc;							// alpha function
	float		alpha1;								// alpha reference value (hack: no se sabe bien que es esto), renombrar estas variables y documentar aqui
	float		alpha2;								// alpha reference value (hack: no se sabe bien que es esto), renombrar estas variables y documentar aqui
	int			paramNum;							// number of parameters
	float		param[SECTION_PARAMS];				// float parameters
	int			stringNum;							// number of strings
	char*		strings[SECTION_STRINGS];			// string parameters
	int			splineNum;							// number of splines
	Motion*		splines[SECTION_SPLINES];			// spline pointers
	char*		splineFiles[SECTION_SPLINES];		// spline file names
	float		splineDuration[SECTION_SPLINES];	// spline duration in seconds
	// int		modifierNum;						// number of spline modifiers
	// Modifier	modifiers[SECTION_MODIFIERS];		// spline modifiers
	void*		vars;								// pointer to section local variables
	char		DataSource[1024];					// origin of this section (for error messages)

	};

// ******************************************************************

// demo states
#define DEMO_LOADING		0
#define DEMO_PLAY			2
#define DEMO_PAUSE			4
#define DEMO_REWIND			8
#define DEMO_FASTFORWARD	16

// ******************************************************************

typedef enum
	{
	sve_variable_type_unknown,
	sve_variable_type_float,
	sve_variable_type_double,
	sve_variable_type_char,
	sve_variable_type_unsigned_char,
	sve_variable_type_short,
	sve_variable_type_unsigned_short,
	sve_variable_type_int,
	sve_variable_type_unsigned_int,
	sve_variable_type_vector_2f,
	sve_variable_type_vector_3f,
	sve_variable_type_vector_4f,
	sve_variable_type_matrix_4x4f
	}
enum_sve_variable_type;

typedef enum
	{
	sve_variable_unknown = 0,
	sve_variable_matrix_world = 1,
	sve_variable_matrix_view = 2,
	sve_variable_matrix_projection = 3,
	sve_variable_matrix_world_view = 4,
	sve_variable_matrix_world_projection = 5,
	sve_variable_matrix_view_projection = 6,
	sve_variable_matrix_world_view_projection = 7,
	sve_variable_matrix_world_inverse = 8,
	sve_variable_matrix_view_inverse = 9,
	sve_variable_matrix_projection_inverse = 10,
	sve_variable_matrix_world_view_inverse = 11,
	sve_variable_matrix_world_projection_inverse = 12,
	sve_variable_matrix_view_projection_inverse = 13,
	sve_variable_matrix_world_view_projection_inverse = 14,
	sve_variable_matrix_world_transpose = 15,
	sve_variable_matrix_view_transpose = 16,
	sve_variable_matrix_projection_transpose = 17,
	sve_variable_matrix_world_view_transpose = 18,
	sve_variable_matrix_world_projection_transpose = 19,
	sve_variable_matrix_view_projection_transpose = 20,
	sve_variable_matrix_world_view_projection_transpose = 21,
	sve_variable_matrix_world_inverse_transpose = 22,
	sve_variable_matrix_view_inverse_transpose = 23,
	sve_variable_matrix_projection_inverse_transpose = 24,
	sve_variable_matrix_world_view_inverse_transpose = 25,
	sve_variable_matrix_world_projection_inverse_transpose = 26,
	sve_variable_matrix_view_projection_inverse_transpose = 27,
	sve_variable_matrix_world_view_projection_inverse_transpose = 28,
	}
enum_sve_variable;

#define MAX_SVE_VARIABLE_ID 38

typedef struct {

	// arguments
	int argc;
	char **argv;
	char *demoDir;

	// application instance handle
	HMODULE hInstance;

	// demoname = window caption
	char *demoName;

	// misc
	int debug;
	int record;
	float recordFps;
	int compressTga;
	int loop;
	int sound;
	int bench;

	int state; // state of the demo (play, pause, loading, etc.)
	float startTime; // first demo second
	float endTime; // last demo second (0 = unlimited)

	// sections list, script order
	tDemoSection *demoSection;

	// ready section list, ascendent order by start time
	tDemoSection *readySection;

	// run section list, ascendent order by layer
	tDemoSection *runSection;

	// loading information
	int numSections;
	int numReadySections;
	int loadedSections;

	// realtime information
	float runTime; // seconds ellapsed since 0.0 <- demo init, not SDL init
	float beforeFrameTime; // time before render the actual frame
	float afterFrameTime;  // time after render the actual frame
	float frameTime; // last frame time (used by sections)
	float realFrameTime; // last frame time (used by kernel)
	unsigned int frameCount; // demo frame count since start

	// fps calculation
	unsigned int accumFrameCount; // frame count since last fps calculation
	float accumFrameTime; // time since last fps calculation
	float fps; // frames per second


	int keys[512]; // keyboard control (SDL 2.0 handles 512 diferent scan codes)
	char mouseButton; // left mouse button status
	char exitDemo; // exits demo at next loop if true

	camera_t *camera; // the actual camera (set by genCamera)
	int rtt, backup; // render to texture shared texture index (RGB8 format)

	int texRenderingBuffer[RENDERING_BUFFERS];	// Array with the indexes to the rendering buffers
	int fboRenderingBuffer[FBO_BUFFERS];		// Array with the indexes to the fbo buffers
	
	// Beat and beat detection parameters
	float beat; // intensity of the current music beat
	float beat_ratio;
	float beat_fadeout;

	int mouseX, mouseY; // Mouse global coordinates
	int mouseXvar, mouseYvar; // Mouse variation rom the center of the screen
	Uint8 mouseButtons; // Mouse buttons state
	
	// network slave mode
	int slaveMode; // 1 = network slave; 0 = standalone mode;
	
	// Drawing states
	char drawSound;	// Draw sound render
	char drawTiming;// Draw time of the demo
	char drawFps;	// Draw FPS's of the demo

	// engine state variables
	matrix_t	m_VariableMatrix[MAX_SVE_VARIABLE_ID+1];
} tDemoSystem;

// ******************************************************************

// demo system data
extern tDemoSystem demoSystem;

// pointer to actual section data
extern tDemoSection *mySection;

// ******************************************************************

typedef struct {

	char *scriptName;

	void (*preload)(); // load static data from disk
	void (*load)();    // load parametrized data from disk
	void (*init)();    // set up local variables before execution
	void (*exec)();    // render a new frame
	void (*end) ();    // destroy allocated memory

} tSectionFunction;

// ******************************************************************

// state variable access
const matrix_t*			get_sve_variable_matrix_4x4f(enum_sve_variable id);
enum_sve_variable		get_sve_variable_id(const char* pString);
//enum_sve_variable_type	get_sve_variable_type(enum_sve_variable id);
void					set_sve_variable_matrix_4x4f(enum_sve_variable id, const matrix_t* pValue);

// sections functions references
extern tSectionFunction sectionFunction[];
int dkernel_createSection(const char* pSectionScript, const char* pDataSource);
void dkernel_setSectionsLayer(char* newLayer, char* identifier);
void dkernel_setSectionsEndTime(char* amount, char* identifiers);
void dkernel_setSectionsStartTime(char* amount, char* identifiers);

// ******************************************************************

int getBlendCodeByName(char *name);
int getAlphaCodeByName(char *name);

int getRenderModeByName(char *name);

int getTextureInternalFormatByName(char *name);
int getTextureFormatByName(char *name);
int getTextureTypeByName(char *name);
	
// ******************************************************************

int getSectionByName(char *name);

// ******************************************************************

void dkernel_getArguments(int argc, char *argv[]);
void dkernel_loadScripts();
void dkernel_loadScriptData(const char* pScript, const char* pDataSource);

void dkernel_initDemo();
void dkernel_mainLoop();
void dkernel_closeDemo();

// ******************************************************************

// Engine state control
void dkernel_play();
void dkernel_pause();
void dkernel_rewind();
void dkernel_fastforward();
void dkernel_restart();

// Enabling and disabling the section drawing routines
void dkernel_toggleSection(const char* pIdentifier);
void dkernel_deleteSection(const char* pIdentifier);

// Time control
void dkernel_setStartTime(float theTime);
void dkernel_setCurrentTime (float theTime);
void dkernel_setEndTime(float theTime);

// Section control
void dkernel_updateSection  (const char* pIdentifier, const char* pScript);

// ******************************************************************

void dkernel_getSpline(int spline, float step, ChanVec resVec);

// ******************************************************************

void dkernel_error(const char* pText, ...);
void dkernel_warn(const char* pText, ...);
void dkernel_trace(const char* pText, ...);
void parse_error(const char* pFilename, int line, const char* pText, ...);
void section_error(const char *pText, ...);

// ******************************************************************
