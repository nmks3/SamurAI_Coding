#include "MyAI.hpp"

using namespace samurai_coding_2015;

#define _CRT_SECURE_NO_WARNINGS


#include <string>
#include <queue>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdio>
#include <random>
#include <map>

#include "Base.hpp"

using namespace std;



bool MyAI::isStart(const Position &q) const {
	for (const Position &p : m_gameInfo.start) {
		if (p == q) {
			return true;
		}
	}
	return false;
}

set<Move_old> MyAI::extractMove(const int id, const Samurai &st0) const {
	if (st0.pos.x < 0 || st0.state < 0) {
		return{};
	}

	vector<Position> attack = kAttack[id];

	Move_old mv0 = {};
	mv0.pos = st0.pos;
	mv0.state = st0.state;

	set<Move_old> vis;
	queue<Move_old> que;

	vis.insert(mv0);
	que.push(mv0);

	const int dx[] = { 0, 1, 0, -1, };
	const int dy[] = { 1, 0, -1, 0, };
	while (!que.empty()) {
		mv0 = que.front();
		que.pop();

		int stateChanged = 0;
		for (int a : mv0.actions) {
			if (a >= 9) stateChanged |= a - 8;
		}

		if (stateChanged != 3) {
			// if (st0.state == mv0.state) {
			if (mv0.state == SamuraiState::Hide && mv0.cost + 1 <= 7 && !m_turnInfo.existSamurai(id, mv0.pos)) {
				// hidden
				Move_old mv = mv0;
				mv.state = 0;
				if (!vis.count(mv)) {
					mv.actions.push_back(10);
					mv.cost += 1;
					vis.insert(mv);
					que.push(mv);
				}
			}

			if (mv0.state == SamuraiState::Appear && mv0.cost + 1 <= 7) {
				// appear
				bool ok = false;
				if (m_turnInfo.isSelfField(id, mv0.pos)) {
					ok = true;
				}
				else {
					for (Position &p : mv0.attack) {
						if (p == mv0.pos) {
							ok = true;
							break;
						}
					}
				}
				if (ok) {
					Move_old mv = mv0;
					mv.state = 1;
					if (!vis.count(mv)) {
						mv.actions.push_back(9);
						mv.cost += 1;
						vis.insert(mv);
						que.push(mv);
					}
				}
			}
		}
		// }

		if (mv0.cost + 2 <= 7) {
			// move
			for (int i = 0; i < 4; i++) {
				Move_old mv = mv0;
				mv.pos.x += dx[i];
				mv.pos.y += dy[i];
				if (!m_gameInfo.valid(mv.pos)) {
					continue;
				}
				if (mv0.state == 1) {
					if (!m_turnInfo.isSelfField(id, mv.pos)) {
						bool attacked = false;
						for (Position &p : mv.attack) {
							if (p == mv.pos) {
								attacked = true;
							}
						}
						if (!attacked) {
							continue;
						}
					}
				}
				else if (m_turnInfo.exist(mv.pos)) {
					continue;
				}
				bool onStart = false;
				for (int i = 0; i < NUM_SAMURAI; i++) {
					if (i != m_gameInfo.type && m_gameInfo.start[i] == mv.pos) {
						onStart = true;
					}
				}
				if (onStart) {
					continue;
				}
				if (!vis.count(mv)) {
					mv.actions.push_back(i + 5);
					mv.cost += 2;
					vis.insert(mv);
					que.push(mv);
				}
			}
		}

		if (mv0.cost + 4 <= 7 && mv0.state == 0) {
			// attack
			for (int i = 0; i < 4; i++) {
				Move_old mv = mv0;
				for (Position &p : attack) {
					Position q = p;
					q.x += mv.pos.x;
					q.y += mv.pos.y;
					if (m_gameInfo.valid(q) && !isStart(q)) {
						mv.attack.push_back(q);
					}
					p = rot90(p);
				}
				if (!vis.count(mv)) {
					mv.actions.push_back(i + 1);
					mv.cost += 4;
					vis.insert(mv);
					que.push(mv);
				}
			}
		}
	}

	return vis;
}

set<Move_old> MyAI::extractMove(int id) const {
	return extractMove(id, m_turnInfo.samurai[id]);
}

#if 0
const int SCORE_FAR = 5;
const int SCORE_HIDDEN = 10 * 2;
const int SCORE_NO_HIDDEN_PENALTY = -50;
const int SCORE_SELF = 10 * 3 / 2;
const int SCORE_EMPTY = 60 * 3 / 2;
const int SCORE_ENEMY = 100 * 3 / 2;
const int SCORE_KILL = 1000*5;
const int SCORE_DEAD = -1000*5;
const int SCORE_DEAD_SUBMARINE = -900*5;
const int SCORE_DEAD_HIDDEN = -700*5;
const int SCORE_DEAD_HIDDEN_SUBMARINE = -600*5;

const int SCORE_SUBMARINE = 1000;
#else
const int SCORE_FAR = 5;
const int SCORE_HIDDEN = 10 * 2;
const int SCORE_NO_HIDDEN_PENALTY = -50;
const int SCORE_SELF = 10 * 3 / 2;
const int SCORE_EMPTY = 60 * 3 / 2;
const int SCORE_ENEMY = 100 * 3 / 2;
const int SCORE_KILL = 1000;
const int SCORE_DEAD = -1000;
const int SCORE_DEAD_SUBMARINE = -900;
const int SCORE_DEAD_HIDDEN = -700;
const int SCORE_DEAD_HIDDEN_SUBMARINE = -600;
const int SCORE_NO_MOVE_PENALTY = -30;

const int SCORE_KILLED_MAYBE_NEAR_ENEMY = -500;
const int SCORE_KILLED_MAYBE_NEAR_ENEMY_HIDDEN = -200;
const int SCORE_MAYBE_KILL_BONUS = -500;

const int SCORE_FOUND_BODY_BY_ENEMY = -300;
const int SCORE_FOUND_ATTACK_BY_ENEMY = -200;
const int SCORE_SUBMARINE = 300;
#endif

const int GREEDY_TURN_THRESHOLD = 3;

static Position targetPosition = { -1,-1, };
static std::mt19937 rnd(std::random_device{}());
static TurnInfo s_prevTurnInfo;
static std::vector<Samurai> s_enemyPos[3];
static std::vector<Position> s_enemyPos2[3];
static int s_moveCount[3];
static int s_cure[3];
static array<int, NUM_SAMURAI> s_restTurn;
static array<bool, 3> s_estimateExact;
static array<bool, 3> s_prevAttacked;

int MyAI::evaluate0(const Move_old &m, const int id) const {
	int score = 0;
	int scorev = 0;

	score += rnd() % 10;

#if 0
	if (m_gameInfo.type == 0) {
		score += dist(m_gameInfo.start[id], m.pos) * 20;
		score -= dist(targetPosition, m.pos) * 30;
		score -= dist(Position(m_gameInfo.w / 2, m_gameInfo.h / 2), m.pos) * 50;
	}
	else {
#if 1
		int nearest = 1 << 29;
		for (int i = 0; i < 3; i++) {
			const Position &p = m_turnInfo.samurai[i + 3].pos;
			if (p.x < 0) continue;
			nearest = std::min(nearest, dist(p, m.pos));
		}
		if (nearest < 1 << 29) {
			score += nearest * 20;
		}
#endif
		score += dist(m_gameInfo.start[id], m.pos) * 20;
		score -= dist(targetPosition, m.pos) * 10;
		score -= dist(Position(m_gameInfo.w / 2, m_gameInfo.h / 2), m.pos) * 20;
	}
#endif

	score += dist(m_gameInfo.start[id], m.pos) * 20;
	score -= dist(targetPosition, m.pos) * 30;
	score -= dist(Position(m_gameInfo.w / 2, m_gameInfo.h / 2), m.pos) * 50;

	if (m.state == 1) {
		score += SCORE_HIDDEN;
	}

	for (const Position &p : m.attack) {
		bool isBase = false;

		for (int i = 0; i < NUM_SAMURAI; i++) {
			if (m_gameInfo.start[i] == p) {
				isBase = true;
				break;
			}
		}

		if (isBase) {
			continue;
		}

		if (m_turnInfo.field[p.y][p.x] == 8 || m_turnInfo.field[p.y][p.x] == 9) {
			scorev += SCORE_EMPTY;
		}
		else if (m_turnInfo.field[p.y][p.x] >= 3) {
			scorev += SCORE_ENEMY;
		}
		else if (m_turnInfo.field[p.y][p.x] != id) {
			scorev += SCORE_SELF;
		}
	}
	for (int i = 0; i < 3; i++) {
		if (i != id) {
			if (dist(m_turnInfo.samurai[i].pos, m.pos) > 10) {
				score += SCORE_FAR;
			}
		}
	}
	if (s_restTurn[m_gameInfo.type] < GREEDY_TURN_THRESHOLD) {
		score /= 10;
	}
	return score + scorev;
}

bool MyAI::canAttack(const Position &target, const SamuraiState &st, int idx, int moveTimes) const {
	const Samurai &samurai = m_turnInfo.samurai[idx];
	if (s_enemyPos2[idx - 3].empty() || moveTimes == 0) {
		return false;
	}
	if (s_enemyPos2[idx - 3].size() > 45) {
		return false;
	}
	int movableDist = (moveTimes - 1) * 3 + 1;
	int cnt = 0;
	for (Position &q : s_enemyPos2[idx - 3]) {
		if (dist(target, q) > 5 && moveTimes == 1) {
			continue;
		}
		if (dist(target, q) > 5 && st == SamuraiState::Hide && moveTimes == 2) {
			continue;
		}
		bool dead = false;
		for (Position p : kAttack[idx % 3]) {
			for (int i = 0; i < 4; i++) {
				// if (dist(target, p + q.pos) - 1 <= movableDist) {
				if (dist(target, p + q) - 0 <= movableDist) {
					dead = true;
					break;
				}
				p = rot90(p);
			}
		}
		if (dead) cnt++;
	}
	// if hidden?
	if (100 * cnt >= 5 * s_enemyPos[idx - 3].size()) {
		return true;
	}
	// if (cnt != 0) return true;
	return false;
}

template <bool scoreDump>
static void dump(char const * msg, int val) {
	if (scoreDump) {
		std::cerr << msg << ": " << val << std::endl;
	}
}

template <bool scoreDump>
int MyAI::evaluate(const Move_old &m, const int id) const {
	// int MyAI::evaluate(const Move_old &m) const {
	int score0 = 0;

	bool kill[3] = {};
	for (int i = 0; i < 3; i++) {
		if (s_enemyPos2[i].empty() || s_cure[i] != 0 || s_enemyPos2[i].size() > 15 || s_restTurn[i + 3] == 0) {
			continue;
		}
		int killed = 0;
		for (Position &ep : s_enemyPos2[i]) {
			bool ok = false;
			for (const Position &p : m.attack) {
				if (p == ep) {
					ok = true;
					break;
				}
			}
			if (ok) {
				killed++;
			}
		}
		kill[i] = 100 * killed >= 80 * s_enemyPos[i].size();
		// kill[i] = 100 * killed >= 100 * s_enemyPos[i].size();
		if (kill[i]) {
			dump<scoreDump>("SCORE_KILL", SCORE_KILL);
			score0 += SCORE_KILL;
		}
	}

	if (s_restTurn[m_gameInfo.type] == 1) {
		return score0 + evaluate0(m, id);
	}

	bool killedExact = false;
	bool killedNonExact = false;
	for (int i = 0; i < 3; i++) {
		// if (!kill[i] && attacked[i].count(m.pos)) {
		if (!kill[i] && canAttack(m.pos, (SamuraiState)m.state, i + 3, s_moveCount[i])) {
			if (scoreDump) {
				std::cerr << "killed: " << (i + 3) << " " << m.pos.x << "," << m.pos.y << " " << s_moveCount[i] << std::endl;
			}
			if (s_estimateExact[i]) {
				killedExact = true;
			}
			else {
				killedNonExact = true;
			}
		}
	}

	if (killedExact || killedNonExact) {
		if (m.state == SamuraiState::Hide) {
			score0 += killedExact ? SCORE_DEAD_HIDDEN : SCORE_DEAD_HIDDEN_SUBMARINE;
			dump<scoreDump>("SCORE_DEAD_HIDDEN", SCORE_DEAD_HIDDEN);
		}
		else {
			score0 += killedExact ? SCORE_DEAD : SCORE_DEAD_SUBMARINE;
			dump<scoreDump>("SCORE_DEAD", SCORE_DEAD);
		}
		score0 += evaluate0(m, id);
		dump<scoreDump>("score0", score0);
		return score0;
	}

	for (int i = 0; i < 3; i++) {
		if (kill[i] || s_moveCount[i] == 0) {
			continue;
		}
		if (!s_enemyPos[i].empty() && s_enemyPos[i].size() <= 5 && s_moveCount[i] > 0) {
			if (m.state == SamuraiState::Hide && m.attack.empty()) {
				for (Position q : s_enemyPos2[i]) {
					if (dist(m.pos, q) <= 4) {
						// score0 += SCORE_FOUND_ATTACK_BY_ENEMY;
						score0 += SCORE_SUBMARINE;
						break;
					}
				}
			}
		}
#if 0
		if (!s_enemyPos[i].empty() && s_enemyPos[i].size() <= 5 || s_prevAttacked[i]) {
			if (m.state == SamuraiState::Appear) {
				for (Position q : s_enemyPos2[i]) {
					if (dist(m.pos, q) <= 3) {
						score0 += SCORE_FOUND_BODY_BY_ENEMY;
						break;
					}
				}
			}

			
			bool done = false;
			for (Position p : m.attack) if(!done){
				for (Position q : s_enemyPos2[i]) {
					if (dist(p, q) <= 2) {
						score0 += SCORE_FOUND_ATTACK_BY_ENEMY;
						done = true;
						break;
					}
				}
			}
		}
#endif
	}

	if (m.state == SamuraiState::Appear) {
		int cost = 0;
		for (int a : m.actions) {
			cost += Action::cost(a);
		}
		if (cost < 7) {
			bool canHidden = false;
			if (m_turnInfo.field[m.pos.y][m.pos.x] < 3) {
				canHidden = true;
			}
			else {
				for (const Position &p : m.attack) {
					if (p == m.pos) {
						canHidden = true;
						break;
					}
				}
			}
			if (canHidden) {
				score0 += SCORE_NO_HIDDEN_PENALTY;
				dump<scoreDump>("SCORE_NO_HIDDEN_PENALTY", SCORE_NO_HIDDEN_PENALTY);
			}
		}
	}

	int score = -(1 << 29);
	set<Move_old> moves = extractMove(m_gameInfo.type, { m.pos, (SamuraiState)m.state });
	for (const Move_old &m3 : moves) {
		Move_old m2 = m3;
		const vector<Position> &a1 = m.attack;
		vector<Position> &a2 = m2.attack;
		a2.insert(a2.end(), a1.begin(), a1.end());
		a2.erase(unique(a2.begin(), a2.end()), a2.end());
		score = std::max(score, evaluate0(m2, id));
	}
	dump<scoreDump>("score0", score0);
	dump<scoreDump>("score", score);
	return score + score0;
}

bool MyAI::canMove(int actionTimes, bool hideStart, bool hideEnd, Position from, Position to, Position c) const {
	if (hideEnd) {
		if (m_turnInfo.field[to.y][to.x] < 3 || m_turnInfo.field[to.y][to.x] == 8) {
			return false;
		}
	}
	int cost = 4;
	if (hideStart) cost++;
	if (hideEnd) cost++;
	if (actionTimes == 1) {
		cost += (dist(from, c) + dist(c, to)) * 2;
		return cost <= 7;
	}
	if (actionTimes == 2) {
		const int d = dist(from, c) + dist(c, to);
		if (d > 4) return false;
		cost += d * 2;
		return cost <= 7 * 2;
	}
	return false;
}

bool MyAI::canMove(int actionTimes, bool hideStart, bool hideEnd, Position from, Position to) const {
	if (hideEnd) {
		if (m_turnInfo.field[to.y][to.x] < 3 || m_turnInfo.field[to.y][to.x] == 8) {
			return false;
		}
	}
	const int d = dist(from, to);
	int cost = 0;
	if (hideStart != hideEnd) cost++;
	cost += d * 2;
	if (actionTimes == 1) {
		return cost <= 7;
	}
	if (actionTimes == 2) {
		return d <= 3 && cost <= 7 * 2;
	}
	return cost == 0;
}

vector<Position> MyAI::AttackPosition(const int id, const Position &p) const {
	vector<Position> res;
	for (Position q : kAttack[id % 3]) {
		for (int i = 0; i < 4; i++) {
			q = rot90(q);
			q += p;
			if (m_gameInfo.valid(q)) {
				res.push_back(q);
			}
			for (int j = 0; j < 4; j++) {
				q.move(j);
				if (m_gameInfo.valid(q)) {
					res.push_back(q);
				}
				q.unmove(j);
			}
			q -= p;
		}
	}
	res.erase(unique(res.begin(), res.end()), res.end());
	return res;
}

void MyAI::updateTurnInfo(const set<Move_old> &moves) {
	if (m_turnInfo.turn < NUM_SAMURAI) {
//		return;
		for (int i = 0; i < 3; i++) {
			s_enemyPos[i].clear();
			s_enemyPos2[i].clear();

			Samurai s;
			s.pos = m_gameInfo.start[i+3];
			s.state = SamuraiState::Appear;
			s_enemyPos[i].push_back(s);
			s_enemyPos2[i].push_back(s.pos);
		}
	}

	std::vector<Position> estimate[3];
	std::vector<Position> estimate2[3];
	std::vector<Position> attacked[3];

	for (int y = 0; y < m_gameInfo.h; y++) {
		for (int x = 0; x < m_gameInfo.w; x++) {
			if (s_prevTurnInfo.field[y][x] != 9 && s_prevTurnInfo.field[y][x] != m_turnInfo.field[y][x]) {
				if (m_turnInfo.field[y][x] >= 3 && m_turnInfo.field[y][x] < 6) {
					attacked[m_turnInfo.field[y][x] - 3].push_back(Position(x, y));
				}
			}
		}
	}

	for (auto &v : attacked) {
		std::sort(v.begin(), v.end());
	}

	// std::cerr << "check...: " << s_moveCount[0] << " " << s_moveCount[1] << " " << s_moveCount[2] << std::endl;
	for (int i = 0; i < 3; i++) if (s_moveCount[i] != 0) {
		const auto& atk = attacked[i];
		s_prevAttacked[i] = false;
		if (!atk.empty()) {
			s_prevAttacked[i] = true;
			int maxOverlap = 0;
			for (int y = 0; y < m_gameInfo.h; y++) {
				for (int x = 0; x < m_gameInfo.w; x++) {
					/*
					if (m_turnInfo.field[y][x] < 3 || m_turnInfo.field[y][x] == 8) {
						// visible, can't hide
						continue;
					}*/

					const Position p0(x, y);

					if (isStart(p0) && m_gameInfo.start[i + 3] != p0) {
						continue;
					}

					int overlap[4] = {};
					int otherColor[4] = {};
					bool bad[4] = {};
					for (Position p : kAttack[i]) {
						for (int d = 0; d < 4; d++) {
							p += p0;
							if (m_gameInfo.valid(p)) {
								if (std::binary_search(atk.cbegin(), atk.cend(), p)) {
									overlap[d]++;
								}
								else if (m_turnInfo.field[p.y][p.x] != 9) {
									if (m_turnInfo.field[p.y][p.x] == 8) {
										bad[d] = true;
									}
									else if (m_turnInfo.field[p.y][p.x] != i + 3) {
										otherColor[d]++;
									}
								}
							}
							p -= p0;
							p = rot90(p);
						}
					}
					for (int d = 0; d < 4; d++) {
						// 7,3
						/*
						if (x == 7 && y == 3) {
							cerr << "chkpos: " << d << " " << bad[d] << " " << overlap[d] << " " << atk.size() << endl;
						}*/
						if (bad[d]) continue;
						if (overlap[d] == atk.size()) {
							maxOverlap = overlap[d];
							if (otherColor[d] != 0) {
								if (estimate2[i].empty() || *estimate2[i].crbegin() != p0) {
									estimate2[i].push_back(p0);
								}
							}
							else {
								estimate[i].push_back(p0);
								break;
							}
						}
						else if (overlap[d] >= maxOverlap) {
							if (overlap[d] > maxOverlap) {
								estimate2[i].clear();
								maxOverlap = overlap[d];
							}
							if (estimate2[i].empty() || *estimate2[i].crbegin() != p0) {
								estimate2[i].push_back(p0);
							}
						}
					}
				}
			}
		}

		const int VISIBLE = 1;
		const int INVISIBLE = 2;
		int state[MAX_H][MAX_W] = {};
		std::vector<Samurai> next;
		auto &est = estimate[i].empty() ? estimate2[i] : estimate[i];
		// std::cerr << "check: " << s_enemyPos[i].size() << " " << estimate[i].size() << " " << estimate2[i].size() << " " << s_moveCount[i] << " " << atk.size() << std::endl;
		if (!s_enemyPos[i].empty()) {
			for (Position p : est) {
				for (Samurai &s : s_enemyPos[i]) {
					const int f1 = m_turnInfo.field[p.y][p.x];
					const int f2 = s_prevTurnInfo.field[p.y][p.x];
					if (s.state == SamuraiState::Hide && (f2 < 3 || f1 == 8 || f2 == 8)) {
						continue;
					}

					for (int dy = -4; dy <= 4; dy++) {
						for (int dx = -4; dx <= 4; dx++) {
							const Position pp = p + Position(dx, dy);
							if (!m_gameInfo.valid(pp) || isStart(pp) && m_gameInfo.start[i + 3] != pp) {
								continue;
							}
							if (canMove(s_moveCount[i], s.state == SamuraiState::Hide, false, s.pos, pp, p)) {
								state[pp.y][pp.x] |= VISIBLE;
							}
							if (canMove(s_moveCount[i], s.state == SamuraiState::Hide, true, s.pos, pp, p)) {
								state[pp.y][pp.x] |= INVISIBLE;
							}
						}
					}
				}
			}
			for (int y = 0; y < m_gameInfo.h; y++) {
				for (int x = 0; x < m_gameInfo.w; x++) {
					if (m_turnInfo.field[y][x] != 9) {
						state[y][x] &= INVISIBLE;
					}

					Samurai s;
					s.pos = Position(x, y);
					if (state[y][x] & VISIBLE) {
						s.state = SamuraiState::Appear;
						next.push_back(s);
					}
					if (state[y][x] & INVISIBLE) {
						s.state = SamuraiState::Hide;
						next.push_back(s);
					}
				}
			}
		}
		if (next.empty()) {
			// ???
			for (Position p : est) {
				const int f = s_prevTurnInfo.field[p.y][p.x];
				for (int dy = -4; dy <= 4; dy++) {
					for (int dx = -4; dx <= 4; dx++) {
						const Position pp = p + Position(dx, dy);
						if (!m_gameInfo.valid(pp) || isStart(pp) && m_gameInfo.start[i + 3] != pp) {
							continue;
						}
						if (f >= 3 && f < NUM_SAMURAI) {
							if (canMove(s_moveCount[i], true, true, p, pp, p)) {
								state[pp.y][pp.x] |= INVISIBLE;
							}
						}
						if (f == 9) {
							if (canMove(s_moveCount[i], false, false, p, pp, p)) {
								state[pp.y][pp.x] |= VISIBLE;
							}
							if (canMove(s_moveCount[i], false, true, p, pp, p)) {
								state[pp.y][pp.x] |= INVISIBLE;
							}
						}
					}
				}
			}
			for (int y = 0; y < m_gameInfo.h; y++) {
				for (int x = 0; x < m_gameInfo.w; x++) {
					if (m_turnInfo.field[y][x] != 9) {
						state[y][x] &= INVISIBLE;
					}

					Samurai s;
					s.pos = Position(x, y);
					if (state[y][x] & VISIBLE) {
						s.state = SamuraiState::Appear;
						next.push_back(s);
					}
					if (state[y][x] & INVISIBLE) {
						s.state = SamuraiState::Hide;
						next.push_back(s);
					}
				}
			}
		}
		if (est.empty()) {
			for (int j = (int)s_enemyPos[i].size() - 1; j >= 0; j--) {
				Samurai &s = s_enemyPos[i][j];
				const int f1 = m_turnInfo.field[s.pos.y][s.pos.x];
				const int f2 = s_prevTurnInfo.field[s.pos.y][s.pos.x];
				if (s.state == SamuraiState::Hide && (f2 < 3 || f1 == 8 || f2 == 8)) {
					continue;
				}
				for (int dy = -4; dy <= 4; dy++) {
					for (int dx = -4; dx <= 4; dx++) {
						const Position pp = s.pos + Position(dx, dy);
						if (!m_gameInfo.valid(pp) || isStart(pp) && m_gameInfo.start[i + 3] != pp) {
							continue;
						}
						if (canMove(s_moveCount[i], s.state == SamuraiState::Hide, false, s.pos, pp)) {
							state[pp.y][pp.x] |= VISIBLE;
						}
						if (canMove(s_moveCount[i], s.state == SamuraiState::Hide, true, s.pos, pp)) {
							state[pp.y][pp.x] |= INVISIBLE;
						}
					}
				}
			}
			for (int y = 0; y < m_gameInfo.h; y++) {
				for (int x = 0; x < m_gameInfo.w; x++) {
					if (m_turnInfo.field[y][x] != 9) {
						state[y][x] &= INVISIBLE;
					}

					Samurai s;
					s.pos = Position(x, y);
					if (state[y][x] & VISIBLE) {
						s.state = SamuraiState::Appear;
						next.push_back(s);
					}
					if (state[y][x] & INVISIBLE) {
						s.state = SamuraiState::Hide;
						next.push_back(s);
					}
				}
			}
		}

		s_estimateExact[i] = next.size() == 1;

#if 1
		// submarine
		const int THRESHOLD_SUBMARINE = 5;
		if (s_enemyPos[i].size() < THRESHOLD_SUBMARINE && next.size() > 10) {
			int cnt = 0;
			vector<Samurai> submarine;
			for (Samurai &s : next) {
				bool escape = true;
				bool attack = false;
				for (auto &m : moves) {
					auto it = find(m.attack.cbegin(), m.attack.cend(), s.pos);
					if (it != m.attack.cend()) {
						escape = false;
						break;
					}
					if (!attack && !m.attack.empty()) {
						for (Position atk : AttackPosition(i, s.pos)) {
							if (atk == m.pos) {
								attack = true;
								break;
							}
						}
					}
				}
				if (attack && escape) {
					submarine.push_back(s);
				}
			}
			if (!submarine.empty()) {
				next = submarine;
			}
		}
#endif

		s_enemyPos[i] = next;
		std::sort(s_enemyPos[i].begin(), s_enemyPos[i].end());
		s_enemyPos[i].erase(std::unique(s_enemyPos[i].begin(), s_enemyPos[i].end()), s_enemyPos[i].end());
	}

	for (int i = 0; i < 3; i++) {
		if (m_turnInfo.samurai[i + 3].pos.x >= 0) {
			Samurai s;
			s.pos = m_turnInfo.samurai[i + 3].pos;
			s.state = SamuraiState::Appear;
			s_enemyPos[i].clear();
			s_enemyPos[i].push_back(s);
			s_estimateExact[i] = true;
		}
		s_enemyPos2[i].clear();
		for (auto &s : s_enemyPos[i]) { 
			s_enemyPos2[i].push_back(s.pos);
		}
		std::sort(s_enemyPos2[i].begin(), s_enemyPos2[i].end());
		s_enemyPos2[i].erase(std::unique(s_enemyPos2[i].begin(), s_enemyPos2[i].end()), s_enemyPos2[i].end());
	}
	/*
	for (Position s : estimate[2]) {
		cerr << "pos: " << s.x << " " << s.y << endl;
	}
	for (Position s : estimate2[2]) {
		cerr << "pos2: " << s.x << " " << s.y << endl;
	}
	for (Position s : attacked[2]) {
		cerr << "atk: " << s.x << " " << s.y << endl;
	}
	for (Samurai s : s_enemyPos[2]) {
		cerr << "chk: " << s.pos.x << " " << s.pos.y << " " << s.state << endl;
	}
	for (Samurai s : s_enemyPos[2]) {
		cerr << "chk: " << s.pos.x << " " << s.pos.y << " " << s.state << endl;
	}
	*/

	/*
	for (int i = 0; i < 3; i++) {
	if (!attacked[i].empty() && s_enemyPos[i].empty()) {
	std::cerr << "bad..." << std::endl;
	}
	for (Position p : attacked[i]) {
	std::cerr << "atked: " << i << " " << p.x << " " << p.y << std::endl;
	}
	for (Samurai p : s_enemyPos[i]) {
	std::cerr << "est: " << i << " " << p.pos.x << " " << p.pos.y << std::endl;
	}
	std::cerr << "-----: " << s_enemyPos[i].size() << std::endl;
	}
	char ch; cin >> ch;
	//*/
}

void MyAI::simulate(std::vector<int> actions) {
	Position p = s_prevTurnInfo.samurai[m_gameInfo.type].pos;
	for (int a : actions) {
		if (a == 0) break;
		if (a <= 4) {
			for (Position q : kAttack[m_gameInfo.type]) {
				for (int i = 0; i < a - 1; i++) {
					q = rot90(q);
				}
				q += p;
				if (m_gameInfo.valid(q) && !isStart(q)) {
					s_prevTurnInfo.field[q.y][q.x] = m_gameInfo.type;

					if (false)
						for (auto &vec : s_enemyPos) {
							std::remove_if(vec.begin(), vec.end(),
								[&](const Samurai &it) { return it.pos == q; });
						}
				}
			}
		}
		else if (a <= 8) {
			p.move(a - 5);
		}
	}
}

void MyAI::calcNextMoves() {
	int cure[3];
	for (int i = 0; i < 3; i++) {
		int t = 0;
		for (int j = m_turnInfo.turn - 1; j >= 0; j--) {
			const int p = getPlayerIndex(j);
			// std::cerr << " (" << i << "," << p << ")";
			s_cure[i] = std::max(s_cure[i] - 1, 0);
			if (m_turnInfo.turn < NUM_SAMURAI) {
				if (p >= 3 && p < 6) {
					s_moveCount[p - 3]++;
				}
			}
			if (p == m_gameInfo.type) {
				break;
			}
			t++;
		}

		if (m_turnInfo.samurai[i + 3].state == SamuraiState::Dead) {
			if (s_cure[i] == 0) {
				s_cure[i] = m_gameInfo.cureTurn - t;
			}
		}
		else {
			s_cure[i] = 0;
		}
		cure[i] = s_cure[i];
	}

	std::fill(m_nextMoveCount.begin(), m_nextMoveCount.end(), 0);
	// std::cerr << "cur turn:";
	for (int i = m_turnInfo.turn + 1; i < m_gameInfo.turn; i++) {
		for (int &c : cure) {
			c--;
		}

		const int p = getPlayerIndex(i);
		// std::cerr << " (" << i << "," << p << ")";
		if (p == m_gameInfo.type) {
			break;
		}
		if (p < 3) continue;
		assert(p >= 3 && p < 6);
		if (cure[p - 3] <= 0) {
			m_nextMoveCount[p - 3]++;
		}
	}

	fill(s_restTurn.begin(), s_restTurn.end(), 0);
	for (int i = m_turnInfo.turn; i < m_gameInfo.turn; i++) {
		s_restTurn[getPlayerIndex(i)]++;
	}
}

void MyAI::startGame(GameInfo gameInfo) {
	m_gameInfo = gameInfo;
}

std::vector<int> MyAI::think(TurnInfo turnInfo_, int score_in) {
	m_turnInfo = turnInfo_;

	set<Move_old> moves = extractMove(m_gameInfo.type);
	calcNextMoves();
	updateTurnInfo(moves);
	std::copy(m_nextMoveCount.cbegin(), m_nextMoveCount.cend(), s_moveCount);


	// ターゲットはもっと賢い方法で決める
	// @todo ここ
	if (targetPosition.x < 0 ||
		dist(targetPosition, m_turnInfo.samurai[m_gameInfo.type].pos) <= 8 ||
		m_turnInfo.curePeriod > 0) {
		const Position tpos[] = {
			m_gameInfo.start[3],
			m_gameInfo.start[4],
			m_gameInfo.start[5],
			// { m_gameInfo.w / 2, m_gameInfo.h / 2 },
		};
		targetPosition = tpos[rnd() % 3];
	}

	if (m_turnInfo.curePeriod > 0) {
		// dead
		// cout << "0" << endl;
		// std::cerr << "dead... " << turnInfo.curePeriod << std::endl;
		s_prevTurnInfo = m_turnInfo;
		return vector<int>();
	}

	int score = -(1 << 29);
	vector<int> actions;
	Move_old res;
	for (const Move_old &m : moves) {
		// int val = evaluate(m);
		int val = evaluate(m, m_gameInfo.type);
		if (val > score) {
			score = val;
			actions = m.actions;
			res = m;
		}
	}
	// evaluate<true>(gameInfo, turnInfo, res, gameInfo.type);
	// std::cerr << "score: " << score << std::endl;

	// for (int a : actions) {
	// 	cout << a << " ";
	// }
	// cout << "0" << endl;
	s_prevTurnInfo = m_turnInfo;

	simulate(actions);

	return actions;
}

#ifdef SUBMIT
template <typename T>
void output(const std::vector<T> &v) {
	for (const T &x : v) {
		std::cout << x << " ";
	}
	std::cout << "0" << std::endl;
}

int main() {
	ios::sync_with_stdio(false);

	MyAI ai;
	GameInfo gameInfo = ReadGameInfo();
	ai.startGame(gameInfo);
	std::cout << "0" << std::endl;
	while (true) {
		output(ai.think(ReadTurnInfo(gameInfo.w, gameInfo.h), -1));
	}
}
#endif // SUBMIT
