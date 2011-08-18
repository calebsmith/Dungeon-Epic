#include <time.h> 
#include <math.h>
#include <new>

#include "classes.h"
using namespace std;
//functions for the Map class
void Map::init()
{
    roomsum = 0; pathsum = 0;
   
    dunginit(MAPW,MAPH,MINROOM,MAXROOM);
    
    int sumempty = 0;
    int i; for(i = 0; i < roomsum; i++) if(box[i].get_active() == 0) sumempty++;
    if (roomsum < 4 || pathsum < 3 || sumempty > 0){
        remove();
        init();     
    }    
    return;    
}
void Map::wildinit(int dungin, int townin)
{
    roomsum = 0;
    pathsum = 1;
    exitsum = 0;
    warpsum = 0;
    dungeonsum = dungin;
    townsum = townin;
    w = MAPW; h = MAPH;
    area = w * h;
    tile = new int[area + 1]; 
    solids = new bool[area + 1];
    graph = new int[area + 1];       
    box = new Room[roomsum + 1];
    line = new Path[pathsum + 1];
    door = new Exit[exitsum + 1];  
    warps = new Warp[warpsum + 1];
    dungeons = new Location[dungeonsum + 1];
    towns = new Location[townsum + 1];
    clear_map();
    
    begin.put_x(random_int(0, w));
    begin.put_y(random_int(0, h));
    
}
void Map::put_trees()
{
    int i;
    int chance; //put a tree here?
    for (i = 0; i < area; i++){
        chance = random_int(0,5);
        if ((chance >= 3)        
        && (tile[i - w] == 0 && i > w) 
        && (tile[i - 1] == 0 && i > 0)
        && (tile[i - w - 1] == 0 && i > w + 1)
        && (tile[i + 1] == 0 && i < area)
        && (tile[i + w] == 0 && i + w < area)){     
           tile[i] = 1;
        }
    }
}
void Map::dunginit(int width, int height, int min, int max)
{  
    roomsum = random_int(min,max);
    pathsum = 1;
    exitsum = 0;
    warpsum = 0;
    dungeonsum = 0;
    townsum = 0;
    w = width;
    h = height;
    area = w * h;
    tile = new int[area + 1]; 
    solids = new bool[area + 1];
    graph = new int[area + 1];       
    box = new Room[roomsum + 1];
    line = new Path[pathsum];
    door = new Exit[exitsum + 1];  
    warps = new Warp[warpsum + 1];
    dungeons = new Location[dungeonsum + 1];
    towns = new Location[townsum + 1];
   //generate rooms, then paths, then exits
    makerooms(); if (roomsum < 4) return;    
    makepaths(); if (pathsum < 3) return;
    makeexits();   
   //make sure all rooms can be found from beginning room. If not, delete the unreachable rooms.      
    search_paths(); if (roomsum < 4 || pathsum < 3) return; 
}
void Map::makerooms(){          
        int i;       
        //randomly generate the rooms
        for (i = 0; i < roomsum; i++) box[i].make(i,w,h,ROOMW,ROOMH);
        
        check_box(); //check for overlap and deactivate rooms that overlap others        
        prune_room(0);//swap out, and delete deactivated rooms
               
        //put rooms on the map so that paths may search for them (will be overwritten after rooms are checked for reachability
        clear_map();
        put_rooms();          
}

void Map::makepaths()
{
    int i, i2;
    int pathid = -1;
    int source,dest;
    int dir;
    bool flip;
    string textdir;
    for (i = 0; i < roomsum; i++)
    {
        for (i2 = i + 1; i2 < roomsum; i2++)
        {
            if (i != i2 && box[i].get_active() == 1 && box[i2].get_active() == 1)
            {
               
                flip = 0;
                dir = path_over(box[i],box[i2]);
                if (dir == -1) {
                    dir = path_over(box[i2], box[i]);
                    flip = 1;
                }
                if (dir != -1) {
                   
                    if (flip == 0) {
                        source = box[i2].get_id();
                        dest = box[i].get_id();
                    }
                    if (flip == 1){
                        source = box[i].get_id();
                        dest = box[i2].get_id();
                    }
                pathid++;
                addpath();
                line[pathid].make(pathid,source,dest,dir,1); 
                }
               
            }
        }
    }
    pathsum --;
    //std::cout << "Total paths: " << pathsum << '\n';
    if (pathsum > 0) findroute();
    return;
}
void Map::makeexits()
{
    int i;   
    int src, dest; 
    Coor exitsource, exitdest;
    int pathid = 0;
    int exitnum = 0;
    
    for(i = 0; i < pathsum; i++)
    {
        if(line[i].get_active() == 1)
        {
        pathid = line[i].get_id();    
        src = box[line[i].get_sourceid()].get_id();
        dest = box[line[i].get_destid()].get_id();
        
        findexit(pathid, exitsource, exitdest);
        
        addexit();       
        makeexit(exitnum, exitsource, src, pathid);    
        box[src].add_exitsum();   
        exitnum++;
        
        addexit();
        makeexit(exitnum, exitdest, dest, pathid);
        box[dest].add_exitsum(); 
        exitnum++;
        }
    }
    //std::cout << "Total exits: " << exitsum << '\n';
    return;
}
void Map::makeexit(int idin, Coor place, int toroom, int topath){	
    door[idin].activate();
    door[idin].put_roomid(toroom);
    door[idin].put_id(idin); 	    
    door[idin].put_coor(place.x,place.y);
    door[idin].put_pathid(topath);	   
}

void Map::makewarps()
{   
    Coor xy;
    int i;
    int mapvalue;
    int mapxy;
    int attempt;
    for (i = 0; i < warpsum; i++)
    {
        warps[i].put_id(i);
        mapvalue = 0;
        attempt = 0;
        while(mapvalue != 1)
        {
            xy = find_coor_room();  
            mapxy = get_xy(xy.x,xy.y);
            mapvalue = get_tile(mapxy); 
            attempt++;
            if (attempt > 10) return;
        }     
        
        warps[i].put_coor(xy.x, xy.y);  
        put_warp_source(i); 
        warps[i].put_sourcemap(id);     
        
        mapvalue = 0;
        attempt = 0;
        while(mapvalue != 1)
        {
            xy = find_coor_room();  
            mapxy = get_xy(xy.x,xy.y);
            mapvalue = get_tile(mapxy); 
            attempt++;
            if (attempt > 10) return;
        }   
       
           
        warps[i].put_dest(xy.x, xy.y);
        put_warp_dest(i);
        warps[i].put_destmap(id);
        
        warps[i].activate();
       
    }
    return;
}
void Map::makeentrance(int dest)
{   
    Coor xy;
    int mapvalue, mapxy;   
    mapvalue = 0;
    while(mapvalue != 1)
    {
        xy = find_coor_room();  
        mapxy = get_xy(xy.x,xy.y);
        mapvalue = get_tile(mapxy); 
    }   
    begin.put_coor(xy.x, xy.y);
    begin.put_sourcemap(id);
    begin.put_destmap(dest);
    begin.activate();
    put_entrance();
}
void Map::makeleave(int dest)
{    
    Coor xy;
    int mapvalue, mapxy;   
    mapvalue = 0;
    while(mapvalue != 1)
    {
        xy = find_coor_room();  
        mapxy = get_xy(xy.x,xy.y);
        mapvalue = get_tile(mapxy); 
    }  
    end.put_coor(xy.x, xy.y);    
    end.put_sourcemap(id);
    end.put_destmap(dest);
    end.activate();
    put_leave();
}

Coor Map::find_coor_room(int chooseroom)
{
    Coor return_xy;
    int xrand = random_int(0, box[chooseroom].get_w() - 2);
    int yrand = random_int(0, box[chooseroom].get_h() - 2);
    int xout = xrand + box[chooseroom].get_x() + 1;
    int yout = yrand + box[chooseroom].get_y() + 1;
    return_xy.x = xout; return_xy.y = yout;    
    return return_xy;
}
void Map::prune_room(bool swapmore)
{
    int i, i2;
    bool curactive;
    //swap out deactivated rooms. Put inactive ones at the end
        for(i = 0; i < roomsum; i++)
        {
            if (box[i].get_active() == 0){
                for(i2 = i + 1; i2 < roomsum; i2++)
                {
                    if(box[i2].get_active() == 1){
                        swaproom(box[i],box[i2]);
                        swaproom_id(box[i],box[i2]);
                        if (pathsum > 1) swap_room_paths(box[i],box[i2]);
                       // if (swapmore == 1) swap_room_paths(box[i],box[i2]);                        
                    }    
                }
            }
        }
       
        //delete inactive rooms starting at the end
        for(i = roomsum - 1; i >= 0; i--)
        {
            curactive = box[i].get_active();
            if (curactive == 0) box[i].remove();
            if (curactive == 1){
                roomsum= i + 1;
                break;
            }
        }
}
void Map::prune_path(bool swapmore)
{
    int i, i2;
     //swap out deactivated paths . Put inactive ones at the end
    for(i = 0; i < pathsum ; i++)
    {       
        if (line[i].get_active() == 0)
        {
            for(i2 = i + 1; i2 < pathsum ; i2++)
            {
                if(line[i2].get_active() == 1)
                {
                    swappath(line[i],line[i2]);
                    swappath_id(line[i], line[i2]);
                    if (exitsum > 0) swap_path_exits(line[i],line[i2]);
                    //if (swapmore == 1) swap_path_exits(line[i],line[i2]);
                    
                }    
            }
        }
    }
    //delete inactive paths  starting at the end
    for(i = pathsum - 1; i >= 0; i--)
    {        
        if (line[i].get_active() == 1) 
        {
            pathsum  = i + 1;
            break;
        }
    }
}
void Map::prune_exit()
{
    int i, i2;
    for(i = 0; i < exitsum ; i++)
    {       
        if (door[i].get_active() == 0)
        {
            for(i2 = i + 1; i2 < exitsum ; i2++)
            {
                if(door[i2].get_active() == 1)
                {
                    swapexit(door[i],door[i2]);
                } 
            }
        }
    }
    //delete inactive exits  starting at the end
    for(i = exitsum - 1; i >= 0; i--)
    {        
        if (door[i].get_active() == 1) 
        {
            exitsum  = i + 1;
            break;
        }
    }    
}

void Map::check_box(){
    int i,i2;
    int check;
    for (i = 0; i < roomsum; i++)
    {
        for (i2 = i + 1; i2 < roomsum; i2++)
        {
            if (box[i].get_active()==1 && box[i2].get_active()==1) 
            {
                if (i != i2)
                {
                    check = 0;
                    check=overlap(box[i],box[i2]); 
                    if (check == 0) check=overlap(box[i2],box[i]);
                    if (check == 1)
                    {
                        box[i2].deactivate();
                    } 
                }      
            }                
        }
    }
return;
}
    
bool Map::overlap(Room& a, Room& b){
    int x1 = a.get_x();
    int x2 = x1 + a.get_w();
    int y1 = a.get_y();
    int y2 = y1 + a.get_h();
    
    int x3 = b.get_x();
    int x4 = x3 + b.get_w();
    int y3 = b.get_y();
    int y4 = y3 + b.get_h();
    
    int i;
    if((x1 > x3 && x1 < x4) || (x2 > x3 && x2 < x4))
    {
        for(i = y1; i <= y2;i++)
        {           
            if(i > y3 && i < y4) return 1;            
        }
    }
    if((y1 > y3 && y1 < y4) || (y2 > y3 && y2 < y4))
    {
        for(i = x1; i <= x2;i++)
        {            
            if(i > x3 && i < x4) return 1;                        
        }
    }
  
       
    return 0;
}

void Map::findroute()
{
    int i;
    //direction and room id's
    bool orient;
    int src, dest;
    int dir;
    int pathid;
    //coor. of both rooms
    int startx, startx2;
    int starty, starty2;
    int endx, endx2;
    int endy, endy2;
    //coor. of the narrow path (where walls of two rooms may intersect with a perpindicular path)
    int pathx, pathx2;
    int pathy, pathy2;
    
    
    bool check;//did the path return succesfully?
   
    for (i = 0; i < pathsum ; i++)
    {
        //establish preliminary values of each path (source, destination, direction and orientation)
        src = box[line[i].get_sourceid()].get_id();
        dest = box[line[i].get_destid()].get_id();
        dir = line[i].get_direction();
        pathid = line[i].get_id();
        if (dir == UP || dir == DOWN) orient = VERTICAL;
        if (dir == LEFT || dir == RIGHT) orient = HORIZONTAL;
        
        //find boundaries of coordinates
        find_bound(dir,src, dest, &startx,&startx2, &starty, &starty2, &endx, &endx2, &endy, &endy2);
        
        //narrow the boundaries so that walls on both sides can match up
        pathx = greater_int(startx, endx); pathy = greater_int(starty, endy);
        pathx2 = lesser_int(startx2,endx2); pathy2 = lesser_int(starty2, endy2);
        
        //check each lane in random order, choose the first that works and apply it.
        check = test_bound(pathid, pathx,pathy,pathx2,pathy2,orient);
        
        //if no lanes can reach deactivate the path        
        if (check == 0) line[i].deactivate();        
    }
    prune_path(0);    
    return;
}

bool Map::test_bound(int pathid, int pathx, int pathy, int pathx2, int pathy2, bool orient)
{
    bool check = 0; // is the path successful?
    int lane;   // what lane is the path successful at?
    int start,end;
    int* pathlist = 0;
    pathlist = new int[1];
    
    int low_val, high_val;
    int xdiff, ydiff,diff;
    xdiff = abs(pathx2 - pathx);
    ydiff = abs(pathy2 - pathy);
    
    switch(orient)
    {
        case 0:
            diff = xdiff + 1;            
            low_val = lesser_int(pathx,pathx2);
            high_val = greater_int(pathx,pathx2);
            start = lesser_int(pathy,pathy2);
            end = greater_int(pathy,pathy2);
            break;
        case 1:
            diff = ydiff + 1;
            low_val = lesser_int(pathy,pathy2);
            high_val = greater_int(pathy,pathy2);
            start = lesser_int(pathx,pathx2);
            end = greater_int(pathx,pathx2);
            break;
    }
    //create list of possible lanes
    pathlist = new int[diff];
    int i, i2;
    for (i = 0; i < diff; i++) 
    {
        i2 = low_val + i + 1;
        pathlist[i] = i2;       
    }
    shuffle(pathlist, diff - 2);//shuffle the order in which to check each lane

    lane = test_lane(pathlist, diff - 2, start, end, orient, PATH);//find the coordinates
    if (lane >= 0) {
        line[pathid].put_coor(lane, start, end, orient);//put the coordinates that were found        
        check = 1;
    }
    delete [] pathlist;
    return check;
}
int Map::test_lane(int* list, int size, int start, int end, bool orient, int findval)
{
    int x1,y1;
    int xy;
    int i, i2;
    int value;
    int lane;
    bool mark;
    for(i = 0; i < size; i++)
    {
        lane = list[i];
        mark = 0;
        for(i2 = start + 1; i2 <= end - 1; i2++)
        {
            switch(orient)
            {
                case VERTICAL:
                    x1 = lane;
                    y1 = i2;
                    break;                  
                case HORIZONTAL:
                    x1 = i2;
                    y1 = lane;
                    break;
            }
            xy = get_xy(x1,y1);
            value = get_tile(xy);
            if (value > 0 || value == findval){          
                mark = 1;
                break;
            }
        }
        if (mark == 0) return lane;         
    }
    return -1;
}
void Map::find_bound(int dir, int src, int dest,int* startx, int* startx2, int* starty, int* starty2, int* endx, int* endx2, int* endy, int* endy2)
{
 switch(dir)
        {
            case UP:
                *startx = box[src].get_x();
                *startx2 = *startx + box[src].get_w() - 1;
                *starty = box[src].get_y();
                *starty2 = *starty;
                *endx = box[dest].get_x();
                *endx2 = *endx + box[dest].get_w() - 1;
                *endy = box[dest].get_y() + box[dest].get_h() - 1;
                *endy2 = *endy;
                break;
            case DOWN:
                *startx = box[src].get_x();
                *startx2 = *startx + box[src].get_w() - 1;
                *starty = box[src].get_y() + box[src].get_h() - 1;
                *starty2 = *starty;
                *endx = box[dest].get_x();
                *endx2 = *endx + box[dest].get_w() - 1;
                *endy = box[dest].get_y();
                *endy2 = *endy;
                break;
            case LEFT:
                *startx = box[src].get_x();
                *startx2 = *startx;
                *starty = box[src].get_y();
                *starty2 = *starty + box[src].get_h() - 1;
                *endx = box[dest].get_x() + box[dest].get_w() - 1;
                *endx2 = *endx;
                *endy = box[dest].get_y();
                *endy2 = *endy + box[dest].get_h() - 1;
                break;
            case RIGHT:
                *startx = box[src].get_x() + box[src].get_w() - 1;
                *startx2 = *startx;
                *starty = box[src].get_y();
                *starty2 = *starty + box[src].get_h() - 1;
                *endx = box[dest].get_x();
                *endx2 = *endx;
                *endy = box[dest].get_y();
                *endy2 = *endy + box[dest].get_h() - 1;
                break;
        }
    return;
}

int Map::path_over(Room& a, Room& b)
{
    int x1 = a.get_x();
    int x2 = x1 + a.get_w();
    int y1 = a.get_y();
    int y2 = y1 + a.get_h();
    
    int x3 = b.get_x();
    int x4 = x3 + b.get_w();
    int y3 = b.get_y();
    int y4 = y3 + b.get_h();
    
    if((x1 > x3 && x1 < x4) || (x2 > x3 && x2 < x4))
    {
        if(y2 <= y3) return UP;   
        if(y1 >= y4) return DOWN;                
    }
    if((y1 > y3 && y1 < y4) || (y2 > y3 && y2 < y4))
    {
        if(x2 <= x3) return LEFT;         
        if(x1 >= x4) return RIGHT;                    
    }
    return -1;
}

void Map::findexit(int pathid, Coor& source, Coor& dest)
{   
    int direction = line[pathid].get_direction();
    int exitx1 = line[pathid].get_x();
    int exity1 = line[pathid].get_y();
    int exitx2 = line[pathid].get_x2();
    int exity2 = line[pathid].get_y2();
    
    Coor exit1, exit2;
    
    exit1.x = lesser_int(exitx1, exitx2);
    exit2.x = greater_int(exitx1, exitx2);
    
    exit1.y = lesser_int(exity1, exity2);
    exit2.y = greater_int(exity1, exity2);
    
    switch(direction){
        case UP:
            source.x = exit1.x; source.y = exit2.y;
            dest.x = exit1.x; dest.y = exit1.y;
            break;
        case DOWN:
            source.x = exit1.x; source.y = exit1.y;
            dest.x = exit1.x; dest.y = exit2.y;
            break;
        case LEFT:
            source.x = exit2.x; source.y = exit1.y;
            dest.x = exit1.x; dest.y = exit1.y;
            break;
        case RIGHT:
            source.x = exit1.x; source.y = exit1.y;
            dest.x = exit2.x; dest.y = exit1.y;
            break;
    }    
}
void Map::addpath(){       
    int i;
    
    Path* buffer;        
    buffer = new Path[pathsum + 1];
    for(i = 0; i < pathsum; i++) {buffer[i] = line[i];}
    
    delete [] line;
    line = new Path[pathsum + 1];
    pathsum = pathsum + 1;
    for(i = 0; i < pathsum - 1; i++) line[i] = buffer[i];
    delete [] buffer;        
}

void Map::addexit()
{	
    int i;     
    Exit* buffer;     
    buffer = new Exit[exitsum + 1];    
    for(i = 0; i < exitsum; i++) buffer[i] = door[i]; 
           
    delete [] door;
    door = new Exit[exitsum + 1];        
    exitsum = exitsum + 1;
    for(i = 0; i < exitsum - 1; i++) door[i] = buffer[i];
    delete [] buffer;   
}
void Map::addwarp()
{
    int i;
    Warp* buffer;
    buffer = new Warp[warpsum + 1];
    for(i = 0; i < warpsum; i++) buffer[i] = warps[i];
    delete [] warps;
    warps = new Warp[warpsum + 1];
    warpsum++;
    for(i = 0; i < warpsum - 1; i++) warps[i] = buffer[i];
    delete [] buffer;
}

void Map::search_paths()
{
    int first, ident;   
    int i, i2;
    //choose a beginning room.
    first = 0;
    for (i = 0; i < roomsum; i++){
        first = i;
        if (box[i].get_exitsum() > 0) break;           
    }
    ident = box[first].get_id();
  
    
    int* net;
    net = new int[roomsum + 1];
    for(i = 0; i < roomsum; i++) net[i] = 0;
    
    follow_path(ident, net, roomsum);//find all rooms that are reachable from the beginning room
    
    //deactivate unattached rooms, paths, and exits
    for(i = 0; i < roomsum; i++){
        if (net[i] == 0) {
            box[i].deactivate();
            for(i2 = 0; i2 < pathsum; i2++)
            {            
                if (line[i2].get_sourceid() == i) line[i2].deactivate();
                if (line[i2].get_destid() == i) line[i2].deactivate();
            }
            for(i2 = 0; i2 < exitsum; i2++)
            {
                if (line[door[i2].get_pathid()].get_active() == 0) door[i2].deactivate();
            }
        }
    }    
    prune_all();
    return;
}
void Map::follow_path(int ident, int* net, int roomsum)
{
    int i;
    int src, dest;  
    bool found = 0;
    for (i = 0; i < pathsum; i++)
    {
        src = line[i].get_sourceid();
        dest = line[i].get_destid();
        if (src == ident && dest < roomsum && net[dest] == 0) net[dest] = 1;   
        if (dest == ident && src < roomsum && net[src] == 0) net[src] = 1; 
        if (src == ident || dest == ident) found = 1;       
    }
    if (found == 1) net[ident] = 2;
    if (found == 0) net[ident] = 3;
    for (i= 0; i < roomsum; i++)
    {
        if(net[i] == 1) follow_path(i, net, roomsum);
    }
    return;
}
int Map::find_dungeon(int xin, int yin)
{
    int value = 0;
    int i;
    for(i = 0; i < dungeonsum; i++){
        if (dungeons[i].get_x() == xin && dungeons[i].get_y() == yin){
            value = dungeons[i].get_id();
        }
    }
    return value;
}
int Map::find_town(int xin, int yin)
{
    int value = 0;
    int i;
    for(i = 0; i < townsum; i++){
        if (towns[i].get_x() == xin && towns[i].get_y() == yin){
            value = towns[i].get_id();
        }
    }
    return value;
}
int Map::door_id(int xin, int yin)
{
    int value = 0;
    int i;
    for (i = 0; i < exitsum; i++)
    {
        if (door[i].get_x() == xin && door[i].get_y() == yin){
            value = door[i].get_id();
            return value;
        }
    }
    cout << "Error finding door id at coordinates (" << xin << "," << yin << ")" << '\n';
    return value;
}
int Map::door_flag(int xin, int yin)
{
    int value = 0;
    int i;
    for (i = 0; i < exitsum; i++)
    {
        if (door[i].get_x() == xin && door[i].get_y() == yin){
            value = door[i].get_flags();
            return value;
        }
    }
    cout << "Error finding door flag at coordinates (" << xin << "," << yin << ")" << '\n';
    return value;
}
int Map::warp_id(int xin, int yin)
{
    int value = 0;
    int i;
    for (i = 0; i < warpsum; i++)
    {
        if ((warps[i].get_x() == xin && warps[i].get_y() == yin) 
            || (warps[i].get_destx() == xin && warps[i].get_desty() == yin)){
            value = warps[i].get_id();
            return value;
        }
    }
    cout << "Error finding warp id! at coordinates (" << xin << "," << yin << ")" << '\n';
    return value;
}
bool Map::warp_side(int iter, int xin, int yin)
{
    if(warps[iter].get_x() == xin && warps[iter].get_y() == yin) return 0;
    return 1;
}
void Map::put_base()
{
    int i; 
    for (i = 0; i < area; i++) {
        if (type == 0) tile[i] = 0;
        if (type == 1) tile[i] = 0;
    }
}
void Map::put_room(int iter)
{
    if (box[iter].get_active() == 0) return;
    int x1 = box[iter].get_x();
    int y1 = box[iter].get_y();
    int w1 = box[iter].get_w();
    int h1 = box[iter].get_h();       
    int x2 = x1 + w1;
    int y2 = y1 + h1;
    int i, i2;
    for (i = y1; i < y2; i++)
    {
        for (i2 = x1; i2 < x2; i2++)
        {
            if (i > y1 && i < y2 - 1 && i2 > x1 && i2 < x2 - 1) put(i2,i,FLOOR);             
          
            else{                  
               put(i2,i,WALL);
            }   
        }
    }
}
void Map::put_path(int iter)
{
    if(line[iter].get_active() == 0) return;
    int i, i2;
    int x1 = line[iter].get_x();
    int y1 = line[iter].get_y();
    int x2 = line[iter].get_x2();
    int y2 = line[iter].get_y2();
    int value;
    int xy;
    for (i = y1; i <= y2; i++)
    {
        for(i2 = x1; i2 <= x2; i2++)
        {
            xy = get_xy(i2,i);
            value = get_tile(xy);
            if (value == 0) put(i2,i,PATH);
        }
    }     
}
void Map::put_exit(int iter)
{
    if(door[iter].get_active() == 0) return;
    int xplace = door[iter].get_x();
    int yplace = door[iter].get_y();
    put(xplace,yplace,EXIT); 	
}
void Map::put_warp_source(int iter)
{
    int xplace = warps[iter].get_x();
    int yplace = warps[iter].get_y();
    put(xplace, yplace, WARP);   
}
void Map::put_warp_dest(int iter)
{
    int xplace = warps[iter].get_destx();
    int yplace = warps[iter].get_desty();
    put(xplace, yplace, WARP);
}
void Map::put_edges(){
    int tilex, tiley;
    for(tilex = 0; tilex < w; tilex++){
        put(tilex, 0, WALL);
        put(tilex, h - 1, WALL);
    }
    for(tiley = 0; tiley < h; tiley++){
        put(0, tiley, EXIT);
        put(w - 1, tiley, WALL);
    }
}
void Map::put_solids()
{
    int i, mapvalue;
    for (i = 0; i < area; i++) solids[i] = 0;    
    for (i = 0; i < area; i++) {
        mapvalue = get_tile(i);
        switch(type) {
            case DUNGEON:
                if (mapvalue == NONE) solids[i] = 1;
                if (mapvalue == WALL) solids[i] = 1;
                if (mapvalue == EXIT) solids[i] = 1;
                break;
            case TOWN:
                if (mapvalue == WALL) solids[i] = 1;                
                if (mapvalue == EXIT) solids[i] = 1;
                break;
            case FOREST:
                if (mapvalue == WALL) solids[i] = 1;                
                if (mapvalue == EXIT) solids[i] = 1;
                if (mapvalue == FLOOR) solids[i] = 1;
                if (mapvalue == CITY) solids[i] = 1;
                if (mapvalue == CAVE) solids[i] = 1;
                break;
        }    
    }
}
void Map::put_graph()
{
    int i, mapvalue;
    for (i = 0; i < area; i++) graph[i] = 0;
    
    for (i = 0; i < area; i++) {
        mapvalue = get_tile(i);
        switch(type) {
            case DUNGEON:
                if (mapvalue == PATH) graph[i] = FLOOR; 
                graph[i] = mapvalue;
                break;
            case TOWN:
                graph[i] = mapvalue;
                break;
            case FOREST:
                graph[i] = mapvalue;
                if (mapvalue == CAVE) graph[i] = UPSTAIRS;
                if (mapvalue == CITY) graph[i] = DOWNSTAIRS;                
                break;
        }    
    }
}

