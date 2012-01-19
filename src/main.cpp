#include <stdlib.h>
#include <string>
#include <time.h>
#include <math.h>
#include <new>

#include <SDL.h> 
#include <SDL_image.h>
#include "SDL/SDL_ttf.h"

#include "constants.h"
#include "functions.h"
#include "variables.h"
#include "classes.h"


int main(int argc, char *argv[]) {
    init(images, fonts);   
    while (gameopen) {
        loadmap(area, level, player1, images); mapstatic = 1;
        while (gameopen && mapstatic) {
            gameopen = get_input(event, input);
            mapstatic = logic(level, player1, camera, input);
            render(level, player1, camera, images, fonts, frametimer);
        }
    }
    quit(images, fonts);
    return 0;
}

