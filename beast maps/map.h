#ifndef MAP_H
#define MAP_H
#include <math.h>

using namespace std;

class Map: private virtual Location{
    private:      
        int w,h;        
        int area;
        int floor;
        int roomsum;
        int pathsum;
        int exitsum;
        int* tile;
        int* exitlist;
        Room* box;
        Path* line;
        Exit* door;
        Stair begin;
        Stair end;
    public:
    Map(){}
    void init();   
    void init(int width, int height, int min, int max);
    
    ~Map(){remove();}
    void remove(){
        delete [] box;
        delete [] line;
        delete [] door;
        delete [] tile;
        delete [] exitlist;
    }
    Map& operator= (const Map& a){
        if (this == &a) return *this;
        id = a.id;        
        x = a.x; y = a.y; z = a.z;        
        type = a.type;
        flags = a.flags;
        active = a.active;
        floor = a.floor;
        w = a.w;
        h = a.h;
        roomsum= a.roomsum;
        pathsum = a.pathsum;
        exitsum = a.exitsum;
        area = a.area;
        delete [] box;
        delete [] line;
        delete [] door;
        delete [] tile;
        delete [] exitlist;
        int i;
        box = new Room[roomsum+ 1];
        line = new Path[pathsum];
        door = new Exit[exitsum];
        tile = new int[area + 1];
        exitlist = new int[1];
        for (i = 0; i < roomsum; i++) box[i] = a.box[i];
        for (i = 0; i < pathsum; i++) line[i] = a.line[i];
        for(i = 0; i < exitsum; i++) door[i] = a.door[i];
        for (i = 0; i < area; i++) tile[i] = a.tile[i];
        return *this;
    }
    void swaproom(Room& a, Room& b){Room c;c = a; a = b; b = c;}
    void swappath(Path& a, Path& b){Path c;c = a; a = b; b = c;}
    void swapexit(Exit& a, Exit& b){Exit c;c = a; a = b; b = c;}
    
    void swaproom_id(Room& a, Room& b){int tempid = a.get_id(); a.put_id(b.get_id()); b.put_id(tempid);}
    void swappath_id(Path& a, Path& b){int tempid = a.get_id(); a.put_id(b.get_id()); b.put_id(tempid);}
    
    void swap_room_paths(Room& a, Room& b){
        int i;
        bool mark;
        for(i = 0; i < pathsum; i++)
        {
            mark = 0;
            if(line[i].get_sourceid() == a.get_id()) {
                line[i].put_src(b.get_id()); mark = 1;
            }
            if(line[i].get_sourceid() == b.get_id() && mark == 0) line[i].put_src(a.get_id());          
            mark = 0;
            if(line[i].get_destid() == a.get_id()) {
                line[i].put_dest(b.get_id()); mark  = 1;
            }
            if(line[i].get_destid() == b.get_id() && mark == 0) line[i].put_dest(a.get_id());           
        }
        for (i = 0; i < exitsum; i++)
        {
            mark = 0;
            if(door[i].get_roomid() == a.get_id()){
                door[i].put_roomid(b.get_id()); mark = 1;
            }
            if(door[i].get_roomid() == b.get_id() && mark == 0) door[i].put_roomid(a.get_id());           
        }
    }
    void swap_path_exits(Path& a, Path& b){
        int i;
        bool mark;
        for (i = 0; i < exitsum; i++)
        {
            mark = 0;
            if(door[i].get_pathid() == a.get_id()){
                door[i].put_pathid(b.get_id()); mark = 1;
            }
            if(door[i].get_pathid() == b.get_id() && mark == 0) door[i].put_pathid(a.get_id());         
        }
    }
    void makerooms();    
    void makepaths();    
    void makeexits();
    void makeentrance();
    void makeleave();
    
    void check_box();       
    bool overlap(Room& a, Room& b);   
    //void checkroom();
    
    void addpath();   
    void findroute();
    void find_bound(int dir, int src, int dest, int* startx, int* startx2, int* starty, int* starty2, int* endx, int* endx2, int* endy, int* endy2);
    bool test_bound(int pathid, int pathx, int pathy, int pathx2, int pathy2, bool orient);
    int path_over(Room& a, Room& b);
    int test_lane(int* list, int size, int start, int end, bool orient, int findval);
   
    void addexit();
    void findexit(int pathid, Coor& source, Coor& dest);
    void makeexit(int idin, Coor place, int toroom, int topath);
    
    void search_paths();
        void follow_path(int ident, int* net, int netsum);
    
    void prune_all(){prune_room(1); prune_path(1); prune_exit();}   
    void prune_room(bool swapmore);
    void prune_path(bool swapmore);
    void prune_exit();
    
        
    int get_begin_x(){int a = begin.get_x(); return a;}
    int get_begin_y(){int a = begin.get_y(); return a;}
    int get_begin_roomid(){int a = begin.get_roomid(); return a;}
    int get_w(){int a = w; return a;}
    int get_h(){int a = h; return a;}
    int get_floor(){int a = floor; return a;}
    int get_roomsum(){int a = roomsum; return a;}
    void get_room(int ident){box[ident].show(); return;}
    int get_pathsum(){int a = pathsum; return a;}
    void get_path(int ident){line[ident].show(); return;}
    int get_exitsum(){int a = exitsum; return a;}
    int get_area(){int a = area; return a;}
    int get_tile(int xy) {int a = tile[xy]; return a;}
    int get_xy(int x1, int y1){
        int a = x1 + y1 * w;        
        if (a < 0) a = 0; if (a > area) a = area;
        return a;
    }
    void put(int x1, int y1, int value){
        int xy = get_xy(x1, y1);
        if (xy < area && xy >= 0) tile[xy] = value;       
        return;
    }
    
    void put_all(){put_rooms(); put_paths(); put_exits(); put_entrance(); put_leave();}
    void put_rooms() {int i; for (i = 0; i < area; i++) tile[i] = 0; for(i = 0; i < roomsum; i++) put_room(i);}
    void put_paths() {int i; for(i = 0; i < pathsum; i++) put_path(i);}
    void put_exits() {int i; for(i = 0; i < exitsum; i++) put_exit(i);}
    void put_entrance() {put(begin.get_x(),begin.get_y(), UPSTAIRS);}
    void put_leave() {put(end.get_x(),end.get_y(), DOWNSTAIRS);}
    
    void put_room(int iter);
    void put_path(int iter);  
    void put_exit(int iter);       
	
    void showall(){show();roomshow();pathshow();exitshow(); entranceshow(); leaveshow();}
    void show(){
        int i,i2;
        for (i = 0; i < h; i++){
            for(i2 = 0; i2 < w; i2++){
                cout << get_tile(get_xy(i2,i));
            }
            cout << '\n';
        }
    }   
    void roomshow(){int i; for(i = 0; i < roomsum; i++) box[i].show();}
    void pathshow(){int i; for(i = 0; i < pathsum; i++) line[i].show();}
    void exitshow(){int i; for(i = 0; i < exitsum; i++) door[i].show();} 
    void entranceshow(){
        cout << "Beginning in room " << begin.get_roomid() << " with coor("
            << begin.get_x() << "," << begin.get_y() << ")" << '\n';
    }
    void leaveshow(){
        cout << "Leaving in room " << end.get_roomid() << " with coor("
            << end.get_x() << "," << end.get_y() << ")" << '\n';
    }
};
class Entity: private virtual Location{
    private:
        int mapsum;
        int startmap;
    public:
        Map* level;
    Entity(){
        base_init();
        mapsum = 1;
        startmap = random_int(0, mapsum);
        level = new Map[mapsum];
        int i;
        for (i = 0; i < mapsum; i++)
        {
            level[i].init();
        }
    }
    ~Entity(){
        delete [] level;
    }
    int get_mapsum(){int a; a = mapsum; return a;}
    int get_startmap(){int a; a = startmap; return a;}
};
#endif
