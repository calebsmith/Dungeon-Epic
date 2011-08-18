#include <stdlib.h>
#include <string>
#include <time.h>
#include <math.h>
#include <new>

#include "constants.h"
#include "functions.h"
#include "variables.h"
#include "classes.h"

#include <SDL.h> 
#include <SDL_image.h>
#include "SDL/SDL_ttf.h"

int main(int argc, char *argv[])
{   
    init(images, fonts);   
    while (gameopen){                                           //loop until x is clicked
        loadmap(area, level, player1, images); mapstatic = 1;
        //begin game loop
        while (gameopen && mapstatic){                          //loop until x is clicked, or changing the map
            gameopen = get_input(event, input);	                //get input, then do logic, then render graphics
            mapstatic = logic(level, player1, camera, input);
            render(level, player1, camera, images, fonts, frametimer);   
        }             
    }    
    quit(images, fonts);//free memory before closing
    return 0;
}

