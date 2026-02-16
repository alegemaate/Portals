#ifndef PLAYER_H
#define PLAYER_H

#include <asw/asw.h>
#include <iostream>

#include "id.h"
#include "tile.h"
#include "tools.h"

class player {
 public:
  player();

  void load_assets();
  void logic(tile newTiles[][32]);
  void draw();
  void reset(int newStartX, int newStartY);
  void setCoorinates(int newPortalCoordinates[][2][2]);

  bool getWin();
  bool getDead();

  int getX();
  int getY();

 protected:
 private:
  // Variables
  int x, y, jumping, jump_height, yVelocity, characterDir;
  bool portalsActive, touchingPortal;
  int portalCoordinates[4][2][2];

  bool dead, win;

  static void ani_ticker();
  static volatile long ani_tick;

  // Images
  asw::Texture character;
  asw::Texture characterTemp;
  asw::Texture cat;

  // Block sounds
  asw::Sample crumble;
  asw::Sample teleport;
  asw::Sample boxslide;
  asw::Sample electricity;
  asw::Sample lever;
  asw::Sample conveyorBelt;
  asw::Sample wind;
  asw::Sample splash;

  // Character sounds
  asw::Sample fallLeaves;
  asw::Sample fallWood;
  asw::Sample oof;
};

#endif  // PLAYER_H
