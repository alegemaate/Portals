#include "tile.h"
#include <loadpng.h>
#include <png.h>
#include <string>

tile::tile() {}

tile::~tile() {
  tileParts.clear();
}

void tile::setValues(int newId,
                     string newType,
                     int newDir,
                     bool newParticlesOn,
                     bool newLightSource,
                     string newName,
                     string newImage1,
                     string newImage2) {
  // ID
  value = newId;

  // Type
  if (newType == "AIR") {
    type = 0;
  } else if (newType == "SOLID") {
    type = 1;
  } else if (newType == "CLIMBABLE") {
    type = 2;
  } else if (newType == "RIDEABLE") {
    type = 3;
  } else if (newType == "DANGER") {
    type = 4;
  } else if (newType == "ENEMY") {
    type = 5;
  } else if (newType == "NCP") {
    type = 6;
  } else if (newType == "PUSHABLE") {
    type = 7;
  } else if (newType == "SWITCH") {
    type = 8;
  } else if (newType == "SWITCH_ON") {
    type = 9;
  } else if (newType == "END") {
    type = 10;
  } else if (newType == "END_ON") {
    type = 11;
  } else {
    allegro_message((("Unknown type in node: type for block ") + name).c_str());
    exit(-1);
  }

  // Image
  if (newImage1 != "") {
    if (!(image[0] = load_png(
              (modFolder + string("/images/blocks/") + newImage1).c_str(),
              NULL))) {
      allegro_message((string("Cannot find image /images/blocks/") + newImage1 +
                       string("\nPlease check your files and try again"))
                          .c_str());
      exit(-1);
    }
  }
  if (newImage2 != "") {
    if (!(image[1] = load_png(
              (modFolder + string("/images/blocks/") + newImage2).c_str(),
              NULL))) {
      allegro_message((string("Cannot find image /images/blocks/") + newImage2 +
                       string("\nPlease check your files and try again"))
                          .c_str());
      exit(-1);
    }
  }

  // Particles
  if (newParticlesOn != true && newParticlesOn != false) {
    allegro_message(
        (("Unknown value in node: newParticlesOn for block ") + name).c_str());
    exit(-1);
  } else {
    particlesOn = newParticlesOn;
  }

  // Lighting
  if (newLightSource != true && newLightSource != false) {
    allegro_message(
        (("Unknown value in node: newLightSource for block ") + name).c_str());
    exit(-1);
  } else {
    lightSource = newLightSource;
  }
}

void tile::logic() {
  // Make particles
  if (particlesOn) {
    if (tileParts.size() < MAXPARTICLES) {
      if (image[0] != NULL) {
        int color = getpixel(image[0], 16, 16);

        if (color != -1) {
          particle newParticle(x + random(0, 40), y + random(0, 40), color, -2,
                               2, -2, 2, CIRCLE, 1);
          tileParts.push_back(newParticle);
        }
      }
    }
    if (random(0, 4) && tileParts.size() > 0) {
      tileParts.erase(tileParts.begin() + (random(0, tileParts.size())));
    }
    for (int partCount = 0; partCount < tileParts.size(); partCount++) {
      tileParts.at(partCount).logic();
    }
  }
}

void tile::draw(BITMAP* temp) {
  // Draw particles
  if (particlesOn) {
    for (int partCount = 0; partCount < tileParts.size(); partCount++) {
      tileParts.at(partCount).draw(temp);
    }
  }
}
