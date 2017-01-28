#include "players.hpp"
#include <random>
#include <cstdlib>

struct AbortingPlayer: Player {
  default_random_engine generator;
  void play(GameInfo& info) {
    int* addr = 0;
    *addr = 1;
  }
};

Player* player = new AbortingPlayer();
