#ifndef TOOLS_H
#define TOOLS_H

#include <allegro.h>
#include <loadpng.h>
#include <math.h>
#include <png.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

extern int resDiv;
extern string modFolder;

string convertBoolToString(bool boolean);
int convertStringToBool(string newString);
string convertIntToString(int number);
int convertStringToInt(string newString);
bool fexists(const char* filename);
int random(int newLowest, int newHighest);
int mouseX();
int mouseY();
bool collisionAny(int xMin1,
                  int xMax1,
                  int xMin2,
                  int xMax2,
                  int yMin1,
                  int yMax1,
                  int yMin2,
                  int yMax2);
bool collisionBottom(int yMin1, int yMax1, int yMin2, int yMax2);
bool collisionTop(int yMin1, int yMax1, int yMin2, int yMax2);
bool collisionRight(int xMin1, int xMax1, int xMin2, int xMax2);
bool collisionLeft(int xMin1, int xMax1, int xMin2, int xMax2);
void highcolor_fade_in(BITMAP* bmp_orig, int speed);
void highcolor_fade_out(int speed);
void abort_on_error(const char* message);

#endif
