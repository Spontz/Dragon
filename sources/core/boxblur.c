#include "../main.h"

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
		gldrv_multitexscreenquad_offset(pixelX, 0);

		gldrv_copyColorBuffer();
		gldrv_multitexscreenquad_offset(0, pixelY);

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
