/*
	highpassfilter.c: high pass filter related code
*/

#include "../main.h"

#include "drivers/gldriver.h"
#include "texture.h"

// ******************************************************************

void render_highpassfilter (int tex, float threshold_R, float threshold_G, float threshold_B, int accum) {

	int i;

	// disable texturing
	glDisable (GL_TEXTURE_2D);

	// invert color buffer
	glBlendFunc (GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glColor4f (1,1,1,1);
	gldrv_screenquad ();

	// add the threshold
	glBlendFunc (GL_ONE, GL_ONE);
	glColor4f (threshold_R, threshold_G ,threshold_B, 1);
	gldrv_screenquad ();

	// invert color buffer
	glBlendFunc (GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glColor4f (1,1,1,1);
	gldrv_screenquad ();

	// enable texturing
	glEnable (GL_TEXTURE_2D);

	// accumulate color buffer
	tex_bind (tex);
	gldrv_copyColorBuffer ();

	glBlendFunc (GL_ONE, GL_ONE);
	glColor4f (1,1,1,1);
	glBegin (GL_QUADS);
	for (i=0; i<accum; i++) {
		glTexCoord2f(0,0);
		glVertex2f(0,0);

		glTexCoord2f(1,0);
		glVertex2f(1,0);

		glTexCoord2f(1,1);
		glVertex2f(1,1);

		glTexCoord2f(0,1);
		glVertex2f(0,1);
		/*
		glTexCoord2f (0,0);
		glVertex2f (0,0);

		glTexCoord2f (gldrv_get_viewport_aspect_ratio(),0);
		glVertex2f (1,0);

		glTexCoord2f (gldrv_get_viewport_aspect_ratio(),(1 / gldrv_get_viewport_aspect_ratio()));
		glVertex2f (1,1);

		glTexCoord2f (0,(1 / gldrv_get_viewport_aspect_ratio()));
		glVertex2f (0,1);
		*/
	}
	glEnd ();
}

// ******************************************************************

extern void draw_offset_quad_multi(float offsetX, float offsetY);

void render_highpassfilter_ext (int tex, float threshold_R, float threshold_G, float threshold_B, int accum)
	{
	int i;
	float x0, y0, x1, y1;

	camera_2d_fit_to_viewport(glDriver.AspectRatio, &x0, &x1, &y0, &y1);

	x1 = x0 + (x1 - x0) / (float)glDriver.vpWidth;
	y1 = y0 + (y1 - y0) / (float)glDriver.vpHeight;

	// disable texturing
	glDisable (GL_TEXTURE_2D);

	// substract the threshold
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
	glColor4f (threshold_R, threshold_G ,threshold_B, 0);
	gldrv_screenquad ();
	glBlendEquationEXT (GL_FUNC_ADD);

	// enable multitexturing
	gldrv_enable_multitexture();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_array[tex]->id);
	gldrv_copyColorBuffer ();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_array[tex]->id);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD);

	glBlendFunc (GL_ONE, GL_ONE);
	glColor4f (1,1,1,1);
	glBegin (GL_QUADS);
	for (i=0; i<accum/2; i++)
		{
		glMultiTexCoord2f(GL_TEXTURE0, 0, 0);
		glMultiTexCoord2f(GL_TEXTURE1, 0, 0);
		glVertex2f(x0, y0);

		glMultiTexCoord2f(GL_TEXTURE0, 1, 0);
		glMultiTexCoord2f(GL_TEXTURE1, 1, 0);
		glVertex2f(x1, y0);

		glMultiTexCoord2f(GL_TEXTURE0, 1, 1);
		glMultiTexCoord2f(GL_TEXTURE1, 1, 1);
		glVertex2f(x1, y1);

		glMultiTexCoord2f(GL_TEXTURE0, 0, 1);
		glMultiTexCoord2f(GL_TEXTURE1, 0, 1);
		glVertex2f(x0, y1);

		/*
		glMultiTexCoord2f(GL_TEXTURE0, 0, 0);
		glMultiTexCoord2f(GL_TEXTURE1, 0, 0);
		glVertex2f (0,0);

		glMultiTexCoord2f(GL_TEXTURE0, gldrv_get_viewport_aspect_ratio(), 0);
		glMultiTexCoord2f(GL_TEXTURE1, gldrv_get_viewport_aspect_ratio(), 0);
		glVertex2f (1,0);

		glMultiTexCoord2f(GL_TEXTURE0, gldrv_get_viewport_aspect_ratio(), (1 / gldrv_get_viewport_aspect_ratio()));
		glMultiTexCoord2f(GL_TEXTURE1, gldrv_get_viewport_aspect_ratio(), (1 / gldrv_get_viewport_aspect_ratio()));
		glVertex2f (1,1);

		glMultiTexCoord2f(GL_TEXTURE0, 0, (1 / gldrv_get_viewport_aspect_ratio()));
		glMultiTexCoord2f(GL_TEXTURE1, 0, (1 / gldrv_get_viewport_aspect_ratio()));
		glVertex2f (0,1);
		*/
		}
	glEnd ();

	gldrv_disable_multitexture();
}
