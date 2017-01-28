#include <cstdio>
#include <unordered_map>
#include <queue>
#include <limits>
#include <algorithm>
#include <cassert>
#include <random>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

struct SamuraiScanner {
  FILE* input;
  char commentChar;
  void skipComments();
  SamuraiScanner();
  SamuraiScanner(FILE* in);
  int get();
  string gets();
};

struct ErrorReport {
  string msg;
  ErrorReport(string s);
};

struct Coordinates {
  const int x;
  const int y;
  Coordinates(int x, int y);
  Coordinates(const int xy[]);
  Coordinates north() const;
  Coordinates south() const;
  Coordinates east() const;
  Coordinates west() const;
  Coordinates operator+(const Coordinates c) const;
  Coordinates rotate(const int direction) const;
  string toString() const;
  bool operator==(const Coordinates another) const;
};

struct CoordHash {
  size_t operator()(const Coordinates& c) const;
};

struct Section;

struct FieldMap: unordered_map <Coordinates, Section*, CoordHash> {
  Section* locate(Coordinates p);
};

struct SamuraiState;
struct GameState;
struct Role;

struct Section {
  const Coordinates coords;
  int state; // -1: free; s>=0: occupied by samurai s
  int population;
  SamuraiState* apparent;
  Section *neighbors[4];
  Section(Coordinates coords);
  void setNeighbors(FieldMap& map);
  void leave(SamuraiState* s);
  void arrive(SamuraiState* s);
  void occupy(Role& role);
};

struct BattleField {
  const int width = 15;
  const int height = 15;
  FieldMap map;
  BattleField();
  Section* section(int x, int y);
  void occupy(GameState& state, Role& role, int direction, Section& pos);
};

struct Role {
  int id;
  vector <Coordinates> reach;
  const int vision = 5;
  const int activity = 7;
  int homeX;
  int homeY;
  void init(int id);
};

const Coordinates homePositions[6] = {
  Coordinates( 0, 5),
  Coordinates( 0, 14),
  Coordinates( 9, 14),
  Coordinates(14, 9),
  Coordinates(14, 0),
  Coordinates( 5, 0)
};

const int weaponReachSize[3] = { 4, 5, 7 };
const int weaponReach[3][7][2] = {
  // Spear
  { {0, 1}, {0, 2}, {0, 3}, {0, 4} },
  // Swords
  { {0, 1}, {0, 2}, {1, 0}, {1, 1}, {2, 0} },
  // Battleax
  { {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, -1}, {1, 0}, {1, 1} }
};

struct Setting {
  const string gameName = string("SamurAI 3x3");
  const int turns = 192;
  const int initTimeAllowed = 5000;
  const int timeAllowed = 100;
  const int cureTurns = 20;
  const int armySize = 3;
  Role roles[6];
  void sendSettingInfo(FILE* os, const int weapon);
  Setting();
};

struct SamuraiState {
  Role* role;
  int side;
  int weapon;
  Section* position;
  int curePeriod;
  bool hidden;
  pid_t processId;
  string invokeCommand;
  string pauseCommand;
  string resumeCommand;
  string nickname = "noname";
  int score = 0;
  int rank = 0;
  FILE* comlog = nullptr;
  FILE* toAI = nullptr;
  SamuraiScanner fromAI;
  bool alive;
  int response;
  bool done;
  queue<int> actions;
  void init(Setting& setting, BattleField& field, int id);
  bool move(GameState& state, int direction, string& comments);
  bool occupy(GameState& state, int direction, string& comments);
  bool hide(string& comments);
  bool appear(string& comments);
  void cure();
  void house(BattleField& field);
  void die(BattleField& field);
  void injure(BattleField& field, Setting& setting);
};
  
extern SamuraiState samuraiStates[6];

struct GameState {
  Setting setting;
  BattleField battleField;
  int turn;
  void init();
  void sendGameInfo(FILE* os, const int id);
  void sendTurnInfo(FILE* os, const int side, const int weapon);
  void receiveActionCommands
  (SamuraiScanner& is, const int side, const int weapon, FILE* log);
};

void receiveFromAI(SamuraiScanner& is, const Setting& setting,
		   GameState& gameState, const int side, const int weapon,
		   FILE* log, int turn);

extern FILE* dump;
extern FILE** ifs;
