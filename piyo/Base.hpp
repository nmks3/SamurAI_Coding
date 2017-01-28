#ifndef BASE_H
#define BASE_H

#define	_CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <iostream>
#include <sstream>
#include <vector>
#include <cassert>

using namespace std;

const int NUM_SAMURAI = 6;
const int MAX_W = 20;
const int MAX_H = 20;

const int dx[] = { 0, 1, 0, -1, };
const int dy[] = { 1, 0, -1, 0, };

enum FieldType {
	Empty = 8,
	Invisible = 9,
};

struct Position {
	int x, y;

	Position() {}
	Position(int x, int y) : x(x), y(y) {
	}

	bool operator<(const Position &p) const {
		return x < p.x || (x == p.x && y < p.y);
	}
	bool operator==(const Position &p) const {
		return x == p.x && y == p.y;
	}
	bool operator!=(const Position &p) const {
		return !(*this == p);
	}
	Position operator+(const Position &p) {
		Position tmp = *this;
		return tmp += p;
	}
	Position &operator+=(const Position &p) {
		x += p.x;
		y += p.y;
		return *this;
	}
	Position &operator-=(const Position &p) {
		x -= p.x;
		y -= p.y;
		return *this;
	}
	void move(const int d) { x += dx[d]; y += dy[d]; }
	void unmove(const int d) { move((d + 2) & 3); }
};

const Position dpos[] = { { dx[0], dy[0] },{ dx[1], dy[1] },{ dx[2], dy[2] },{ dx[3], dy[3] }, };

int dist(const Position &p1, const Position &p2);

enum SamuraiState {
	Appear = 0,
	Hide = 1,
	Dead = -1,
	Unknown = -2,
};

struct Samurai {
	Position pos;
	SamuraiState state;

	bool isHide() const {
		return state == SamuraiState::Hide;
	}

	bool isAppear() const {
		return state == SamuraiState::Appear;
	}

	bool isDead() const {
		return state == SamuraiState::Dead;
	}

	bool operator<(const Samurai &o) const {
		return state < o.state || state == o.state && pos < o.pos;
	}

	bool operator==(const Samurai &o) const {
		return state == o.state && pos == o.pos;
	}
	bool operator!=(const Samurai &o) const {
		return !(*this == o);
	}
};

struct GameInfo {
	int turn;
	int teamId;
	int type;
	int w;
	int h;
	int cureTurn;
	Position start[NUM_SAMURAI];
	int rank[NUM_SAMURAI];
	int score[NUM_SAMURAI];

	bool valid(Position p) const {
		return p.x >= 0 && p.x < w && p.y >= 0 && p.y < h;
	}
};

struct TurnInfo {
	int turn;
	int curePeriod;
	Samurai samurai[NUM_SAMURAI];
	int field[MAX_H][MAX_W];

	bool exist(Position p) const {
		for (const Samurai &s : samurai) {
			if (s.pos == p) {
				return true;
			}
		}
		return false;
	}

	bool isSelfField(int id, Position pos) const {
		if (field[pos.y][pos.x] >= 8) return false;
		return field[pos.y][pos.x] / 3 == id / 3;
	}

	bool isEnemyField(int id, Position pos) const {
		if (field[pos.y][pos.x] >= 8) return false;
		return field[pos.y][pos.x] / 3 != id / 3;
	}

	bool existSamurai(int id, Position pos) const {
		for (int i = 0; i < NUM_SAMURAI; i++) {
			if (i != id && samurai[i].pos == pos) {
				return true;
			}
		}
		return false;
	}
};

string getline_skip();
GameInfo ReadGameInfo();
TurnInfo ReadTurnInfo(int w, int h);
Position rot90(Position p);


const vector<Position> kAttack[] = {
	{ { 0, 1 },{ 0, 2 },{ 0, 3 },{ 0, 4 }, },
	{ { 0, 1 },{ 0, 2 },{ 1, 0 },{ 1, 1 },{ 2, 0 }, },
	{ { -1, -1 },{ -1, 0 },{ -1, 1 },{ 0, 1 },{ 1, -1 },{ 1, 0 },{ 1, 1 }, },
};

const int kPlayerTurn[] = {
    0, 3, 4, 1, 2, 5,
    3, 0, 1, 4, 5, 2,
};

struct Attack {
    Position pos;
    int dir;

	Attack() {}
	Attack(Position pos, int dir) : pos(pos), dir(dir) {}

	bool operator<(const Attack &m) const {
		if (dir != m.dir) return dir < m.dir;
		return pos < m.pos;
	}
};

struct Move {
	Samurai samurai;
    vector<Attack> atk;
	vector<int> act;

	bool operator<(const Move &m) const {
		if (samurai != m.samurai) return samurai < m.samurai;
		return atk < m.atk;
	}
};


class Action {
	static const int kActionCost[1 + 4 + 4 + 2];

public:
	static int cost(unsigned int act) {
		assert(act < sizeof(kActionCost) / sizeof(kActionCost[0]));
		return kActionCost[act];
	}

	static const int None = 0;
	static const int AttackS = 1;
	static const int AttackE = 2;
	static const int AttackN = 3;
	static const int AttackW = 4;
	static const int MoveS = 5;
	static const int MoveE = 6;
	static const int MoveN = 7;
	static const int MoveW = 8;
	static const int Hide = 9;
	static const int Appear = 10;
};

#endif  // BASE_H
