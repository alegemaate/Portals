#ifndef PLAYER_H
#define PLAYER_H

#include <allegro.h>

#include <iostream>
#include "id.h"
#include "tile.h"
#include "tools.h"

class player
{
  public:
    player();
    virtual ~player();

    void load_assets();
    void logic( tile newTiles[][32]);
    void draw( BITMAP *tempBuffer);
    void reset( int newStartX, int newStartY);
    void setCoorinates( int newPortalCoordinates[][2][2]);

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
    BITMAP *character;
    BITMAP *characterTemp;
    BITMAP *cat;

    // Block sounds
    SAMPLE *crumble, *teleport, *boxslide, *electricity, *lever, *conveyorBelt, *wind, *splash;

    // Character sounds
    SAMPLE *fallLeaves, *fallWood, *oof;

};

#endif // PLAYER_H
