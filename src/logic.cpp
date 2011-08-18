#include <stdlib.h>
#include <string>
#include <time.h>

#include <SDL.h>
#include "SDL_image.h"
#include "SDL/SDL_ttf.h"

#include "constants.h"
#include "functions.h"
#include "classes.h"

bool logic(Map& level, Player& player1, Viewpoint& camera, Idevice& input)
{
    int mapvalue;
    if (input.get_click() == ON)
	  {
			mapvalue = find_tile(level, input, camera);
		  if (mapvalue == WARP) click_warp(level, player1, input, camera);
		  if (mapvalue == EXIT) click_door(level, player1, input, camera);
		  if (mapvalue == UPSTAIRS || mapvalue == DOWNSTAIRS) click_stairs(level, player1, input, camera);
		  if (mapvalue == CAVE)	click_dungeon(level, player1, input, camera);
		  if (mapvalue == CITY)	click_town(level, player1, input, camera);
    }
    input.put_click(OFF); 
    
    if(player1.check_map() | player1.check_floor()) return 0;
    
    move_player(level, player1, input);
    move_camera(level, player1, camera); 
    
    return 1;
}
void click_dungeon(Map& level, Player& player1, Idevice& input, Viewpoint& camera)
{
    if(!(player_on_or_near(player1, input, camera))) return;
    int tilex = tile_x(input, camera);
    int tiley = tile_y(input, camera);
    int dest;
    dest = level.find_dungeon(tilex, tiley);
    
    player1.put_destmap(dest);
    
}
void click_town(Map& level, Player& player1, Idevice& input, Viewpoint& camera)
{
    if(!(player_on_or_near(player1, input, camera))) return;
    int tilex = tile_x(input, camera);
    int tiley = tile_y(input, camera);
    int dest;
    dest = level.find_town(tilex, tiley);
    
    player1.put_destmap(dest);
    
}
void click_warp(Map& level, Player& player1, Idevice& input, Viewpoint& camera)
{
    if(!(player_on_or_near(player1, input, camera))) return;
    int xmov, ymov, mapmov;
    int tilex = tile_x(input, camera);
    int tiley = tile_y(input, camera);
    int warpid;
    bool warpside;
    warpid = level.warp_id(tilex,tiley);
    warpside = level.warp_side(warpid, tilex, tiley);
    switch(warpside){
        case 0:
            xmov = level.warp_destx(warpid);
            ymov = level.warp_desty(warpid);
            mapmov = level.warp_destmap(warpid);
            break;
        case 1:
            xmov = level.warp_x(warpid);
            ymov = level.warp_y(warpid);
            mapmov = level.warp_sourcemap(warpid);
            break;
    }   
    player1.put_xy(xmov * TILEW, ymov * TILEH);
    player1.put_destmap(mapmov);
    return;
}
void click_stairs(Map& level, Player& player1, Idevice& input, Viewpoint& camera)
{
  
    if(!(player_on_or_near(player1, input, camera))) return;
   
    int mapvalue = find_tile(level, input, camera);    
    int dest;
    switch(mapvalue)
    {
        case UPSTAIRS:
            dest = player1.get_floor() - 1; 
            player1.put_stairs(0);           
            break;
        case DOWNSTAIRS:
            dest = player1.get_floor() + 1;
            player1.put_stairs(1);
            break;
    }
    player1.put_destfloor(dest);
    return;    
}
void click_door(Map& level, Player& player1, Idevice& input, Viewpoint& camera)
{      
   //make sure player is not on top of the door.
    if(player_on_tile(player1, input, camera)) return;
    //make sure the player is beside the door they are trying to open
    if(!(player_near_tile(player1,input, camera))) return;
   
    int tilex = tile_x(input, camera);
    int tiley = tile_y(input, camera);
    int xy = level.get_xy(tilex, tiley);
    int doorvalue, doorid;   
    //find the door's id, and flags
    doorid = level.door_id(tilex, tiley);
    doorvalue = level.door_flag(tilex, tiley);
    
    //toggle its value
    if (doorvalue == 1) {
        level.door_open(doorid); 
        level.toggle_solid(xy); 
        level.set_graph(xy, FLOOR);
        input.put_click(OFF); 
        return;
    }        
    if (doorvalue == 0) {
        level.door_close(doorid); 
        level.toggle_solid(xy);
        level.set_graph(xy, EXIT); 
        input.put_click(OFF); 
        return;
    }
        
    return;
}
bool player_on_or_near(Player& player1, Idevice& input, Viewpoint& camera)
{
    bool check, ontop, parallel;
    SDL_Rect playrect = player1.get_rect();
        playrect.x += PLAYER_MOVE_PIXELS; 
        playrect.y += PLAYER_MOVE_PIXELS; 
        playrect.w -= PLAYER_MOVE_PIXELS; 
        playrect.h -= PLAYER_MOVE_PIXELS; 
    SDL_Rect tilerect = find_map_rect(input, camera);       
        
    ontop = collide_rect_rect(tilerect, playrect);
    parallel = parallel_rect_rect(tilerect, playrect);
    check = ontop | parallel;
    return check;
}
bool player_on_tile(Player& player1, Idevice& input, Viewpoint& camera)
{
    bool ontop = 0;
     //set up the values for checks. 
    SDL_Rect playrect = player1.get_rect();
        playrect.x += PLAYER_MOVE_PIXELS + 1; 
        playrect.y += PLAYER_MOVE_PIXELS + 1; 
        playrect.w -= PLAYER_MOVE_PIXELS + 1; 
        playrect.h -= PLAYER_MOVE_PIXELS + 1; 
    SDL_Rect tilerect = find_map_rect(input, camera);       
        
    ontop = collide_rect_rect(tilerect, playrect);
    return ontop;    
}
bool player_near_tile(Player& player1, Idevice& input, Viewpoint& camera)
{
    bool parallel = 0;
    SDL_Rect playrect = player1.get_rect();
        playrect.x += PLAYER_MOVE_PIXELS; 
        playrect.y += PLAYER_MOVE_PIXELS; 
        playrect.w -= PLAYER_MOVE_PIXELS; 
        playrect.h -= PLAYER_MOVE_PIXELS;
        
    SDL_Rect tilerect = find_map_rect(input, camera);     
        
    parallel = parallel_rect_rect(tilerect, playrect);
    return parallel;   
}
int tile_x(Idevice& input, Viewpoint& camera)
{
    int tilex = (input.get_x() + camera.get_x()) / TILEW;
    return tilex;
}
int tile_y(Idevice& input, Viewpoint& camera)
{
    int tiley = (input.get_y() + camera.get_y()) / TILEH;
    return tiley;
}
int find_tile(Map& level, Idevice& input, Viewpoint& camera)
{     
    int tilex = tile_x(input, camera);
    int tiley = tile_y(input, camera);
    int mapvalue = level.get_tile( level.get_xy(tilex,tiley )); 
    return mapvalue;
}
SDL_Rect find_map_rect(Idevice& input, Viewpoint& camera)
{
    SDL_Rect tilerect;
    int tilex, tiley;
    tilex = (input.get_x() + camera.get_x()) / TILEW;
    tiley = (input.get_y() + camera.get_y()) / TILEH;
    tilerect.x = tilex * TILEW; tilerect.y = tiley * TILEH;
    tilerect.w = TILEW; tilerect.h = TILEH;
    return tilerect;
}
void move_player(Map& level, Player& player1, Idevice& input)
{
    bool mvup, mvdown, mvleft, mvright;
    bool check;
    int charx, chary;
    int bottomlimit;
    int rightlimit;
    
    mvup = input.get_up();
    mvdown = input.get_down();
    mvleft = input.get_left();
    mvright = input.get_right();
    
    check = 0;
    if(mvup) {check = check_player_map(level, player1, UP); if (check) mvup = 0;}
    if(mvdown) {check = check_player_map(level, player1, DOWN); if (check) mvdown = 0;}
    check = 0;
    if(mvleft) {check = check_player_map(level, player1, LEFT); if (check) mvleft = 0;}
    if(mvright) {check = check_player_map(level, player1, RIGHT); if (check) mvright = 0;}
    
    charx = player1.get_x();
    chary = player1.get_y();
   
    bottomlimit = (level.get_h()) * TILEH - CHARH;   
    rightlimit = (level.get_w()) * TILEW - CHARW;
    
    if (mvup && chary > 0) player1.move_y(-PLAYER_MOVE_PIXELS);
    if (mvdown && chary < bottomlimit) player1.move_y(PLAYER_MOVE_PIXELS);
    if (mvleft && charx > 0) player1.move_x(-PLAYER_MOVE_PIXELS);
    if (mvright && charx < rightlimit) player1.move_x(PLAYER_MOVE_PIXELS);             
}
void move_camera(Map& level, Player& player1, Viewpoint& camera)
{
    int camx, camy;
    int llimit, rlimit;
    int tlimit, blimit;
    camx = player1.get_x() - SCRWIDTH / 2;
    camy = player1.get_y() - SCRHEIGHT / 2;    
    
    llimit = SCRWIDTH /2;
    tlimit = SCRHEIGHT / 2;
    llimit = 0;
    tlimit = 0;
    rlimit = level.get_w() * TILEW - SCRWIDTH;
    blimit = level.get_h() * TILEH - SCRHEIGHT;
    if(camx < 0) camx = llimit;
    if(camx > rlimit) camx = rlimit;
    if(camy < 0) camy = tlimit;
    if(camy > blimit) camy = blimit;
    
    camera.put_xy(camx,camy);    
}

bool check_player_map(Map& level, Player& player1, int direction)
{
    bool check = 0;
    SDL_Rect playrect;
    Coor topleft, topright, bottomleft, bottomright;
    Line wall;
    
    //find player dimensions, and assign them to four coor values
    playrect = player1.get_rect();    
   
    topleft.x = playrect.x; topleft.y = playrect.y;
    topright.x = playrect.x + playrect.w; topright.y = playrect.y;
    bottomleft.x = playrect.x; bottomleft.y = playrect.y + playrect.h;
    bottomright.x = playrect.x + playrect.w; bottomright.y = playrect.y + playrect.h;
    
    //adjust by PLAYER_MOVE_PIXELS to allow for movement along a wall while holding a diagonal
    
    switch(direction){
        case UP: 
            topleft.x+=PLAYER_MOVE_PIXELS; 
            bottomleft.x+=PLAYER_MOVE_PIXELS; 
            topright.x-=PLAYER_MOVE_PIXELS; 
            bottomright.x-=PLAYER_MOVE_PIXELS; 
            break;
        case DOWN: 
            topleft.x+=PLAYER_MOVE_PIXELS; 
            bottomleft.x+=PLAYER_MOVE_PIXELS; 
            topright.x-=PLAYER_MOVE_PIXELS; 
            bottomright.x-=PLAYER_MOVE_PIXELS; 
            break;
        case LEFT: 
            topleft.y+=PLAYER_MOVE_PIXELS; 
            bottomleft.y-=PLAYER_MOVE_PIXELS; 
            topright.y+=PLAYER_MOVE_PIXELS; 
            bottomright.y-=PLAYER_MOVE_PIXELS; 
            break;
        case RIGHT: 
            topleft.y+=PLAYER_MOVE_PIXELS; 
            bottomleft.y-=PLAYER_MOVE_PIXELS; 
            topright.y+=PLAYER_MOVE_PIXELS; 
            bottomright.y-=PLAYER_MOVE_PIXELS; 
            break;
    }  
              
    switch(direction){
        case UP: wall.a = topleft; wall.b = topright; break;
        case DOWN: wall.a = bottomleft; wall.b = bottomright; break;
        case LEFT: wall.a = topleft; wall.b = bottomleft; break;
        case RIGHT: wall.a = topright; wall.b = bottomright; break;
    }
    check = collide_map_line(level, wall);
    return check;
}
bool collide_map_rect(Map& level, SDL_Rect& rect){bool check = collide_map_rect(level, rect, -1);return check;}
bool collide_map_rect(Map& level, SDL_Rect& rect, int value)
{
    bool check = 0;
    bool check1, check2, check3, check4;
    Coor coor1, coor2, coor3, coor4;
        coor1.x = rect.x; coor1.y = rect.y;
        coor2.x = rect.x + rect.w; coor2.y = rect.y;
        coor3.x = rect.x; coor3.y = rect.y + rect.h;
        coor4.x = rect.x + rect.w; coor4.y = rect.y + rect.h;
    check1 = collide_map_point(level, coor1, value);
    check2 = collide_map_point(level, coor2, value);
    check3 = collide_map_point(level, coor3, value);
    check4 = collide_map_point(level, coor4, value);
    check = check1 | check2 | check3 | check4;
    return check;
}

bool collide_map_line(Map& level, Line& wall){ bool check = collide_map_line(level, wall, -1); return check;}
bool collide_map_line(Map& level, Line& wall, int value)
{
    bool check = 0; bool check1, check2;
    check1 = collide_map_point(level, wall.a, value);
    check2 = collide_map_point(level, wall.b, value);
    check = check1 | check2;
    return check;
}
bool collide_map_point(Map& level, Coor& point){bool check = collide_map_point(level, point, -1);return check;}
bool collide_map_point(Map& level, Coor& point, int value)
{
    bool check = 0;
    int xy;
    int tilevalue;
    bool solidvalue;
    int xtile, ytile;
    xtile = point.x / TILEW;
    ytile = point.y / TILEH;
    xy = level.get_xy(xtile,ytile);
    tilevalue = level.get_tile(xy);
    solidvalue = level.get_solid(xy);
    bool closed;
    if (value == -1)
    {
        if (tilevalue == EXIT){
            closed = level.door_flag(xtile,ytile);
        }
        if(solidvalue) return 1;
        //if ((tilevalue == EXIT && closed == 1) || (tilevalue == WALL || tilevalue == NONE)) {check = 1; return check;}
    }
    if (value >= 0) if (tilevalue == value) check = 1;    
    return check;    
}


bool collide_rect_rect(SDL_Rect& recta, SDL_Rect& rectb)
{
    bool check, check1, check2, check3, check4;
    check = 0;
    Coor coor1, coor2, coor3, coor4;
    coor1.x = rectb.x; coor1.y = rectb.y;
    coor2.x = rectb.x + rectb.w; coor2.y = rectb.y;
    coor3.x = rectb.x; coor3.y = rectb.y + rectb.h;
    coor4.x = rectb.x + rectb.w; coor4.y = rectb.y + rectb.h;
    
    check1 = collide_rect_point(recta, coor1);
    check2 = collide_rect_point(recta, coor2);
    check3 = collide_rect_point(recta, coor3);
    check4 = collide_rect_point(recta, coor4);
    
    check = check1 | check2 | check3 | check4;
    return check;
}
bool collide_rect_line(SDL_Rect& rectin, Line& wallin)
{
    bool check, check1, check2;
    check = 0;
    check1 = collide_rect_point(rectin, wallin.a);
    check2 = collide_rect_point(rectin, wallin.b);
    check = check1 | check2;
    return check;
}
bool collide_rect_point(SDL_Rect& rectin, Coor& pointin)
{
    bool check = 0;
    if (pointin.x > rectin.x && pointin.x < rectin.x + rectin.w &&
        pointin.y > rectin.y && pointin.y < rectin.y + rectin.h) check = 1;
    return check;
}
bool parallel_rect_rect(SDL_Rect& recta, SDL_Rect& rectb)
{
    bool parallel = 0;
    bool parx, pary;
    parx = parallel_rect_rect(recta, rectb, HORIZONTAL);
    pary = parallel_rect_rect(recta, rectb, VERTICAL);
    parallel = parx | pary;
    return parallel;
}
bool parallel_rect_rect(SDL_Rect& recta, SDL_Rect& rectb, bool orientation)
{
    bool check, check1, check2, check3, check4;
    check = 0;
    Coor coor1, coor2, coor3, coor4;
    coor1.x = rectb.x; coor1.y = rectb.y;
    coor2.x = rectb.x + rectb.w; coor2.y = rectb.y;
    coor3.x = rectb.x; coor3.y = rectb.y + rectb.h;
    coor4.x = rectb.x + rectb.w; coor4.y = rectb.y + rectb.h;
    
    check1 = parallel_rect_point(recta, coor1, orientation);
    check2 = parallel_rect_point(recta, coor2, orientation);
    check3 = parallel_rect_point(recta, coor3, orientation);
    check4 = parallel_rect_point(recta, coor4, orientation);
    
    check = check1 | check2 | check3 | check4;    
    return check;
}
bool parallel_rect_point(SDL_Rect& rectin, Coor pointin, bool orientation)
{
    bool check = 0;
    switch(orientation){
        case VERTICAL:
            if (pointin.x > rectin.x && pointin.x < rectin.x + rectin.w &&
                pointin.y > rectin.y - TILEH && pointin.y < rectin.y + rectin.h + TILEH) return 1;
            break;
        case HORIZONTAL:
            if (pointin.y > rectin.y && pointin.y < rectin.y + rectin.h &&
                pointin.x > rectin.x - TILEW && pointin.x < rectin.x + rectin.w + TILEW) return 1;
            break;
    }
    return check;
}
