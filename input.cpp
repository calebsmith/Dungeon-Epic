#include <stdlib.h>
#include <string>
#include <time.h>

#include <SDL.h>
#include "SDL_image.h"
#include "SDL/SDL_ttf.h"

#include "constants.h"
#include "functions.h"
#include "classes.h"

bool get_input(SDL_Event event, Idevice& input){
    
    if(SDL_PollEvent(&event) == 0) return 1;
    if (event.type == SDL_QUIT) return 0;	
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) return 0;
    
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)  keyin(event, input);
    if (event.type == SDL_MOUSEBUTTONDOWN) mousein(event, input);	
    
    return 1;
}

void keyin(SDL_Event event, Idevice& input)
{  
    switch(event.type)
    {
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
                case SDLK_UP: input.up(); break;                    
                case SDLK_DOWN: input.down(); break;
                case SDLK_LEFT: input.left(); break;
                case SDLK_RIGHT: input.right(); break;  
                default: break;              
            }
            break;
        case SDL_KEYUP:
            switch(event.key.keysym.sym)
            {
                case SDLK_UP: input.up(); break;
                case SDLK_DOWN: input.down(); break;
                case SDLK_LEFT: input.left(); break;
                case SDLK_RIGHT: input.right(); break;
                default: break;
            }
            break;
    }
}
void mousein(SDL_Event event, Idevice& input)
{
    input.put_x(event.motion.x);
    input.put_y(event.motion.y);
    
    if(event.button.button == SDL_BUTTON(SDL_BUTTON_LEFT) && event.type == SDL_MOUSEBUTTONDOWN){        
		input.put_click(ON);
	}
	if(event.button.button == SDL_BUTTON(SDL_BUTTON_LEFT) && event.type == SDL_MOUSEBUTTONUP){         
		input.put_click(OFF);
	}	
	
}
