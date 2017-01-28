#ifndef MY_AI_H
#define MY_AI_H

#include "Base.hpp"
#include "AI.hpp"

#include <vector>
#include <set>
#include <array>

namespace samurai_coding_2015 {

	struct Move_old {
		Position pos;
		int state;
		int cost;
		vector<Position> attack;
		vector<int> actions;

		bool operator<(const Move_old &m) const {
			if (pos != m.pos) return pos < m.pos;
			if (state != m.state) return state < m.state;
			if (attack != m.attack) return attack < m.attack;
//			return cost < m.cost;
			return 0;
		}
	};

	class MyAI : public AI {
		GameInfo m_gameInfo;
		TurnInfo m_turnInfo;
		array<int, 3> m_nextMoveCount;

		bool isStart(const Position &q) const;
		set<Move_old> extractMove(int id) const;
		set<Move_old> extractMove(const int id, const Samurai &st0) const;
		int evaluate0(const Move_old &m, const int id) const;
		bool canAttack(const Position &target, const SamuraiState &st, int idx, int moveTimes) const;

		template <bool scoreDump = false>
		int evaluate(const Move_old &m, const int id) const;
		// int evaluate(const Move_old &m) const;

		inline int getPlayerIndex(int turn) const {
			return kPlayerTurn[(turn + m_gameInfo.teamId * 6) % (NUM_SAMURAI * 2)];
		}


		bool canMove(int actionTimes, bool hideStart, bool hideEnd, Position from, Position to, Position c) const;
		bool canMove(int actionTimes, bool hideStart, bool hideEnd, Position from, Position to) const;
		void updateTurnInfo(const set<Move_old> &moves);
		void simulate(std::vector<int> actions);
		vector<Position> AttackPosition(const int id, const Position &p) const;
		void calcNextMoves();

	public:
		virtual ~MyAI() {}
		virtual void startGame(GameInfo gameInfo);
		virtual std::vector<int> think(TurnInfo turnInfo, int score);
		virtual void endGame(TurnInfo turnInfo, double result) {}
	};

}

#endif  // MY_AI_H
