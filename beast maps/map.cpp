#include <time.h> 
#include <math.h>
#include <new>

#include "classes.h"
using namespace std;
//functions for the Map class
void Map::init()
{
    base_init();
    roomsum = 0; pathsum = 0;
   
    init(MAPW,MAPH,MINROOM,MAXROOM);
    int sumempty = 0;
    int i; for(i = 0; i < roomsum; i++) if(box[i].get_active() == 0) sumempty++;
    if (roomsum < 2 || pathsum < 3 || sumempty == roomsum){
        remove();
        init();     
    }
    return;
    
}
void Map::init(int width, int height, int min, int max)
{  
    roomsum = random_int(min,max);
    pathsum = 1;
    exitsum = 0;
    floor = id;
    w = width;
    h = height;
    area = w * h;
    exitlist = new int[1];
    exitlist[0] = -1; exitlist[1] = -1;
    tile = new int[area + 1];        
    box = new Room[roomsum + 1];
    line = new Path[pathsum];
    door = new Exit[exitsum + 1];  
    
    makerooms(); if (roomsum < 2) return;    
    makepaths(); if (pathsum < 3) return;
    makeexits();
    
//   checkroom(); if (roomsum < 2) return;  
    
   search_paths(); if(roomsum < 2) return;
   
   makeentrance();
   makeleave();
   
   put_all();
}
void Map::makerooms(){          
        int i;       
        //randomly generate the rooms
        for (i = 0; i < roomsum; i++) box[i].make(i,w,h,ROOMW,ROOMH);
        
        check_box(); //check for overlap and deactivate rooms that overlap others        
        prune_room(0);//swap out, and delete deactivated rooms        
        
        //put rooms on the map so that paths may search for them (will be overwritten after rooms are checked for reachability
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
void Map::makeentrance()
{   
    int xout, yout;
    int xrand, yrand;
    int chooseroom;
    chooseroom = random_int(0, roomsum - 1);
    chooseroom = box[chooseroom].get_id();
    begin.put_roomid(chooseroom);
    
    xrand = random_int(0, box[chooseroom].get_w() - 2); 
    yrand = random_int(0, box[chooseroom].get_h() - 2);
    xout = xrand + box[chooseroom].get_x() + 1;
    yout = yrand + box[chooseroom].get_y() + 1;  
    begin.put_coor(xout,yout);
    begin.activate();
}
void Map::makeleave()
{
    int xout, yout;
    int xrand, yrand;
    int chooseroom;
    chooseroom = random_int(0, roomsum - 1);
    chooseroom = box[chooseroom].get_id();
    if (chooseroom == begin.get_roomid()) chooseroom++;
    if (chooseroom > roomsum - 1) chooseroom = 0;
    end.put_roomid(chooseroom);
    
    xrand = random_int(0, box[chooseroom].get_w() - 2); 
    yrand = random_int(0, box[chooseroom].get_h() - 2);
    xout = xrand + box[chooseroom].get_x() + 1;
    yout = yrand + box[chooseroom].get_y() + 1;  
    
    end.put_coor(xout,yout);
    end.activate();
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

//void Map::checkroom()
//{
//    int i;
//    bool mark = 0;
//    for (i = 0; i < roomsum; i++)
//    {
//      //  cout << "Room " << i << " has " << box[i].get_exitsum() << " exits" << '\n';
//        if( box[i].get_exitsum() < 1) {
//            box[i].deactivate();
//            mark = 1;
//        //    cout << "Room " << i << " has no exits...deactivating" << '\n';
//        }
//    }
//    if (mark == 1) prune_room(1);
//    return;
//}

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
        //if (line[i].get_id() == -1) line[i].deactivate();
        
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
void Map::makeexit(int idin, Coor place, int toroom, int topath){	
    door[idin].activate();
    door[idin].put_roomid(toroom);
    door[idin].put_id(idin); 	    
    door[idin].put_coor(place.x,place.y);
    door[idin].put_pathid(topath);	   
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
