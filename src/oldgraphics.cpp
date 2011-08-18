#include <stdlib.h>
#include <string>
#include <time.h>

#include <SDL.h>
#include "SDL_image.h"
#include "SDL/SDL_ttf.h"

#include "constants.h"
#include "functions.h"
#include "classes.h"

void render(Map& level, Player& player1, Viewpoint& camera, Surface& images, Font& fonts, Ticks& frametimer)
{
    //backdrop(images);
    map_render(level, camera, images);
    map_display(level, camera, images);
    player_display(level, player1, camera, images);
       
   // text_display(images,fonts);
    frametimer.wait_frame();
    SDL_Flip(images.buffer);
    return;
}


void backdrop(Surface& images)
{	
    if (images.background != NULL)
        SDL_BlitSurface(images.background,NULL,images.buffer,NULL);
	return;
}
void map_render(Map& level, Viewpoint& camera, Surface& images)
{
    SDL_Rect tilexy; //where on the screen to put the tile
    tilexy.w = SCRWIDTH;
    tilexy.h = SCRHEIGHT;
    int camx = camera.get_x();
    int camy = camera.get_y(); 
       
    SDL_Rect thistile;//which tile on the sheet to display
    thistile.w = TILEW;
    thistile.h = TILEH;
    int graphdata;
    
    int x, y;
    int w, h;
    int xy;
    w = level.get_w();
    h = level.get_h();
    
    for(y = 0; y < h; y++)
    {
        tilexy.y = y * TILEH - camy;
        while(tilexy.y + TILEH < 0 && y < h){
            tilexy.y+= TILEH;
            y++;
        }
        if(tilexy.y > SCRHEIGHT) break;
        
        for(x = 0; x < w; x++)
        {
            tilexy.x = x * TILEW  - camx;
            while(tilexy.x + TILEW < 0 && x < w) {
                tilexy.x+= TILEW;
                x++;
            }
            if(tilexy.x > SCRWIDTH) break;
            
            xy = level.get_xy(x,y);
            graphdata = level.get_graph(xy);
            
            thistile.y = graphdata * TILEH;
            thistile.x = 0;            
             blit(images.tilesheet, thistile, images.map_buffer,tilexy);      
        }        
    }

    return;
}
void player_display(Map& level, Player& player1, Viewpoint& camera, Surface& images)
{
    SDL_Rect thissprite;
    thissprite.x = 0;
    thissprite.y = 0;
    thissprite.w = CHARW;
    thissprite.h = CHARH;
    SDL_Rect charxy;
    charxy.x = player1.get_x();
    charxy.y = player1.get_y();
    charxy.x -= camera.get_x();
    charxy.y -= camera.get_y();
    
    blit(images.playersheet, thissprite, images.buffer,charxy);
    
    return;
}
     
void text_display(Surface& images, Font& fonts)
{
    SDL_Surface* message;
    SDL_Rect txtplace;            
    SDL_Rect txtpart;
        txtpart.x = 0; txtpart.y = 0;
        txtpart.w = SCRWIDTH - txtplace.x;
        txtpart.h = SCRHEIGHT - txtplace.h;
        
    SDL_Color color = {0,3,0};    
    txtplace.x = SCRWIDTH - 64 * 3;
    txtplace.y = 5;
    
    message = textbox("Hello World", fonts.font1, color);
    blit(message,txtpart,images.buffer,txtplace);
    
    return;
}

void blit(SDL_Surface* tile, SDL_Rect part, SDL_Surface* buffer, SDL_Rect place)
{
    if (tile != NULL)
    {
        SDL_BlitSurface(tile, &part, buffer, &place);
    }
    return;
}

SDL_Surface* textbox(string words, TTF_Font* font, SDL_Color color)
{
    SDL_Surface *message;
     
    message = TTF_RenderText_Blended(font,words.c_str(), color);

    return message;
}

void putpixel(Surface& images, int x, int y, int color)
{
    unsigned int *ptr = (unsigned int*)images.buffer->pixels;
    int lineoffset = y * (images.buffer->pitch / 4);
    ptr[lineoffset + x] = color;
    return;
}

void putrect(Surface& images, SDL_Rect& quad, int color)
{
    int x, y;
    int xlimit, ylimit;
    xlimit = quad.x + quad.w;
    ylimit = quad.y + quad.h;
    for (y = 0; y < ylimit; y++)
    {
        for (x = 0; x < xlimit; x++)
        {
            putpixel(images, x, y, color);
        }
    }
    return;
}

