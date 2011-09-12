#ifndef CONSTANTS_H
#define CONSTANTS_H
const int SCRWIDTH=800;
const int SCRHEIGHT=600;
const int SCRBPP=32;

const bool FRAMECAP = 1;
const int MAXFPS_SECONDS = 60;
const int MAXFPS_MS = 1000 / MAXFPS_SECONDS;

const int TILEW = 64;
const int TILEH = 64;

const int CHARW = 32;
const int CHARH = 32;

const int PLAYER_MOVE_PIXELS = 3;

enum{DUNGEON, TOWN, FOREST};
enum{ON, OFF};
enum{OPEN, CLOSED};
enum{UP, DOWN, LEFT, RIGHT};
enum{VERTICAL, HORIZONTAL};
enum{NONE, FLOOR, WALL, EXIT, PATH, UPSTAIRS, DOWNSTAIRS, WARP, CITY, CAVE};
const int MAPW = 60;
const int MAPH = 30;
const int ROOMW = 15;
const int ROOMH = 10;
const int MINROOM = 6;
const int MAXROOM = 14;
#endif
