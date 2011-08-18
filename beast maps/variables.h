#ifndef VARIABLES_H
#define VARIABLES_H
#include <SDL.h>
#include <SDL_image.h>
#include "SDL/SDL_ttf.h"

#include "classes.h"

    bool gameopen = 1;   
    bool mapstatic = 1;
    SDL_Event event;
    Idevice input;     
    Viewpoint camera;
    Map level;
    Surface images;
    Font fonts;
#endif
