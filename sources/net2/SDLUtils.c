/*
    NET2 is a threaded, event based, network IO library for SDL.
    Copyright (C) 2002 Bob Pendleton

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public License
    as published by the Free Software Foundation; either version 2.1
    of the License, or (at your option) any later version.
    
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.
    
    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA

    If you do not wish to comply with the terms of the LGPL please
    contact the author as other terms are available for a fee.
    
    Bob Pendleton
    Bob@Pendleton.com
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
	#include <windows.h>
#endif

#include "SDLUtils.h"
#include "net2.h"
#include "fastevents.h"

//----------------------------------------
//
// SDL helper functions
//

void mySDL_Quit()
{
  NET2_Quit();
  FE_Quit();
  SDLNet_Quit();
  SDL_Quit();
}

char *mySDL_Init(Uint32 flags)
{
  if (-1 == SDL_Init(flags))
  {
    return SDL_GetError();
  }

  if (-1 == SDLNet_Init())
  {
    return SDLNet_GetError();
  }

  if (-1 == FE_Init())
  {
    return FE_GetError();
  }

  if (-1 == NET2_Init())
  {
    return NET2_GetError();
  }

  return NULL;
}

void mySDLInitOrQuit(Uint32 flags)
{
  char *message = NULL;


  if (NULL != (message = mySDL_Init(flags)))
  {
    printf("Failed to initialize SDL error=%s\n", message);
	#ifdef WIN32
		MessageBox(0, "Failed to initialize SDL", "ERROR",0);
	#endif
    exit(1);
  }

  atexit(mySDL_Quit);
}

