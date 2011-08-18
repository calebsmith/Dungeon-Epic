#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <string>

#include <SDL.h>
#include <SDL_image.h>
#include "SDL/SDL_ttf.h"

#include "classes.h"

void init(Surface& images, Font& fonts);
    SDL_Surface *load_bitmaps(std::string filename);
    TTF_Font *load_font(std::string filename, int fontsize);

void loadmap(Zone& area, Map& level, Player& player1, Surface& images);

bool get_input(SDL_Event event, Idevice& input);
    void keyin(SDL_Event event, Idevice& input);
    void mousein(SDL_Event event, Idevice& input);
    
bool logic(Map& level, Player& player1, Viewpoint& camera, Idevice& input);
    void click_dungeon(Map& level, Player& player1, Idevice& input, Viewpoint& camera);
    void click_town(Map& level, Player& player1, Idevice& input, Viewpoint& camera);

    void click_door(Map& level, Player& player1, Idevice& input, Viewpoint& camera);
    void click_stairs(Map& level, Player& player1, Idevice& input, Viewpoint& camera);
    void click_warp(Map& level, Player& player1, Idevice& input, Viewpoint& camera);

    
        int find_tile(Map& level, Idevice& input, Viewpoint& camera);
        int tile_x(Idevice& input, Viewpoint& camera);
        int tile_y(Idevice& input, Viewpoint& camera);
        
        bool player_on_or_near(Player& player1, Idevice& input, Viewpoint& camera);
        bool player_on_tile(Player& player1, Idevice& input, Viewpoint& camera);
        bool player_near_tile(Player& player1, Idevice& input, Viewpoint& camera);
        
        SDL_Rect find_map_rect(Idevice& input, Viewpoint& camera);
    void move_player(Map& level, Player& player1, Idevice& input);    
        bool check_player_map(Map& level, Player& player1, int direction);
            bool collide_map_rect(Map& level, SDL_Rect& rectin, int value);
            bool collide_map_rect(Map& level, SDL_Rect& rectin);
            bool collide_map_line(Map& level, Line& wall, int value);
            bool collide_map_line(Map& level, Line& wall);    
            bool collide_map_point(Map& level, Coor& point, int value);
            bool collide_map_point(Map& level, Coor& point);        
        
        bool collide_rect_rect(SDL_Rect& a, SDL_Rect& b);
        bool collide_rect_line(SDL_Rect& rectin, Line& wallin);
        bool collide_rect_point(SDL_Rect& rectin, Coor& pointin);       
        
        bool parallel_rect_rect(SDL_Rect& recta, SDL_Rect& rectb);
        bool parallel_rect_rect(SDL_Rect& a, SDL_Rect& b, bool orientation);
        bool parallel_rect_point(SDL_Rect& rectin, Coor pointin, bool orientation);
        
    void move_camera(Map& level, Player& player1, Viewpoint& camera);

void render(Map& level, Player& player1, Viewpoint& camera, Surface& images, Font& fonts, Ticks& frametimer);
    void backdrop(Surface& images);
   // void map_render(Map& level, Viewpoint& camera, Surface& images);
    void map_display(Map& level, Viewpoint& camera, Surface& images);
    void player_display(Map& level, Player& player1, Viewpoint& camera, Surface& images);

        void blit(SDL_Surface* tile, int n, SDL_Surface* buffer, SDL_Rect place);
        void blit(SDL_Surface* tile, int n, SDL_Surface* buffer, int n2);
        void blit(SDL_Surface* tile, SDL_Rect part, SDL_Surface* buffer, SDL_Rect place);

        void text_display(Surface& images, Font& fonts);
        SDL_Surface* textbox(string words, TTF_Font* font, SDL_Color color);

        void putrect(Surface& images, SDL_Rect& quad, int color);
        void putpixel(Surface& images, int x, int y, int color);

void quit(Surface& images, Font& fonts);

#endif
