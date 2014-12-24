#ifndef TILE_H
#define TILE_H

#include <allegro.h>
#include <string>
#include <vector>
#include "id.h"
#include "particle.h"
#include "tools.h"

#define MAXPARTICLES 20

using namespace std;

class tile
{
  public:
    tile();
    virtual ~tile();

    void setValues( int newId, string newType, int newDir, bool newParticlesOn, bool newLightSource, string newName, string newImage1, string newImage2);
    void logic();
    void draw( BITMAP *temp);

    BITMAP* image[2];
    string name;
    int value;
    int type;
    int dir;
    int x;
    int y;
    bool particlesOn;
    bool lightSource;
    vector <particle> tileParts;

  protected:

  private:
};

#endif // TILE_H
