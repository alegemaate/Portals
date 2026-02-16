#ifndef PARTICLE_H
#define PARTICLE_H

#define PIXEL 0
#define SQUARE 1
#define CIRCLE 2
#define RANDOM 3

#include <asw/asw.h>
#include "tools.h"

class particle {
 public:
  int x;
  int y;

  asw::Color particleColor;

  int type;
  int size;

  int xMax;
  int xMin;
  int yMin;
  int yMax;

  particle(int newX,
           int newY,
           asw::Color newColor,
           int newXMin,
           int newXMax,
           int newYMin,
           int newYMax,
           int newType,
           int newSize);
  ~particle();

  void logic();
  void draw();

 private:
};

#endif
