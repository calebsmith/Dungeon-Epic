#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <string>

#include <SDL.h>
#include <SDL_image.h>
#include "SDL/SDL_ttf.h"

#include "classes.h"

    
void init(Map& level, Surface& images, Font& fonts);
    SDL_Surface *load_bitmaps(std::string filename);
    TTF_Font *load_font(std::string filename, int fontsize);

bool get_input(Map& level, Viewpoint& camera, SDL_Event event, Idevice& input);
    void keyin(SDL_Event event, Idevice& input);
    
bool logic(Map& level, Viewpoint& camera, Idevice& input);
    void move_camera(Map& level, Viewpoint& camera, Idevice& input);

void render(Map& level, Viewpoint& camera, Surface& images, Font& fonts);
    void backdrop(Surface& images);
    void map_display(Map& level, Viewpoint& camera, Surface& images);

        void blit(SDL_Surface* tile, int n, SDL_Surface* buffer, SDL_Rect place);
        void blit(SDL_Surface* tile, int n, SDL_Surface* buffer, int n2);
        void blit(SDL_Surface* tile, SDL_Rect part, SDL_Surface* buffer, SDL_Rect place);

        void text_display(Surface& images, Font& fonts);
        SDL_Surface* textbox(string words, TTF_Font* font, SDL_Color color);

        void putrect(Surface& images, SDL_Rect& quad, int color);
        void putpixel(Surface& images, int x, int y, int color);

void quit(Surface& images, Font& fonts);

#endif
