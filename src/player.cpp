#include "player.h"

volatile long player::ani_tick = 0;

player::player() {
  // Assign Variables
  reset(0, 0);
}

void player::load_assets() {
  // Create images
  characterTemp = asw::assets::createTexture(40, 40);

  // Load images
  character = asw::assets::loadTexture(modFolder + "/images/character.png");

  // Load sound effects
  crumble = asw::assets::loadSample(modFolder + "/sfx/crumble.wav");
  teleport = asw::assets::loadSample(modFolder + "/sfx/teleport.wav");
  boxslide = asw::assets::loadSample(modFolder + "/sfx/boxslide.wav");
  electricity = asw::assets::loadSample(modFolder + "/sfx/electricity.wav");
  lever = asw::assets::loadSample(modFolder + "/sfx/lever.wav");
  conveyorBelt = asw::assets::loadSample(modFolder + "/sfx/conveyorBelt.wav");
  wind = asw::assets::loadSample(modFolder + "/sfx/wind.wav");
  splash = asw::assets::loadSample(modFolder + "/sfx/splash.wav");
  oof = asw::assets::loadSample(modFolder + "/sfx/oof.wav");
}

bool player::getWin() {
  return win;
}

bool player::getDead() {
  return dead;
}

int player::getX() {
  return x;
}

int player::getY() {
  return y;
}

// Reset player
void player::reset(int newStartX, int newStartY) {
  // Assign Variables
  characterDir = RIGHT;
  jump_height = 0;
  yVelocity = 0;
  jumping = false;
  jump_height = 0;
  yVelocity = 0;
  characterDir = RIGHT;
  ani_tick = 0;

  x = newStartX;
  y = newStartY;

  portalsActive = true;
  touchingPortal = false;

  dead = false;
  win = false;
}

void player::setCoorinates(int newPortalCoordinates[][2][2]) {
  for (int i = 0; i < 4; i++) {
    for (int t = 0; t < 2; t++) {
      for (int u = 0; u < 2; u++) {
        portalCoordinates[i][t][u] = newPortalCoordinates[i][t][u];
      }
    }
  }
}

void player::logic(tile newTiles[][32]) {
  // Collision stuff
  bool canMoveLeft = true;
  bool canMoveRight = true;
  bool canClimbUp = true;
  bool canClimbDown = true;
  bool canClimbUp2 = false;
  bool canClimbDown2 = false;
  bool canJump = true;
  bool canJumpUp = true;

  // Check for collision
  for (int i = 0; i < 24; i++) {
    for (int t = 0; t < 32; t++) {
      if (newTiles[i][t].type == SOLID || newTiles[i][t].type == RIDEABLE ||
          newTiles[i][t].type == PUSHABLE) {
        if (collisionAny(x + 4, x + 36, t * 40,
                         t * 40 + newTiles[i][t].image[0]->w, y, y + 40, i * 40,
                         i * 40 + newTiles[i][t].image[0]->h) &&
            collisionLeft(x + 4, x + 36, t * 40, t * 40 + 40)) {
          canMoveLeft = false;
        }
      }
      if (newTiles[i][t].type == SOLID || newTiles[i][t].type == RIDEABLE ||
          newTiles[i][t].type == PUSHABLE) {
        if (collisionAny(x + 4, x + 36, t * 40,
                         t * 40 + newTiles[i][t].image[0]->w, y, y + 40, i * 40,
                         i * 40 + newTiles[i][t].image[0]->h) &&
            collisionRight(x + 4, x + 36, t * 40, t * 40 + 40)) {
          canMoveRight = false;
        }
      }
      if (newTiles[i][t].type == CLIMBABLE || newTiles[i][t].type == PUSHABLE) {
        if (collisionAny(x + 10, x + 30, t * 40,
                         t * 40 + newTiles[i][t].image[0]->w, y, y + 40, i * 40,
                         i * 40 + newTiles[i][t].image[0]->h)) {
          canClimbUp2 = true;
        }
      }
      if (newTiles[i][t].type == CLIMBABLE || newTiles[i][t].type == PUSHABLE) {
        if (collisionAny(x + 10, x + 30, t * 40,
                         t * 40 + newTiles[i][t].image[0]->w, y, y + 50, i * 40,
                         i * 40 + newTiles[i][t].image[0]->h)) {
          canClimbDown2 = true;
        }
      }
      if (newTiles[i][t].type == SOLID || newTiles[i][t].type == PUSHABLE) {
        if (collisionAny(x + 10, x + 30, t * 40,
                         t * 40 + newTiles[i][t].image[0]->w, y - 10, y, i * 40,
                         i * 40 + newTiles[i][t].image[0]->h)) {
          canClimbUp = false;
        }
      }
      if (newTiles[i][t].type == SOLID || newTiles[i][t].type == PUSHABLE ||
          newTiles[i][t].type == RIDEABLE) {
        if (collisionAny(x + 10, x + 30, t * 40,
                         t * 40 + newTiles[i][t].image[0]->w, y, y + 50, i * 40,
                         i * 40 + newTiles[i][t].image[0]->h)) {
          canClimbDown = false;
        }
      }
      if (newTiles[i][t].type != AIR && newTiles[i][t].type != DANGER &&
          newTiles[i][t].type != NPC && newTiles[i][t].type != SWITCH &&
          newTiles[i][t].type != SWITCH_ON) {
        if (collisionAny(x + 10, x + 30, t * 40,
                         t * 40 + newTiles[i][t].image[0]->w, y, y + 40, i * 40,
                         i * 40 + newTiles[i][t].image[0]->h)) {
          canJump = false;
        }
      }
      if (newTiles[i][t].type != AIR && newTiles[i][t].type != DANGER &&
          newTiles[i][t].type != NPC && newTiles[i][t].type != SWITCH &&
          newTiles[i][t].type != SWITCH_ON) {
        if (collisionAny(x + 10, x + 30, t * 40,
                         t * 40 + newTiles[i][t].image[0]->w, y - yVelocity,
                         y + 40 - yVelocity, i * 40,
                         i * 40 + newTiles[i][t].image[0]->h) &&
            collisionTop(i * 40, i * 40 + 40, y - yVelocity,
                         y + 40 - yVelocity)) {
          canJumpUp = false;
        }
      }
    }
  }

  // Change in direction
  int delta_x = 0;
  int delta_y = 0;

  // Move up
  if (asw::input::getKey(asw::input::Key::Up) ||
      asw::input::getKey(asw::input::Key::W)) {
    characterDir = UP;
    if (canClimbUp2 && canClimbUp) {
      delta_y = -10;
    }
  }

  // Move down
  else if (asw::input::getKey(asw::input::Key::Down) ||
           asw::input::getKey(asw::input::Key::S)) {
    characterDir = UP;
    if (canClimbDown2 && canClimbDown) {
      delta_y = 10;
    }
  }

  // Move right
  else if (asw::input::getKey(asw::input::Key::Right) ||
           asw::input::getKey(asw::input::Key::D)) {
    characterDir = RIGHT;
    if (newTiles[y / 40][x / 40 + 1].type == PUSHABLE &&
        newTiles[y / 40][x / 40 + 2].value == AIR) {
      newTiles[y / 40][x / 40 + 2].value = newTiles[y / 40][x / 40 + 1].value;
      newTiles[y / 40][x / 40 + 1].value = AIR;
      asw::sound::play(boxslide);
    }
    if (canMoveRight) {
      delta_x = 6;
    }
  }

  // Move left
  else if (asw::input::getKey(asw::input::Key::Left) ||
           asw::input::getKey(asw::input::Key::A)) {
    characterDir = LEFT;
    if (newTiles[y / 40][x / 40].type == PUSHABLE &&
        newTiles[y / 40][x / 40 - 1].value == AIR) {
      newTiles[y / 40][x / 40 - 1].value = newTiles[y / 40][x / 40].value;
      newTiles[y / 40][x / 40].value = AIR;
      asw::sound::play(boxslide);
    }
    if (canMoveLeft) {
      delta_x = -6;
    }
  } else {
    characterDir = NONE;
  }

  // Move
  x += delta_x;
  y += delta_y;

  bool canFall = true;
  bool smoothFall = false;

  // Falling (calculated seperately to ensure collision accurate)
  for (int i = 0; i < 24; i++) {
    for (int t = 0; t < 32; t++) {
      if (newTiles[i][t].type == SOLID || newTiles[i][t].type == CLIMBABLE ||
          newTiles[i][t].type == PUSHABLE) {
        if (collisionAny(x + 10, x + 30, t * 40, t * 40 + 40, y, y + 50, i * 40,
                         i * 40 + 40) &&
            collisionTop(y, y + 50, i * 40, i * 40 + 40)) {
          canFall = false;
          if (!collisionAny(x + 10, x + 30, t * 40, t * 40 + 40, y, y + 41,
                            i * 40, i * 40 + 40) &&
              !collisionTop(y, y + 41, i * 40, i * 40 + 40)) {
            smoothFall = true;
          }
        }
      }
    }
  }

  // Fall
  if (canFall && !jumping) {
    y += 10;
  }
  // Smooth falling
  if (smoothFall) {
    y += 1;
  }

  // Jumping
  if (jumping && canJumpUp) {
    if (canFall || jump_height > 0) {
      y -= yVelocity;
    }

    if (jump_height <= 0) {
      if (yVelocity > -16) {
        yVelocity -= 2;
      } else {
        yVelocity = 0;
        jumping = false;
      }
    } else {
      jump_height -= yVelocity;
    }
  } else if (jumping && !canJumpUp) {
    jumping = false;
  }

  // Jump
  if (asw::input::getKey(asw::input::Key::Space)) {
    if (!canFall && !smoothFall && canJump && !jumping) {
      yVelocity = 16;
      jump_height = 20;
      jumping = true;
    }
  }

  // Check switch
  bool switchOn = false;
  for (int i = 0; i < 24; i++) {
    for (int t = 0; t < 32; t++) {
      if (newTiles[i][t].type == SWITCH_ON) {
        switchOn = true;
      }
    }
  }

  // Fall off world
  if (x < 0 || x > 1280 || y < 0 || y > 920) {
    dead = true;
  }

  touchingPortal = false;

  // Check environment
  for (int i = 0; i < 24; i++) {
    for (int t = 0; t < 32; t++) {
      // Win
      if (collisionAny(x, x + 40, t * 40, t * 40 + 40, y, y + 40, i * 40,
                       i * 40 + 40) &&
          newTiles[i][t].type == END_ON && switchOn == true) {
        asw::sound::play(electricity);
        win = true;
      }

      // Die
      if (collisionAny(x, x + 40, t * 40, t * 40 + 40, y, y + 40, i * 40,
                       i * 40 + 40) &&
          (newTiles[i][t].type == DANGER || newTiles[i][t].type == ENEMY)) {
        if (newTiles[i][t].value == WATER) {
          asw::sound::play(splash);
        } else {
          asw::sound::play(oof);
        }
        dead = true;
      }

      // Fan Movements
      if (collisionAny(x + 10, x + 30, t * 40, t * 40 + 40, y, y + 40, i * 40,
                       i * 40 + 40)) {
        if (newTiles[i][t].value == WIND) {
          asw::sound::play(wind);
          if (canJumpUp) {
            y -= 12;
          }
        }
      }

      // Conveyor Belt Movements
      if (collisionAny(x + 10, x + 30, t * 40, t * 40 + 40, y, y + 50, i * 40,
                       i * 40 + 40)) {
        // Left
        if (newTiles[i][t].value == CONVEYORRIGHT) {
          if (canMoveLeft) {
            asw::sound::play(conveyorBelt);
            x -= 4;
            t += 1;
          }
        }
        // Right
        if (newTiles[i][t].value == CONVEYORLEFT &&
            newTiles[y / 40][x / 40 + 1].type != SOLID) {
          if (canMoveRight) {
            asw::sound::play(conveyorBelt);
            x += 4;
            t += 1;
          }
        }
      }

      // Elevator Movements
      if (newTiles[i][t].value == ELEVATOR1) {
        if (newTiles[i][t].dir == RIGHT) {
          if (y / 40 + 1 == i && x / 40 == t) {
            x += 40;
          }
        }
      }
      if (newTiles[i][t].value == ELEVATOR2) {
        if (newTiles[i][t].dir == LEFT) {
          if (y / 40 + 1 == i && x / 40 == t) {
            x -= 40;
          }
        }
      }

      // Check collisions
      if (collisionAny(x, x + 40, t * 40, t * 40 + 40, y, y + 40, i * 40,
                       i * 40 + 40)) {
        // Teleport
        if (portalsActive) {
          if (newTiles[i][t].value == PORTALR1) {
            y = portalCoordinates[0][1][0] * 40;
            x = portalCoordinates[0][1][1] * 40;
            asw::sound::play(teleport);
            portalsActive = false;
            touchingPortal = true;
          } else if (newTiles[i][t].value == PORTALR2) {
            y = portalCoordinates[0][0][0] * 40;
            x = portalCoordinates[0][0][1] * 40;
            asw::sound::play(teleport);
            portalsActive = false;
            touchingPortal = true;
          } else if (newTiles[i][t].value == PORTALY1) {
            y = portalCoordinates[1][1][0] * 40;
            x = portalCoordinates[1][1][1] * 40;
            asw::sound::play(teleport);
            portalsActive = false;
            touchingPortal = true;
          } else if (newTiles[i][t].value == PORTALY2) {
            y = portalCoordinates[1][0][0] * 40;
            x = portalCoordinates[1][0][1] * 40;
            asw::sound::play(teleport);
            portalsActive = false;
            touchingPortal = true;
          } else if (newTiles[i][t].value == PORTALB1) {
            y = portalCoordinates[2][1][0] * 40;
            x = portalCoordinates[2][1][1] * 40;
            asw::sound::play(teleport);
            portalsActive = false;
            touchingPortal = true;
          } else if (newTiles[i][t].value == PORTALB2) {
            y = portalCoordinates[2][0][0] * 40;
            x = portalCoordinates[2][0][1] * 40;
            asw::sound::play(teleport);
            portalsActive = false;
            touchingPortal = true;
          } else if (newTiles[i][t].value == PORTALG1) {
            y = portalCoordinates[3][1][0] * 40;
            x = portalCoordinates[3][1][1] * 40;
            asw::sound::play(teleport);
            portalsActive = false;
            touchingPortal = true;
          } else if (newTiles[i][t].value == PORTALG2) {
            y = portalCoordinates[3][0][0] * 40;
            x = portalCoordinates[3][0][1] * 40;
            asw::sound::play(teleport);
            portalsActive = false;
            touchingPortal = true;
          }
        }

        if (newTiles[i][t].value == PORTALR1) {
          touchingPortal = true;
        } else if (newTiles[i][t].value == PORTALR2) {
          touchingPortal = true;
        } else if (newTiles[i][t].value == PORTALY1) {
          touchingPortal = true;
        } else if (newTiles[i][t].value == PORTALY2) {
          touchingPortal = true;
        } else if (newTiles[i][t].value == PORTALB1) {
          touchingPortal = true;
        } else if (newTiles[i][t].value == PORTALB2) {
          touchingPortal = true;
        } else if (newTiles[i][t].value == PORTALG1) {
          touchingPortal = true;
        } else if (newTiles[i][t].value == PORTALG2) {
          touchingPortal = true;
        }

        // Power switch
        if (newTiles[i][t].type == SWITCH) {
          newTiles[i][t].value += 1;
          asw::sound::play(lever);
        }
      }
    }
  }
  if (!touchingPortal) {
    portalsActive = true;
  }
}

void player::draw() {
  // Draw character based on direction

  std::cout << characterDir << "\n";
  // if (characterDir == NONE) {
  //   blit(character, characterTemp, ani_tick * 40, 120, 0, 0, 40, 40);
  //   draw_sprite(tempBuffer, characterTemp, x, y);
  // } else if (characterDir == UP) {
  //   blit(character, characterTemp, ani_tick * 40, 80, 0, 0, 40, 40);
  //   draw_sprite(tempBuffer, characterTemp, x, y);
  // } else if (characterDir == LEFT) {
  //   blit(character, characterTemp, ani_tick * 40, 40, 0, 0, 40, 40);
  //   draw_sprite(tempBuffer, characterTemp, x, y);
  // } else if (characterDir == RIGHT) {
  //   blit(character, characterTemp, ani_tick * 40, 0, 0, 0, 40, 40);
  //   draw_sprite(tempBuffer, characterTemp, x, y);
  // }
}
