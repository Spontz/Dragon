/*
	fbo.c: fbo (Frame Buffer Object) management library
*/

#include "../main.h"

#include <gl/GLU.h>
#include <gl/GL.h>


// ******************************************************************

#define MAX_FBO 256

fbo_t *fbo_array[MAX_FBO];

// fbo counter
static int fbo_count = 0;

// fbo binding counter
static int fbo_bind_count = 0;

// ******************************************************************

// actual bind
static int fbo_current = -1;
static int fbo_current_tex = -1;

// ******************************************************************

int fbo_init () {

	if (fbo_count >= MAX_FBO) dkernel_error ("Too many fbo's (limited to %d)", MAX_FBO);

	// create fbo structure
	fbo_array[fbo_count] = (fbo_t *) malloc(sizeof(fbo_t));
	memset(fbo_array[fbo_count], 0, sizeof(fbo_t));

	// fbo is not uploaded
	fbo_array[fbo_count]->id_fbo = -1;
	fbo_array[fbo_count]->id_depthBuffer = -1;
	fbo_array[fbo_count]->id_tex = -1;

	// reset texture properties
	fbo_properties (fbo_count, 0);

	return fbo_count++;
}

// ******************************************************************

int fbo_new (int width, int height, int iformat, int format, int ttype) {

	int fbo = fbo_init ();

	// setup texture configuration
	fbo_array[fbo]->width = width;
	fbo_array[fbo]->height = height;
	fbo_array[fbo]->iformat = iformat;
	fbo_array[fbo]->format = format;
	fbo_array[fbo]->ttype = ttype;

	return fbo;
}

// ******************************************************************

void fbo_properties (int index, int flags) {

	fbo_t *fbo = fbo_array[index];
	fbo->properties = flags;

	fbo->target = GL_TEXTURE_2D;
	fbo->wrap = GL_CLAMP_TO_EDGE;

	if (flags & MODULATE) {
		fbo->texfunc = GL_MODULATE;
	} else {
		fbo->texfunc = GL_REPLACE;
	}
}

// ******************************************************************

void fbo_upload(int index, int cache)
	{
	char	OGLError[1024];
	GLenum	error;
	fbo_t*	fbo = fbo_array[index];
	#ifndef WIN32
		GLenum status;
	#endif

	// fbo already uploaded? check only when the engine is loading scripts from the hasrd disk
	if (demoSystem.state == DEMO_LOADING)
		if (cache == 1)
			if (fbo->id_tex != -1) return;

	// clear error flags
	error = glGetError();
	if (error != GL_NO_ERROR)
		dkernel_error("fbo_upload: Error before upload fbo '%d': %s", fbo->id_fbo, gluErrorString(error));

	// Setup our FBO
	glGenFramebuffers(1, &(fbo->id_fbo));
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->id_fbo);

	// Create the render buffer for depth	
	glGenRenderbuffers(1, &(fbo->id_depthBuffer));
	glBindRenderbuffer(GL_RENDERBUFFER, fbo->id_depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fbo->width, fbo->height);

	// Setup a texture to render to
	glGenTextures(1, &(fbo->id_tex));
	glBindTexture(GL_TEXTURE_2D, fbo->id_tex);
	
	glTexImage2D(GL_TEXTURE_2D, 0, fbo->iformat,  fbo->width, fbo->height, 0, fbo->format, fbo->ttype, NULL);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Bind buffers
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->id_fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, fbo->id_depthBuffer);

	// And attach it to the FBO so we can render to it
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->id_tex, 0);
	// Attach the depth render buffer to the FBO as it's depth attachment
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo->id_depthBuffer);

	while( gl_drv_check_for_gl_errors(OGLError))
		dkernel_error("OGL Error inside fbo_upload (0) :\n\n%s", OGLError);

	// hack: Para ver el error de una textura mal subida
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		{
		switch(status)
			{
			case GL_FRAMEBUFFER_COMPLETE:
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				dkernel_error("fbo_upload: Error uploading fbo '%d' (id='%d'): glCheckFramebufferStatus returned GL_FRAMEBUFFER_UNSUPPORTED. Choose other format, this is not supported in the current system.", index, fbo->id_fbo);
				break;
			default:
				dkernel_error("fbo_upload: Error uploading fbo '%d' (id='%d'): Invalid framebuffer status.", index, fbo->id_fbo);
				break;
			}
		}

	// Unbind buffers
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// the binded texture has changed
	fbo_reset_bind();

	while (gl_drv_check_for_gl_errors(OGLError))
		dkernel_error("fbo_upload: Error uploading fbo '%d' (id='%d'):\n\n%s",  index, fbo->id_fbo, OGLError);
	}

// ******************************************************************

void fbo_reset_bind () {

	fbo_current = -1;
}

// ******************************************************************

int fbo_get_texbind_id(int index) {

	if (index < MAX_FBO) {
		return fbo_array[index]->id_tex;
	}
	else
		return -1;
}

// ******************************************************************

void fbo_bind (int index) {

	fbo_t *fbo = fbo_array[index];

	if (fbo->id_fbo != fbo_current) {
		// Bind buffers and make attachments
		glBindFramebuffer(GL_FRAMEBUFFER, fbo->id_fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, fbo->id_depthBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->id_tex, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo->id_depthBuffer);

		// Save the view port and set it to the size of the texture
		glPushAttrib(GL_VIEWPORT_BIT);
		SPZ_VIEWPORT(0,0,fbo->width,fbo->height);
		fbo_current = fbo->id_fbo;
		fbo_bind_count++;
	}
}

// ******************************************************************

void fbo_bind_tex (int index) {

	fbo_t *fbo = fbo_array[index];

	if (fbo->id_tex != fbo_current_tex) {
		glBindTexture (GL_TEXTURE_2D, fbo->id_tex);
		tex_reset_bind ();	// Me must advise to the texture array that an fbo has been used 
		fbo_current_tex = fbo->id_tex;
	}
}

// ******************************************************************

void fbo_reset_bind_tex () {

	fbo_current_tex = -1;
}

// ******************************************************************

void fbo_unbind () {

	if (fbo_bind_count>0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		fbo_current = -1;
		// Restore old view port and set rendering back to default frame buffer
		glPopAttrib();
		fbo_bind_count--;
	}
	else
		dkernel_trace("fbo_unbind [aborted]: System has tried to unbind an fbo, but it has been already unbinded!");
}

// *************************************************************

void fbo_free (int index) {

	fbo_t *fbo = fbo_array[index];

	if (fbo) {
		if (fbo->id_fbo != -1)			glDeleteFramebuffers(1, &(fbo->id_fbo));
		if (fbo->id_depthBuffer != -1)	glDeleteRenderbuffers(1, &(fbo->id_depthBuffer));
		if (fbo->id_tex != -1)			glDeleteTextures(1, &(fbo->id_tex));
	}
}

// ******************************************************************

