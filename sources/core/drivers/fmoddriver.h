#ifndef __FMODDRIVER_H
#define __FMODDRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

void sound_init( );
void sound_play( );
void sound_update( );
void sound_pause( );
void sound_stop( );
void sound_end( );

float sound_cpu( );
int sound_channels( );

// ******************************************************************

/*#ifdef _WIN32
    #include <fmod.h>
    #include <fmod_errors.h>
#elif __APPLE__
    #include "fmodex/include/fmod.h"
    #include "fmodex/include/fmod_errors.h"
#else
    #include "../fmodex/include/fmod.h"
    #include "../fmodex/include/fmod_errors.h"
#endif*/

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
