#ifndef __EVENTS_H
#define __EVENTS_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

// definition of engine control keys
#define KEY_EXIT			SDL_SCANCODE_ESCAPE
#define KEY_SCREENSHOT		SDL_SCANCODE_PRINTSCREEN
#define KEY_TIME			SDL_SCANCODE_RETURN
#define KEY_SHOWTIME		SDL_SCANCODE_T
#define KEY_SHOWFPS			SDL_SCANCODE_U
#define KEY_SHOWSOUND		SDL_SCANCODE_I


#define KEY_PLAY_PAUSE		SDL_SCANCODE_F1
#define KEY_REWIND			SDL_SCANCODE_F2
#define KEY_FASTFORWARD		SDL_SCANCODE_F3
#define KEY_RESTART			SDL_SCANCODE_F4

// definition of camera control keys
#define KEY_CAPTURE			SDL_SCANCODE_SPACE
#define KEY_FORWARD			SDL_SCANCODE_UP
#define KEY_BACKWARD		SDL_SCANCODE_DOWN
#define KEY_STRAFELEFT		SDL_SCANCODE_LEFT
#define KEY_STRAFERIGHT		SDL_SCANCODE_RIGHT
#define KEY_CAMRESET		SDL_SCANCODE_R
#define KEY_FOVUP			SDL_SCANCODE_Q
#define KEY_FOVDOWN			SDL_SCANCODE_A
#define KEY_ROLLUP			SDL_SCANCODE_W
#define KEY_ROLLDOWN	    SDL_SCANCODE_S
#define KEY_MULTIPLIER		SDL_SCANCODE_RSHIFT

// definition of spline control keys
#define KEY_NUM_1			SDL_SCANCODE_1
#define KEY_NUM_2			SDL_SCANCODE_2
#define KEY_NUM_3			SDL_SCANCODE_3
#define KEY_NUM_4			SDL_SCANCODE_4
#define KEY_NUM_5			SDL_SCANCODE_5
#define KEY_NUM_6			SDL_SCANCODE_6
#define KEY_NUM_7			SDL_SCANCODE_7
#define KEY_NUM_8			SDL_SCANCODE_8
#define KEY_NUM_9			SDL_SCANCODE_9
#define KEY_NUM_0			SDL_SCANCODE_0

// ******************************************************************

int eventHandler(SDL_Event event);

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
