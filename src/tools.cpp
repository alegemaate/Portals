#include "tools.h"
#include <asw/asw.h>
#include <string>

string modFolder;

// Random number generator. Use int random(highest,lowest);
int random(int newLowest, int newHighest) {
  int lowest = newLowest, highest = newHighest;
  int range = (highest - lowest) + 1;
  int randomNumber = lowest + int(range * rand() / (RAND_MAX + 1.0));
  return randomNumber;
}

// Mouse x
int mouseX() {
  return asw::input::mouse.position.x;
}

// Mouse y
int mouseY() {
  return asw::input::mouse.position.y;
}

// Collision
bool collisionAny(int xMin1,
                  int xMax1,
                  int xMin2,
                  int xMax2,
                  int yMin1,
                  int yMax1,
                  int yMin2,
                  int yMax2) {
  if (xMin1 < xMax2 && yMin1 < yMax2 && xMin2 < xMax1 && yMin2 < yMax1) {
    return true;
  }
  return false;
}
bool collisionBottom(int yMin1, int yMax1, int yMin2, int yMax2) {
  if (yMin1 < yMax2 && yMax1 > yMax2) {
    return true;
  }
  return false;
}
bool collisionTop(int yMin1, int yMax1, int yMin2, int yMax2) {
  if (yMin2 < yMax1 && yMin1 < yMin2) {
    return true;
  }
  return false;
}
bool collisionRight(int xMin1, int xMax1, int xMin2, int xMax2) {
  if (xMin2 < xMax1 && xMin1 < xMin2) {
    return true;
  }
  return false;
}
bool collisionLeft(int xMin1, int xMax1, int xMin2, int xMax2) {
  if (xMin1 < xMax2 && xMax1 > xMax2) {
    return true;
  }
  return false;
}
