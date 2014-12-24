#ifndef TOOLS_H
#define TOOLS_H

#include <allegro.h>
#include <alpng.h>

#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
#include <iostream>

using namespace std;

extern int resDiv;
extern string modFolder;

string convertBoolToString(bool boolean);
int convertStringToBool(string newString);
string convertIntToString(int number);
int convertStringToInt(string newString);
bool fexists(const char *filename);
int random(int newLowest, int newHighest);
int mouseX();
int mouseY();
bool collisionAny(int xMin1, int xMax1, int xMin2, int xMax2, int yMin1, int yMax1, int yMin2, int yMax2);
bool collisionBottom(int yMin1, int yMax1, int yMin2, int yMax2);
bool collisionTop(int yMin1, int yMax1, int yMin2, int yMax2);
bool collisionRight(int xMin1, int xMax1, int xMin2, int xMax2);
bool collisionLeft(int xMin1, int xMax1, int xMin2, int xMax2);
void highcolor_fade_in(BITMAP* bmp_orig, int speed);
void highcolor_fade_out(int speed);
void abort_on_error(const char *message);

#endif
