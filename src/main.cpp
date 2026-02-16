/*
 *  Allan Legemaate
 *  25/02/14
 *  Portals Main
 */

#include <asw/asw.h>

#include <math.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"

#include "id.h"
#include "particle.h"
#include "player.h"
#include "tile.h"
#include "tools.h"

using namespace std;
using namespace rapidxml;

/*
 * IMAGES
 */
// Menu/GUI
asw::Texture levelSelectLeft, levelSelectRight, levelSelectNumber, cursor[2],
    menuselect, menu, menu_edit, help;

// In Game
asw::Texture speech, speech_sign, error, back_mask, editBack;

// Lighting effects
asw::Texture darkness, darkness_old, lightBuffer, spotlight;

/*
 * SOUNDS
 */
// Sounds
asw::Sample click;
asw::Sample crumble;

/*
 * FONTS
 */
// Temporary
asw::Font font;

/*
 * VARIABLES
 */
// Map
int map[24][32];

// Menu
int selectorY, selectorX, newSelectorY, selected_object;
int cursor_x, cursor_y;
int menu_view_x, menu_view_y;
bool menuOpen, editMode;

// In game
int animationStep;
float stepAcc;
constexpr float STEP_TIME = 200.0F;  // Time in ms for each step

int startX, startY;
int scrollX, scrollY;
int levelOn, customLevelOn;
bool customRun, paused;
tile tiles[24][32], backTiles[24][32], allTiles[1000];

// Configuration file
int mouseSpeedX, mouseSpeedY;
bool sound;
bool debugEnabled;
string finalFile;
int currentLevel;

player gameCharacter;

/*
 * FUNCTIONS
 */

// Levels
struct level {
  int map[24][32];
  int backMap[24][32];
  string levelName;
  string fileName[2];
  int levelID;
  bool light;
  bool particles;
  string signText;
  string npcText;

  asw::Texture image[2];
  asw::Music music = nullptr;
  asw::Music ambience = nullptr;
} levels[100], editLevels[100];

/*
 *  UPDATE BLOCKS
 */
// Update Blocks
void resetBlocks(int newI, int newT) {
  int portalCoordinatesTemp[4][2][2];
  for (int i = 0; i < 24; i++) {
    for (int t = 0; t < 32; t++) {
      if (newI != -1 && newT != -1) {
        i = newI;
        t = newT;
      }
      tiles[i][t].x = t * 40;
      tiles[i][t].y = i * 40;

      if (allTiles[tiles[i][t].value].image[0] != NULL) {
        tiles[i][t].image[0] = allTiles[tiles[i][t].value].image[0];
      } else {
        tiles[i][t].image[0] = NULL;
      }
      if (allTiles[tiles[i][t].value].image[1] != NULL) {
        tiles[i][t].image[1] = allTiles[tiles[i][t].value].image[1];
      } else {
        tiles[i][t].image[1] = NULL;
      }
      tiles[i][t].type = allTiles[tiles[i][t].value].type;
      tiles[i][t].particlesOn = allTiles[tiles[i][t].value].particlesOn;
      tiles[i][t].lightSource = allTiles[tiles[i][t].value].lightSource;

      if (tiles[i][t].value == SPAWN) {
        startX = t * 40;
        startY = i * 40;
      } else if (tiles[i][t].value == PORTALR1) {
        portalCoordinatesTemp[0][0][0] = i;
        portalCoordinatesTemp[0][0][1] = t;
      } else if (tiles[i][t].value == PORTALR2) {
        portalCoordinatesTemp[0][1][0] = i;
        portalCoordinatesTemp[0][1][1] = t;
      } else if (tiles[i][t].value == PORTALY1) {
        portalCoordinatesTemp[1][0][0] = i;
        portalCoordinatesTemp[1][0][1] = t;
      } else if (tiles[i][t].value == PORTALY2) {
        portalCoordinatesTemp[1][1][0] = i;
        portalCoordinatesTemp[1][1][1] = t;
      } else if (tiles[i][t].value == PORTALB1) {
        portalCoordinatesTemp[2][0][0] = i;
        portalCoordinatesTemp[2][0][1] = t;
      } else if (tiles[i][t].value == PORTALB2) {
        portalCoordinatesTemp[2][1][0] = i;
        portalCoordinatesTemp[2][1][1] = t;
      } else if (tiles[i][t].value == PORTALG1) {
        portalCoordinatesTemp[3][0][0] = i;
        portalCoordinatesTemp[3][0][1] = t;
      } else if (tiles[i][t].value == PORTALG2) {
        portalCoordinatesTemp[3][1][0] = i;
        portalCoordinatesTemp[3][1][1] = t;
      }

      if (newI != -1 && newT != -1) {
        break;
      }
    }
    if (newI != -1 && newT != -1) {
      break;
    }
  }
  if (newI == -1 || newT == -1) {
    gameCharacter.setCoorinates(portalCoordinatesTemp);
  }
}

void resetBack(int newI, int newT) {
  for (int i = 0; i < 24; i++) {
    for (int t = 0; t < 32; t++) {
      if (newI != -1 && newT != -1) {
        i = newI;
        t = newT;
      }
      backTiles[i][t].x = t * 40;
      backTiles[i][t].y = i * 40;

      if (allTiles[backTiles[i][t].value].image[0] != NULL) {
        backTiles[i][t].image[0] = allTiles[backTiles[i][t].value].image[0];
      } else {
        backTiles[i][t].image[0] = NULL;
      }
      if (allTiles[backTiles[i][t].value].image[1] != NULL) {
        backTiles[i][t].image[1] = allTiles[backTiles[i][t].value].image[1];
      } else {
        backTiles[i][t].image[1] = NULL;
      }
      backTiles[i][t].type = allTiles[backTiles[i][t].value].type;
      backTiles[i][t].particlesOn = allTiles[backTiles[i][t].value].particlesOn;
      backTiles[i][t].lightSource = allTiles[backTiles[i][t].value].lightSource;

      if (newI != -1 && newT != -1) {
        break;
      }
    }
    if (newI != -1 && newT != -1) {
      break;
    }
  }
}

/*
 *  CHANGE MAP
 */
void changeMap() {
  if (customRun || editMode) {
    // Load changes
    ifstream read(modFolder + "/data/" + editLevels[customLevelOn].fileName[0]);
    if (!read.fail()) {
      for (int i = 0; i < 24; i++) {
        for (int t = 0; t < 32; t++) {
          read >> editLevels[customLevelOn].map[i][t];
        }
      }
    } else {
      asw::util::abortOnError("Cannot find map file, " + modFolder + "/data/" +
                              editLevels[customLevelOn].fileName[0]);
    }

    read.close();

    read.open(modFolder + "/data/" + editLevels[customLevelOn].fileName[1]);
    if (!read.fail()) {
      for (int i = 0; i < 24; i++) {
        for (int t = 0; t < 32; t++) {
          read >> editLevels[customLevelOn].backMap[i][t];
        }
      }
    } else {
      asw::util::abortOnError("Cannot find map file, " + modFolder + "/data/" +
                              editLevels[customLevelOn].fileName[1]);
    }
    read.close();

    // Play level music and ambience
    if (levels[levelOn].music) {
      asw::sound::playMusic(levels[levelOn].music);
    }

    if (levels[levelOn].ambience) {
      asw::sound::playMusic(levels[levelOn].ambience);
    }

    // Set map
    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        tiles[i][t].value = editLevels[customLevelOn].map[i][t];
        tiles[i][t].dir = NONE;
      }
    }
    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        backTiles[i][t].value = editLevels[customLevelOn].backMap[i][t];
        backTiles[i][t].dir = NONE;
      }
    }
  } else {
    // Play level music and ambience
    if (levels[levelOn].music) {
      asw::sound::playMusic(levels[levelOn].music);
    }

    if (levels[levelOn].ambience) {
      asw::sound::playMusic(levels[levelOn].ambience);
    }

    // Set map
    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        tiles[i][t].value = levels[levelOn].map[i][t];
        tiles[i][t].dir = NONE;
      }
    }
    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        backTiles[i][t].value = levels[levelOn].backMap[i][t];
        backTiles[i][t].dir = NONE;
      }
    }
  }
  resetBlocks(-1, -1);
  resetBack(-1, -1);
}

/*
 *  SETUP
 */
void setupGame(bool first) {
  if (first) {
    // Variables

    customLevelOn = 1;
    levelOn = 1;
    selected_object = 1;
    customRun = false;
    newSelectorY = 637;
    selectorY = 637;
    selectorX = 60;

    gameCharacter.load_assets();

    // Read config file
    ifstream read(modFolder + "/data/config.txt");
    if (read.fail()) {
      asw::util::abortOnError(
          "Cannot find file /data/config.txt \n Please check your files and "
          "try again");
    }

    string config;
    read >> config;
    if (config == "sound:") {
      read >> config;
      sound = config == "true";
    }
    read >> config;
    if (config == "mouseSpeedX:") {
      read >> config;
      mouseSpeedX = std::stoi(config);
    }
    read >> config;
    if (config == "mouseSpeedY:") {
      read >> config;
      mouseSpeedY = std::stoi(config);
    }
    read >> config;
    if (config == "currentLevel:") {
      read >> config;
      levelOn = std::stoi(config);
    }
    read >> config;
    if (config == "debugEnabled:") {
      read >> config;
      debugEnabled = config == "true";
    }
    read.close();

    // Load images
    lightBuffer = asw::assets::createTexture(1280, 960);

    darkness = asw::assets::loadTexture(modFolder +
                                        "/images/backgrounds/darkness.png");

    darkness_old = asw::assets::loadTexture(modFolder +
                                            "/images/backgrounds/darkness.png");
    spotlight = asw::assets::loadTexture(modFolder + "/images/spotlight.png");
    menu = asw::assets::loadTexture(modFolder + "/images/gui/menu.png");
    menu_edit =
        asw::assets::loadTexture(modFolder + "/images/gui/menu_edit.png");
    menuselect =
        asw::assets::loadTexture(modFolder + "/images/gui/menuSelector.png");
    help = asw::assets::loadTexture(modFolder + "/images/gui/help.png");
    levelSelectLeft =
        asw::assets::loadTexture(modFolder + "/images/gui/levelSelectLeft.png");
    levelSelectRight = asw::assets::loadTexture(
        modFolder + "/images/gui/levelSelectRight.png");
    levelSelectNumber = asw::assets::loadTexture(
        modFolder + "/images/gui/levelSelectNumber.png");
    speech = asw::assets::loadTexture(modFolder + "/images/speech.png");
    speech_sign =
        asw::assets::loadTexture(modFolder + "/images/speech_sign.png");
    error = asw::assets::loadTexture(modFolder + "/images/error.png");
    back_mask = asw::assets::loadTexture(modFolder + "/images/back_mask.png");
    editBack = asw::assets::loadTexture(modFolder +
                                        "/images/backgrounds/editBack.png");

    // Load sound effects
    click = asw::assets::loadSample(modFolder + "/sfx/click.wav");
    crumble = asw::assets::loadSample(modFolder + "/sfx/crumble.wav");

    // TODO Load font

    xml_document<> doc;
    ifstream file(modFolder + "/data/levels.xml");

    // Check exist
    if (file.fail()) {
      asw::util::abortOnError(
          "Cannot find file /data/levels.xml \n Please check your files and "
          "try again");
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string content(buffer.str());
    doc.parse<0>(&content[0]);

    xml_node<>* allLevels = doc.first_node();

    // Load levels
    for (xml_node<>* cLevel = allLevels->first_node("level"); cLevel;
         cLevel = cLevel->next_sibling()) {
      // Read xml variables
      string name = cLevel->first_node("name")->value();
      int id = atoi(cLevel->first_node("id")->value());
      string mapFile = cLevel->first_node("map")->value();
      string backMapFile = cLevel->first_node("backMap")->value();
      bool lightingEnabled = cLevel->first_node("lighting")->value() == "true";
      bool particlesEnabled =
          cLevel->first_node("particles")->value() == "true";
      string backgroundImage = cLevel->first_node("background")->value();
      string foregroundImage = cLevel->first_node("foreground")->value();
      string musicFile = cLevel->first_node("music")->value();
      string ambienceFile = cLevel->first_node("ambience")->value();
      string signTextRead = cLevel->first_node("signText")->value();
      string npcTextRead = cLevel->first_node("npcText")->value();

      // Load map
      levels[id].fileName[0] = mapFile;
      levels[id].fileName[1] = backMapFile;

      levels[id].levelName = name;
      levels[id].levelID = id;

      ifstream read(modFolder + "/data/" + mapFile);
      if (!read.fail()) {
        for (int i = 0; i < 24; i++) {
          for (int t = 0; t < 32; t++) {
            read >> levels[id].map[i][t];
          }
        }
      } else {
        asw::util::abortOnError("Cannot find map file, " + modFolder +
                                "/data/" + mapFile);
      }

      read.close();

      read.open(modFolder + "/data/" + backMapFile);
      if (!read.fail()) {
        for (int i = 0; i < 24; i++) {
          for (int t = 0; t < 32; t++) {
            read >> levels[id].backMap[i][t];
          }
        }
      } else {
        asw::util::abortOnError("Cannot find map file, " + modFolder +
                                "/data/" + backMapFile);
      }

      read.close();

      // Lighting
      if (lightingEnabled != true && lightingEnabled != false) {
        asw::util::abortOnError(
            "Unknown value in node: lightingEnabled for level " + name);
      } else {
        levels[id].light = lightingEnabled;
      }

      // Particles
      if (particlesEnabled != true && particlesEnabled != false) {
        asw::util::abortOnError(
            "Unknown value in node: particlesEnabled for level " + name);
      } else {
        levels[id].particles = particlesEnabled;
      }

      // Background/Foreground
      if (backgroundImage != "") {
        levels[id].image[0] = asw::assets::loadTexture(
            modFolder + string("/images/backgrounds/") + backgroundImage);
      }
      if (foregroundImage != "") {
        levels[id].image[1] = asw::assets::loadTexture(
            modFolder + string("/images/backgrounds/") + foregroundImage);
      }

      // Music/Ambience
      if (musicFile != "") {
        levels[id].music =
            asw::assets::loadMusic(modFolder + "/sfx/music/" + musicFile);
      }
      if (ambienceFile != "") {
        levels[id].ambience =
            asw::assets::loadMusic(modFolder + "/sfx/ambience/" + ambienceFile);
      }

      // Sign Text
      levels[id].signText = signTextRead;

      // NPC Text
      levels[id].npcText = npcTextRead;
    }

    // Read levels
    xml_document<> doc3;
    ifstream file3(modFolder + "/data/editLevels.xml");

    // Check exist
    if (file3.fail()) {
      asw::util::abortOnError(
          "Cannot find file /data/editLevels.xml \n Please check your files "
          "and try again");
    }

    stringstream buffer3;
    buffer3 << file3.rdbuf();
    string content3(buffer3.str());
    doc3.parse<0>(&content3[0]);

    xml_node<>* allEditLevels = doc3.first_node();

    // Load levels
    for (xml_node<>* cLevel = allEditLevels->first_node("level"); cLevel;
         cLevel = cLevel->next_sibling()) {
      // Read xml variables
      string name = cLevel->first_node("name")->value();
      int id = atoi(cLevel->first_node("id")->value());
      string mapFile = cLevel->first_node("map")->value();
      string backMapFile = cLevel->first_node("backMap")->value();
      bool lightingEnabled = cLevel->first_node("lighting")->value() == "true";
      bool particlesEnabled =
          cLevel->first_node("particles")->value() == "true";
      string backgroundImage = cLevel->first_node("background")->value();
      string foregroundImage = cLevel->first_node("foreground")->value();
      string musicFile = cLevel->first_node("music")->value();
      string ambienceFile = cLevel->first_node("ambience")->value();
      string signTextRead = cLevel->first_node("signText")->value();
      string npcTextRead = cLevel->first_node("npcText")->value();

      // Load map
      editLevels[id].levelName = name;
      editLevels[id].levelID = id;

      editLevels[id].fileName[0] = mapFile;
      editLevels[id].fileName[1] = backMapFile;

      ifstream read(modFolder + "/data/" + mapFile);
      if (read.fail()) {
        asw::util::abortOnError("Cannot find map file, " + modFolder +
                                "/data/" + mapFile);
      } else {
        for (int i = 0; i < 24; i++) {
          for (int t = 0; t < 32; t++) {
            read >> editLevels[id].map[i][t];
          }
        }
      }

      read.close();

      read.open(modFolder + "/data/" + backMapFile);
      if (read.fail()) {
        asw::util::abortOnError("Cannot find map file, " + modFolder +
                                "/data/" + backMapFile);
      } else {
        for (int i = 0; i < 24; i++) {
          for (int t = 0; t < 32; t++) {
            read >> editLevels[id].backMap[i][t];
          }
        }
      }

      read.close();

      // Lighting
      if (lightingEnabled != true && lightingEnabled != false) {
        asw::util::abortOnError(
            "Unknown value in node: lightingEnabled for level " + name);
      } else {
        editLevels[id].light = lightingEnabled;
      }

      // Particles
      if (particlesEnabled != true && particlesEnabled != false) {
        asw::util::abortOnError(
            "Unknown value in node: particlesEnabled for level " + name);
      } else {
        editLevels[id].particles = particlesEnabled;
      }

      // Background/Foreground
      if (backgroundImage != "") {
        editLevels[id].image[0] = asw::assets::loadTexture(
            modFolder + "/images/backgrounds/" + backgroundImage);
      }
      if (foregroundImage != "") {
        editLevels[id].image[1] = asw::assets::loadTexture(
            modFolder + "/images/backgrounds/" + foregroundImage);
      }

      // Music/Ambience
      if (musicFile != "") {
        editLevels[id].music =
            asw::assets::loadMusic(modFolder + "/sfx/music/" + musicFile);
      }
      if (ambienceFile != "") {
        editLevels[id].ambience =
            asw::assets::loadMusic(modFolder + "/sfx/ambience/" + ambienceFile);
      }

      // Sign Text
      editLevels[id].signText = signTextRead;

      // NPC Text
      editLevels[id].npcText = npcTextRead;
    }

    // Read blocks
    xml_document<> doc2;
    ifstream file2(modFolder + "/data/blocks.xml");

    // Check exist
    if (file2.fail()) {
      asw::util::abortOnError(
          "Cannot find file /data/blocks.xml \n Please check your files and "
          "try again");
    }

    std::stringstream buffer2;
    buffer2 << file2.rdbuf();
    std::string content2(buffer2.str());
    doc2.parse<0>(&content2[0]);

    xml_node<>* allBlocks = doc2.first_node();

    // Load blocks
    for (xml_node<>* cBlock = allBlocks->first_node("block"); cBlock;
         cBlock = cBlock->next_sibling()) {
      // Read xml variables
      string blockName = cBlock->first_node("name")->value();
      int blockId = atoi(cBlock->first_node("id")->value());
      string blockType = cBlock->first_node("type")->value();
      string blockImage1 = cBlock->first_node("image1")->value();
      string blockImage2 = cBlock->first_node("image2")->value();
      bool blockParticlesEnabled =
          cBlock->first_node("particles")->value() == "true";
      bool blockLightingEnabled =
          cBlock->first_node("lighting")->value() == "true";

      // Setup the blocks
      allTiles[blockId].setValues(blockId, blockType, 0, blockParticlesEnabled,
                                  blockLightingEnabled, blockName, blockImage1,
                                  blockImage2);
    }

    // Setup menu
    menu_view_x = asw::random::between(0, 640);
    menu_view_y = asw::random::between(340, 480);
  }

  // Assign Variables
  cursor_x = 40;
  cursor_y = 40;
  animationStep = 0;
  stepAcc = 0.0F;

  menuOpen = false;

  changeMap();

  gameCharacter.reset(startX, startY);
}

/*
 *  RUN STEP
 */
void stepgo() {
  // Increase Animation Step
  if (animationStep == 0) {
    animationStep += 1;
  } else if (animationStep == 1) {
    animationStep = 0;
  }

  // Update blocks
  for (int i = 0; i < 24; i++) {
    for (int t = 0; t < 32; t++) {
      // Crumble blocks
      if (tiles[i][t].value == BLOCK1 || tiles[i][t].value == BLOCK2 ||
          tiles[i][t].value == BLOCK3 || tiles[i][t].value == BLOCK4 ||
          tiles[i][t].value == BLOCK5 || tiles[i][t].value == BLOCK6) {
        tiles[i][t].value += 1;
      } else if (tiles[i][t].value == BLOCK7) {
        tiles[i][t].value = AIR;
        asw::sound::play(crumble);
      }

      // Elevator Movements
      if (tiles[i][t].value == ELEVATOR1) {
        if (tiles[i][t].dir == NONE) {
          tiles[i][t].dir = LEFT;
          tiles[i][t + 1].dir = LEFT;
        } else if (tiles[i][t].dir == RIGHT) {
          if (tiles[i][t + 2].value == RELEVATOR) {
            tiles[i][t].dir = LEFT;
            tiles[i][t + 1].dir = LEFT;
          } else {
            tiles[i][t].value = AIR;
            tiles[i][t].dir = NONE;
            tiles[i][t + 1].value = ELEVATOR1;
            tiles[i][t + 1].dir = RIGHT;
            tiles[i][t + 2].value = ELEVATOR2;
            tiles[i][t + 2].dir = RIGHT;
            t += 1;
          }
        } else if (tiles[i][t].dir == LEFT) {
          if (tiles[i][t - 1].value == LELEVATOR) {
            tiles[i][t].dir = RIGHT;
            tiles[i][t + 1].dir = RIGHT;
          } else {
            tiles[i][t + 1].value = AIR;
            tiles[i][t + 1].dir = NONE;
            tiles[i][t].value = ELEVATOR2;
            tiles[i][t].dir = LEFT;
            tiles[i][t - 1].value = ELEVATOR1;
            tiles[i][t - 1].dir = LEFT;
          }
        }
      }

      // Enemy Movements
      if (tiles[i][t].type == ENEMY) {
        if (tiles[i][t].dir == NONE) {
          tiles[i][t].dir = LEFT;
        }
        if (tiles[i + 1][t].type == AIR) {
          tiles[i + 1][t].value = tiles[i][t].value;
          tiles[i + 1][t].dir = RIGHT;
          tiles[i][t].value = AIR;
          tiles[i][t].dir = NONE;
          t += 1;
        } else if (tiles[i][t].dir == RIGHT) {
          if (tiles[i][t + 1].type != AIR || tiles[i + 1][t + 1].type == AIR ||
              tiles[i][t + 1].type == ENEMY) {
            tiles[i][t].dir = LEFT;
            tiles[i][t].value -= 1;
          } else {
            tiles[i][t + 1].value = tiles[i][t].value;
            tiles[i][t + 1].dir = RIGHT;
            tiles[i][t].value = AIR;
            tiles[i][t].dir = NONE;
            t += 1;
          }
        } else if (tiles[i][t].dir == LEFT) {
          if (tiles[i][t - 1].type != AIR || tiles[i + 1][t - 1].type == AIR ||
              tiles[i][t - 1].type == ENEMY) {
            tiles[i][t].dir = RIGHT;
            tiles[i][t].value += 1;
          } else {
            tiles[i][t - 1].value = tiles[i][t].value;
            tiles[i][t - 1].dir = LEFT;
            tiles[i][t].value = AIR;
            tiles[i][t].dir = NONE;
          }
        }
      }
    }
  }
}

enum Scenes { Splash, Menu, Editor, Game };

class GameScene : public asw::scene::Scene<Scenes> {
 public:
  using asw::scene::Scene<Scenes>::Scene;

  void init() override { miniMap = asw::assets::createTexture(160, 120); }

  void update(float deltaTime) override {
    // Pause Game
    if (asw::input::getKeyDown(asw::input::Key::P)) {
      paused = !paused;
    }

    if (paused) {
      return;
    }

    Scene::update(deltaTime);

    gameCharacter.logic(tiles);

    // Die
    if (gameCharacter.getDead() || asw::input::getKeyDown(asw::input::Key::K)) {
      setupGame(false);
    }

    // Win
    if (gameCharacter.getWin() == true) {
      // Next map
      if (!customRun) {
        if (levels[levelOn + 1].image) {
          levelOn += 1;
        }
        setupGame(false);
      }
      // Back to menu
      else {
        sceneManager.setNextScene(Scenes::Menu);
        setupGame(false);
      }
    }

    // Check switch
    bool switchOn = false;
    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        if (tiles[i][t].type == SWITCH_ON) {
          switchOn = true;
        }
      }
    }

    // Tile updates
    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        tiles[i][t].logic();

        // Change unpowered portal to powered portal
        if (tiles[i][t].type == END && switchOn == true) {
          tiles[i][t].value += 1;
        }

        // Make wind
        if ((tiles[i][t].value == FAN && tiles[i - 1][t].value == AIR) ||
            (tiles[i][t].value == WIND && tiles[i - 1][t].value == AIR)) {
          if (i > 0) {
            tiles[i - 1][t].value = WIND;
          }
        }

        // Box drop
        if (tiles[i][t].type == PUSHABLE && tiles[i + 1][t].value == AIR) {
          tiles[i + 1][t].value = tiles[i][t].value;
          tiles[i][t].value = AIR;
        }

        // Reset blocks
        if (tiles[i][t].value != map[i][t]) {
          resetBlocks(i, t);
        }
        map[i][t] = tiles[i][t].value;
      }
    }

    // Crumble block
    if (tiles[gameCharacter.getY() / 40 + 1][gameCharacter.getX() / 40].value ==
        BLOCK) {
      tiles[gameCharacter.getY() / 40 + 1][gameCharacter.getX() / 40].value =
          BLOCK1;
    }

    // Goto Menu!
    if (asw::input::getKeyDown(asw::input::Key::M)) {
      sceneManager.setNextScene(Scenes::Menu);
      setupGame(false);
    }

    // Set scroll
    if (gameCharacter.getX() > 320 && gameCharacter.getX() < 960) {
      scrollX = gameCharacter.getX();
    } else if (gameCharacter.getX() <= 320) {
      scrollX = 320;
    } else if (gameCharacter.getX() >= 960) {
      scrollX = 960;
    }
    if (gameCharacter.getY() > 240 && gameCharacter.getY() < 720) {
      scrollY = gameCharacter.getY();
    } else if (gameCharacter.getY() <= 240) {
      scrollY = 240;
    } else if (gameCharacter.getY() >= 720) {
      scrollY = 720;
    }

    // Change step
    stepAcc += deltaTime;
    if (stepAcc > STEP_TIME) {
      stepgo();
      stepAcc -= STEP_TIME;
    }
  }
  void draw() override {
    // Reset darkness
    asw::display::setRenderTarget(darkness);
    asw::draw::sprite(darkness_old, {0, 0});
    asw::display::resetRenderTarget();

    // Background
    if (customRun) {
      asw::draw::sprite(editLevels[customLevelOn].image[0], {0, 0});
    } else {
      asw::draw::sprite(levels[levelOn].image[0], {0, 0});
    }

    // Mini map background
    asw::display::setRenderTarget(miniMap);
    asw::draw::rectFill(asw::Quad<float>(0, 0, miniMap->w, miniMap->h),
                        asw::color::white);
    asw::display::resetRenderTarget();

    // Paralax
    if (customRun) {
      asw::draw::stretchSprite(editLevels[customLevelOn].image[1],
                               {0, 0, 1280, 960});
    } else {
      asw::draw::stretchSprite(
          levels[levelOn].image[1],
          asw::Quad<float>(0 - (scrollX - 320) / 2, 0 + (scrollY - 240) / 2,
                           1280 * 2 - (scrollX - 320) / 2,
                           960 * 2 + (scrollY - 240) / 2));
    }

    // Draw tiles
    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        // Back
        if (backTiles[i][t].image[0]) {
          // Images
          if (backTiles[i][t].image[1]) {
            asw::draw::sprite(
                backTiles[i][t].image[animationStep],
                asw::Vec2<float>(backTiles[i][t].x, backTiles[i][t].y));
          } else if (backTiles[i][t].value != SPAWN) {
            asw::draw::sprite(
                backTiles[i][t].image[0],
                asw::Vec2<float>(backTiles[i][t].x, backTiles[i][t].y));
          }
          asw::draw::sprite(back_mask, asw::Vec2<float>(backTiles[i][t].x,
                                                        backTiles[i][t].y));
          // Light
          // if (backTiles[i][t].lightSource == true) {
          //   draw_sprite(darkness, spotlight, t * 40 + 20 - (spotlight->w /
          //   2),
          //               i * 40 + 20 - (spotlight->h / 2));
          // }
        }

        // Draw/Animate
        if (tiles[i][t].image[0] != NULL) {
          // Mini map
          asw::display::setRenderTarget(miniMap);
          asw::draw::stretchSprite(tiles[i][t].image[0],
                                   asw::Quad<float>(t * 5, i * 5, 5, 5));
          asw::display::resetRenderTarget();

          // Images
          if (tiles[i][t].image[1] != NULL) {
            asw::draw::sprite(tiles[i][t].image[animationStep],
                              asw::Vec2<float>(tiles[i][t].x, tiles[i][t].y));
          } else if (tiles[i][t].value != SPAWN) {
            asw::draw::sprite(tiles[i][t].image[0],
                              asw::Vec2<float>(tiles[i][t].x, tiles[i][t].y));
          }
          // Light
          // if (tiles[i][t].lightSource == true) {
          //   draw_sprite(darkness, spotlight, t * 40 + 20 - (spotlight->w /
          //   2),
          //               i * 40 + 20 - (spotlight->h / 2));
          // }
        }
      }
    }

    // Draw tiles (particles)
    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        tiles[i][t].draw();
      }
    }

    // Draw character
    gameCharacter.draw();

    // Lighting effects
    // if (customRun) {
    //   if (editLevels[customLevelOn].light == true) {
    //     // draw_sprite(darkness, spotlight,
    //     //             gameCharacter.getX() + 20 - (spotlight->w / 2),
    //     //             gameCharacter.getY() + 20 - (spotlight->h / 2));
    //     asw::draw::sprite(darkness, {0, 0});
    //   }
    // } else {
    //   if (levels[levelOn].light == true) {
    //     // draw_sprite(darkness, spotlight,
    //     //             gameCharacter.getX() + 20 - (spotlight->w / 2),
    //     //             gameCharacter.getY() + 20 - (spotlight->h / 2));
    //     asw::draw::sprite(darkness, {0, 0});
    //   }
    // }

    // Sign and NPC speech
    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        // Sign drawing
        if (tiles[i][t].value == SIGN &&
            collisionAny(gameCharacter.getX(), gameCharacter.getX() + 40,
                         t * 40, t * 40 + 40, gameCharacter.getY(),
                         gameCharacter.getY() + 40, i * 40, i * 40 + 40)) {
          asw::Texture tempSign = asw::assets::createTexture(760, 680);
          asw::display::setRenderTarget(tempSign);
          asw::draw::clearColor(asw::color::transparent);
          asw::draw::sprite(speech_sign, {0, 0});

          string allLines = "";
          if (customRun) {
            allLines = string(editLevels[customLevelOn].signText);
          } else {
            allLines = string(levels[levelOn].signText);
          }

          int lineHeight = 100;
          const int maxWidth =
              600;  // 760 texture width - 80px margin each side

          std::string remaining = allLines;
          while (!remaining.empty()) {
            // Find how many characters fit on this line
            std::string line;
            size_t lastSpace = std::string::npos;

            for (size_t i = 0; i < remaining.size(); i++) {
              std::string test = remaining.substr(0, i + 1);
              const auto textSize = asw::util::getTextSize(font, test);

              if (remaining[i] == ' ') {
                lastSpace = i;
              }

              if (textSize.x > maxWidth) {
                // Wrap at the last space if possible
                if (lastSpace != std::string::npos) {
                  line = remaining.substr(0, lastSpace);
                  remaining = remaining.substr(lastSpace + 1);
                } else {
                  // No space found — force break at current position
                  line = remaining.substr(0, i);
                  remaining = remaining.substr(i);
                }
                break;
              }
            }

            // If we got through the whole string without exceeding width
            if (line.empty()) {
              line = remaining;
              remaining.clear();
            }

            const auto textSize = asw::util::getTextSize(font, line);
            asw::draw::text(font, line, asw::Vec2<float>(80, lineHeight),
                            asw::color::black);
            lineHeight += textSize.y;
          }

          asw::display::resetRenderTarget();
          asw::draw::stretchSprite(tempSign,
                                   asw::Quad<float>(160, 200, 760, 680));
        }

        // NPC drawing
        if (tiles[i][t].type == NPC &&
            collisionAny(gameCharacter.getX(), gameCharacter.getX() + 40,
                         t * 40, t * 40 + 40, gameCharacter.getY(),
                         gameCharacter.getY() + 40, i * 40, i * 40 + 40)) {
          asw::Texture tempNpc = asw::assets::createTexture(760, 680);
          asw::display::setRenderTarget(tempNpc);
          asw::draw::clearColor(asw::color::transparent);
          asw::draw::sprite(speech, {0, 0});

          string allLines = "";
          if (customRun) {
            allLines = string(editLevels[customLevelOn].npcText);
          } else {
            allLines = string(levels[levelOn].npcText);
          }

          int lineHeight = 100;
          const int maxWidth =
              600;  // 760 texture width - 80px margin each side

          std::string remaining = allLines;
          while (!remaining.empty()) {
            // Find how many characters fit on this line
            std::string line;
            size_t lastSpace = std::string::npos;

            for (size_t i = 0; i < remaining.size(); i++) {
              std::string test = remaining.substr(0, i + 1);
              const auto textSize = asw::util::getTextSize(font, test);

              if (remaining[i] == ' ') {
                lastSpace = i;
              }

              if (textSize.x > maxWidth) {
                // Wrap at the last space if possible
                if (lastSpace != std::string::npos) {
                  line = remaining.substr(0, lastSpace);
                  remaining = remaining.substr(lastSpace + 1);
                } else {
                  // No space found — force break at current position
                  line = remaining.substr(0, i);
                  remaining = remaining.substr(i);
                }
                break;
              }
            }

            // If we got through the whole string without exceeding width
            if (line.empty()) {
              line = remaining;
              remaining.clear();
            }

            const auto textSize = asw::util::getTextSize(font, line);
            asw::draw::text(font, line, asw::Vec2<float>(80, lineHeight),
                            asw::color::black);
            lineHeight += textSize.y;
          }

          asw::display::resetRenderTarget();
          asw::draw::stretchSprite(tempNpc,
                                   asw::Quad<float>(160, 200, 760, 680));

          asw::draw::stretchSprite(tiles[i][t].image[1],
                                   asw::Quad<float>(160, 200, 160, 160));
        }
      }
    }

    // Lighting
    // circlefill(miniMap, gameCharacter.getX() / 8, gameCharacter.getY() / 8,
    // 3,
    //            makecol(255, 0, 0));
    // set_trans_blender(0, 0, 0, 160);

    // Minimap (top left)
    asw::draw::sprite(miniMap, {0, 0});
  }

 private:
  asw::Texture miniMap;
};

class MenuScene : public asw::scene::Scene<Scenes> {
 public:
  using asw::scene::Scene<Scenes>::Scene;

  void update(float deltaTime) override {
    Scene::update(deltaTime);

    menuOpen = false;

    // Move selector
    if (selectorY != newSelectorY) {
      int selectorVelocity = 0;
      if (newSelectorY != selectorY) {
        selectorVelocity = (newSelectorY - selectorY) / 6;
        if (newSelectorY - selectorY < 0) {
          selectorVelocity -= 1;
        } else if (newSelectorY - selectorY > 0) {
          selectorVelocity += 1;
        }
      }
      selectorY += selectorVelocity;
    }

    if (!editMode) {
      // Hover play
      if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(), 637,
                       637 + 45)) {
        if (newSelectorY != 637) {
          newSelectorY = 637;
          selectorX = 60;
          asw::sound::play(click);
        }
      }
      // Hover edit
      else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                            700, 700 + 45)) {
        if (newSelectorY != 700) {
          newSelectorY = 700;
          selectorX = 60;
          asw::sound::play(click);
        }
      }
      // Hover help
      else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                            763, 763 + 45)) {
        if (newSelectorY != 763) {
          newSelectorY = 763;
          selectorX = 60;
          asw::sound::play(click);
        }
        menuOpen = true;
      }
      // Hover exit
      else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                            828, 828 + 45)) {
        if (newSelectorY != 828) {
          newSelectorY = 828;
          selectorX = 60;
          asw::sound::play(click);
        }
      }

      // Select button
      if (asw::input::getMouseButtonDown(asw::input::MouseButton::Left) ||
          asw::input::getKeyDown(asw::input::Key::Return)) {
        // level select left
        if (collisionAny(mouseX(), mouseX(), 1100, 1140, mouseY(), mouseY(), 80,
                         120)) {
          if (levelOn > 1) {
            // stop_sample(editLevels[levelOn].ambience);
            levelOn -= 1;
            asw::sound::play(click);
            changeMap();
            menu_view_x = asw::random::between(0, 640);
            menu_view_y = asw::random::between(340, 480);
          }
        }
        // level select right
        else if (collisionAny(mouseX(), mouseX(), 1200, 1240, mouseY(),
                              mouseY(), 80, 120)) {
          if (levels[levelOn + 1].image[0] != NULL) {
            // stop_sample(editLevels[levelOn].ambience);
            levelOn += 1;
            asw::sound::play(click);
            changeMap();
            menu_view_x = asw::random::between(0, 640);
            menu_view_y = asw::random::between(340, 480);
          }
        }
        // Start
        else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                              637, 637 + 45)) {
          customRun = false;
          sceneManager.setNextScene(Scenes::Game);
          setupGame(false);
        }
        // Edit
        else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                              700, 700 + 45)) {
          editMode = true;
          customRun = true;
          selectorY = 700;
          newSelectorY = 700;
          setupGame(false);
        }
        // Help
        else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                              763, 763 + 45)) {
        }
        // Quit
        else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                              828, 828 + 45)) {
          asw::core::exit = true;
        }
      }
    } else {
      // Hover edit
      if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(), 700,
                       700 + 45)) {
        if (newSelectorY != 700) {
          newSelectorY = 700;
          selectorX = 60;
          asw::sound::play(click);
        }
      }
      // Hover play
      else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                            763, 763 + 45)) {
        if (newSelectorY != 763) {
          newSelectorY = 763;
          selectorX = 60;
          asw::sound::play(click);
        }
      }
      // Hover back
      else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                            828, 828 + 45)) {
        if (newSelectorY != 828) {
          newSelectorY = 828;
          selectorX = 60;
          asw::sound::play(click);
        }
      }

      // Select button
      if (asw::input::getMouseButtonDown(asw::input::MouseButton::Left) ||
          asw::input::getKeyDown(asw::input::Key::Return)) {
        // level select left
        if (collisionAny(mouseX(), mouseX(), 1100, 1140, mouseY(), mouseY(), 80,
                         120)) {
          if (customLevelOn > 1) {
            // stop_sample(editLevels[customLevelOn].ambience);
            customLevelOn -= 1;
            asw::sound::play(click);
            changeMap();
            menu_view_x = asw::random::between(0, 640);
            menu_view_y = asw::random::between(340, 480);
          }
        }
        // level select right
        else if (collisionAny(mouseX(), mouseX(), 1200, 1240, mouseY(),
                              mouseY(), 80, 120)) {
          if (editLevels[customLevelOn + 1].image[0]) {
            // stop_sample(editLevels[customLevelOn].ambience);
            customLevelOn += 1;
            asw::sound::play(click);
            changeMap();
            menu_view_x = asw::random::between(0, 640);
            menu_view_y = asw::random::between(340, 480);
          }
        }
        // Play
        if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(), 700,
                         700 + 45)) {
          for (int i = 0; i < 24; i++) {
            for (int t = 0; t < 32; t++) {
              tiles[i][t].value = AIR;
            }
          }
          sceneManager.setNextScene(Scenes::Editor);
          setupGame(false);
        }
        // Edit
        else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                              763, 763 + 45)) {
          sceneManager.setNextScene(Scenes::Editor);
          setupGame(false);
        }
        // Back
        else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                              828, 828 + 45)) {
          editMode = false;
          customRun = false;
          newSelectorY = 637;
          selectorY = 637;
          setupGame(false);
        }
      }
    }

    // Live background
    // Tile updates
    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        tiles[i][t].logic();

        // Make wind
        if ((tiles[i][t].value == FAN && tiles[i - 1][t].value == AIR) ||
            (tiles[i][t].value == WIND && tiles[i - 1][t].value == AIR)) {
          if (i > 0) {
            tiles[i - 1][t].value = WIND;
          }
        }

        // Box drop
        if (tiles[i][t].type == PUSHABLE && tiles[i + 1][t].value == AIR) {
          tiles[i + 1][t].value = tiles[i][t].value;
          tiles[i][t].value = AIR;
        }

        // Reset blocks
        if (tiles[i][t].value != map[i][t]) {
          resetBlocks(i, t);
        }
        map[i][t] = tiles[i][t].value;
      }
    }

    // Change step
    stepAcc += deltaTime;
    if (stepAcc > STEP_TIME) {
      stepgo();
      stepAcc -= STEP_TIME;
    }
  }

  void draw() override {
    // Live background
    // Reset darkness
    asw::display::setRenderTarget(darkness);
    asw::draw::sprite(darkness_old, {0, 0});
    asw::display::resetRenderTarget();

    if (!editMode) {
      asw::draw::sprite(levels[levelOn].image[0], {0, 0});
      if (levels[levelOn].image[1] != NULL) {
        asw::draw::sprite(levels[levelOn].image[1], {0, 0});
      }
    } else {
      asw::draw::sprite(editLevels[customLevelOn].image[0], {0, 0});
      if (editLevels[customLevelOn].image[1]) {
        asw::draw::sprite(editLevels[customLevelOn].image[1], {0, 0});
      }
    }

    // Draw tiles
    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        // Back
        if (backTiles[i][t].image[0]) {
          // Images
          if (backTiles[i][t].image[1]) {
            asw::draw::sprite(
                backTiles[i][t].image[animationStep],
                asw::Vec2<float>(backTiles[i][t].x, backTiles[i][t].y));
          } else if (backTiles[i][t].value != SPAWN) {
            asw::draw::sprite(
                backTiles[i][t].image[0],
                asw::Vec2<float>(backTiles[i][t].x, backTiles[i][t].y));
          }
          // draw_trans_sprite(liveBuffer, back_mask, backTiles[i][t].x,
          //                   backTiles[i][t].y);
          // Light
          // if (backTiles[i][t].lightSource == true) {
          //   draw_sprite(darkness, spotlight,
          //               backTiles[i][t].x + 20 - (spotlight->w / 2),
          //               backTiles[i][t].y + 20 - (spotlight->h / 2));
          // }
        }

        // Draw/Animate
        if (tiles[i][t].image[0] != NULL) {
          if (tiles[i][t].image[1] != NULL) {
            asw::draw::sprite(tiles[i][t].image[animationStep],
                              asw::Vec2<float>(tiles[i][t].x, tiles[i][t].y));
          } else if (tiles[i][t].value != SPAWN) {
            asw::draw::sprite(tiles[i][t].image[0],
                              asw::Vec2<float>(tiles[i][t].x, tiles[i][t].y));
          }
          // Light
          // if (tiles[i][t].lightSource == true) {
          //   draw_sprite(darkness, spotlight,
          //               tiles[i][t].x + 20 - (spotlight->w / 2),
          //               tiles[i][t].y + 20 - (spotlight->h / 2));
          // }
        } else {
          asw::draw::sprite(error,
                            asw::Vec2<float>(tiles[i][t].x, tiles[i][t].y));
        }
      }
    }

    // Draw tiles (particles)
    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        tiles[i][t].draw();
      }
    }

    // Lighting effects
    // if (!editMode) {
    //   if (levels[levelOn].light == true) {
    //     asw::draw::sprite(darkness, {0, 0});
    //   }
    // } else {
    //   if (editLevels[customLevelOn].light == true) {
    //     asw::draw::sprite(darkness, {0, 0});
    //   }
    // }

    // // Overlay
    if (editMode) {
      asw::draw::sprite(menu_edit, {0, 0});
    } else {
      asw::draw::sprite(menu, {0, 0});
    }

    asw::draw::sprite(menuselect, asw::Vec2<float>(selectorX, selectorY));

    // Level selection
    asw::draw::sprite(levelSelectLeft, {1100, 80});
    asw::draw::sprite(levelSelectNumber, {1145, 80});
    if (editMode) {
      asw::draw::textCenter(font, std::to_string(customLevelOn), {1168, 73},
                            asw::color::black);
    } else {
      asw::draw::textCenter(font, std::to_string(levelOn), {1168, 73},
                            asw::color::black);
    }
    asw::draw::sprite(levelSelectRight, {1200, 80});

    // Hover select left
    if (asw::Quad<float>(1100, 80, 40, 40)
            .contains(asw::input::mouse.position)) {
      asw::draw::sprite(levelSelectLeft, {1100, 80});
    }
    // Hover select right
    if (asw::Quad<float>(1200, 80, 40, 40)
            .contains(asw::input::mouse.position)) {
      asw::draw::sprite(levelSelectRight, {1200, 80});
    }

    // Select button
    if (asw::input::getMouseButtonDown(asw::input::MouseButton::Left) ||
        asw::input::getKeyDown(asw::input::Key::Return)) {
      if (selectorY == 610) {
        asw::draw::sprite(menu, {0, 0});
        asw::draw::sprite(help, {0, 0});
      }
    }

    if (menuOpen) {
      asw::draw::sprite(help, {0, 0});
    }
  }
};

class EditorScene : public asw::scene::Scene<Scenes> {
 public:
  using asw::scene::Scene<Scenes>::Scene;

  void update(float deltaTime) override {
    Scene::update(deltaTime);

    if (asw::input::getKeyDown(asw::input::Key::S)) {
      tiles[0][0].value = tiles[0][1].value;
      backTiles[0][0].value = backTiles[0][1].value;
      resetBlocks(0, 0);
      resetBack(0, 0);

      // save raw
      ofstream saveRaw(modFolder + "/data/" +
                       editLevels[customLevelOn].fileName[0]);

      for (int i = 0; i < 24; i++) {
        for (int t = 0; t < 32; t++) {
          saveRaw << tiles[i][t].value << " ";
          if (t == 31) {
            saveRaw << "\n";
          }
        }
      }
      saveRaw.close();

      // save raw
      saveRaw.open(modFolder + "/data/" +
                   editLevels[customLevelOn].fileName[1]);

      for (int i = 0; i < 24; i++) {
        for (int t = 0; t < 32; t++) {
          saveRaw << backTiles[i][t].value << " ";
          if (t == 31) {
            saveRaw << "\n";
          }
        }
      }
      saveRaw.close();
    }

    if (asw::input::getKeyDown(asw::input::Key::R)) {
      for (int i = 0; i < 24; i++) {
        for (int t = 0; t < 32; t++) {
          tiles[i][t].value = AIR;
          backTiles[i][t].value = AIR;
          resetBlocks(i, t);
          resetBack(i, t);
        }
      }
    }
    if (asw::input::getKeyDown(asw::input::Key::Up)) {
      if (selected_object < 999) {
        selected_object++;
        tiles[0][0].value = selected_object;
        resetBlocks(0, 0);
        if (tiles[0][0].image[0] == NULL) {
          while (tiles[0][0].image[0] == NULL) {
            selected_object++;
            tiles[0][0].value = selected_object;
            resetBlocks(0, 0);
            if (selected_object >= 999) {
              break;
            }
          }
        }
        // rest(100);
      }
    }
    if (asw::input::getKeyDown(asw::input::Key::Down)) {
      if (selected_object > 1) {
        selected_object--;
        tiles[0][0].value = selected_object;
        resetBlocks(0, 0);
        if (tiles[0][0].image[0] == NULL) {
          while (tiles[0][0].image[0] == NULL) {
            if (selected_object < 2) {
              break;
            }
            selected_object--;
            tiles[0][0].value = selected_object;
            resetBlocks(0, 0);
          }
        }
        // rest(100);
      }
    }

    if (asw::input::getMouseButtonDown(asw::input::MouseButton::Left)) {
      if (asw::input::getKeyDown(asw::input::Key::LShift)) {
        backTiles[cursor_y][cursor_x].value = selected_object;
        resetBack(cursor_y, cursor_x);
      } else {
        tiles[cursor_y][cursor_x].value = selected_object;
        resetBlocks(cursor_y, cursor_x);
      }
    }

    if (asw::input::getMouseButtonDown(asw::input::MouseButton::Right)) {
      if (asw::input::getKeyDown(asw::input::Key::LShift)) {
        backTiles[cursor_y][cursor_x].value = AIR;
        resetBack(cursor_y, cursor_x);
      } else {
        tiles[cursor_y][cursor_x].value = AIR;
        resetBlocks(cursor_y, cursor_x);
      }
    }

    cursor_x = mouseX() / 40;
    cursor_y = mouseY() / 40;
    tiles[0][0].value = selected_object;
    resetBlocks(0, 0);

    if (asw::input::getKeyDown(asw::input::Key::M)) {
      sceneManager.setNextScene(Scenes::Menu);
      setupGame(false);
    }

    // rest(20);
  }

  void draw() override {
    asw::draw::sprite(editBack, {0, 0});
    asw::draw::rectFill({0, 0, 40, 40}, asw::Color(33, 33, 33));

    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        // Back
        if (backTiles[i][t].image[0] != NULL) {
          asw::draw::sprite(
              backTiles[i][t].image[0],
              asw::Vec2<float>(backTiles[i][t].x, backTiles[i][t].y));
          asw::draw::sprite(back_mask, asw::Vec2<float>(backTiles[i][t].x,
                                                        backTiles[i][t].y));
        } else if (backTiles[i][t].value != AIR) {
          asw::draw::sprite(error, asw::Vec2<float>(t * 40, i * 40));
        }

        // Front
        if (tiles[i][t].image[0] != NULL) {
          asw::draw::sprite(tiles[i][t].image[0],
                            asw::Vec2<float>(t * 40, i * 40));
        } else if (tiles[i][t].value != AIR) {
          asw::draw::sprite(error, asw::Vec2<float>(t * 40, i * 40));
        }
      }
    }

    // textprintf_ex(buffer, font, 25, 25, makecol(0, 0, 0), -1,
    // "Selected:%i",
    //               selected_object);

    if (asw::input::getKeyDown(asw::input::Key::S)) {
      asw::draw::textCenter(font,
                            std::format("Map Saved: {}, {}",
                                        editLevels[customLevelOn].fileName[0],
                                        editLevels[customLevelOn].fileName[1]),
                            {640, 20}, asw::color::black);
    }

    if (asw::input::getKeyDown(asw::input::Key::R)) {
      asw::draw::textCenter(font, "Tiles Reset", {640, 20}, asw::color::black);
    }
  }
};

class SplashScene : public asw::scene::Scene<Scenes> {
 public:
  using asw::scene::Scene<Scenes>::Scene;

  void init() override {
    acc = 0;
    intro = asw::assets::loadTexture(modFolder + "/images/intro.png");
  }

  void update(float deltaTime) override {
    Scene::update(deltaTime);

    acc += deltaTime;

    if (acc > 2000.0F || asw::input::keyboard.anyPressed) {
      sceneManager.setNextScene(Scenes::Menu);
    }
  }

  void draw() override { asw::draw::sprite(intro, {0, 0}); }

 private:
  float acc;

  asw::Texture intro;
};

/*
 *  MAIN
 */
int main(int argc, char* argv[]) {
  asw::core::init(1280, 960);
  asw::display::setTitle("Loading Assets");

  font = asw::assets::loadFont("assets/fonts/dosis.ttf", 32);

  if (argc == 1) {
    modFolder = "assets/portals";
  } else {
    modFolder = "assets/" + string(argv[1]);
    // Read config file
    auto modfile = std::ifstream(modFolder + "/data/config.txt");
    if (!modfile.is_open()) {
      asw::util::abortOnError(
          "Cannot find file mod " + modFolder +
          " make sure your config.txt file is available and try again.");
    }
    modfile.close();
  }
  asw::display::setTitle(modFolder);

  // Setup Game
  setupGame(true);

  // Starts Game
  auto app = asw::scene::SceneManager<Scenes>();
  app.registerScene<SplashScene>(Scenes::Splash, app);
  app.registerScene<MenuScene>(Scenes::Menu, app);
  app.registerScene<EditorScene>(Scenes::Editor, app);
  app.registerScene<GameScene>(Scenes::Game, app);
  app.setNextScene(Scenes::Splash);
  app.start();

  return 0;
}
