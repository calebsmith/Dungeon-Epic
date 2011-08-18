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
    Ticks frametimer;
    
    Map level;    
    Zone area;
    Player player1;
    
    Surface images;
    Font fonts;
#endif
