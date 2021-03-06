#ifndef MOB_H
#define MOB_H
#include "classes.h"
#include <math.h>
struct Skills{
    int melee;
    int missle;
    int magic;
};
struct Attributes{
    int str;
    int con;
    int intelligence;
};
class Mob: public virtual Location{
    public:
        int w,h;   
        int sourcemap;   
        int destmap;  
        int sourcefloor;
        int destfloor;
        bool stairs; // did the player go up or downstairs?
        Skills skill;
        Attributes attribute;
        int gold;
        int health, mana;
        int maxhealth,maxmana;
        int exp;
        
        Mob(){init();}
        ~Mob(){remove();}
        void init(){
            id = -1;
            type = -1;
            flags = 0;
            active = 0;
            maxhealth = 100;
            health = maxhealth;
            maxmana = 100;
            mana = maxmana;
            x = -1; y = -1; z = -1;
            w = CHARW; h = CHARH;
            sourcefloor = 0;
            destfloor = 0;
            sourcemap = -1;
            destmap = -1;            
        }
        void remove(){
            deactivate();
        }
        void add_flags(int value){flags += value;}
        void sub_flags(int value){flags -= value; if (flags < 0) flags = 0;}
        void put_type(int typein){type = typein;}
        
        void put_w(int win){w = win;}
        void put_h(int hin){h = hin;} 
        void put_map(int value){sourcemap = value; destmap = value;}
        void put_floor(int value){sourcefloor = value; destfloor = value;}
        void put_destfloor(int value){destfloor = value;}
        void put_destmap(int value){destmap = value;}
        void put_stairs(bool value){stairs = value;}
        bool check_map(){if (sourcemap != destmap) return 1; return 0;}
        bool check_floor(){if (sourcefloor != destfloor) return 1; return 0;}
        
        void move_x(int xin){x += xin;}
        void move_y(int yin){y += yin;}
        
        void put_gold(int value){gold = value;}
        void add_gold(int value){gold += value;}
        void sub_gold(int value){gold -= value; if (gold < 0) gold = 0;}
        void put_exp(int value){exp = value;}
        void add_exp(int value){exp += value;}
        void sub_exp(int value){exp -= value; if (exp < 0) exp = 0;}
        void put_health(int value){health = value; if (health <0) health = 0;}
        void add_health(int value){health+=value; if (health > maxhealth) health = maxhealth;}
        void sub_health(int value){health-= value; if(health < 0) health = 0;}
        void put_mana(int value){mana = value; if (mana < 0) mana = 0;}
        void add_mana(int value){mana += value; if (mana > maxmana) mana = maxmana;}
        void sub_mana(int value){mana -= value; if (mana < 0) mana = 0;}

        int get_w(){int a = w; return a;}
        int get_h(){int a = h; return a;}
        SDL_Rect get_rect(){SDL_Rect a; a.x = x; a.y = y; a.w = CHARW; a.h = CHARH; return a;}
        int get_floor(){int a = sourcefloor; return a;}
        int get_destfloor(){int a = destfloor; return a;}        
        int get_map(){int a = sourcemap; return a;}
        int get_destmap(){int a = destmap; return a;}
        bool get_stairs(){bool a = stairs; return a;}
        
        int get_gold(){int a = gold; return a;}
        int get_exp(){int a = exp; return a;}
};

class Player: public virtual Mob{
    private:
    public:
        
};

class Monster: public virtual Mob{
    private:
    public:
};
#endif
