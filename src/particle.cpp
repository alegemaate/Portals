#include "particle.h"

#include <asw/asw.h>

// Constructor
particle::particle(int newX,
                   int newY,
                   asw::Color newColor,
                   int newXMin,
                   int newXMax,
                   int newYMin,
                   int newYMax,
                   int newType,
                   int newSize) {
  x = newX;
  y = newY;

  particleColor = newColor;

  type = newType;
  size = newSize;

  xMin = newXMin;
  xMax = newXMax;
  yMin = newYMin;
  yMax = newYMax;
}

// Deconstructor
particle::~particle() {}

// Logic
void particle::logic() {
  x += asw::random::between(xMin, xMax);
  y += asw::random::between(yMin, yMax);

  // Change color
  if (particleColor.r > 0) {
    particleColor.r -= 1;
  }
  if (particleColor.g > 0) {
    particleColor.g -= 1;
  }
  if (particleColor.b > 0) {
    particleColor.b -= 1;
  }
}

// Draw
void particle::draw() {
  auto pos = asw::Vec2<float>(x, y);
  auto quad = asw::Quad<float>(pos, asw::Vec2<float>(size, size));

  if (type == PIXEL) {
    asw::draw::point(pos, particleColor);
  } else if (type == SQUARE) {
    asw::draw::rectFill(quad, particleColor);
  } else if (type == CIRCLE) {
    asw::draw::circleFill(pos, size, particleColor);
  } else if (type == RANDOM) {
    switch (asw::random::between(0, 3)) {
      case 0:
        asw::draw::point(pos, particleColor);
        break;
      case 1:
        asw::draw::circleFill(pos, size, particleColor);
        break;
      case 2:
        asw::draw::rectFill(quad, particleColor);
        break;
    }
  }
}
