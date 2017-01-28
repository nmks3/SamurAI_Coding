#include <iostream>
#include <cstring>
#include <list>

using namespace std;

struct CommentedIStream {
  istream* is;
  char commentChar = '#';
  CommentedIStream() {};
  CommentedIStream(istream &is);
  int get();
  void skipComments();
};

CommentedIStream& operator>>(CommentedIStream& cs, int& i);

void rotate(int direction, int x0, int y0, int& x, int& y);

struct SamuraiInfo {
  int homeX, homeY;
  int rank, score;
  int curX, curY;
  bool alive;
  int hidden;
  void homePosition(CommentedIStream& is);
  void readScoreInfo(CommentedIStream& is);
  void readTurnInfo(CommentedIStream& is);
};

struct FieldUndo {
  int *section;
  int state;
  FieldUndo(int* sect);
  void apply();
};

struct SamuraiUndo {
  SamuraiInfo* si;
  int x, y;
  int hidden;
  SamuraiUndo(SamuraiInfo* si);
  void apply();
};

struct Undo {
  list<FieldUndo> fieldUndo;
  list<SamuraiUndo> samuraiUndo;
  Undo() {};
  void recField(int* sect);
  void recSamurai(SamuraiInfo* si);
  void apply();
};

struct GameInfo {
  // Game information
  int turns;
  int side;
  int weapon;
  int width, height;
  int cureTurns;
  SamuraiInfo samuraiInfo[6];
  // Turn information
  int turn, curePeriod;
  int* field;
  GameInfo(CommentedIStream& is);
  void readTurnInfo(CommentedIStream& is);
  bool isValid(int action) const;
  bool isValidAt(int action, int x, int y, int hidden) const;
  void occupy(int direction);
  void tryAction(int action, Undo& undo,
		 int& territory, int& selfTerritory,
		 int& injury, int& hiding);
  void doAction(int action);
};

struct Player {
  virtual void play(GameInfo& info) = 0;
};

extern bool logging;
extern Player* player;
