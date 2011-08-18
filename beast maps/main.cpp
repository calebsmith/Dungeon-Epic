
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
   srand(time(NULL));
   
   Map* g;
             
    g = new Map[10];
       
   int i;
   for(i = 0; i < 10; i++) {
        g[i].init();
       g[i].showall();
        
    }
  
   
    return 0;
}









