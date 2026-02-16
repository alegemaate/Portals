#ifndef TOOLS_H
#define TOOLS_H

#include <math.h>
#include <png.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

extern string modFolder;

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

#endif
