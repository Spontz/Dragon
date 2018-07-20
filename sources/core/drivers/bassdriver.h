#ifndef __BASSDRIVER_H
#define __BASSDRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

extern void sound_init( );
extern void sound_play( );
extern void sound_update( );
extern void sound_pause( );
extern void sound_stop( );
extern void sound_end( );

float sound_cpu( );
int sound_channels( );

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
