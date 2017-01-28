#include "players.hpp"
#include <random>
#include <thread>

struct TimeoutPlayer: Player {
  default_random_engine generator;
  void play(GameInfo& info) {
    if (info.turn >= 13) {
      this_thread::sleep_for(chrono::seconds(10));
    }
    int power = 7;
    while (power >= 2) {
      int action = generator()/1024%10 + 1;
      static const int required[] = {0, 4, 4, 4, 4, 2, 2, 2, 2, 1, 1}; 
      if (required[action] <= power && info.isValid(action)) {
	power -= required[action];
	info.doAction(action);
      }
    }
    if (power != 0) {
      if (generator()/1024%11>8) {
	int action = (info.samuraiInfo[info.weapon].hidden ? 10 : 9);
	if (info.isValid(action)) {
	  info.doAction(action);
	}
      }
    }
  }
};

Player* player = new TimeoutPlayer();
