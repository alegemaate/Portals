#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <allegro.h>

class gameObject {
 public:
  gameObject(int x, int y);
  virtual ~gameObject();

  int x;
  int y;

  BITMAP* image;

 protected:
 private:
};

#endif  // GAMEOBJECT_H
