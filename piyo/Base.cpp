
#define	_CRT_SECURE_NO_WARNINGS

#include "Base.hpp"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <vector>
#include <complex>

using namespace std;

int dist(const Position &p1, const Position &p2) {
	return std::abs(p1.x - p2.x) + std::abs(p1.y - p2.y);
}

string getline_skip() {
	string buf;
	while (true) {
		getline(cin, buf);
		if (!(buf.length() == 0 || buf[0] == '#')) {
			return buf;
		}
	}
}

GameInfo ReadGameInfo() {
	GameInfo info;

	sscanf(getline_skip().c_str(), "%d%d%d%d%d%d", &info.turn, &info.teamId, &info.type, &info.w, &info.h, &info.cureTurn);

	for (Position &p : info.start) {
		sscanf(getline_skip().c_str(), "%d%d", &p.x, &p.y);
	}

    for(int i = 0; i < NUM_SAMURAI; i++) {
		sscanf(getline_skip().c_str(), "%d%d", &info.rank[i], &info.score[i]);
    }

	// cout << "0" << endl;
	return info;
}

TurnInfo ReadTurnInfo(int w, int h) {
	TurnInfo info;

	sscanf(getline_skip().c_str(), "%d", &info.turn);
	sscanf(getline_skip().c_str(), "%d", &info.curePeriod);

	for (Samurai &s : info.samurai) {
		int st;
		sscanf(getline_skip().c_str(), "%d%d%d", &s.pos.x, &s.pos.y, &st);
		s.state = (SamuraiState)st;
	}

	for (int y = 0; y < h; y++) {
		istringstream iss(getline_skip());
		for (int x = 0; x < w; x++) {
			iss >> info.field[y][x];
		}
	}

	return info;
}

Position rot90(Position p) {
	return{ p.y, -p.x, };
}

const int Action::kActionCost[] = { 0, 4, 4, 4, 4, 2, 2, 2, 2, 1, 1, };
