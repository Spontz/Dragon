/*
	gldriver.c: SDL OpenGL init and setup
*/

#include <math.h>
#include "gldriver.h"

#include "../../main.h"
#include "../dkernel.h"
#include "../texture.h"
#include "../glslshader.h"
#include "../tga.h"

#define GL_DRV__SUBPIXEL_BIAS 0.005f // hack

// ******************************************************************

tGlDriver glDriver;

/* *************************************************************
 create gl driver and store default settings
**************************************************************** */

void gldrv_create()
{
	int i;

	glDriver.fullScreen = 0;
	glDriver.saveInfo = 0;

	glDriver.width = 640;
	glDriver.height = 480;

	glDriver.AspectRatio = 0;

	glDriver.bpp = 32;
	glDriver.zbuffer = 16;
	glDriver.stencil = 0;
	glDriver.accum = 0;

	glDriver.multisampling = 0;

	glDriver.gamma = 1.0f;

	for (i = 0; i < FBO_BUFFERS; i++) {
		glDriver.fbo[i].width = glDriver.fbo[i].height = glDriver.fbo[i].ratio = 0;
	}
}

/* *************************************************************
 write driver strings to file
**************************************************************** */

void writeSpacedString(char *s) {

	char seps[] = " ,\t\n";
	char *token;

	if (s == NULL) return;

	token = strtok(s, seps);
	while (token != NULL) {
		dkernel_trace("   %s\n", token);
		token = strtok(NULL, seps);
	}
}

void gldrv_saveInfo() {
	const unsigned char *s = NULL;

	dkernel_trace("\n%sOpenGL driver information\n", log_separator);
	dkernel_trace("vendor: %s\n", glGetString(GL_VENDOR));
	dkernel_trace("renderer: %s\n", glGetString(GL_RENDERER));
	dkernel_trace("version: %s\n", glGetString(GL_VERSION));
}


/* *************************************************************
 init default client state
**************************************************************** */

int gl_drv_check_for_gl_errors(char* pOut)
{
	GLenum err = glGetError();

	if (pOut)
		strcpy(pOut, (const char*)gluErrorString(err));

	if (err == GL_NO_ERROR)
		return 0;

	return 1;
}

void gldrv_initState() {
	char OGLError[1024];

	// Check for ogl errors
	while (gl_drv_check_for_gl_errors(OGLError))
		dkernel_warn("An OpenGL error has been produced before gldrv_initState:\n\n%s", OGLError);

	// Set default ogl states
	glMatrixMode(GL_PROJECTION);

	while (gl_drv_check_for_gl_errors(OGLError))
		dkernel_warn("OpenGL Error while setting the default state in gldrv_initState:\n\n%s", OGLError);

	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	while (gl_drv_check_for_gl_errors(OGLError))
		dkernel_warn("OpenGL Error while setting the default state in gldrv_initState:\n\n%s", OGLError);

	glClearColor(0, 0, 0, 0);
	glDisable(GL_BLEND);						// blending disabled
	glDisable(GL_ALPHA_TEST);					// alpha test disabled
	glDisable(GL_LIGHTING);						// lighting disabled
	glDisable(GL_LIGHT0);						// default light disabled too

	while (gl_drv_check_for_gl_errors(OGLError))
		dkernel_warn("OpenGL Error while setting the default state in gldrv_initState:\n\n%s", OGLError);

	glDisable(GL_CULL_FACE);					// cull face disabled
	glEnable(GL_TEXTURE_2D);					// textures enabled
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// draw cwise and ccwise in fill mode

	while (gl_drv_check_for_gl_errors(OGLError))
		dkernel_warn("OpenGL Error while setting the default state in gldrv_initState:\n\n%s", OGLError);

	glShadeModel(GL_FLAT);						// polygon smoothing disabled
	glEnable(GL_DEPTH_TEST);					// depth test enabled
	glDepthFunc(GL_LEQUAL);						// depth test comparison function set to LEQUAL

	// Enable multisampling
	if (glDriver.multisampling && glDriver.ext.multisample)
		glEnable(GL_MULTISAMPLE);

	// Check for ogl errors
	while (gl_drv_check_for_gl_errors(OGLError))
		dkernel_warn("OpenGL Error while setting the default state in gldrv_initState:\n\n%s", OGLError);
}

void gldrv_initViewport()
{
	int i;
	char OGLError[1024];

	if (glDriver.fullScreen) {
		const float DisplayAspectRatio = (float)glDriver.width / (float)glDriver.height;
		int x, y, width, height;

		// TODO merlucin: Calculate the viewport to match the current display as much as possible
		// while keeping the original aspect ratio
		if (glDriver.AspectRatio > DisplayAspectRatio) {
			// we need to cut tops and downs
			x = 0;
			width = glDriver.width;
			height = (int)((float)glDriver.height / glDriver.AspectRatio * DisplayAspectRatio);
			y = (glDriver.height - height) / 2;
		}
		else {
			// we need to cut sides
			y = 0;
			height = glDriver.height;
			width = (int)(((float)glDriver.width) * glDriver.AspectRatio / DisplayAspectRatio);
			x = (glDriver.width - width) / 2;
		}

		glDriver.vpWidth = width;
		glDriver.vpHeight = height;
		glDriver.vpXOffset = x;
		glDriver.vpYOffset = y;

	}
	else {
		// In window mode, the viewport has the same size as the window
		glDriver.vpWidth = glDriver.width;
		glDriver.vpHeight = glDriver.height;
		glDriver.vpYOffset = 0;
		glDriver.vpXOffset = 0;
	}

	// init shared rtt textures
	demoSystem.rtt = tex_new(glDriver.vpWidth, glDriver.vpHeight, GL_RGB, 3);
	tex_properties(demoSystem.rtt, NO_MIPMAP | CLAMP_TO_EDGE);
	tex_properties(demoSystem.rtt, NO_MIPMAP);
	tex_upload(demoSystem.rtt, NO_CACHE);

	demoSystem.backup = tex_new(glDriver.vpWidth, glDriver.vpHeight, GL_RGB, 3);

	tex_properties(demoSystem.backup, NO_MIPMAP);
	tex_upload(demoSystem.backup, NO_CACHE);

	// init render buffers
	for (i = 0; i < RENDERING_BUFFERS; i++)
	{
		demoSystem.texRenderingBuffer[i] = tex_new(glDriver.vpWidth, glDriver.vpHeight, GL_RGB, 3);

		while (gl_drv_check_for_gl_errors(OGLError))
			section_error("OGL Error creating texRenderingBuffer:\n\n%s", OGLError);

		tex_properties(demoSystem.texRenderingBuffer[i], MODULATE);
		tex_upload(demoSystem.texRenderingBuffer[i], NO_CACHE);
	}

	// init fbo's
	for (i = 0; i < FBO_BUFFERS; i++) {
		if (((glDriver.fbo[i].width != 0) && (glDriver.fbo[i].height != 0)) || (glDriver.fbo[i].ratio != 0)) {
			if (glDriver.fbo[i].ratio != 0) {
				glDriver.fbo[i].width = (glDriver.width / glDriver.fbo[i].ratio);
				glDriver.fbo[i].height = (glDriver.height / glDriver.fbo[i].ratio);
			}

			glDriver.fbo[i].tex_iformat = getTextureInternalFormatByName(glDriver.fbo[i].format);
			glDriver.fbo[i].tex_format = getTextureFormatByName(glDriver.fbo[i].format);
			glDriver.fbo[i].tex_type = getTextureTypeByName(glDriver.fbo[i].format);
			// Check if the format is valid
			if (glDriver.fbo[i].tex_format > 0) {
				demoSystem.fboRenderingBuffer[i] = fbo_new(glDriver.fbo[i].width, glDriver.fbo[i].height, glDriver.fbo[i].tex_iformat, glDriver.fbo[i].tex_format, glDriver.fbo[i].tex_type);
				fbo_properties(demoSystem.fboRenderingBuffer[i], MODULATE | NO_MIPMAP); // Delete "NO_MIPMAP" to disable filtering 
				fbo_upload(demoSystem.fboRenderingBuffer[i], NO_CACHE);
				dkernel_trace("Fbo %i uploaded: width: %i, height: %i, format: %s (%i), iformat: %i, type: %i", i, glDriver.fbo[i].width, glDriver.fbo[i].height, glDriver.fbo[i].format, glDriver.fbo[i].tex_format, glDriver.fbo[i].tex_iformat, glDriver.fbo[i].tex_type);
			}
			else {
				dkernel_error("Error in FBO definition: FBO number %i has a non recongised format: '%s', please check 'graphics.spo' file.", i, glDriver.fbo[i].format);
			}
		}
	}
}

/**************************************************************
 create window and opengl context
**************************************************************/

void gldrv_init()
{
	GLenum			GLEWError;
	uint32_t		SDLWindowFlags;

	dkernel_trace("Initializing SDL...");
	// first, initialize SDL's video subsystem
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		dkernel_error("Video initialization failed: %s", SDL_GetError());

	dkernel_trace("Getting video info...");
	// get some video information
	/*
	info = SDL_GetVideoInfo();
	if (!info)
		dkernel_error("Get video info failed: %s", SDL_GetError());
	*/

	dkernel_trace("Setting cursor settings...");
	if (demoSystem.debug)
		SDL_ShowCursor(TRUE);
	else
		SDL_ShowCursor(FALSE);

	dkernel_trace("Setting OGL states...");
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, glDriver.zbuffer);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, glDriver.stencil);

	if (glDriver.accum) {
		dkernel_trace("Setting OGL acumulation buffer states...");
		SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 8);
	}

	if (glDriver.multisampling == 1) {
		dkernel_trace("Setting 2x multisampling...");
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
	}
	else if (glDriver.multisampling == 2) {
		dkernel_trace("Setting 4x multisampling...");
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	}
	else {
		glDriver.multisampling = 0;
	}

	// set window properties
	SDLWindowFlags = SDL_WINDOW_OPENGL;
	if (glDriver.fullScreen)
	{
		SDLWindowFlags |= SDL_WINDOW_FULLSCREEN;
		//		SDL_DisplayMode current;
		//		if (!SDL_GetCurrentDisplayMode(0, &current))
		//			dkernel_error("gldriver.c: Cannot get the current Display Mode: %s", SDL_GetError());
	}
	// Get the current aspect ratio before setting new resolution
	glDriver.AspectRatio = (float)glDriver.width / (float)glDriver.height;

	// TODO: glDriver.bpp = info->vfmt->BitsPerPixel;
	glDriver.pSDLWindow = SDL_CreateWindow(
		demoSystem.demoName,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		glDriver.width,
		glDriver.height,
		SDLWindowFlags
	);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	glDriver.pSDLContext = SDL_GL_CreateContext(glDriver.pSDLWindow);

	glClearColor(0, 0, 0, 1);

	/*
		if (!demoSystem.debug)
			{
			// Enable double buffering in order to avoid screen tearing
			SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
			}
		*/

		// init glew
	dkernel_trace("Initializing glew...");
	GLEWError = glewInit();

	if (GLEWError != GLEW_OK)
		dkernel_error("glewInit failed:\n\n%s", glewGetErrorString(GLEW_OK));

	// save opengl strings
	dkernel_trace("Saving ogl info...");
	if (glDriver.saveInfo) {
		gldrv_saveInfo();
	}

	// set the default gl state
	dkernel_trace("Setting default ogl state...");
	gldrv_initState();

	// init viewport, rtt shared texture and fbo's
	dkernel_trace("Initializing viewport, rtts and fbos...");
	gldrv_initViewport();
}

/* *************************************************************
 delete window and opengl context
**************************************************************** */

void gldrv_close()
{
	// clean up SDL
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}


/* *************************************************************
 draw screen quads
**************************************************************** */

void gldrv_screenquad()
{
	float x0, y0, x1, y1;

	camera_2d_fit_to_viewport(glDriver.AspectRatio, &x0, &x1, &y0, &y1);

	x1 = x0 + (x1 - x0) / (float)glDriver.vpWidth * (float)glDriver.vpWidth;
	y1 = y0 + (y1 - y0) / (float)glDriver.vpHeight * (float)glDriver.vpHeight;

	glBegin(GL_QUADS);
	glVertex2f(x0 - GL_DRV__SUBPIXEL_BIAS, y0 - GL_DRV__SUBPIXEL_BIAS);
	glVertex2f(x1 + GL_DRV__SUBPIXEL_BIAS, y0 - GL_DRV__SUBPIXEL_BIAS);
	glVertex2f(x1 + GL_DRV__SUBPIXEL_BIAS, y1 + GL_DRV__SUBPIXEL_BIAS);
	glVertex2f(x0 - GL_DRV__SUBPIXEL_BIAS, y1 + GL_DRV__SUBPIXEL_BIAS);
	glEnd();
}

void gldrv_texscreenquad()
{
	// TODO: ASK IVAN
	// 1- ¿Porque a veces se usa la función "gldrv_get_viewport_aspect_ratio" y otras "gldrv_get_aspect_ratio"??
	//     ¿No sería mas facil calcularlo una vez y usarlo siempre? son un mogollon de divisiones!!! --> He puesto el cálculo del AspectRatio al principio,
	//		Como la resolución no cambia, yo CREO que la fucnión de gldrv_get_aspect_ratio no debería existir, y siempre q se quiera el aspect ratio, solo hay q mirar el valor de la variable "glDriver.AspectRatio"
	// 2- ¿porque tenemos tantas funciones para pintar quads? por ejemplo....  en redenrfbo.c -> render_renderFbo () se pinta un quan en el code... y tambien en renderBuffer.c -> draw_fucking_quad ()
	float x0, y0, x1, y1;

	camera_2d_fit_to_viewport(glDriver.AspectRatio, &x0, &x1, &y0, &y1);

	x1 = x0 + (x1 - x0) / (float)glDriver.vpWidth * (float)(glDriver.vpWidth);
	y1 = y0 + (y1 - y0) / (float)glDriver.vpHeight * (float)(glDriver.vpHeight);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(x0 - GL_DRV__SUBPIXEL_BIAS, y0 - GL_DRV__SUBPIXEL_BIAS);
	glTexCoord2f(1, 0); glVertex2f(x1 + GL_DRV__SUBPIXEL_BIAS, y0 - GL_DRV__SUBPIXEL_BIAS);
	glTexCoord2f(1, 1); glVertex2f(x1 + GL_DRV__SUBPIXEL_BIAS, y1 + GL_DRV__SUBPIXEL_BIAS);
	glTexCoord2f(0, 1); glVertex2f(x0 - GL_DRV__SUBPIXEL_BIAS, y1 + GL_DRV__SUBPIXEL_BIAS);

	glEnd();
}

void gldrv_multitexscreenquad()
{
	float x0, y0, x1, y1;

	camera_2d_fit_to_viewport(glDriver.AspectRatio, &x0, &x1, &y0, &y1);

	x1 = x0 + (x1 - x0) / (float)glDriver.vpWidth * (float)(glDriver.vpWidth);
	y1 = y0 + (y1 - y0) / (float)glDriver.vpHeight * (float)(glDriver.vpHeight);

	glBegin(GL_QUADS);
	glMultiTexCoord2f(GL_TEXTURE0, 0, 0);
	glMultiTexCoord2f(GL_TEXTURE1, 0, 0);
	glVertex2f(x0 - GL_DRV__SUBPIXEL_BIAS, y0 - GL_DRV__SUBPIXEL_BIAS);
	
	glMultiTexCoord2f(GL_TEXTURE0, 1, 0);
	glMultiTexCoord2f(GL_TEXTURE1, 1, 0);
	glVertex2f(x1 + GL_DRV__SUBPIXEL_BIAS, y0 - GL_DRV__SUBPIXEL_BIAS);

	glMultiTexCoord2f(GL_TEXTURE0, 1, 1);
	glMultiTexCoord2f(GL_TEXTURE1, 1, 1);
	glVertex2f(x1 + GL_DRV__SUBPIXEL_BIAS, y1 + GL_DRV__SUBPIXEL_BIAS);
	
	glMultiTexCoord2f(GL_TEXTURE0, 0, 1);
	glMultiTexCoord2f(GL_TEXTURE1, 0, 1);
	glVertex2f(x0 - GL_DRV__SUBPIXEL_BIAS, y1 + GL_DRV__SUBPIXEL_BIAS);

	glEnd();
}

void gldrv_multitexscreenquad_offset(float offsetX, float offsetY)
{
	float x0, y0, x1, y1;

	camera_2d_fit_to_viewport(glDriver.AspectRatio, &x0, &x1, &y0, &y1);

	x1 = x0 + (x1 - x0) / (float)glDriver.vpWidth * (float)(glDriver.vpWidth);
	y1 = y0 + (y1 - y0) / (float)glDriver.vpHeight * (float)(glDriver.vpHeight);

	glBegin(GL_QUADS);
	glMultiTexCoord2f(GL_TEXTURE0, 0, 0);
	glMultiTexCoord2f(GL_TEXTURE1, 0, 0);
	glVertex2f(x0 - GL_DRV__SUBPIXEL_BIAS + offsetX, y0 - GL_DRV__SUBPIXEL_BIAS + offsetY);

	glMultiTexCoord2f(GL_TEXTURE0, 1, 0);
	glMultiTexCoord2f(GL_TEXTURE1, 1, 0);
	glVertex2f(x1 + GL_DRV__SUBPIXEL_BIAS + offsetX, y0 - GL_DRV__SUBPIXEL_BIAS + offsetY);

	glMultiTexCoord2f(GL_TEXTURE0, 1, 1);
	glMultiTexCoord2f(GL_TEXTURE1, 1, 1);
	glVertex2f(x1 + GL_DRV__SUBPIXEL_BIAS + offsetX, y1 + GL_DRV__SUBPIXEL_BIAS + offsetY);

	glMultiTexCoord2f(GL_TEXTURE0, 0, 1);
	glMultiTexCoord2f(GL_TEXTURE1, 0, 1);
	glVertex2f(x0 - GL_DRV__SUBPIXEL_BIAS + offsetX, y1 + GL_DRV__SUBPIXEL_BIAS + offsetY);

	glEnd();
}


/* *************************************************************
 this needs to be called before render a frame
**************************************************************** */

void gldrv_initRender(int clear)
{
	// reset the default gl state
	gldrv_initState();
	// set the viewport to the correct size
	SPZ_VIEWPORT(glDriver.vpXOffset, glDriver.vpYOffset, glDriver.vpWidth, glDriver.vpHeight);

	// clear some buffers if needed
	if (clear) {
		glClearColor(0, 0, 0, 0);

		if (glDriver.stencil > 0) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}
		else {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}
}


/* *************************************************************
 this needs to be called after render a frame
**************************************************************** */

void gldrv_endRender()
{
	// error rendering the frame ?
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		dkernel_warn("Error in OpenGL: %s", gluErrorString(error));
	}

	return;
}


/* *************************************************************
 swap front and back buffers
**************************************************************** */

void gldrv_swap()
{
	glFlush();
	SDL_GL_SwapWindow(glDriver.pSDLWindow);
}

/* *************************************************************
 copies the color buffer into the current texture
**************************************************************** */

void gldrv_copyColorBuffer()
{
	char OGLError[1024];

	GLenum target = GL_TEXTURE_2D;
	GLint level = 0;
	GLint xoffset = 0;
	GLint yoffset = 0;
	GLint x = glDriver.vpXOffset;
	GLint y = glDriver.vpYOffset;
	GLsizei width = glDriver.vpWidth;
	GLsizei height = glDriver.vpHeight;

	glCopyTexSubImage2D(target, level, xoffset, xoffset, x, y, width, height);
	GL_DRV__CHECK_FOR_GL_ERRORS;
}

/* *************************************************************
 multitexture begin / end procedures
**************************************************************** */

void gldrv_enable_multitexture() {

	tex_reset_bind();

	glActiveTexture(GL_TEXTURE0);
	glClientActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE1);
	glClientActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE2);
	glClientActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE3);
	glClientActiveTexture(GL_TEXTURE3);
	glEnable(GL_TEXTURE_2D);
}

void gldrv_disable_multitexture() {

	glActiveTexture(GL_TEXTURE3);
	glClientActiveTexture(GL_TEXTURE3);
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE2);
	glClientActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE1);
	glClientActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);
	glClientActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	tex_reset_bind();
}


/* *************************************************************
 screenshot and capture code
**************************************************************** */

void gldrv_screenshot() {

	char *buffer, picname[80];
	int i;
	FILE *f;

	strcpy(picname, "screen00.tga");

	for (i = 0; i <= 99; i++) {
		picname[6] = i / 10 + '0';
		picname[7] = i % 10 + '0';
		f = fopen(picname, "rb");
		if (!f) break; // file doesn't exist
		fclose(f);
	}

	if (i == 100) {
		// 100 screenshots were made
		return;
	}

	buffer = malloc(glDriver.width * glDriver.height * 4);
	glReadPixels(0, 0, glDriver.width, glDriver.height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	tga_save(picname, buffer, glDriver.width, glDriver.height, demoSystem.compressTga);
	free(buffer);
}


void gldrv_capture()
{
	char *buffer, picname[80];

	sprintf(picname, "video/%08u.tga", demoSystem.frameCount);

	buffer = malloc(glDriver.width * glDriver.height * 3);
	glReadPixels(0, 0, glDriver.width, glDriver.height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	tga_save(picname, buffer, glDriver.width, glDriver.height, demoSystem.compressTga);
	free(buffer);
}

float gldrv_get_viewport_aspect_ratio()
{
	return (float)glDriver.vpWidth / (float)glDriver.vpHeight;
}
