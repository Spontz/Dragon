#include "../interface/demo.h"
#include <io.h>

typedef enum {
	video_drawing_mode_fit_to_viewport = 0,	// ajusta la imagen para que ocupe todo el viewport (sin distorsion de aspect)
	video_drawing_mode_fit_to_content = 1	// ajusta la imagen para se vea completa (sin distorsion de aspect)
} enum_video_drawing_mode;


typedef struct	{
	char		*folder;
	rect2d_t	quad;
	int			total_fps;
	float		fps;
	char		loop;
	float		time_start;
	float		time_end;
	int			*textures;
	enum_video_drawing_mode	mode;
} video_section;

static video_section *local;

void preload_video() {
}

void load_video() {
	// script validation
	if (mySection->stringNum != 1) {
		section_error("1 string needed: The folder that contains the video images");
		return;
	}

	if (mySection->paramNum != 3) {
		section_error("3 params needed: fps, loop mode and fullscreen");
		return;
	}

	local = (video_section*) malloc(sizeof(video_section));
	mySection->vars = (void *) local;

	local->fps = mySection->param[0];
	local->loop = (char)mySection->param[1];
	switch((unsigned int)mySection->param[2]) {
		case video_drawing_mode_fit_to_content:
			local->mode = video_drawing_mode_fit_to_content;
			break;
		case video_drawing_mode_fit_to_viewport:
			local->mode = video_drawing_mode_fit_to_viewport;
			break;
		default:
			section_error("Invalid value for parameter 3 (fullscreen mode)");
	}

	// Load the textures
	local->folder = mySection->strings[0];
	struct _finddata_t FindData;
	intptr_t hFile;
	char fullpath[512];
	char ImageRelativePath[512];
	strcpy(fullpath, local->folder);
	strcat(fullpath, "/*.*");
	int i = 0;
	// get number of textures to process
	if ((hFile = _findfirst(fullpath, &FindData)) != -1L) {
		do {
			if (!(FindData.attrib & _A_SUBDIR))
				i++;
		} while (_findnext(hFile, &FindData) == 0);
	}
	if (i == 0)
		section_error("Video: No files to process!");

	local->total_fps = i;
	local->textures = (int*)malloc(sizeof(int)*local->total_fps);
	// Load textures
	i = 0;
	if ((hFile = _findfirst(fullpath, &FindData)) != -1L) {
		do {
			if (!(FindData.attrib & _A_SUBDIR)) { // If it's not a subfolder... we process it
				strcpy(ImageRelativePath, local->folder);
				strcat(ImageRelativePath, "/");
				strcat(ImageRelativePath, FindData.name);
				// Load a texture
				local->textures[i] = tex_load(ImageRelativePath, USE_CACHE);
				if (local->textures[i]== -1) {
					section_error("Video: Error loading image: %s", ImageRelativePath);
					return;
				}
				tex_properties(local->textures[i], CLAMP);
				tex_upload(local->textures[i], USE_CACHE);
				i++;
			}
		} while (_findnext(hFile, &FindData) == 0);
	}
	if (i == 0) {
		section_error("Video: Video not found on: %s", fullpath);
		return;
	}

	mySection->loaded = 1;
}

void init_video() {
}

void render_video() {
	int i;

	local = (video_section *) mySection->vars;

	glDisable(GL_DEPTH_TEST); 
	{
		if (mySection->hasBlend) {
			glBlendFunc(mySection->sfactor, mySection->dfactor);
			glEnable(GL_BLEND);
		}
		
		if (mySection->hasAlpha) {
			float alpha = mySection->alpha1 + mySection->runTime * (mySection->alpha2 - mySection->alpha1) / mySection->duration;
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(mySection->alphaFunc, alpha);
		}
		
		// Calculate the image to draw
		int myframe = (int)round (mySection->runTime * local->fps);
		if (local->loop) {
			myframe = myframe % local->total_fps;
		}	else {
			if (myframe >= local->total_fps)
				myframe = (local->total_fps - 1);
		}

		

		float TextureAspectRatio = (float)tex_array[local->textures[myframe]]->width / (float)tex_array[local->textures[myframe]]->height;
		switch(local->mode) {
			case video_drawing_mode_fit_to_viewport:
				camera_2d_fit_to_viewport(TextureAspectRatio, &local->quad.x0, &local->quad.x1, &local->quad.y0, &local->quad.y1);
				break;
			case video_drawing_mode_fit_to_content:
				camera_2d_fit_to_content(TextureAspectRatio, &local->quad.x0, &local->quad.x1, &local->quad.y0, &local->quad.y1);
				break;
			default:
				section_error("Invalid video section drawing mode");
				break;
		}
		

		// store vertex values
		local->quad.pc[0][0] = local->quad.x0;
		local->quad.pc[1][0] = local->quad.x1;
		local->quad.pc[2][0] = local->quad.x1;
		local->quad.pc[3][0] = local->quad.x0;

		local->quad.pc[0][1] = local->quad.y0;
		local->quad.pc[1][1] = local->quad.y0;
		local->quad.pc[2][1] = local->quad.y1;
		local->quad.pc[3][1] = local->quad.y1;

		local->quad.tc[0][0] = 0; local->quad.tc[0][1] = 1;
		local->quad.tc[1][0] = 1; local->quad.tc[1][1] = 1;
		local->quad.tc[2][0] = 1; local->quad.tc[2][1] = 0;
		local->quad.tc[3][0] = 0; local->quad.tc[3][1] = 0;

		camera_set2d();

		tex_bind(local->textures[myframe]);
		glBegin(GL_QUADS);
			for (i=0; i<4; i++) {
				glTexCoord2fv(local->quad.tc[i]);
				glVertex2fv(local->quad.pc[i]);
			}
		glEnd();

		camera_restore();

		if (mySection->hasAlpha)
			glDisable(GL_ALPHA_TEST);

		if (mySection->hasBlend)
			glDisable(GL_BLEND);
	}
	glEnable(GL_DEPTH_TEST);
}

// ******************************************************************

void end_video () {
}
