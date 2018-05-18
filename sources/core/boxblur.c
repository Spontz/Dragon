#include "../main.h"

/*
void drawOffsetQuadMulti(float offsetX, float offsetY) {

	glMultiTexCoord2f(GL_TEXTURE0, 0, 0);
	glMultiTexCoord2f(GL_TEXTURE1, 0, 0);
	glVertex2f(0.0f+offsetX,0.0f+offsetY);

	glMultiTexCoord2f(GL_TEXTURE0, gldrv_get_viewport_aspect_ratio(), 0);
	glMultiTexCoord2f(GL_TEXTURE1, gldrv_get_viewport_aspect_ratio(), 0);
	glVertex2f(1.0f+offsetX,0.0f+offsetY);

	glMultiTexCoord2f(GL_TEXTURE0, gldrv_get_viewport_aspect_ratio(), (1 / gldrv_get_viewport_aspect_ratio()));
	glMultiTexCoord2f(GL_TEXTURE1, gldrv_get_viewport_aspect_ratio(), (1 / gldrv_get_viewport_aspect_ratio()));
	glVertex2f(1.0f+offsetX,1.0f+offsetY);

	glMultiTexCoord2f(GL_TEXTURE0, 0, (1 / gldrv_get_viewport_aspect_ratio()));
	glMultiTexCoord2f(GL_TEXTURE1, 0, (1 / gldrv_get_viewport_aspect_ratio()));
	glVertex2f(0.0f+offsetX,1.0f+offsetY);
}
*/

void draw_offset_quad_multi(float offsetX, float offsetY)
{
	float x0, y0, x1, y1;

	camera_2d_fit_to_viewport(glDriver.AspectRatio, &x0, &x1, &y0, &y1);

	x1 = x0 + (x1 - x0) / (float)glDriver.vpWidth;
	y1 = y0 + (y1 - y0) / (float)glDriver.vpHeight;

	glBegin(GL_QUADS);
	glMultiTexCoord2f(GL_TEXTURE0, 0, 0);
	glMultiTexCoord2f(GL_TEXTURE1, 0, 0);
	glVertex2f(x0 + offsetX, y0 + offsetY);

	glMultiTexCoord2f(GL_TEXTURE0, 1, 0);
	glMultiTexCoord2f(GL_TEXTURE1, 1, 0);
	glVertex2f(x1 + offsetX, y0 + offsetY);

	glMultiTexCoord2f(GL_TEXTURE0, 1, 1);
	glMultiTexCoord2f(GL_TEXTURE1, 1, 1);
	glVertex2f(x1 + offsetX, y1 + offsetY);

	glMultiTexCoord2f(GL_TEXTURE0, 0, 1);
	glMultiTexCoord2f(GL_TEXTURE1, 0, 1);
	glVertex2f(x0 + offsetX, y1 + offsetY);
	glEnd();
}

void render_boxblur (int tex, float radius) {
	float color[4] = { 0.5f,0.5f,0.5f,0.5f };
	float pixelX, pixelY;
	float nOffset;

	gldrv_enable_multitexture();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_array[tex]->id);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_array[tex]->id);
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, color); //-V525
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);

	glColor4f(1, 1, 1, 0.33f);

	nOffset = 1.0f;
	pixelX = nOffset / (float)glDriver.vpWidth;
	pixelY = nOffset / (float)glDriver.vpHeight;

	for (;;)
	{
		gldrv_copyColorBuffer();
		draw_offset_quad_multi(pixelX, 0);

		gldrv_copyColorBuffer();
		draw_offset_quad_multi(0, pixelY);

		if (nOffset < radius)
		{
			nOffset *= 2;
			if (nOffset > radius)
				nOffset = radius;
			pixelX = nOffset / (float)glDriver.vpWidth;
			pixelY = nOffset / (float)glDriver.vpHeight;
		}
		else
		{
			break;
		}
	}

	gldrv_disable_multitexture();
}
