#include "samurai.hpp"
#include <thread>
#include <signal.h>

void Role::init(int id) {
  this->id = id;
  int reachSize;
  int weapon = id%3;
  reachSize = weaponReachSize[id%3];
  for (int r = 0; r != weaponReachSize[weapon]; r++) {
    reach.push_back(Coordinates(weaponReach[weapon][r]));
  }
  homeX = homePositions[id].x;
  homeY = homePositions[id].y;
}

Setting::Setting() {
  for (int id = 0; id != 6; id++) {
    roles[id].init(id);
  }
}

void SamuraiState::init
(Setting& setting, BattleField& field, int id) {
  side = id/3;
  weapon = id%3;
  role = &setting.roles[id];
  position = field.section(role->homeX, role->homeY);
  position->arrive(this);
  curePeriod = 0;
  hidden = false;
  int pipeToAI[2];
  if (pipe(pipeToAI) < 0) {
    throw ErrorReport("Failed to create a pipe: "+errno);
  }
  int pipeFromAI[2];
  if (pipe(pipeFromAI) < 0) {
    throw ErrorReport("Failed to create a pipe: "+errno);
  }
  processId = fork();
  if (processId == -1) {
    throw ErrorReport("Failed to fork an AI process: "+errno);
  }
  if (processId == 0) {
    // Child process
    // stdin from the pipe
    close(pipeToAI[1]);
    dup2(pipeToAI[0], 0);
    close(pipeToAI[0]);
    // stdout to the pipe
    close(pipeFromAI[0]);
    dup2(pipeFromAI[1], 1);
    close(pipeFromAI[1]);
    if (system(invokeCommand.c_str()) < 0) {
      throw ErrorReport("Failed to execute "+invokeCommand+": "
			+to_string(errno));
    }
    exit(0);
  } else {
    close(pipeToAI[0]);
    close(pipeFromAI[1]);
    FILE* fai = fdopen(pipeFromAI[0], "r");
    if (fai == nullptr) {
      throw ErrorReport("Failed to open pipe to AI: "+errno);
    }
    fromAI = SamuraiScanner(fai);
    toAI = fdopen(pipeToAI[1], "w");
    if (toAI == nullptr) {
      throw ErrorReport("Failed to open pipe from AI: "+errno);
    }
  }
}

bool SamuraiState::move(GameState& state, int direction, string& comments) {
  Section* p = position->neighbors[direction];
  if (p == 0) {
    comments += "Invalid move direction: " + to_string(direction)
      + " from " + position->coords.toString();
    return false;
  }
  for (int s = 0; s != 6; s++) {
    SamuraiState& ss = samuraiStates[s];
    if (&ss != this &&
	ss.role->homeX == p->coords.x &&
	ss.role->homeY == p->coords.y) {
      comments += "Moving to home of another samurai: "
	+ position->coords.toString();
      return false;
    }
  }
  if (hidden) {
    if (p->state < 0 || p->state/3 != role->id/3) {
      comments += "Hidden move to non-territory from: "
	+ position->coords.toString()
	+ " to " + p->coords.toString();
      return false;
    }
  } else {
    if (p->apparent != 0) {
      comments += "Moving to an already filled section: "
	+ position->coords.toString()
	+ " to " + p->coords.toString();
      return false;
    }
  }
  if (dump) {
    fprintf(dump, "Samurai %d.%d moves from %s to %s\n",
	    side, weapon, position->coords.toString().c_str(),
	    p->coords.toString().c_str());
  }
  position->leave(this);
  position = p;
  position->arrive(this);
  return true;
}

bool SamuraiState::occupy(GameState& state, int direction, string& comments) {
  if (hidden) {
    comments += "Hidden samurai tried occupation";
    return false;
  }
  state.battleField.occupy(state, *role, direction, *position);
  return true;
}
bool SamuraiState::hide(string& comments) {
  if (hidden) {
    comments += "Hidden samurai tries to hide itself further";
    return false;
  }
  assert(position->apparent == this);
  if (position->state < 0 || position->state/3 != role->id/3) {
    comments += "Trying to hide itself at non-territory: ";
    return false;
  }
  hidden = true;
  position->apparent = 0;
  return true;
}
bool SamuraiState::appear(string& comments) {
  if (!hidden) {
    comments += "Non-hidden samurai tries to appear";
    return false;
  }
  if (position->apparent != 0) return false;
  hidden = false;
  position->apparent = this;
  return true;
}
void SamuraiState::cure() {
  if (curePeriod > 0) curePeriod -= 1;
}
void SamuraiState::house(BattleField& field) {
  Section* home = field.section(role->homeX, role->homeY);
  position->leave(this);
  hidden = false;
  home->arrive(this);
  position = home;
}
void SamuraiState::die(BattleField& field) {
  if (dump) {
    fprintf(dump, "Samurai %d.%d disqualified\n", side, weapon);
  }
  house(field);
  alive = false;
}
void SamuraiState::injure(BattleField& field, Setting& setting) {
  if (dump) {
    fprintf(dump, "Samurai %d.%d injured\n", side, weapon);
  }
  house(field);
  curePeriod = setting.cureTurns;
}

void readResponse(SamuraiState& ss) {
  ss.response = ss.fromAI.get();
  ss.done = true;
}

void GameState::init() {
  for (int s = 0; s != 6; s++) {
    samuraiStates[s].init(setting, battleField, s);
  }
  for (int s = 0; s != 6; s++) {
    SamuraiState& ss = samuraiStates[s];
    sendGameInfo(ss.toAI, s);
    if (ifs != nullptr) sendGameInfo(ifs[s], s);
    ss.response = -1;
    ss.done = false;
    thread* readerThread = new thread(readResponse, ref(ss));
    chrono::system_clock::time_point till =
      chrono::system_clock::now() +
      chrono::milliseconds(setting.initTimeAllowed);
    while (!ss.done) {
      this_thread::sleep_for(chrono::milliseconds(10));
      chrono::system_clock::time_point now = 
	chrono::system_clock::now();
      if (now >= till) break;
    }
    if (system(ss.pauseCommand.c_str()) < 0) {
      throw ErrorReport("Failed to execute "+ss.pauseCommand+": "
			+to_string(errno));
    }
    ss.alive = ss.done;
    readerThread->join();
    if (!ss.alive) {
      fprintf(stderr, "Time out at initiatin: samurai %d.%d\n", s/3, s%3);
    } else {
      if (ss.response != 0) {
	fprintf(stderr, "Wrong response %d at initiation of samurai %d.%d\n",
		ss.response, s/3, s%3);
	ss.alive = false;
      }
    }
    delete readerThread;
    if (!ss.alive) {
      kill(ss.processId, SIGKILL);
    }
  }
  turn = 0;
}

void GameState::sendGameInfo(FILE* out, int id) {
  fprintf(out,
	  "# <# turns> <side> <weapon> <width> <height> <cure period>\n"
	  "%d %d %d %d %d %d\n",
	  setting.turns, id/3, id%3,
	  battleField.width, battleField.height,
	  setting.cureTurns);
  fprintf(out, "# Home positions\n");
  for (int a = id/3; a != id/3+2; a++) {
    for (int w = 0; w != 3; w++) {
      int s = (3*a+w)%6;
      fprintf(out, "%d %d\n", setting.roles[s].homeX, setting.roles[s].homeY);
    }
  }
  fprintf(out, "# Ranks and scores of samurai\n");
  for (int s = 0; s != 6; s++) {
    SamuraiState& ss = samuraiStates[s];
    fprintf(out, "%d %d\n", ss.rank, ss.score);
  }
  fflush(out);
}

void GameState::sendTurnInfo(FILE* out, int side, int weapon) {
  SamuraiState ss = samuraiStates[3*side+weapon];
  if (ss.alive) {
    fprintf(out, 
	    "# Turn information for samurai of side %d with weapon %d\n",
	    side, weapon);
    fprintf(out, "# <turn>\n%d\n", turn);
    fprintf(out, "# <cure period>\n%d\n", ss.curePeriod);
    const int width = battleField.width;
    const int height = battleField.height;
    bool visible[width*height] = {};
    for (int w = 0; w != 3; w++) {
      int who = 3*side+w;
      const int v = setting.roles[who].vision;
      const Section& s = *samuraiStates[who].position;
      int x0 = s.coords.x;
      int y0 = s.coords.y;
      for (int dy = -v; dy <= v; dy++) {
	int y = y0+dy;
	if (0 <= y && y < height) {
	  int v1 = v-abs(dy);
	  for (int dx = -v1; dx <= v1; dx++) {
	    int x = x0+dx;
	    if (0 <= x && x < width) {
	      visible[y*width+x] = true;
	    }
	  }
	}
      }
    }
    fprintf(out, "# Samurai states: <position x> <position y> <hiding>\n");
    for (int a = 0; a != 2; a++) {
      for (int w = 0; w != 3; w++) {
	int s = 3*((a+side)%2)+w;
	SamuraiState& ss = samuraiStates[s];
	int x = ss.position->coords.x;
	int y = ss.position->coords.y;
	if (a == 0 || (visible[y*width+x] && !ss.hidden)) {
	  fprintf(out, "%d %d %d\n",
		  x, y, (ss.alive ?  (ss.hidden ? 1 : 0)  : -1));
	} else {
	  fprintf(out, "-1 -1 %d\n", (ss.alive ? 1 : -1));
	}
      }
    }
    fprintf(out, "# Battle field states\n");
    for (int y = 0; y != height; y++) {
      for (int x = 0; x != width; x++) {
	if (visible[y*width+x]) {
	  Section* sect = battleField.map.at(Coordinates(x, y));
	  int sectState =
	    (sect->state == -1 ? 8 :
	     (side == 0 ? sect->state :
	      (sect->state >= 3 ? sect->state -3 :
	       sect->state + 3)));
	  fprintf(out, " %d", sectState);
	} else {
	  fprintf(out, " 9");
	}
      }
      fprintf(out, "\n");
    }
    fflush(out);
  }
}

void readActions(SamuraiState& ss) {
  while (!ss.actions.empty()) {
    ss.actions.pop();
  }
  while (ss.alive) {
    int action = ss.fromAI.get();
    if (action == 0) break;
    ss.actions.push(action);
    if (dump) fprintf(dump, " %d", action);
  }
  if (dump) {
    fprintf(dump, "\n");
    fflush(dump);
  }
  ss.done = true;
}

void GameState::receiveActionCommands
(SamuraiScanner& is, const int side, const int weapon, FILE* log) {
  int id = 3*side+weapon;
  fprintf(log, "# Turn %d/%d\n", turn, setting.turns);
  fprintf(log, "# <samurai id>\n%d\n", id);
  fprintf(log, "# <think time> <used time>\n%d %d\n",
	  setting.timeAllowed, 0);
  string turnComments;
  SamuraiState& ss = samuraiStates[id];
  if (ss.alive) {
    if (system(ss.resumeCommand.c_str()) < 0) {
      throw ErrorReport("Failed to execute "+ss.resumeCommand+": "
			+to_string(errno));
    }
    ss.done = false;
    thread* readerThread = new thread(readActions, ref(ss));
    chrono::system_clock::time_point till =
      chrono::system_clock::now() +
      chrono::milliseconds(setting.timeAllowed);
    while (!ss.done) {
      this_thread::sleep_for(chrono::milliseconds(1));
      chrono::system_clock::time_point now = 
	chrono::system_clock::now();
      if (now >= till) break;
    }
    if (system(ss.pauseCommand.c_str()) < 0) {
      throw ErrorReport("Failed to execute "+ss.pauseCommand+": "
			+to_string(errno));
    }
    if (!ss.done) {
      fprintf(stderr, "Timed out @ turn %d\n", turn);
      turnComments += "Timed out";
      readerThread->detach();
      kill(ss.processId, SIGKILL);
      goto DEAD;
    }
    readerThread->join();
    delete readerThread;
    fprintf(log, "# Actions\n");
    {
      int power = ss.role->activity;
      if (!ss.actions.empty() && ss.curePeriod != 0) {
	turnComments += "Trying to act under recovery";
	goto FINISH;
      }
      while (!ss.actions.empty()) {
	int action = ss.actions.front();
	if (action < 0 || action > 10) {
	  turnComments += "Invalid action specified: " + action;
	  goto FINISH;
	} else {
	  static int required[] = {0, 4, 4, 4, 4, 2, 2, 2, 2, 1, 1}; 
	  power -= required[action];
	  if (power < 0) {
	    turnComments += "Action limit exceeded";
	    goto FINISH;
	  }
	  if (dump) {
	    fprintf(dump, "Samurai %d.%d performs action: %d\n",
		    ss.side, ss.weapon, action);
	  }
	  ss.actions.pop();
	  if (action <= 4) {
	    if (!ss.occupy(*this, action-1, turnComments)) goto FINISH;
	  } else if (action <= 8) {
	    if (!ss.move(*this, action-5, turnComments)) goto FINISH;
	  } else if (action == 9) {
	    if (!ss.hide(turnComments)) goto FINISH;
	  } else if (action == 10) {
	    if (!ss.appear(turnComments)) goto FINISH;
	  }
	  fprintf(log, "%d ", action);
	}
      }
    }
  FINISH:
    fprintf(log, "0\n# Comments\n\"%s\"\n", turnComments.c_str());
    fflush(log);
    return;
  } else {
    turnComments += "Disqualified";
  }
 DEAD:
  ss.die(battleField);
  fprintf(log, "-1\n# Comments\n\"%s\"\n", turnComments.c_str());
  return;
}
