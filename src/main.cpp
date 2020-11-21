/*
 *  Allan Legemaate
 *  25/02/14
 *  Portals Main
 */

#include <allegro.h>
#include <loadpng.h>
#include <logg.h>

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
// Buffer
BITMAP* buffer;

// Menu/GUI
BITMAP *levelSelectLeft, *levelSelectRight, *levelSelectNumber, *cursor[2],
    *menuselect, *menu, *menu_edit, *intro, *help;
BITMAP* miniMap;

// In Game
BITMAP *speech, *speech_sign, *error, *back_mask, *editBack;

// Lighting effects
COLOR_MAP light_table;
PALLETE pal;
BITMAP *darkness, *darkness_old, *lightBuffer, *spotlight;

/*
 * SOUNDS
 */
// Music
SAMPLE* music_menu;

// Sounds
SAMPLE* click;
SAMPLE* crumble;

/*
 * FONTS
 */
// Temporary
FONT *f1, *f2, *f3, *f4, *f5;

/*
 * VARIABLES
 */
// Close button
volatile int close_button_pressed = FALSE;

// Map
int map[24][32];

// Menu
int selectorY, selectorX, newSelectorY, selected_object;
int cursor_x, cursor_y;
int menu_view_x, menu_view_y;
bool menuOpen, editMode;

// Misc
int gameScreen;

// In game
int animationStep, step;
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

// FPS System
volatile int ticks = 0;
const int updates_per_second = 40;
volatile int game_time = 0;
int fps, frames_done, old_time;

void draw(bool toScreen);

player gameCharacter;

/*
 * FUNCTIONS
 */

// Ticker for FPS
void ticker() {
  ticks++;
}
END_OF_FUNCTION(ticker)

// Game ticker for FPS
void game_time_ticker() {
  game_time++;
}
END_OF_FUNCTION(ticker)

// Close button handler for closing game using [x]
void close_button_handler(void) {
  close_button_pressed = TRUE;
}
END_OF_FUNCTION(close_button_handler)

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

  BITMAP* image[2];
  SAMPLE* music;
  SAMPLE* ambience;
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
  stop_sample(editLevels[customLevelOn].music);
  stop_sample(editLevels[customLevelOn].ambience);

  if (customRun || editMode) {
    // Load changes
    ifstream read(
        (modFolder + "/data/" + editLevels[customLevelOn].fileName[0]).c_str());
    if (fexists((modFolder + "/data/" + editLevels[customLevelOn].fileName[0])
                    .c_str())) {
      for (int i = 0; i < 24; i++) {
        for (int t = 0; t < 32; t++) {
          read >> editLevels[customLevelOn].map[i][t];
        }
      }
    } else {
      abort_on_error(
          (("Cannot find map file, ") +
           (modFolder + "/data/" + editLevels[customLevelOn].fileName[0]))
              .c_str());
    }
    read.close();

    read.open(
        (modFolder + "/data/" + editLevels[customLevelOn].fileName[1]).c_str());
    if (fexists((modFolder + "/data/" + editLevels[customLevelOn].fileName[1])
                    .c_str())) {
      for (int i = 0; i < 24; i++) {
        for (int t = 0; t < 32; t++) {
          read >> editLevels[customLevelOn].backMap[i][t];
        }
      }
    } else {
      abort_on_error(
          (("Cannot find map file, ") +
           (modFolder + "/data/" + editLevels[customLevelOn].fileName[1]))
              .c_str());
    }
    read.close();

    // Play level music and ambience
    if (gameScreen != 2) {
      if (gameScreen == 3) {
        play_sample(editLevels[customLevelOn].music, 100, 127, 1000, 1);
      }
      play_sample(editLevels[customLevelOn].ambience, 100, 127, 1000, 1);
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
    if (gameScreen != 2) {
      if (gameScreen == 3) {
        play_sample(levels[levelOn].music, 100, 127, 1000, 1);
      }
      play_sample(levels[levelOn].ambience, 100, 127, 1000, 1);
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
    srand(time(NULL));

    // Set screen mode
    if (false == true) {
      resDiv = 1;
      if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 1280, 960, 0, 0) != 0) {
        resDiv = 2;
        if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 640, 480, 0, 0) != 0) {
          resDiv = 4;
          if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 320, 240, 0, 0) != 0) {
            set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
            allegro_message("Unable to go into full screen graphic mode\n%s\n",
                            allegro_error);
            exit(1);
          }
        }
      }
    } else {
      resDiv = 1;
      if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 1280, 960, 0, 0) != 0) {
        resDiv = 2;
        if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0) != 0) {
          resDiv = 4;
          if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 320, 240, 0, 0) != 0) {
            set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
            allegro_message("Unable to set any windowed graphic mode\n%s\n",
                            allegro_error);
            exit(1);
          }
        }
      }
    }

    // Setup for FPS system
    LOCK_VARIABLE(ticks);
    LOCK_FUNCTION(ticker);
    install_int_ex(ticker, BPS_TO_TIMER(updates_per_second));

    LOCK_VARIABLE(game_time);
    LOCK_FUNCTION(game_time_ticker);
    install_int_ex(game_time_ticker, BPS_TO_TIMER(10));

    // Close button
    LOCK_FUNCTION(close_button_handler);
    set_close_button_callback(close_button_handler);

    // Variables
    gameScreen = 0;
    customLevelOn = 1;
    levelOn = 1;
    selected_object = 1;
    customRun = false;
    mouse_x = 40;
    mouse_y = 40;
    newSelectorY = 637;
    selectorY = 637;
    selectorX = 60;

    gameCharacter.load_assets();

    // Read config file
    if (fexists((modFolder + "/data/config.txt").c_str())) {
      finalFile = (modFolder + "/data/config.txt").c_str();
    } else {
      abort_on_error(
          "Cannot find file /data/config.txt \n Please check your files and "
          "try again");
    }

    ifstream read(finalFile.c_str());

    string config;
    read >> config;
    if (config == "sound:") {
      read >> config;
      sound = convertStringToBool;
    }
    read >> config;
    if (config == "mouseSpeedX:") {
      read >> config;
      mouseSpeedX = convertStringToInt(config);
    }
    read >> config;
    if (config == "mouseSpeedY:") {
      read >> config;
      mouseSpeedY = convertStringToInt(config);
    }
    read >> config;
    if (config == "currentLevel:") {
      read >> config;
      levelOn = convertStringToInt(config);
    }
    read >> config;
    if (config == "debugEnabled:") {
      read >> config;
      debugEnabled = convertStringToBool(config);
    }
    read.close();

    // init FMOD and allegro sound systems if enabled
    if (sound) {
      install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, ".");
    }

    // Build a color lookup table for lighting effects1280, 960,
    get_palette(pal);
    create_light_table(&light_table, pal, 0, 0, 0, NULL);

    // Mouse acceleration
    set_mouse_speed(mouseSpeedX, mouseSpeedY);

    // Load images
    buffer = create_bitmap(1280, 960);
    lightBuffer = create_bitmap(1280, 960);
    miniMap = create_bitmap(160, 120);
    if (!(darkness = load_bitmap(
              (modFolder + "/images/backgrounds/darkness.png").c_str(),
              NULL))) {
      abort_on_error(
          "Cannot find image /images/backgrounds/darkness.png \n Please check "
          "your files and try again");
    }
    if (!(darkness_old = load_bitmap(
              (modFolder + "/images/backgrounds/darkness.png").c_str(),
              NULL))) {
      abort_on_error(
          "Cannot find image /images/backgrounds/darkness.png \n Please check "
          "your files and try again");
    }
    if (!(spotlight = load_bitmap((modFolder + "/images/spotlight.png").c_str(),
                                  NULL))) {
      abort_on_error(
          "Cannot find image /images/spotlight.png \n Please check your files "
          "and try again");
    }
    if (!(intro =
              load_bitmap((modFolder + "/images/intro.png").c_str(), NULL))) {
      abort_on_error(
          "Cannot find image /images/intro.png \n Please check your files and "
          "try again");
    }
    if (!(menu = load_bitmap((modFolder + "/images/gui/menu.png").c_str(),
                             NULL))) {
      abort_on_error(
          "Cannot find image /images/gui/menu.png \n Please check your files "
          "and try again");
    }
    if (!(menu_edit = load_bitmap(
              (modFolder + "/images/gui/menu_edit.png").c_str(), NULL))) {
      abort_on_error(
          "Cannot find image /images/gui/menu_edit.png \n Please check your "
          "files and try again");
    }
    if (!(menuselect = load_bitmap(
              (modFolder + "/images/gui/menuSelector.png").c_str(), NULL))) {
      abort_on_error(
          "Cannot find image /images/gui/menuSelector.png \n Please check your "
          "files and try again");
    }
    if (!(help = load_bitmap((modFolder + "/images/gui/help.png").c_str(),
                             NULL))) {
      abort_on_error(
          "Cannot find image /images/gui/help.png \n Please check your files "
          "and try again");
    }
    if (!(cursor[0] = load_bitmap(
              (modFolder + "/images/gui/cursor1.png").c_str(), NULL))) {
      abort_on_error(
          "Cannot find image /images/gui/cursor1.png \n Please check your "
          "files and try again");
    }
    if (!(cursor[1] = load_bitmap(
              (modFolder + "/images/gui/cursor2.png").c_str(), NULL))) {
      abort_on_error(
          "Cannot find image /images/gui/cursor2.png \n Please check your "
          "files and try again");
    }
    if (!(levelSelectLeft = load_bitmap(
              (modFolder + "/images/gui/levelSelectLeft.png").c_str(), NULL))) {
      abort_on_error(
          "Cannot find image /images/gui/levelSelectLeft.png \n Please check "
          "your files and try again");
    }
    if (!(levelSelectRight = load_bitmap(
              (modFolder + "/images/gui/levelSelectRight.png").c_str(),
              NULL))) {
      abort_on_error(
          "Cannot find image /images/gui/levelSelectRight.png \n Please check "
          "your files and try again");
    }
    if (!(levelSelectNumber = load_bitmap(
              (modFolder + "/images/gui/levelSelectNumber.png").c_str(),
              NULL))) {
      abort_on_error(
          "Cannot find image /images/gui/levelSelectNumber.png \n Please check "
          "your files and try again");
    }
    if (!(speech =
              load_bitmap((modFolder + "/images/speech.png").c_str(), NULL))) {
      abort_on_error(
          "Cannot find image /images/speech.png \n Please check your files and "
          "try again");
    }
    if (!(speech_sign = load_bitmap(
              (modFolder + "/images/speech_sign.png").c_str(), NULL))) {
      abort_on_error(
          "Cannot find image /images/speech_sign.png \n Please check your "
          "files and try again");
    }
    if (!(error =
              load_bitmap((modFolder + "/images/error.png").c_str(), NULL))) {
      abort_on_error(
          "Cannot find image /images/error.png \n Please check your files and "
          "try again");
    }
    if (!(back_mask = load_bitmap((modFolder + "/images/back_mask.png").c_str(),
                                  NULL))) {
      abort_on_error(
          "Cannot find image /images/back_mask.png \n Please check your files "
          "and try again");
    }
    if (!(editBack = load_bitmap(
              (modFolder + "/images/backgrounds/editBack.png").c_str(),
              NULL))) {
      abort_on_error(
          "Cannot find image /images/backgrounds/editBack.png \n Please check "
          "your files and try again");
    }

    // Load music
    music_menu = load_sample((modFolder + "/sfx/music/Menu.ogg").c_str());

    // Load sound effects
    if (!(click = load_sample((modFolder + "/sfx/click.wav").c_str()))) {
      abort_on_error(
          "Cannot find sound /sfx/click.png \n Please check your files and try "
          "again");
    }
    if (!(crumble = load_sample((modFolder + "/sfx/crumble.wav").c_str()))) {
      abort_on_error(
          "Cannot find sound /sfx/crumble.png \n Please check your files and "
          "try again");
    }

    // Sets Font
    if (!(f1 = load_font((modFolder + "/fonts/arial.pcx").c_str(), NULL,
                         NULL))) {
      abort_on_error(
          "Cannot find font /fonts/arial.png \n Please check your files and "
          "try again");
    }
    f2 = extract_font_range(f1, ' ', 'A' - 1);
    f3 = extract_font_range(f1, 'A', 'Z');
    f4 = extract_font_range(f1, 'Z' + 1, 'z');

    // Merge fonts
    font = merge_fonts(f4, f5 = merge_fonts(f2, f3));

    // Destroy temporary fonts
    destroy_font(f1);
    destroy_font(f2);
    destroy_font(f3);
    destroy_font(f4);
    destroy_font(f5);

    xml_document<> doc;
    ifstream file;

    // Check exist
    if (fexists((modFolder + "/data/levels.xml").c_str())) {
      file.open((modFolder + "/data/levels.xml").c_str());
    } else {
      abort_on_error(
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
      bool lightingEnabled =
          convertStringToBool(cLevel->first_node("lighting")->value());
      bool particlesEnabled =
          convertStringToBool(cLevel->first_node("particles")->value());
      string backgroundImage = cLevel->first_node("background")->value();
      string foregroundImage = cLevel->first_node("foreground")->value();
      string musicFile = cLevel->first_node("music")->value();
      string ambienceFile = cLevel->first_node("ambience")->value();
      string signTextRead = cLevel->first_node("signText")->value();
      string npcTextRead = cLevel->first_node("npcText")->value();

      if (debugEnabled == true) {
        // Draw to screen
        textprintf_centre_ex(screen, font, 640, 700, makecol(0, 0, 0),
                             makecol(255, 255, 255), "Name:%s Id:%i Map:%s",
                             name.c_str(), id, mapFile.c_str());
        textprintf_centre_ex(screen, font, 640, 760, makecol(0, 0, 0),
                             makecol(255, 255, 255), "Image:%s %s Particles:%i",
                             backgroundImage.c_str(), foregroundImage.c_str(),
                             particlesEnabled);
        textprintf_centre_ex(
            screen, font, 640, 820, makecol(0, 0, 0), makecol(255, 255, 255),
            "Lighting:%i Music:%s Ambience:%s", lightingEnabled,
            musicFile.c_str(), ambienceFile.c_str());
        textprintf_centre_ex(screen, font, 640, 880, makecol(0, 0, 0),
                             makecol(255, 255, 255), "Sign:%i Npc:%s",
                             signTextRead.c_str(), npcTextRead.c_str());
      }

      // Load map
      levels[id].fileName[0] = mapFile;
      levels[id].fileName[1] = backMapFile;

      levels[id].levelName = name;
      levels[id].levelID = id;

      ifstream read((modFolder + "/data/" + mapFile).c_str());
      if (fexists((modFolder + "/data/" + mapFile).c_str())) {
        for (int i = 0; i < 24; i++) {
          for (int t = 0; t < 32; t++) {
            read >> levels[id].map[i][t];
          }
        }
      } else {
        abort_on_error(
            (("Cannot find map file, ") + (modFolder + "/data/" + mapFile))
                .c_str());
      }

      read.close();

      read.open((modFolder + "/data/" + backMapFile).c_str());
      if (fexists((modFolder + "/data/" + backMapFile).c_str())) {
        for (int i = 0; i < 24; i++) {
          for (int t = 0; t < 32; t++) {
            read >> levels[id].backMap[i][t];
          }
        }
      } else {
        abort_on_error(
            (("Cannot find map file, ") + ("/data/" + backMapFile)).c_str());
      }

      read.close();

      // Lighting
      if (lightingEnabled != true && lightingEnabled != false) {
        abort_on_error(
            (("Unknown value in node: lightingEnabled for level ") + name)
                .c_str());
      } else {
        levels[id].light = lightingEnabled;
      }

      // Particles
      if (particlesEnabled != true && particlesEnabled != false) {
        abort_on_error(
            (("Unknown value in node: particlesEnabled for level ") + name)
                .c_str());
      } else {
        levels[id].particles = particlesEnabled;
      }

      // Background/Foreground
      if (backgroundImage != "NULL") {
        if (!(levels[id].image[0] = load_bitmap(
                  (modFolder + string("/images/backgrounds/") + backgroundImage)
                      .c_str(),
                  NULL))) {
          abort_on_error((string("Cannot find image images/backgrounds/") +
                          backgroundImage +
                          string("\nPlease check your files and try again"))
                             .c_str());
        }
      }
      if (foregroundImage != "NULL") {
        if (!(levels[id].image[1] = load_bitmap(
                  (modFolder + string("/images/backgrounds/") + foregroundImage)
                      .c_str(),
                  NULL))) {
          abort_on_error((string("Cannot find image images/backgrounds/") +
                          foregroundImage +
                          string("\nPlease check your files and try again"))
                             .c_str());
        }
      }

      // Music/Ambience
      if (musicFile != "NULL") {
        if (!(levels[id].music = load_sample(
                  (modFolder + string("/sfx/music/") + musicFile).c_str()))) {
        }
      }
      if (ambienceFile != "NULL") {
        if (!(levels[id].ambience = load_sample(
                  (modFolder + string("/sfx/ambience/") + ambienceFile)
                      .c_str()))) {
        }
      }

      // Sign Text
      levels[id].signText = signTextRead;

      // NPC Text
      levels[id].npcText = npcTextRead;
    }

    // Read levels
    xml_document<> doc3;
    ifstream file3;

    // Check exist
    if (fexists((modFolder + "/data/editLevels.xml").c_str())) {
      file3.open((modFolder + "/data/editLevels.xml").c_str());
    } else {
      abort_on_error(
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
      bool lightingEnabled =
          convertStringToBool(cLevel->first_node("lighting")->value());
      bool particlesEnabled =
          convertStringToBool(cLevel->first_node("particles")->value());
      string backgroundImage = cLevel->first_node("background")->value();
      string foregroundImage = cLevel->first_node("foreground")->value();
      string musicFile = cLevel->first_node("music")->value();
      string ambienceFile = cLevel->first_node("ambience")->value();
      string signTextRead = cLevel->first_node("signText")->value();
      string npcTextRead = cLevel->first_node("npcText")->value();

      if (debugEnabled == true) {
        // Draw to screen
        textprintf_centre_ex(screen, font, 640, 700, makecol(0, 0, 0),
                             makecol(255, 255, 255), "Name:%s Id:%i Map:%s",
                             name.c_str(), id, mapFile.c_str());
        textprintf_centre_ex(screen, font, 640, 760, makecol(0, 0, 0),
                             makecol(255, 255, 255), "Image:%s %s Particles:%i",
                             backgroundImage.c_str(), foregroundImage.c_str(),
                             particlesEnabled);
        textprintf_centre_ex(
            screen, font, 640, 820, makecol(0, 0, 0), makecol(255, 255, 255),
            "Lighting:%i Music:%s Ambience:%s", lightingEnabled,
            musicFile.c_str(), ambienceFile.c_str());
        textprintf_centre_ex(screen, font, 640, 880, makecol(0, 0, 0),
                             makecol(255, 255, 255), "Sign:%i Npc:%s",
                             signTextRead.c_str(), npcTextRead.c_str());
      }

      // Load map
      editLevels[id].levelName = name;
      editLevels[id].levelID = id;

      editLevels[id].fileName[0] = mapFile;
      editLevels[id].fileName[1] = backMapFile;

      ifstream read((modFolder + "/data/" + mapFile).c_str());
      if (fexists((modFolder + "/data/" + mapFile).c_str())) {
        for (int i = 0; i < 24; i++) {
          for (int t = 0; t < 32; t++) {
            read >> editLevels[id].map[i][t];
          }
        }
      } else {
        abort_on_error(
            (("Cannot find map file, ") + (modFolder + "/data/" + mapFile))
                .c_str());
      }

      read.close();

      read.open((modFolder + "/data/" + backMapFile).c_str());
      if (fexists((modFolder + "/data/" + backMapFile).c_str())) {
        for (int i = 0; i < 24; i++) {
          for (int t = 0; t < 32; t++) {
            read >> editLevels[id].backMap[i][t];
          }
        }
      } else {
        abort_on_error(
            (("Cannot find map file, ") + (modFolder + "/data/" + backMapFile))
                .c_str());
      }

      read.close();

      // Lighting
      if (lightingEnabled != true && lightingEnabled != false) {
        abort_on_error(
            (("Unknown value in node: lightingEnabled for level ") + name)
                .c_str());
      } else {
        editLevels[id].light = lightingEnabled;
      }

      // Particles
      if (particlesEnabled != true && particlesEnabled != false) {
        abort_on_error(
            (("Unknown value in node: particlesEnabled for level ") + name)
                .c_str());
      } else {
        editLevels[id].particles = particlesEnabled;
      }

      // Background/Foreground
      if (backgroundImage != "NULL") {
        if (!(editLevels[id].image[0] = load_bitmap(
                  (modFolder + string("/images/backgrounds/") + backgroundImage)
                      .c_str(),
                  NULL))) {
          abort_on_error((string("Cannot find image images/backgrounds/") +
                          backgroundImage +
                          string("\nPlease check your files and try again"))
                             .c_str());
        }
      }
      if (foregroundImage != "NULL") {
        if (!(editLevels[id].image[1] = load_bitmap(
                  (modFolder + string("/images/backgrounds/") + foregroundImage)
                      .c_str(),
                  NULL))) {
          abort_on_error((string("Cannot find image images/backgrounds/") +
                          foregroundImage +
                          string("\nPlease check your files and try again"))
                             .c_str());
        }
      }

      // Music/Ambience
      if (musicFile != "NULL") {
        if (!(editLevels[id].music = load_sample(
                  (modFolder + string("/sfx/music/") + musicFile).c_str()))) {
          // allegro_message((string("Cannot find music soundtrack sfx/music/")
          // + musicFile + string("\nPlease check your files and try
          // again")).c_str()); exit(-1);
        }
      }
      if (ambienceFile != "NULL") {
        if (!(editLevels[id].ambience = load_sample(
                  (modFolder + string("/sfx/ambience/") + ambienceFile)
                      .c_str()))) {
          // allegro_message((string("Cannot find ambience soundtrack
          // sfx/ambience/") + ambienceFile + string("\nPlease check your files
          // and try again")).c_str()); exit(-1);
        }
      }

      // Sign Text
      editLevels[id].signText = signTextRead;

      // NPC Text
      editLevels[id].npcText = npcTextRead;
    }

    // Read blocks
    xml_document<> doc2;
    ifstream file2;

    // Check exist
    if (fexists((modFolder + "/data/blocks.xml").c_str())) {
      file2.open((modFolder + "/data/blocks.xml").c_str());
    } else {
      abort_on_error(
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
          convertStringToBool(cBlock->first_node("particles")->value());
      bool blockLightingEnabled =
          convertStringToBool(cBlock->first_node("lighting")->value());

      if (debugEnabled == true) {
        // Draw to screen
        textprintf_centre_ex(screen, font, 640, 700, makecol(0, 0, 0),
                             makecol(255, 255, 255), "Name:%s Id:%i Type:%s",
                             blockName.c_str(), blockId, blockType.c_str());
        textprintf_centre_ex(
            screen, font, 640, 760, makecol(0, 0, 0), makecol(255, 255, 255),
            "Image:%s %s Particles:%i Lighting:%i", blockImage1.c_str(),
            blockImage2.c_str(), blockParticlesEnabled, blockLightingEnabled);
      }

      // Setup the blocks
      allTiles[blockId].setValues(blockId, blockType, 0, blockParticlesEnabled,
                                  blockLightingEnabled, blockName, blockImage1,
                                  blockImage2);
    }

    // Setup menu
    menu_view_x = random(0, 640);
    menu_view_y = random(340, 480);
  }

  // Assign Variables
  step = 0;
  cursor_x = 40;
  cursor_y = 40;
  animationStep = 0;

  menuOpen = false;

  changeMap();

  gameCharacter.reset(startX, startY);
}

/*
 *  RUN STEP
 */
void stepgo() {
  step = 0;

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
        play_sample(crumble, 255, 125, random(800, 1200), 0);
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

/*
 *  DO GAME LOGIC
 */
void game() {
  // Get joystick input
  poll_joystick();

  // Splash
  if (gameScreen == 0) {
    draw(false);
    highcolor_fade_in(intro, 16);
    rest(2000);
    highcolor_fade_out(16);
    gameScreen = 1;
    draw(false);
    highcolor_fade_in(buffer, 16);
    draw_trans_sprite(buffer, menu, 0, 0);
  }

  // Menu
  if (gameScreen == 1) {
    menuOpen = false;

    // Move selector
    if (selectorY != newSelectorY) {
      int selectorVelocity;
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
          play_sample(click, 255, 125, 1000, 0);
        }
      }
      // Hover edit
      else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                            700, 700 + 45)) {
        if (newSelectorY != 700) {
          newSelectorY = 700;
          selectorX = 60;
          play_sample(click, 255, 125, 1000, 0);
        }
      }
      // Hover help
      else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                            763, 763 + 45)) {
        if (newSelectorY != 763) {
          newSelectorY = 763;
          selectorX = 60;
          play_sample(click, 255, 125, 1000, 0);
        }
        menuOpen = true;
      }
      // Hover exit
      else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                            828, 828 + 45)) {
        if (newSelectorY != 828) {
          newSelectorY = 828;
          selectorX = 60;
          play_sample(click, 255, 125, 1000, 0);
        }
      }

      // Select button
      if (mouse_b & 1 || key[KEY_ENTER] || joy[0].button[0].b) {
        // level select left
        if (collisionAny(mouseX(), mouseX(), 1100, 1140, mouseY(), mouseY(), 80,
                         120)) {
          if (levelOn > 1) {
            levelOn -= 1;
            play_sample(click, 255, 125, 1000, 0);
            while (mouse_b & 1) {
            }
            changeMap();
            menu_view_x = random(0, 640);
            menu_view_y = random(340, 480);
          }
        }
        // level select right
        else if (collisionAny(mouseX(), mouseX(), 1200, 1240, mouseY(),
                              mouseY(), 80, 120)) {
          if (levels[levelOn + 1].image[0] != NULL) {
            levelOn += 1;
            play_sample(click, 255, 125, 1000, 0);
            while (mouse_b & 1) {
            }
            changeMap();
            menu_view_x = random(0, 640);
            menu_view_y = random(340, 480);
          }
        }
        // Start
        else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                              637, 637 + 45)) {
          customRun = false;
          stop_sample(music_menu);
          highcolor_fade_out(16);
          gameScreen = 3;
          setupGame(false);
        }
        // Edit
        else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                              700, 700 + 45)) {
          editMode = true;
          customRun = true;
          selectorY = 700;
          newSelectorY = 700;
          highcolor_fade_out(16);
          stop_sample(music_menu);
          setupGame(false);
          draw(false);
          highcolor_fade_in(buffer, 16);
        }
        // Help
        else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                              763, 763 + 45)) {
        }
        // Quit
        else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                              828, 828 + 45)) {
          stop_sample(music_menu);
          close_button_pressed = true;
        }
      }
    } else {
      // Hover edit
      if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(), 700,
                       700 + 45)) {
        if (newSelectorY != 700) {
          newSelectorY = 700;
          selectorX = 60;
          play_sample(click, 255, 125, 1000, 0);
        }
      }
      // Hover play
      else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                            763, 763 + 45)) {
        if (newSelectorY != 763) {
          newSelectorY = 763;
          selectorX = 60;
          play_sample(click, 255, 125, 1000, 0);
        }
      }
      // Hover back
      else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                            828, 828 + 45)) {
        if (newSelectorY != 828) {
          newSelectorY = 828;
          selectorX = 60;
          play_sample(click, 255, 125, 1000, 0);
        }
      }

      // Select button
      if (mouse_b & 1 || key[KEY_ENTER] || joy[0].button[0].b) {
        // level select left
        if (collisionAny(mouseX(), mouseX(), 1100, 1140, mouseY(), mouseY(), 80,
                         120)) {
          if (customLevelOn > 1) {
            customLevelOn -= 1;
            play_sample(click, 255, 125, 1000, 0);
            while (mouse_b & 1) {
            }
            changeMap();
            menu_view_x = random(0, 640);
            menu_view_y = random(340, 480);
          }
        }
        // level select right
        else if (collisionAny(mouseX(), mouseX(), 1200, 1240, mouseY(),
                              mouseY(), 80, 120)) {
          if (editLevels[customLevelOn + 1].image[0] != NULL) {
            customLevelOn += 1;
            play_sample(click, 255, 125, 1000, 0);
            while (mouse_b & 1) {
            }
            changeMap();
            menu_view_x = random(0, 640);
            menu_view_y = random(340, 480);
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
          highcolor_fade_out(16);
          gameScreen = 2;
          setupGame(false);
        }
        // Edit
        else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                              763, 763 + 45)) {
          if (mouse_b & 1) {
            draw(false);
            highcolor_fade_out(16);
            gameScreen = 3;
            setupGame(false);
            highcolor_fade_in(buffer, 16);
          }
        }
        // Back
        else if (collisionAny(mouseX(), mouseX(), 60, 270, mouseY(), mouseY(),
                              828, 828 + 45)) {
          editMode = false;
          customRun = false;
          newSelectorY = 637;
          selectorY = 637;
          highcolor_fade_out(16);
          setupGame(false);
          draw(false);
          highcolor_fade_in(buffer, 16);
        }
      }
    }

    // Live background
    // Tile updates
    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        tiles[i][t].logic();

        // Make wind
        if (tiles[i][t].value == FAN && tiles[i - 1][t].value == AIR ||
            tiles[i][t].value == WIND && tiles[i - 1][t].value == AIR) {
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
    step++;
    if (step == 10) {
      stepgo();
    }
  }

  // Level editor
  else if (gameScreen == 2) {
    if (key[KEY_S]) {
      tiles[0][0].value = tiles[0][1].value;
      backTiles[0][0].value = backTiles[0][1].value;
      resetBlocks(0, 0);
      resetBack(0, 0);

      finalFile = modFolder + "/data/" + editLevels[customLevelOn].fileName[0];

      // save raw
      ofstream saveRaw;
      saveRaw.open(finalFile.c_str());

      for (int i = 0; i < 24; i++) {
        for (int t = 0; t < 32; t++) {
          saveRaw << tiles[i][t].value << " ";
          if (t == 31) {
            saveRaw << "\n";
          }
        }
      }
      saveRaw.close();

      finalFile = modFolder + "/data/" + editLevels[customLevelOn].fileName[1];

      // save raw
      saveRaw.open(finalFile.c_str());

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

    if (key[KEY_R]) {
      for (int i = 0; i < 24; i++) {
        for (int t = 0; t < 32; t++) {
          tiles[i][t].value = AIR;
          backTiles[i][t].value = AIR;
          resetBlocks(i, t);
          resetBack(i, t);
        }
      }
    }
    if (key[KEY_UP]) {
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
        rest(100);
      }
    }
    if (key[KEY_DOWN]) {
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
        rest(100);
      }
    }

    if (mouse_b & 1) {
      if (key[KEY_LSHIFT]) {
        backTiles[cursor_y][cursor_x].value = selected_object;
        resetBack(cursor_y, cursor_x);
      } else {
        tiles[cursor_y][cursor_x].value = selected_object;
        resetBlocks(cursor_y, cursor_x);
      }
    }
    if (mouse_b & 2) {
      if (key[KEY_LSHIFT]) {
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

    if (key[KEY_M]) {
      highcolor_fade_out(16);
      gameScreen = 1;
      setupGame(false);
      draw(false);
      highcolor_fade_in(buffer, 16);
    }

    rest(20);
  }

  // Ingame Loops
  if (gameScreen == 3) {
    gameCharacter.logic(tiles);

    // Die
    if (gameCharacter.getDead() == true || key[KEY_K] || joy[0].button[1].b) {
      setupGame(false);
    }

    // Win
    if (gameCharacter.getWin() == true) {
      // Next map
      if (!customRun) {
        highcolor_fade_out(16);
        if (levels[levelOn + 1].image != NULL) {
          levelOn += 1;
        }
        setupGame(false);
      }
      // Back to menu
      else {
        highcolor_fade_out(16);
        gameScreen = 1;
        setupGame(false);
        draw(false);
        highcolor_fade_in(buffer, 16);
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
        if (tiles[i][t].value == FAN && tiles[i - 1][t].value == AIR ||
            tiles[i][t].value == WIND && tiles[i - 1][t].value == AIR) {
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

    // Pause Game
    if (key[KEY_P] || joy[0].button[1].b) {
      while (key[KEY_P] || joy[0].button[1].b) {
      }
      // Draw pause menu
      while (!key[KEY_P] && !key[KEY_ESC] && !joy[0].button[1].b) {
        textout_centre_ex(buffer, font, "Paused press P to resume", 640, 440,
                          makecol(0, 0, 0), makecol(255, 255, 255));
        draw_sprite(screen, buffer, 0, 0);
      }
      while (key[KEY_P] || joy[0].button[1].b) {
      }
    }

    // Goto Menu!
    if (key[KEY_M] || joy[0].button[1].b) {
      highcolor_fade_out(16);
      gameScreen = 1;
      setupGame(false);
      draw(false);
      highcolor_fade_in(buffer, 16);
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
    step++;
    if (step == 10) {
      stepgo();
    }
  }
}

/*
 *  DRAW
 */
void draw(bool toScreen) {
  set_alpha_blender();

  // Splash
  if (gameScreen == 0) {
  }

  // Menu
  if (gameScreen == 1) {
    // Live background
    // Reset darkness
    draw_sprite(darkness, darkness_old, 0, 0);
    BITMAP* liveBuffer = create_bitmap(1280, 960);

    if (!editMode) {
      draw_sprite(liveBuffer, levels[levelOn].image[0], 0, 0);
      if (levels[levelOn].image[1] != NULL) {
        draw_sprite(liveBuffer, levels[levelOn].image[1], 0, 0);
      }
    } else {
      draw_sprite(liveBuffer, editLevels[customLevelOn].image[0], 0, 0);
      if (editLevels[customLevelOn].image[1] != NULL) {
        draw_sprite(liveBuffer, editLevels[customLevelOn].image[1], 0, 0);
      }
    }

    // Draw tiles
    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        // Back
        if (backTiles[i][t].image[0] != NULL) {
          // Images
          if (backTiles[i][t].image[1] != NULL) {
            draw_sprite(liveBuffer, backTiles[i][t].image[animationStep],
                        backTiles[i][t].x, backTiles[i][t].y);
          } else if (backTiles[i][t].value != SPAWN) {
            draw_sprite(liveBuffer, backTiles[i][t].image[0], backTiles[i][t].x,
                        backTiles[i][t].y);
          }
          draw_trans_sprite(liveBuffer, back_mask, backTiles[i][t].x,
                            backTiles[i][t].y);
          // Light
          if (backTiles[i][t].lightSource == true) {
            draw_sprite(darkness, spotlight,
                        backTiles[i][t].x + 20 - (spotlight->w / 2),
                        backTiles[i][t].y + 20 - (spotlight->h / 2));
          }
        }

        // Draw/Animate
        if (tiles[i][t].image[0] != NULL) {
          if (tiles[i][t].image[1] != NULL) {
            draw_sprite(liveBuffer, tiles[i][t].image[animationStep],
                        tiles[i][t].x, tiles[i][t].y);
          } else if (tiles[i][t].value != SPAWN) {
            draw_sprite(liveBuffer, tiles[i][t].image[0], tiles[i][t].x,
                        tiles[i][t].y);
          }
          // Light
          if (tiles[i][t].lightSource == true) {
            draw_sprite(darkness, spotlight,
                        tiles[i][t].x + 20 - (spotlight->w / 2),
                        tiles[i][t].y + 20 - (spotlight->h / 2));
          }
        } else {
          draw_sprite(buffer, error, tiles[i][t].x, tiles[i][t].y);
        }
      }
    }

    // Draw tiles (particles)
    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        tiles[i][t].draw(liveBuffer);
      }
    }

    // Lighting effects
    if (!editMode) {
      if (levels[levelOn].light == true) {
        draw_trans_sprite(liveBuffer, darkness, 0, 0);
      }
    } else {
      if (editLevels[customLevelOn].light == true) {
        draw_trans_sprite(liveBuffer, darkness, 0, 0);
      }
    }

    // Draw background to screen
    stretch_blit(liveBuffer, buffer, menu_view_x, menu_view_y, 640, 480, 0, 0,
                 1280, 960);
    destroy_bitmap(liveBuffer);

    // Overlay
    if (editMode) {
      draw_trans_sprite(buffer, menu_edit, 0, 0);
    } else {
      draw_trans_sprite(buffer, menu, 0, 0);
    }

    draw_trans_sprite(buffer, menuselect, selectorX, selectorY);

    // Level selection
    draw_trans_sprite(buffer, levelSelectLeft, 1100, 80);
    draw_trans_sprite(buffer, levelSelectNumber, 1145, 80);
    if (editMode) {
      textprintf_centre_ex(buffer, font, 1168, 73, makecol(0, 0, 0), -1, "%i",
                           customLevelOn);
    } else {
      textprintf_centre_ex(buffer, font, 1168, 73, makecol(0, 0, 0), -1, "%i",
                           levelOn);
    }
    draw_trans_sprite(buffer, levelSelectRight, 1200, 80);

    // Hover select left
    if (collisionAny(mouseX(), mouseX(), 1100, 1140, mouseY(), mouseY(), 80,
                     120)) {
      draw_trans_sprite(buffer, levelSelectLeft, 1100, 80);
    }
    // Hover select right
    if (collisionAny(mouseX(), mouseX(), 1200, 1240, mouseY(), mouseY(), 80,
                     120)) {
      draw_trans_sprite(buffer, levelSelectRight, 1200, 80);
    }

    // Cursor
    stretch_sprite(buffer, cursor[0], mouseX(), mouseY(), 21 * resDiv,
                   26 * resDiv);

    // Select button
    if (mouse_b & 1 || key[KEY_ENTER] || joy[0].button[0].b) {
      if (selectorY == 610) {
        do {
          draw_sprite(buffer, menu, 0, 0);
          draw_sprite(buffer, help, 0, 0);
          stretch_sprite(buffer, cursor[0], mouseX(), mouseY(), 21 * resDiv,
                         26 * resDiv);
          draw_sprite(screen, buffer, 0, 0);
        } while (!key[KEY_ESC] && !mouse_b & 1 && !joy[0].button[0].b);
      }
    }
    if (menuOpen) {
      draw_trans_sprite(buffer, help, 0, 0);
    }
  }

  // Level editor
  else if (gameScreen == 2) {
    draw_sprite(buffer, editBack, 0, 0);
    rectfill(buffer, 0, 0, 40, 40, makecol(33, 33, 33));

    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        // Back
        if (backTiles[i][t].image[0] != NULL) {
          draw_sprite(buffer, backTiles[i][t].image[0], backTiles[i][t].x,
                      backTiles[i][t].y);
          draw_trans_sprite(buffer, back_mask, backTiles[i][t].x,
                            backTiles[i][t].y);
        } else if (backTiles[i][t].value != AIR) {
          draw_sprite(buffer, error, t * 40, i * 40);
        }

        // Front
        if (tiles[i][t].image[0] != NULL) {
          draw_sprite(buffer, tiles[i][t].image[0], t * 40, i * 40);
        } else if (tiles[i][t].value != AIR) {
          draw_sprite(buffer, error, t * 40, i * 40);
        }
      }
    }

    textprintf_ex(buffer, font, 25, 25, makecol(0, 0, 0), -1, "Selected:%i",
                  selected_object);
    stretch_sprite(buffer, cursor[0], mouseX(), mouseY(), 21 * resDiv,
                   26 * resDiv);

    if (key[KEY_S]) {
      textprintf_centre_ex(buffer, font, 640, 20, makecol(0, 0, 0),
                           makecol(255, 255, 255), "Map Saved: %s, %s",
                           editLevels[customLevelOn].fileName[0].c_str(),
                           editLevels[customLevelOn].fileName[1].c_str());
    }

    if (key[KEY_R]) {
      textout_centre_ex(buffer, font, "Tiles Reset", 640, 20, makecol(0, 0, 0),
                        makecol(255, 255, 255));
    }
  }

  // Ingame Loops
  if (gameScreen == 3) {
    // Reset darkness
    draw_sprite(darkness, darkness_old, 0, 0);

    // Background
    if (customRun) {
      if (editLevels[customLevelOn].image[0] != NULL) {
        draw_sprite(buffer, editLevels[customLevelOn].image[0], 0, 0);
      }
    } else {
      if (levels[levelOn].image[0] != NULL) {
        draw_sprite(buffer, levels[levelOn].image[0], 0, 0);
      }
    }

    // Mini map background
    rectfill(miniMap, 0, 0, miniMap->w, miniMap->h, makecol(255, 255, 255));

    // Paralax
    if (customRun) {
      if (editLevels[customLevelOn].image[1] !=
          NULL) {  // x 320, 960  y 240, 720
        stretch_sprite(buffer, editLevels[customLevelOn].image[1], 0, 0, 1280,
                       960);
      }
    } else {
      if (levels[levelOn].image[1] != NULL) {
        stretch_sprite(buffer, levels[levelOn].image[1],
                       0 - (scrollX - 320) / 2, 0 + (scrollY - 240) / 2,
                       1280 * 2 - (scrollX - 320) / 2,
                       960 * 2 + (scrollY - 240) / 2);
      }
    }

    // Draw tiles
    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        // Back
        if (backTiles[i][t].image[0] != NULL) {
          // Images
          if (backTiles[i][t].image[1] != NULL) {
            draw_sprite(buffer, backTiles[i][t].image[animationStep],
                        backTiles[i][t].x, backTiles[i][t].y);
          } else if (backTiles[i][t].value != SPAWN) {
            draw_sprite(buffer, backTiles[i][t].image[0], backTiles[i][t].x,
                        backTiles[i][t].y);
          }
          draw_trans_sprite(buffer, back_mask, backTiles[i][t].x,
                            backTiles[i][t].y);
          // Light
          if (backTiles[i][t].lightSource == true) {
            draw_sprite(darkness, spotlight, t * 40 + 20 - (spotlight->w / 2),
                        i * 40 + 20 - (spotlight->h / 2));
          }
        }

        // Draw/Animate
        if (tiles[i][t].image[0] != NULL) {
          // Mini map
          stretch_sprite(miniMap, tiles[i][t].image[0], t * 5, i * 5, 5, 5);

          // Images
          if (tiles[i][t].image[1] != NULL) {
            draw_sprite(buffer, tiles[i][t].image[animationStep], tiles[i][t].x,
                        tiles[i][t].y);
          } else if (tiles[i][t].value != SPAWN) {
            draw_sprite(buffer, tiles[i][t].image[0], tiles[i][t].x,
                        tiles[i][t].y);
          }
          // Light
          if (tiles[i][t].lightSource == true) {
            draw_sprite(darkness, spotlight, t * 40 + 20 - (spotlight->w / 2),
                        i * 40 + 20 - (spotlight->h / 2));
          }
        }
      }
    }

    // Draw tiles (particles)
    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        tiles[i][t].draw(buffer);
      }
    }

    // Draw character
    gameCharacter.draw(buffer);

    // Lighting effects
    if (customRun) {
      if (editLevels[customLevelOn].light == true) {
        draw_sprite(darkness, spotlight,
                    gameCharacter.getX() + 20 - (spotlight->w / 2),
                    gameCharacter.getY() + 20 - (spotlight->h / 2));
        draw_trans_sprite(buffer, darkness, 0, 0);
      }
    } else {
      if (levels[levelOn].light == true) {
        draw_sprite(darkness, spotlight,
                    gameCharacter.getX() + 20 - (spotlight->w / 2),
                    gameCharacter.getY() + 20 - (spotlight->h / 2));
        draw_trans_sprite(buffer, darkness, 0, 0);
      }
    }

    // Sign and NPC speech
    for (int i = 0; i < 24; i++) {
      for (int t = 0; t < 32; t++) {
        // Sign drawing
        if (tiles[i][t].value == SIGN &&
            collisionAny(gameCharacter.getX(), gameCharacter.getX() + 40,
                         t * 40, t * 40 + 40, gameCharacter.getY(),
                         gameCharacter.getY() + 40, i * 40, i * 40 + 40)) {
          BITMAP* tempSign;
          tempSign = create_bitmap(760, 680);
          clear_to_color(tempSign, makecol(255, 0, 255));
          draw_sprite(tempSign, speech_sign, 0, 0);
          string allLines;
          if (customRun) {
            allLines = string(editLevels[customLevelOn].signText);
          } else {
            allLines = string(levels[levelOn].signText);
          }
          int lineHeight = 100;
          for (int line = 0; line < allLines.length(); line++) {
            if (text_length(font, (allLines.substr(0, line)).c_str()) > 700) {
              int endCharacter = allLines.substr(0, line).find_last_of(' ');
              textout_justify_ex(tempSign, font,
                                 allLines.substr(0, endCharacter).c_str(), 80,
                                 680, lineHeight, 600, makecol(0, 0, 0), -1);
              lineHeight += text_height(font);
              allLines.erase(0, endCharacter);
              line = 0;
            } else if (text_length(
                           font,
                           (allLines.substr(0, allLines.length())).c_str()) <
                       700) {
              textout_justify_ex(
                  tempSign, font, allLines.substr(0, allLines.length()).c_str(),
                  80, 680, lineHeight, 600, makecol(0, 0, 0), -1);
              allLines.erase(0, allLines.length());
            }
          }
          stretch_sprite(buffer, tempSign, scrollX - 160, scrollY - 200, 380,
                         340);
          destroy_bitmap(tempSign);
        }

        // NPC drawing
        if (tiles[i][t].type == NPC &&
            collisionAny(gameCharacter.getX(), gameCharacter.getX() + 40,
                         t * 40, t * 40 + 40, gameCharacter.getY(),
                         gameCharacter.getY() + 40, i * 40, i * 40 + 40)) {
          BITMAP* tempNpc;
          tempNpc = create_bitmap(760, 680);
          clear_to_color(tempNpc, makecol(255, 0, 255));
          draw_sprite(tempNpc, speech, 0, 0);
          string allLines;
          if (customRun) {
            allLines = string(editLevels[customLevelOn].npcText);
          } else {
            allLines = string(levels[levelOn].npcText);
          }
          int lineHeight = 100;
          for (int line = 0; line < allLines.length(); line++) {
            if (text_length(font, (allLines.substr(0, line)).c_str()) > 700) {
              int endCharacter = allLines.substr(0, line).find_last_of(' ');
              textout_justify_ex(tempNpc, font,
                                 allLines.substr(0, endCharacter).c_str(), 80,
                                 680, lineHeight, 600, makecol(0, 0, 0), -1);
              lineHeight += text_height(font);
              allLines.erase(0, endCharacter);
              line = 0;
            } else if (text_length(
                           font,
                           (allLines.substr(0, allLines.length())).c_str()) <
                       700) {
              textout_justify_ex(
                  tempNpc, font, allLines.substr(0, allLines.length()).c_str(),
                  80, 680, lineHeight, 600, makecol(0, 0, 0), -1);
              allLines.erase(0, allLines.length());
            }
          }
          stretch_sprite(buffer, tempNpc, scrollX - 160, scrollY - 200, 380,
                         340);
          stretch_sprite(buffer, tiles[i][t].image[1], scrollX - 180,
                         scrollY + 80, 80, 80);
          destroy_bitmap(tempNpc);
        }
      }
    }

    // Lighting
    circlefill(miniMap, gameCharacter.getX() / 8, gameCharacter.getY() / 8, 3,
               makecol(255, 0, 0));
    set_trans_blender(0, 0, 0, 160);

    // Minimap
    draw_trans_sprite(buffer, miniMap, scrollX - 300, scrollY - 225);
  }

  if (toScreen) {
    // Draw buffer
    if (gameScreen == 3) {
      stretch_sprite(screen, buffer, -scrollX * 2 / resDiv + (SCREEN_W / 2),
                     -scrollY * 2 / resDiv + (SCREEN_H / 2), SCREEN_W * 2,
                     SCREEN_H * 2);
    } else {
      stretch_sprite(screen, buffer, 0, 0, SCREEN_W, SCREEN_H);
    }
  }

  if (debugEnabled) {
    textprintf_ex(screen, font, 0, 0, makecol(0, 0, 0), makecol(255, 255, 255),
                  "FPS:%i", fps);
  }
}

/*
 *  CLEAN UP
 */
void exit() {
  // Save config
  /*ifstream saveFile;
  string write;
  saveFile.open("data/config.txt");

  while (true) {
    saveFile >> write;
    cerr << write << endl;
    if( write == "currentLevel:"){
      cerr << write << endl;

    }
    if( saveFile.eof() ) break;
  }

  saveFile.close();*/

  // Destory Bitmaps
  destroy_bitmap(intro);
  destroy_bitmap(buffer);
  destroy_bitmap(menu);
  destroy_bitmap(menuselect);
  destroy_bitmap(help);
  destroy_bitmap(speech);
  destroy_bitmap(speech_sign);
  destroy_bitmap(error);
  destroy_bitmap(cursor[0]);
  destroy_bitmap(cursor[1]);
  destroy_bitmap(levelSelectLeft);
  destroy_bitmap(levelSelectRight);
  destroy_bitmap(levelSelectNumber);
  destroy_bitmap(darkness);
  destroy_bitmap(darkness_old);
  destroy_bitmap(lightBuffer);
  destroy_bitmap(spotlight);
  destroy_bitmap(miniMap);

  // Destory Samples
  destroy_sample(click);
  destroy_sample(crumble);
}

/*
 *  MAIN
 */
int main(int argc, char* argv[]) {
  // Initializing
  allegro_init();
  install_timer();
  install_keyboard();
  install_mouse();

  set_color_depth(32);

  install_joystick(JOY_TYPE_AUTODETECT);
  set_window_title("Loading Assets");
  int old_ticks = ticks;

  if (argc == 1) {
    modFolder = "portals";
  } else {
    modFolder = argv[1];
    // Read config file
    if (!fexists((modFolder + "/data/config.txt").c_str())) {
      abort_on_error(
          ("Cannot find file mod " + modFolder +
           " make sure your config.txt file is available and try again.")
              .c_str());
    }
  }
  set_window_title(modFolder.c_str());

  // Setup Game
  setupGame(true);

  while (!key[KEY_ESC] && !close_button_pressed && !joy[0].button[4].b) {
    while (ticks == 0) {
      rest(1);
    }
    while (ticks > 0) {
      int old_ticks = ticks;

      game();

      ticks--;
      if (old_ticks <= ticks) {
        break;
      }
    }
    if (game_time - old_time >= 10) {
      fps = frames_done;
      frames_done = 0;
      old_time = game_time;
    }
    draw(true);
    frames_done++;
  }

  exit();
  return 0;
}
END_OF_MAIN()
