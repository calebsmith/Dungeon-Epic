#include <stdio.h>
#include <string>
#include <time.h>

#include <SDL.h>
#include "SDL_image.h"
#include "SDL/SDL_ttf.h"

#include "constants.h"
#include "classes.h"
#include "functions.h"

//initialize SDL and game window
void init(Surface& images, Font& fonts)
{
    //initialize SDL, the window, and fonting
    SDL_Init(SDL_INIT_EVERYTHING);   
    SDL_WM_SetCaption("Dungeon Epic", NULL);  
	TTF_Init();
	//initialize the video buffer in the window   
       
    images.buffer = SDL_SetVideoMode (SCRWIDTH,SCRHEIGHT, SCRBPP,SDL_HWSURFACE); 
     
    //load graphics
    
    //images.spritesheet = load_bitmaps("data/images/.jpg"); 
    
    images.background = load_bitmaps("data/images/gothic.jpg"); 
    images.playersheet = load_bitmaps("data/images/player.png");
    
    //load fonts
    fonts.font1 = load_font("data/fonts/NotCourierSans.otf", 18);
    
    return;
}

void loadmap(Zone& area, Map& level, Player& player1, Surface& images)
{ 
    int mapnum, floornum;
    bool stairs;
    int playx,playy;
    //load world and set initial values the first time through
    if (player1.get_map() == -1) {
        
        srand(time(NULL));
        mapnum = area.get_id();          
        floornum = 0;  
        area.init();
        cout << "area loaded." << '\n';
        level = area.zones[mapnum].floors[floornum]; 
        playx = level.get_begin_x() * TILEW;
        playy = level.get_begin_y() * TILEH;      
        player1.put_stairs(1);          
    }    
    //load a new map if the player has changed maps midplay
    if (player1.get_map() >= 0) {
        mapnum = player1.get_destmap();
        floornum = player1.get_destfloor();
        if (floornum < 0) {
            floornum = 0;
            player1.put_stairs(1);   
            mapnum = area.get_id(); 
            playx = area.zones[player1.get_map()].get_x() * TILEW;
            playy = (area.zones[player1.get_map()].get_y() + 1) * TILEH;
        }        
        area.zones[player1.get_map()].floors[player1.get_floor()] = level; 
    }
    
    cout << "Map: " << mapnum << " Floor: " << floornum << '\n';
    level = area.zones[mapnum].floors[floornum]; 
    level.set_rendered(0); //area is not yet rendered for graphics
    if (area.zones[mapnum].floors[floornum].get_type() == TOWN){
        playx = MAPW / 2 * TILEW;
        playy = (MAPH - 2) * TILEH;
    }
    if (area.zones[mapnum].floors[floornum].get_type() == DUNGEON){    
        stairs = player1.get_stairs();
        if (stairs == 0) {
            playx = level.get_end_x() * TILEW;
            playy = level.get_end_y() * TILEH;
        }
        if (stairs == 1) {
            playx = level.get_begin_x() * TILEW;
            playy = level.get_begin_y() * TILEH;
        }    
    }
    player1.put_coor(playx, playy);    
    player1.put_map(mapnum);
    player1.put_floor(floornum);
    
    
    //load appropriate spritesheet for this kind of level
    switch(level.get_type())
    {
        case DUNGEON:
            images.tilesheet = load_bitmaps("data/images/dungeon.jpg");
            break;
        case TOWN:
            images.tilesheet = load_bitmaps("data/images/town.jpg");
            break;
        case FOREST:
            images.tilesheet = load_bitmaps("data/images/forest.jpg");
            break;
    }    
};

TTF_Font *load_font(std::string filename, int fontsize)
{
    TTF_Font* fontget;
    fontget = TTF_OpenFont(filename.c_str(), fontsize);
    return fontget;
}

SDL_Surface *load_bitmaps(std::string filename)
{
    SDL_Surface* image = NULL;
    SDL_Surface* unopt0 = NULL;
    unopt0 = IMG_Load(filename.c_str());
    image = SDL_DisplayFormatAlpha(unopt0);
    SDL_FreeSurface(unopt0);
    if (image == NULL) cout << "File not found while trying to load " << filename << '\n';
    return image;

}

void quit(Surface& images, Font& fonts)
{

    SDL_FreeSurface (images.buffer);
    SDL_FreeSurface (images.background);
    SDL_FreeSurface (images.spritesheet);
    
	TTF_CloseFont(fonts.font1);
	
	TTF_Quit();
	SDL_Quit();
	return;
 }
 
