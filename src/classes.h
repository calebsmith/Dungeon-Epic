#ifndef CLASSES_H
#define CLASSES_H
#include <math.h>
#include <time.h>
#include <iostream>
#include <new>
#include <exception>

#include <SDL.h> 
#include <SDL_image.h>
#include "SDL/SDL_ttf.h"
//#include "SDL_timer.h"

#include "constants.h"

//SDL helper classes/structs
struct Keyboard{
    bool up,down,left,right;
};
struct Pointdevice{
    bool click, rclick;
    int x, y;
};
struct Surface{
    SDL_Surface* buffer;
    SDL_Surface* background;
    SDL_Surface* map_buffer;
    SDL_Surface* tilesheet;
    SDL_Surface* playersheet;
    SDL_Surface* spritesheet;
};
struct Font{
    TTF_Font* font1;
};
struct Coor{
    int x, y;
};
struct Line{
    Coor a, b;
};
class Idevice{
    private:
        Keyboard arrow;
        Pointdevice mouse;    
    public:
        Idevice(){            
            mouse.x = 0; mouse.y = 0;
            mouse.click = 0; mouse.rclick = 0;
            arrow.up = 0; arrow.down = 0; arrow.left = 0; arrow.right = 0;
        }
        void put_x(int xin){mouse.x = xin;}
        void put_y(int yin){mouse.y = yin;}
        void put_xy(int xin, int yin){put_x(xin); put_y(yin);}
        void put_coor(Coor xy){put_x(xy.x); put_y(xy.y);}
        void put_click(bool value){mouse.click = value;} 
        void put_rclick(bool value){mouse.rclick = value;}
        
        void arrow_off(){arrow.up = 0; arrow.down = 0; arrow.left = 0; arrow.right = 0;}
        void up(){
            if (arrow.up == 0) {arrow.up = 1; return;}
            if (arrow.up == 1) {arrow.up = 0; return;}
        }
        void down(){
            if (arrow.down == 0) {arrow.down = 1; return;}
            if (arrow.down == 1) {arrow.down = 0; return;}
        }
        void left(){
            if (arrow.left == 0) {arrow.left = 1; return;}
            if (arrow.left == 1) {arrow.left = 0; return;}
        }
        void right(){
            if (arrow.right == 0) {arrow.right = 1; return;}
            if (arrow.right == 1) {arrow.right = 0; return;}
        }
        
        int get_x(){int a = mouse.x; return a;}
        int get_y(){int a = mouse.y; return a;}
        Coor get_xy(){Coor a; a.x = mouse.x; a.y = mouse.y; return a;} 
        bool get_click(){bool a = mouse.click; return a;}
        bool get_rclick(){bool a = mouse.rclick; return a;}
        
        bool get_up(){bool a = arrow.up; return a;}
        bool get_down(){bool a = arrow.down; return a;}
        bool get_left(){bool a = arrow.left; return a;}
        bool get_right(){bool a = arrow.right; return a;}
        void show(){
            std::cout << "Keyboard data: " << '\n';
            std::cout << "    Up:    " << arrow.up << '\n';
            std::cout << "    Down:  " << arrow.down << '\n';
            std::cout << "    Left:  " << arrow.left << '\n';
            std::cout << "    Right: " << arrow.right << '\n';
            std::cout << '\n';
            std::cout << "Mouse data: " << '\n';
            std::cout << "    (" << mouse.x << "," << mouse.y << ")" << '\n';
            std::cout << "    Left button: " << mouse.click << '\n';
            std::cout << "    Right button: " << mouse.rclick << '\n';
        }
};


class Ticks
{
    private:
        int start, now, diff;
        int max;
        bool run, goflag;
    public:    
        Ticks(){
            start = SDL_GetTicks();
            now = 0;
            diff = 0;
            run = 1;
            goflag = 0;
        };
        void reset(){start = SDL_GetTicks(); now = start;}
        void restart(){reset(); run = 1;}
        void pause(){run = 0;}
        void play(){run = 1;}
        void wait(float time){SDL_Delay(time);}
        void wait_frame(){
            get_diff(); 
            if (diff < MAXFPS_MS){
                wait(MAXFPS_MS - diff);
            }
            reset();
        }
        
        void set_now(){if(run == 1)now = SDL_GetTicks();}
        
        int get_start(){int out; out = start; return out;}
        int get_now(){int out; set_now(); out = now; return now;}
        int get_diff(){int out; diff = get_now() - start; out = diff; return out;}             
};

class Viewpoint{
    private:
       int x, y;
       int w, h;
    public:
        Viewpoint(){
            x = 0; y = 0;
            w = SCRWIDTH;
            h = SCRHEIGHT;
        }
        void put_x(int xin){x = xin;}
        void put_y(int yin){y = yin;}
        void put_xy(int xin, int yin){put_x(xin); put_y(yin);}
        void put_coor(int xin, int yin){put_xy(xin,yin);}
        void put_coor(Coor xyin){x = xyin.x; y = xyin.y;}
    
        void move_x(int xin){x += xin;}
        void move_y(int yin){y += yin;}
        void move_coor(int xin, int yin){x += xin; y += yin;}
        void move_coor(Coor xyin){x += xyin.x; y += xyin.y;}
        
        int get_x(){int a = x; return a;}
        int get_y(){int a = y; return a;}
        Coor get_coor(){Coor a; a.x = x; a.y = y; return a;}
};

//base class for all location classes (entities, maps, rooms, paths, exits, entrances, leaving, and warps)
class Location{
    public:
        int id;
        int x,y,z;
        int type;
        int flags;
        bool active;
        void base_init(){
            id = -1;
            x = -1; y = -1; z = -1;
            type = -1;
            flags = 0;
            active = 0;
        }
        virtual void remove(){base_init();}
        void put_coor(int xin, int yin){x = xin; y = yin;}
        void put_id(int idin){id = idin;}
        void put_x(int xin){x = xin;}
        void put_y(int yin){y = yin;}
        void put_z(int zin){z = zin;}
        virtual void deactivate(){active = 0;}
        void activate(){active = 1;}
        void put_type(int typein){type = typein;}
        void put_flags(int flagsset){flags = flagsset;}
        void add_flag(int flagsin){flags+=flagsin;}
        void sub_flag(int flagsin){flags-=flagsin;}
        
        int get_id(){int a; a = id; return a;}
        int get_x(){int a; a = x; return a;}
        int get_y(){int a; a = y; return a;}
        int get_z(){int a; a = z; return a;}
        int get_type(){int a; a = type; return a;}
        int get_flags(){int a; a = flags; return a;}
        virtual bool get_active(){bool a; a = active; return a;}
        
        int random_int(int low, int high){int a = rand() % high + low; return a;}
        void shuffle(int* a, int size)
        {
            int i, i2;
            int b;
            for (i = 0; i < size; i++)
            {
                i2 = random_int(0, size);
                if (i != i2)
                {
                    b = a[i];
                    a[i] = a[i2];
                    a[i2] = b;
                }
            }
        }
        int lesser_int(int a, int b) {if (a < b) return a; return b;}
        int greater_int(int a, int b) {if (a > b) return a; return b;}
};  
class Warp: public virtual Location{
    private:
        int roomid;
        int sourcemap;
        int destmap;
        int destx, desty;
    public:
    Warp(){
        base_init();
        roomid = -1;
        destmap = -1;
        destx = -1; desty = -1;
    }
        void put_roomid(int ident){roomid = ident; return;}
        void put_sourcemap(int sourcein){sourcemap = sourcein; return;}
        void put_destmap(int destin){destmap = destin; return;}
        void put_dest(int xin, int yin){put_destx(xin); put_desty(yin);}
        void put_destx(int xin){destx = xin;}
        void put_desty(int yin){desty = yin;}
        int get_roomid(){int a; a = roomid; return a;}  
        int get_sourcemap(){int a = sourcemap; return a;} 
        int get_destmap(){int a = destmap; return a;}
        Coor get_dest(){Coor a; a.x = destx; a.y = desty; return a;}
        int get_destx(){int a = destx; return a;}
        int get_desty(){int a = desty; return a;}  
        void show(){
            std::cout << "Warp " << id << " goes from (" << x << "," << y << ") to (" << destx << "," << desty << ")" << '\n';
        }
};

//for entering/leaving a map
class Stair: public virtual Location{
    private:
        int roomid;
        int sourcemap;
        int destmap;
    public:
        Stair(){
            base_init();
            roomid = -1;
            sourcemap = -1;
            destmap = -1;
        }       
        void make(int xin, int yin, int roomin, int destin){put_coor(xin,yin); put_roomid(roomin);put_destmap(destin);}
        void put_roomid(int ident){roomid = ident; return;}

        void put_sourcemap(int ident){sourcemap = ident; return;}
        void put_destmap(int ident){destmap = ident; return;}
        int get_roomid(){int a; a = roomid; return a;}
        int get_sourcemap(){int a; a =sourcemap; return a;}
        int get_destmap(){int a = destmap; return a;}
};
//exits between pathways and rooms (i.e. doorways)
class Exit: public virtual Location{
    private:
    int pathid;
    int roomid;
    public:
    Exit(){base_init(); close();}
    
    
    Exit& operator= (const Exit& a){
        if (this == &a) return *this;
        id = a.id;        
        x = a.x; y = a.y; z = a.z;        
        type = a.type;
        flags = a.flags;
        active = a.active;
        pathid = a.pathid;
        roomid = a.roomid;
        return *this;
    }
    
    void make(int ident, int typeset, int flagsset){
        id = ident;
        type = typeset;
        flags = flagsset;
        active = 1;
    }
    void put_pathid(int ident){pathid = ident;}
    void put_roomid(int ident){roomid = ident;}
    
    int get_pathid(){int a; a = pathid; return a;}    
    int get_roomid(){int a; a = roomid; return a;}
    void open(){sub_flag(1);}
    void close(){add_flag(1);} 
    void show(){
       
        std::cout << "Exit " << id << " goes from room " << roomid << " to path " << pathid << " with coordinates (" 
                  << x << "," << y << ")" << '\n';
        return;
    }
};
//pathways between rooms/town buildings
class Path: public virtual Location{
    private:            
        int sourceid;
        int destid;
        int direction;
        int x2,y2;
        int length;        
    public:
    Path(){base_init(); init();}
    
    void init(){       
        sourceid = -1;
        destid = -1;
        direction = -1;       
        x2 = -1; y2 = -1;
        length = 0;        
    }

    
    Path& operator= (const Path& a){
        if (this == &a) return *this;
        id = a.id;        
        x = a.x; y = a.y; z = a.z;        
        type = a.type;
        flags = a.flags;
        active = a.active;

        sourceid = a.sourceid;
        destid = a.destid;
        direction = a.direction;       
        x2 = a.x2;
        y2 = a.y2;
        length = a.length;        
        return *this;
    }
    void make(int ident, int source, int dest, int dir, int newtype){
        id = ident;
        sourceid = source;
        destid = dest;
        direction = dir;
        type = newtype;
        active = 1;
        return;        
    }
    void put_coor(int lane, int start, int end, bool orient){
        switch(orient)
        {
            case VERTICAL:
                x = lane; x2 = x;
                y = start; y2 = end;
                break;
            case HORIZONTAL:
                x = start; x2 = end;
                y = lane; y2 = y;
                break;
        }
        return;
    }
    void show(){
       // if (active == 0) return;
        std::cout << "Path " << id << " goes " << direction << " from room " << sourceid << " to room " << destid << "." << '\n';
        std::cout << "Path coordinates: (" << x << "," << y << ")-(" << x2 << "," << y2 << ")" << '\n';
         if (active == 0) std::cout << "...Path deactivated..." << '\n';
        return;
    }
    
    void put_dest(int ident){destid = ident;}
    void put_src(int ident){sourceid = ident;}      
    int get_x2(){int a; a = x2; return a;}
    int get_y2(){int a; a = y2; return a;}
        
    int get_sourceid(){int a; a = sourceid; return a;}
    int get_destid(){int a; a = destid; return a;}
    int get_direction(){int a; a = direction; return a;}
   
    int get_length(){int a; a = length; return a;}
    
};
//rooms
class Room: public virtual Location{
    private:
        int w,h;
        int area;
        int exitsum;
    public:
    Room(){
        init();
    }
    void init(){
        base_init();
        w = 0;
        h = 0;
        area = 0;
        exitsum = 0;         
    }
    ~Room(){remove();}
    void remove(){        
        deactivate();
        return;
    }
    Room& operator= (const Room& a){
        if (this == &a) return *this;
        id = a.id;        
        x = a.x; y = a.y; z = a.z;        
        type = a.type;
        flags = a.flags;
        active = a.active;
        w = a.w;
        h = a.h;
        area = a.area;
        exitsum = a.exitsum;        
        return *this;
    }
	void swap(Room a, Room b){Room c; c = a; a = b; b = c;}    
	
	void make(int ident){make(ident, MAPW,MAPH);}
	void make(int ident, int width, int height){
	    make(ident, width,height,ROOMW,ROOMH);
	}
	void make(int ident, int width, int height, int wmax, int hmax){
	    id = ident;
	    int xmax = width - wmax - 1;
	    int ymax = height - hmax - 1;
	    x = random_int(0,xmax);
	    y = random_int(0,ymax);
	    w = random_int(3,wmax);
	    h = random_int(3,hmax);
	    area = w * h;
	    active = 1;
	}	
	
	void add_exitsum(){exitsum++;} 
		
    int get_w(){int a = w; return a;}
    int get_h(){int a = h; return a;}
    int get_area(){int a = area; return a;}   
    int get_exitsum(){int a = exitsum; return a;}
    
    void show() {
        std::cout << "Room " << id << " has coordinates " << x << "," << y << " and w,h is " << w << "," << h << " with active status: " << active <<'\n';
    }  
};

#include "map.h"
#include "mob.h"
#endif
