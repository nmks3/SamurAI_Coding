#include "players.hpp"
#include <list>
#include <utility>

extern double territoryMerits;
extern double selfTerritoryMerits;
extern double hurtingMerits;
extern double hidingMerits;

list<int> bestPlay;
list<int> currentPlay;
double bestMerits;

struct PlanningPlayer: Player {
  void plan(GameInfo& info, SamuraiInfo& me, int power, double merits) {
    if (merits > bestMerits) {
      bestMerits = merits;
      bestPlay = currentPlay;
    }
    static const int required[] = {0, 4, 4, 4, 4, 2, 2, 2, 2, 1, 1}; 
    for (int action = 1; action != 11; action++) {
      if (required[action] <= power &&
	  info.isValidAt(action, me.curX, me.curY, me.hidden)) {
	currentPlay.push_back(action);
	Undo undo;
	int territory, selfTerritory, injury, hiding;
	info.tryAction(action, undo, territory, selfTerritory, injury, hiding);
	double gain = territoryMerits*territory
	  + selfTerritoryMerits*selfTerritory
	  + hurtingMerits*injury
	  + hidingMerits*hiding;
	plan(info, me, power-required[action], merits+gain);
	undo.apply();
	currentPlay.pop_back();
      }
    }
  }
  void play(GameInfo& info) {
    currentPlay.clear();
    bestMerits = -1;
    plan(info, info.samuraiInfo[info.weapon], 7, 0);
    for (int action: bestPlay) {
      cout << action << ' ';
    }
  }
};

Player* player = new PlanningPlayer();
