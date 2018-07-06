/*
	texture.c: texture management library
*/

#include "../main.h"

// ******************************************************************

#define MAX_TEXTURES 65535
//#define MAX_TEXTURES 256

texture_t *tex_array[MAX_TEXTURES];

// texture counter
static int tex_count = 0;

// actual bind and env mode
static int tex_current = -1;
static int env_mode = -1;

// ******************************************************************

int getCompressedFormat(texture_t *tex);

// ******************************************************************

int tex_init () {

	if (tex_count >= MAX_TEXTURES) dkernel_error ("Too many textures (limited to %d)", MAX_TEXTURES);

	// create texture
	tex_array[tex_count] = (texture_t *) malloc(sizeof(texture_t));
	memset(tex_array[tex_count], 0, sizeof(texture_t));

	// texture is not uploaded
	tex_array[tex_count]->id = -1;

	// reset texture properties
	tex_properties (tex_count, 0);

	return tex_count++;
}

// ******************************************************************

int tex_new (int width, int height, int format, int components) {

	int tex = tex_init ();

	// setup texture configuration
	// custom textures are not restricted to texture detail setting
	tex_array[tex]->data = malloc (width * height * components);
	tex_array[tex]->width = width;
	tex_array[tex]->height = height;
	tex_array[tex]->format = format;
	tex_array[tex]->iformat = components;
	tex_array[tex]->dynamic = TRUE;

	return tex;
}

// ******************************************************************

void tex_properties (int index, int flags) {

	texture_t *tex = tex_array[index];
	tex->properties = flags;

	if (flags & TEXTURE_1D) tex->target = GL_TEXTURE_1D;
	else tex->target = GL_TEXTURE_2D;

	if (flags & NO_MIPMAP) tex->mipmap = GL_LINEAR;
	else tex->mipmap = GL_LINEAR_MIPMAP_LINEAR;	// GL_LINEAR_MIPMAP_LINEAR -> Best Quality // GL_LINEAR_MIPMAP_NEAREST -> Best Performance

	if (flags & CLAMP) tex->wrap = GL_CLAMP;
	else if (flags & CLAMP_TO_EDGE) tex->wrap = GL_CLAMP_TO_EDGE;
	else tex->wrap = GL_REPEAT;
	
	if (flags & MODULATE) {
		tex->texfunc = GL_MODULATE;
	} else {
		tex->texfunc = GL_REPLACE;
	}
}

// ******************************************************************

void tex_persistent (int index, int value) {
	tex_array[index]->persistent = value;
}

// ******************************************************************

void tex_clean (int index) {

	texture_t *tex = tex_array[index];
	memset (tex->data, 0, tex->width * tex->height * tex->iformat);
}

// ******************************************************************

void tex_upload (int index, int cache)
	{
	GLenum error;
	int curW, curH, format;
	texture_t *tex = tex_array[index];

	// texture already uploaded? check only when the engine is loading scripts from the hard disk
	if (demoSystem.state == DEMO_LOADING)
		if (cache == 1)
			if (tex->id != -1) return;

	// clear error flags
	error = glGetError ();
	if (error != GL_NO_ERROR)
		dkernel_error("tex_upload: Error before upload texture '%s': %s", tex->name);

	// get texture size
	curW = tex->width;
	curH = tex->height;
	if (tex->mipmap != GL_LINEAR_MIPMAP_LINEAR)
		tex->memory = curW * curH * tex->iformat;
	else
		{
		tex->memory = 0;
		while ((curW != 1) || (curH != 1))
			{
			tex->memory += curW * curH * tex->iformat;
			if (curW > 1) curW /= 2;
			if (curH > 1) curH /= 2;
			}
		}

	// reserve a new texture object
	glGenTextures (1, (GLuint *)&tex->id);
	glBindTexture (tex->target, tex->id);

	// setup texture parameters
	glTexParameteri (tex->target, GL_TEXTURE_MIN_FILTER, tex->mipmap);
	glTexParameteri (tex->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (tex->target, GL_TEXTURE_WRAP_S, tex->wrap);
	glTexParameteri (tex->target, GL_TEXTURE_WRAP_T, tex->wrap);

	if (tex->properties & TEXTURE_1D) {// 1D textures
		glTexImage1D (GL_TEXTURE_1D, 0, tex->iformat, tex->width, 0, tex->format, GL_UNSIGNED_BYTE, tex->data);
	}
	else {	// 2D textures
		if (tex->mipmap != GL_LINEAR_MIPMAP_LINEAR) { // Load Texture without generating MipMaps
			format = getCompressedFormat(tex);
			glTexImage2D(GL_TEXTURE_2D, 0, format, tex->width, tex->height, 0, tex->format, GL_UNSIGNED_BYTE, tex->data);
		}
		else { // Load Texture with MipMaps generation
			format = getCompressedFormat(tex);
			glTexImage2D(GL_TEXTURE_2D, 0, format, tex->width, tex->height, 0, tex->format, GL_UNSIGNED_BYTE, tex->data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}

	// error uploading the texture ?
	error = glGetError();

	if (error != GL_NO_ERROR)
		dkernel_error("tex_upload: Error uploading texture '%s': %s", tex->name);

	if (!tex->persistent) {
		// free SDL allocated data
		if (tex->SDL_surface) {
			SDL_FreeSurface(tex->SDL_surface);
			tex->SDL_surface = NULL;
			tex->data = NULL;
		}
		else if (tex->data)	{
			// free custom data
			free(tex->data);
			tex->data = NULL;
		}
	}

	// the binded texture has changed
	tex_reset_bind ();
}

// ******************************************************************

void tex_reset_bind() {
	tex_current = -1;
	env_mode = -1;
}

// ******************************************************************

void tex_bind(int index) {
	char OGLError[1024];

	texture_t* pTexture = tex_array[index];

	if (pTexture->id != tex_current) {
		glBindTexture(pTexture->target, pTexture->id);
		// Advise to the fbo array that a normal texture has been used
		fbo_reset_bind_tex();
		tex_current = pTexture->id;
	}

	while (gl_drv_check_for_gl_errors(OGLError))
		dkernel_warn("OGL Error in tex_bind glBindTexture:\n\n%s", OGLError);

	if (env_mode != pTexture->texfunc) {
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, pTexture->texfunc);
		env_mode = pTexture->texfunc;
	}

	while (gl_drv_check_for_gl_errors(OGLError))
		dkernel_warn("OGL Error in tex_bind glTexEnvi:\n\n%s", OGLError);
}

// *************************************************************

void tex_envmode (int mode) {
	glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode);
	env_mode = mode;
}

// *************************************************************

extern unsigned int get_closest_power_of_two(unsigned int value);

int tex_load(const char* fname, int cache)	{
	SDL_Surface*	TextureImage;
	texture_t*		tex;
	int				tex_number, i;

	// texture is already loaded? check only if the engine is in the loading state
	if (demoSystem.state == DEMO_LOADING) {
		if (cache == 1) {
			for (i=0; i<tex_count; i++)	{
				if (tex_array[i] && tex_array[i]->name && spz_strcmpi(tex_array[i]->name, fname) == 0)
					return i;
			}
		}
	}
	
	// load texture resource
	if (!(TextureImage = IMG_Load(fname)))	{
		dkernel_warn("Error loading texture \"%s\":\n%s", fname, IMG_GetError());
		return -1;
	}

	// paletized textures not allowed
	if (TextureImage->format->palette) {
		dkernel_warn("Unsupported paletted texture '%s'", fname);
		return -1;
	}

	tex_number = tex_init();
	tex = tex_array[tex_number];

	tex->name = _strdup (fname);
	tex->data = TextureImage->pixels;
	tex->SDL_surface = (void *) TextureImage;

	tex->width = TextureImage->w;
	tex->height = TextureImage->h;
	tex->iformat = TextureImage->format->BytesPerPixel;

	// 24 bits texture format finder
	if (TextureImage->format->BitsPerPixel == 24) {
		if (TextureImage->format->Rshift < TextureImage->format->Bshift)
			tex->format = GL_RGB;
		else
			tex->format = GL_BGR_EXT;
	}

	// 32 bits texture format finder
	else if (TextureImage->format->BitsPerPixel == 32) {
		if (TextureImage->format->Rshift < TextureImage->format->Bshift)
			tex->format = GL_RGBA;
		else
			tex->format = GL_BGRA_EXT;
	}
	else {
		// unsupported texture format
		dkernel_warn("Unsupported texture format in \"%s\"", fname);
		return -1;
	}

	return tex_number;
}

// ******************************************************************

int tex_get_OpenGLid(int index) {

	texture_t *tex = tex_array[index];
	return tex->id;
}

// ******************************************************************

void tex_free (int index) {

	texture_t *tex = tex_array[index];

	if (tex) {
		if (tex->id != -1)
			glDeleteTextures(1, (GLuint *)&tex->id);
		free(tex->name);
		if (tex->SDL_surface)
			SDL_FreeSurface(tex->SDL_surface);
		free(tex->data);
		free(tex);
		tex_array[index] = NULL;
	}
}

// ******************************************************************

int tex_get_used_memory () {
	int i, memory = 0;

	for (i = 0; i < tex_count; i++) {
		if (tex_array[i] != NULL) {
			memory += tex_array[i]->memory;
		}
	}

	return memory;
}

// ******************************************************************

void load_palette (const char *fname, palette pal) {
	FILE *f = fopen(fname, "rb");
	if (!f)
		dkernel_error("Error opening '%s'", fname);
	fread(pal, 1, 768, f);
	fclose(f);
}

// ******************************************************************

int getCompressedFormat(texture_t *tex) {

	if (!glDriver.ext.texture_compression) return tex->iformat;
	if (tex->dynamic) return tex->iformat;

	// get new format for compressed texture
	switch (tex->iformat) {
		case 3: return GL_COMPRESSED_RGB;
		case 4: return GL_COMPRESSED_RGBA;
	}

	return tex->iformat;
}

