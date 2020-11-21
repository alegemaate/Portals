#ifndef PARTICLE_H
#define PARTICLE_H

#define PIXEL 0
#define SQUARE 1
#define CIRCLE 2
#define RANDOM 3

#include <allegro.h>
#include "tools.h"

class particle {
 public:
  int x;
  int y;

  int particleColor;
  int particleRed;
  int particleGreen;
  int particleBlue;
  int particleAlpha;

  int type;
  int size;

  int xMax;
  int xMin;
  int yMin;
  int yMax;

  particle(int newX,
           int newY,
           int newColor,
           int newXMin,
           int newXMax,
           int newYMin,
           int newYMax,
           int newType,
           int newSize);
  ~particle();

  void logic();
  void draw(BITMAP* tempBitmap);

 private:
};

#endif
