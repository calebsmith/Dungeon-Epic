#ifndef MAP_H
#define MAP_H
#include <math.h>

using namespace std;

class Map: public virtual Location{
    private:
        bool rendered;      
        int w,h;        
        int area;
        int floor;
        int roomsum;
        int pathsum;
        int exitsum;
        int warpsum;
        int townsum;
        int dungeonsum;
        int* tile;
        bool* solids;
        int* graph;
        Room* box;
        Path* line;
        Exit* door;
        Warp* warps;
        Location* dungeons;
        Location* towns;
        Stair begin;
        Stair end;
    public:
    Map(){rendered = 0;}
    void init();
    void wildinit(int dungin, int townin);   
    //void crap_begin();
    void dunginit(int width, int height, int min, int max);
    
    ~Map(){remove();}
    void remove(){
        delete [] box;
        delete [] line;
        delete [] door;
        delete [] tile;
        delete [] graph;
        delete [] solids;
        delete [] warps;
        delete [] dungeons;
        delete [] towns;
    }
    Map& operator= (const Map& a){
        if (this == &a) return *this;
        id = a.id;        
        x = a.x; y = a.y; z = a.z;        
        type = a.type;
        flags = a.flags;
        active = a.active;
        floor = a.floor;
        rendered = a.rendered;
        w = a.w;
        h = a.h;
        roomsum= a.roomsum;
        pathsum = a.pathsum;
        exitsum = a.exitsum;
        warpsum = a.warpsum;
        dungeonsum = a.dungeonsum;
        townsum = a.townsum;
        area = a.area;
        begin = a.begin;
        end = a.end;
        remove();
        int i;
        box = new Room[roomsum+ 1];
        line = new Path[pathsum];
        door = new Exit[exitsum];
        tile = new int[area + 1];
        solids = new bool[area + 1];
        graph = new int[area + 1];
        warps = new Warp[warpsum + 1];
        dungeons = new Location[dungeonsum + 1];
        towns = new Location[townsum + 1];
        for (i = 0; i < roomsum; i++) box[i] = a.box[i];
        for (i = 0; i < pathsum; i++) line[i] = a.line[i];
        for(i = 0; i < exitsum; i++) door[i] = a.door[i];
        for (i = 0; i < area; i++) tile[i] = a.tile[i];
        for (i = 0; i < area; i++) graph[i] = a.graph[i];
        for (i = 0; i < area; i++) solids[i] = a.solids[i];
        for (i = 0; i < warpsum; i++) warps[i] = a.warps[i];
        for (i = 0; i < dungeonsum; i++) dungeons[i] = a.dungeons[i];
        for (i = 0; i < townsum; i++) towns[i] = a.towns[i];
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
    void makewarps();
    void makeentrance(int dest);
    void makeleave(int dest);
    
    void check_box();       
    bool overlap(Room& a, Room& b);   
    
    void addpath();   
    void findroute();
    void find_bound(int dir, int src, int dest, int* startx, int* startx2, int* starty, int* starty2, int* endx, int* endx2, int* endy, int* endy2);
    bool test_bound(int pathid, int pathx, int pathy, int pathx2, int pathy2, bool orient);
    int path_over(Room& a, Room& b);
    int test_lane(int* list, int size, int start, int end, bool orient, int findval);
   
    void addexit();
    void findexit(int pathid, Coor& source, Coor& dest);
    void makeexit(int idin, Coor place, int toroom, int topath);
    
    void addwarp();
    
    int random_room(){
        int chooseroom = random_int(0,roomsum - 1); 
        if (box[chooseroom].get_active() == 0) random_room();
        return chooseroom;
    }
    Coor find_coor_room(){int chooseroom = random_room(); Coor xy = find_coor_room(chooseroom); return xy;}
    Coor find_coor_room(int chooseroom);
    
    void search_paths();
        void follow_path(int ident, int* net, int netsum);
    
    void prune_all(){prune_room(1); prune_path(1); prune_exit();}   
    void prune_room(bool swapmore);
    void prune_path(bool swapmore);
    void prune_exit();
    
    void set_rendered(bool val){rendered = val;}
    bool get_rendered(){bool a = rendered; return a;}
    int get_begin_x(){int a = begin.get_x(); return a;}
    int get_begin_y(){int a = begin.get_y(); return a;}
    int get_end_x(){int a = end.get_x(); return a;}
    int get_end_y(){int a = end.get_y(); return a;}
    int get_begin_roomid(){int a = begin.get_roomid(); return a;}
    int get_w(){int a = w; return a;}
    int get_h(){int a = h; return a;}
    int get_floor(){int a = floor; return a;}
    int get_roomsum(){int a = roomsum; return a;}
    void get_room(int ident){box[ident].show(); return;}
    int get_pathsum(){int a = pathsum; return a;}
    void get_path(int ident){line[ident].show(); return;}
    int get_exitsum(){int a = exitsum; return a;}
    int get_dungeonsum(){int a = dungeonsum; return a;}
    int get_townsum(){int a = townsum; return a;}
    int get_area(){int a = area; return a;} 
    int get_tile(int xyin) {int a = tile[xyin]; return a;}
    int get_graph(int xy) {int a = graph[xy]; return a;}
    bool get_solid(int xy) {bool a = solids[xy]; return a;}
    
    int get_xy(int x1, int y1){
        int a = x1 + y1 * w;        
        if (a < 0) a = 0; if (a > area) a = area;
        return a;
    }
    
    int door_id(int xin, int yin);
    int door_flag(int xin, int yin);
    void door_open(int iter){ if(iter < exitsum && iter >= 0) door[iter].open();}
    void door_close(int iter){ if(iter < exitsum && iter >= 0) door[iter].close();}
    int warp_id(int xin, int yin);
    bool warp_side(int iter, int xin, int yin);
    int warp_x(int ident){int a = warps[ident].get_x(); return a;}
    int warp_y(int ident){int a = warps[ident].get_y(); return a;}
    int warp_destx(int ident){int a = warps[ident].get_destx(); return a;}
    int warp_desty(int ident){int a = warps[ident].get_desty(); return a;}
    int warp_sourcemap(int ident){int a = warps[ident].get_sourcemap(); return a;}
    int warp_destmap(int ident){int a = warps[ident].get_destmap(); return a;}
    
    int find_dungeon(int xin, int yin);
    int find_town(int xin, int yin);
    
    Coor random_empty(){
        Coor xy;
        int randx, randy;
        int tile_num;
        bool is_full;
        randx = random_int(0, w - 1);
        randy = random_int(0, h - 1);
        tile_num = get_xy(randx, randy);
        is_full = get_graph(tile_num);
        if (is_full > 0) {
            cout << randx << "," << randy << " was occupied by " << is_full << '\n';
            xy = random_empty();
        }
        xy.x = randx;
        xy.y = randy;
        return xy;        
    }
    //like random_empty, but make sure it is clear for entrance/exit in the wilderness
    Coor random_empty_clearing(){
        Coor xy = random_empty();
        //check the tile below
        int tile_num = get_xy(xy.x, xy.y + 1);
        bool is_full = get_graph(tile_num);
        if (is_full > 0 || xy.y + 1 == h){
            xy = random_empty_clearing();            
        }
        return xy;
        
    }
    
    void toggle_solid(int xy){
        if (xy < 0 || xy >= area) return; if (solids[xy] == 1) {solids[xy] = 0; return;} solids[xy] = 1;}
    void set_graph(int xy, int value){if (xy < area && xy >= 0) graph[xy] = value;}
    
    void put_floor(int value){floor = value;}
    void put_begin_xy(Coor xyin){begin.put_x(xyin.x); begin.put_y(xyin.y); return;}
    void put_begin_sourcemap(int ident){begin.put_sourcemap(ident); return;}
    void put_begin_destmap(int ident){begin.put_destmap(ident); return;}
    void put_end_sourcemap(int ident){end.put_sourcemap(ident); return;}
    void put_end_destmap(int ident){end.put_destmap(ident); return;}        
    int get_begin_sourcemap(){int a; a =begin.get_sourcemap(); return a;}
    int get_begin_destmap(){int a = begin.get_destmap(); return a;}
    int get_end_sourcemap(){int a; a = end.get_sourcemap(); return a;}
    int get_end_destmap(){int a = end.get_destmap(); return a;}        
    
    void put(int x1, int y1, int value){
        int xy = get_xy(x1, y1);
        if (xy < area && xy >= 0) tile[xy] = value;       
        return;
    }
    
    void put_all(){put_base(); put_rooms(); put_paths(); put_exits(); put_warps(); 
        if(type == DUNGEON) {
            put_entrance(); 
            put_leave();
        } 
        if(type == TOWN){
            put_edges();
        }
        put_solids(); 
        put_graph();
    }
         
    void clear_map() {int i; for (i = 0; i < area; i++) tile[i] = 0;}
    void put_base();
    void put_rooms() {int i; for(i = 0; i < roomsum; i++) put_room(i);}
    void put_paths() {int i; for(i = 0; i < pathsum; i++) put_path(i);}
    void put_exits() {int i; for(i = 0; i < exitsum; i++) put_exit(i);}
    void put_warps() {int i; for(i = 0; i < warpsum; i++) {put_warp_source(i); put_warp_dest(i);}}
    void put_entrance() {if (begin.get_active()) put(begin.get_x(),begin.get_y(), UPSTAIRS);}
    void put_leave() {if (end.get_active()) put(end.get_x(),end.get_y(), DOWNSTAIRS);}
    void put_edges();
    
    void put_dungeonsum(int value){dungeonsum = value;}
    void put_townsum(int value){townsum = value;}
    
    void put_trees();
    void put_dungeon(int xin, int yin, int iter, int idin){
        dungeons[iter].put_x(xin); 
        dungeons[iter].put_y(yin);
        dungeons[iter].put_id(idin);
    }
    void put_town(int xin, int yin, int iter, int idin){
        towns[iter].put_x(xin); 
        towns[iter].put_y(yin); 
        towns[iter].put_id(idin);
    }
    
    void put_solids();
    void put_graph();
    
    void put_room(int iter);
    void put_path(int iter);  
    void put_exit(int iter);   
    void put_warp_source(int iter);    
	void put_warp_dest(int iter);
	
    void showall(){show();roomshow();pathshow();exitshow(); warpshow(); entranceshow(); leaveshow();}
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
    void warpshow(){int i; for(i = 0; i < warpsum; i++) warps[i].show();}
    void entranceshow(){
        cout << "Beginning in room " << begin.get_roomid() << " with coor("
            << begin.get_x() << "," << begin.get_y() << ")" << '\n';
    }
    void leaveshow(){
        cout << "Leaving in room " << end.get_roomid() << " with coor("
            << end.get_x() << "," << end.get_y() << ")" << '\n';
    }
};
/*An entity contains several floors, each with a unique map
This is used for dungeons so they can have multiple floors. Entity still wraps
towns and regional maps as well */
class Entity: public virtual Location{
    private:
        int mapsum;
        int startmap;
        int wildmap;
    public:
        Map* floors;
    Entity(){
        base_init();
        mapsum = 0;
        startmap = -1;
        wildmap = 0;    
    }
    ~Entity(){
        delete [] floors;
    }
    //void init(int idin){init(idin, 0);}
    void init(int idin, int typein, int dungin, int townin){
        
        if (typein == DUNGEON) {
            mapsum = random_int(3,5);
            type = DUNGEON;
        }
        if (typein == TOWN) {
            mapsum = 1;
            type = TOWN;
        }
        if (typein == FOREST) {
            mapsum = 1;
            type = FOREST;
        }
        startmap = 0;
        id = idin;
        int maxwarps = random_int(0,2);
        floors = new Map[mapsum];
        int i, i2;
        for (i = 0; i < mapsum; i++)
        {                      
            floors[i].base_init();
            floors[i].put_id(id);
            floors[i].put_floor(i);
            floors[i].put_type(type);
            if (type != FOREST) floors[i].init();                
            
            //if the entity has more than one floor (dungeons only)
            if (mapsum > 1){
                if (i == 0) floors[i].makeentrance(wildmap);//stairs go up to wilderness
                if (i > 0) floors[i].makeentrance(i - 1);
                if (i < mapsum - 1) floors[i].makeleave(i + 1);
                //make sure warps can fit if there are only a few small rooms
                if (maxwarps > floors[i].get_roomsum() - 3) maxwarps = floors[i].get_roomsum() -3;
                for (i2 = 0; i2 < maxwarps; i2++) {
                    floors[i].addwarp();
                }                
                floors[i].makewarps(); 
            }
            if (type != FOREST) floors[i].put_all();
            if (type == FOREST) floors[i].wildinit(dungin, townin);
        }
    }
   
    void put_wildmap(int value){wildmap = value;}
    int get_wildmap(){int a; a = wildmap; return a;}
    int get_mapsum(){int a; a = mapsum; return a;}
    int get_startmap(){int a; a = startmap; return a;}
};
class Zone: public virtual Location{
    private:        
        int dungeonsum;
        int townsum;
    public:
        Entity* zones;
        
        Zone(){}
        ~Zone(){remove();}
        void init(){
            id = 0;
            dungeonsum = random_int(5,10);
            townsum = (3,5);
            zones = new Entity[townsum + dungeonsum + 1];
            int mapid;
            int x,y;
            int i;
            
            zones[id].init(id, 2, dungeonsum, townsum);
            
            for (i = 0; i < townsum; i++)
            {
                mapid = id + i + 1;
                zones[mapid].init(mapid, 1, 0 ,0);
                Coor xy = zones[mapid].floors[0].random_empty_clearing();
                    x = xy.x;
                    y = xy.y;
                zones[mapid].put_x(x);
                zones[mapid].put_y(y);
                zones[id].floors[0].put_town(x,y, i, mapid);
                zones[id].floors[0].put(zones[mapid].get_x(), zones[mapid].get_y(),CITY);
            }       
            
            zones[id].floors[0].put_graph();
            for (i = 0; i < dungeonsum; i++)
            {   
                mapid = id + i + 1 + townsum;
                zones[mapid].init(mapid, 0, 0 ,0);
                Coor xy = zones[mapid].floors[0].random_empty_clearing();
                    x = xy.x;
                    y = xy.y;
                zones[mapid].put_x(x);
                zones[mapid].put_y(y);
                zones[id].floors[0].put_dungeon(x,y, i, mapid);
                zones[id].floors[0].put(zones[mapid].get_x(),zones[mapid].get_y(),CAVE); 
            }  
            zones[id].floors[0].put_trees();
            zones[id].floors[0].put_solids();
            zones[id].floors[0].put_graph();
            //mark players initial position
            Coor player_xy;
            int tile_num = 0;
            while(1){
                player_xy = zones[id].floors[0].random_empty();                
                tile_num = zones[id].floors[0].get_xy(player_xy.x, player_xy.y);
                if(zones[id].floors[0].get_tile(tile_num) == 0) break;
            }
                zones[id].floors[0].put_begin_xy(player_xy); 
            //cout << "Player: " << player_xy.x << ", " << player_xy.y << '\n';
            
            //cout << zones[id].floors[0].get_tile(tile_num) << '\n';
        }
        void remove(){
            delete [] zones;
        }
};

#endif
