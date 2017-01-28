#ifndef AI_H
#define AI_H

#include "Base.hpp"
#include <vector>


namespace samurai_coding_2015 {

class AI {
public:
	virtual ~AI() {}
    virtual void startGame(GameInfo gameInfo) = 0;
    virtual std::vector<int> think(TurnInfo turnInfo, int score) = 0;
	virtual void endGame(TurnInfo turnInfo, double result) = 0;
};

}

#endif  // AI_H
