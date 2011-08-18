#ifndef MOB_H
#define MOB_H
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
class Mob{
    public:
        int id;        
        int type;
        int flags;
        bool active;
        int x,y,z;
        int w,h;        
        Skills skill;
        Attributes attribute;
        int gold;
        int exp;
        
        Mob(){init();}
        ~Mob(){remove();}
        void init(){
            id = -1;
            type = -1;
            flags = 0;
            active = 0;
            x = -1; y = -1; z = -1;
            w = CHARW; h = CHARH;
            
        }
        void remove(){
            deactivate();
        }
        void activate(){active = 1;}
        void deactivate(){active = 0;}
        
        void put_id(int idin){id = idin;}
        void put_flags(int flagsin){flags = flagsin;}
        void add_flags(int value){flags += value;}
        void sub_flags(int value){flags -= value; if (flags < 0) flags = 0;}
        void put_type(int typein){type = typein;}
        void put_x(int xin){x = xin;}
        void put_y(int yin){y = yin;}
        void put_z(int zin){z = zin;}
        void put_w(int win){w = win;}
        void put_h(int hin){h = hin;}
        void put_xy(int xin, int yin){put_x(xin); put_y(yin);}
        void put_coor(Coor xy){put_x(xy.x); put_y(xy.y);}
        void put_coor(int xin, int yin){put_xy(xin, yin);}
        void put_gold(int value){gold = value;}
        void add_gold(int value){gold += value;}
        void sub_gold(int value){gold -= value; if (gold < 0) gold = 0;}
        void put_exp(int value){exp = value;}
        void add_exp(int value){exp += value;}
        void sub_exp(int value){exp -= value; if (exp < 0) exp = 0;}
        
        int get_id(){int a = id; return a;}
        int get_flags(){int a = flags; return a;}
        int get_type(){int a = type; return a;}
        int get_x(){int a = x; return a;}
        int get_y(){int a = y; return a;}
        int get_z(){int a = z; return a;}
        int get_w(){int a = w; return a;}
        int get_h(){int a = h; return a;}
        Coor get_coor(){Coor a; a.x = x; a.y = y; return a;}
        int get_gold(){int a = gold; return a;}
        int get_exp(){int a = exp; return a;}
        bool get_active(){bool a = active; return a;}
};

class Player: public virtual Mob{
    private:
    public:
        
};
#endif
