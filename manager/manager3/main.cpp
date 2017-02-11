#include "samurai.hpp"
#include <cctype>
#include <cstring>
#include <signal.h>

void encodeString(const char* s, string& str) {
  for (const char* p = s; *p != '\0' && *p != '\n'; p++) {
    char c = *p;
    if (c == '\\' || c == '\"') str += "\\";
    str += c;
  }
}

void initLog(FILE* out, GameState& gameState) {
  Setting& setting = gameState.setting;
  fprintf(out, "### SamurAI 3x3 Game Log ###\n"
	  "# <game name> <#turns> <width> <height>\n"
	  "\"SamurAI 3x3\" %d %d %d %d\n",
	  setting.turns, 
	  gameState.battleField.width, gameState.battleField.height,
	  setting.cureTurns);
  fprintf(out, "# Army sizes\n 3 3\n");
  fprintf(out, "# Samurai Info\n");
  for (int s = 0; s != 6; s++) {
    SamuraiState &ss = samuraiStates[s];
    fprintf(out, "# Army %d Samurai %d\n", s/3, s%3);
    fprintf(out, "\"%s\" %d %d\n", ss.nickname.c_str(), ss.rank, ss.score);
    Role& role = setting.roles[s];
    fprintf(out, "%d", (int)role.reach.size());
    for (Coordinates c: role.reach) {
      fprintf(out, " %d %d", c.x, c.y);
    }
    fprintf(out, "\n");
    fprintf(out, "%d %d %d %d\n",
	    role.vision, role.activity, role.homeX, role.homeY);
  }
}

FILE* dump;

[[noreturn]]
static void invOpt(char* argv[], int k, string msg = "Invalid option") {
  fprintf(stderr,
	  "%s: %s\n"
	  "Usage: %s [<option> ...]\n"
	  "%s -h for detailed help on options\n",
	  msg.c_str(), argv[k], argv[0], argv[0]);
  exit(-1);
}

[[noreturn]]
static void helpMessage(char* argv[]) {
  fprintf(stderr, "Usage: %s [<option> ...]\n", argv[0]);
  fprintf(stderr,
	  "  -h: Obtain this help message.\n"
          "AI program invocation and control options:\n"
	  "  -a <command>: Set invocation command for AI,\n"
	  "  -p <command>: Set command for posing an AI,\n"
	  "  -u <command>: Set command for resuming an AI.\n"
	  " The options -a, -p, and -u should be specified for six samurai."
	  "Debugging options:\n"
	  "  -l <path>: Game log output to the specified path,\n"
	  "  -d <path>: Dump AI program interface output.\n"
	  "     Output path to player N is concatenation of <path> and N.\n"
	  "     For example, when <path> is \"dump\", output goes to\n"
	  "     dump0 through dump5.\n"
	  "Output options:\n"
	  "  -t: plain text output instead of JSON format\n"
	  );
  exit(0);
}

static int integerOption(int argc, char* argv[], int& argIndex) {
  if (argIndex+1 >= argc) {
    invOpt(argv, argIndex,
		  "Argument required for option "+to_string(*argv[argIndex]));
  }
  return atoi(argv[++argIndex]);
}

static string stringOption(int argc, char* argv[], int& argIndex) {
  if (argIndex+1 >= argc) {
    invOpt(argv, argIndex,
		  "Argument required for option "+to_string(*argv[argIndex]));
  }
  char* arg = argv[++argIndex];
  if (*arg != '\"') return string(arg);
  string ret;
  while (*(++arg) != '\"') {
    if (*arg == '\\') ++arg;
    if (*arg == '\0') {
      invOpt(argv, 0, 
		    "Invalid option argument for option "
		    +string(argv[argIndex]));
    }
    ret += *arg;
  }
  return ret;
}

static FILE* openLog(string path, char* argv[], int argIndex) {
  FILE* ret = fopen(path.c_str(), "w");
  if (ret == nullptr) {
    invOpt(argv, argIndex, "Cannot open log file "+string(argv[argIndex]));
  }
}

FILE** ifs;

SamuraiState samuraiStates[6];
bool jsonOutput = true;

static void processOptions(int argc, char* argv[]) {
  int ax = 0;
  int px = 0;
  int ux = 0;
  int nx = 0;
  int sx = 0;
  int rx = 0;
  for (int argIndex = 1; argIndex < argc; argIndex++) {
    if (*argv[argIndex] != '-') invOpt(argv, argIndex);
    char* option = argv[argIndex] + 1;
    switch (option[0]) {
    case 'a': {
      if (ax >= 6) invOpt(argv, argIndex, "More than 6 -a options");
      samuraiStates[ax++].invokeCommand = stringOption(argc, argv, argIndex);
      break;
    }
    case 'p': {
      if (px >= 6) invOpt(argv, argIndex, "More than 6 -p options");
      samuraiStates[px++].pauseCommand = stringOption(argc, argv, argIndex);
      break;
    }
    case 'u': {
      if (ux >= 6) invOpt(argv, argIndex, "More than 6 -u options");
      samuraiStates[ux++].resumeCommand = stringOption(argc, argv, argIndex);
      break;
    }
    case 'n': {
      if (nx >= 6) invOpt(argv, argIndex, "More than 6 -n options");
      samuraiStates[nx++].nickname = stringOption(argc, argv, argIndex);
      break;
    }
    case 'r': {
      if (rx >= 6) invOpt(argv, argIndex, "More than 6 -r options");
      samuraiStates[rx++].rank = integerOption(argc, argv, argIndex);
      break;
    }
    case 's': {
      if (sx >= 6) invOpt(argv, argIndex, "More than 6 -s options");
      samuraiStates[sx++].score = integerOption(argc, argv, argIndex);
      break;
    }
    case 'l': {
      string optarg = stringOption(argc, argv, argIndex);
      dump = openLog(optarg, argv, argIndex);
      break;
    }
    case 'd': {
      string optarg = stringOption(argc, argv, argIndex);
      ifs = new FILE*[6];
      for (int s = 0; s != 6; s++) {
	ifs[s] = openLog(optarg+to_string(s), argv, argIndex);
      }
      break;
    }
    case 't': {
      jsonOutput = false;
      break;
    }
    case 'h':
      helpMessage(argv);
      break;
    default:
      invOpt(argv, argIndex);
    }
  }
  if (ax != 6) invOpt(argv, 0, "Six -a options should be specified");
  if (px != 6) invOpt(argv, 0, "Six -p options should be specified");
  if (ux != 6) invOpt(argv, 0, "Six -u options should be specified");
}

void finalReport(FILE* in, int scores[]) {
  printf("{\n"
	 "  \"scores\": [");
  char const *delimiter = "";
  for (int s = 0; s != 6; s++) {
    printf("%s%d", delimiter, scores[s]);
    delimiter = ", ";
  }
  printf("  ],\n"
	 "  \"log\": [],\n"
	 "  \"winner\": \"\",\n"
	 "  \"replay\": [");
  const int bufSize = 1024;
  char buf[bufSize];
  delimiter = "";
  while (fgets(buf, bufSize, in) != nullptr) {
    puts(delimiter);
    delimiter = ",";
    printf("    \"");
    string encoded;
    encodeString(buf, encoded);
    printf("%s", encoded.c_str());
    putchar('\"');
  }
  printf("\n"
	 "  ]\n"
	 "}\n");
  fflush(stdout);
}

static void handleSigpipe(int sig) {
  // fprintf(stderr, "SIGPIPE caught\n");
}

int main(int argc, char* argv[]) {
  signal(SIGPIPE, handleSigpipe);
  int turn = -1;
  try {
    GameState gameState;
    processOptions(argc, argv);
    gameState.init();
    const char* templateString = "SamuraiLogXXXXXX";
    char tempFileName[strlen(templateString)+1];
    strcpy(tempFileName, templateString);
    mkstemp(tempFileName);
    FILE* logOutput= (jsonOutput ? fopen(tempFileName, "w") : stdout);
    initLog(logOutput, gameState);
    int order[] = { 0, 3, 4, 1, 2, 5, 3, 0, 1, 4, 5, 2 };
    for (turn = 0; turn != gameState.setting.turns; turn++) {
      gameState.turn = turn;
      for (int s = 0; s != 6; s++) {
	samuraiStates[s].cure();
      }
      int samuraiUp = order[turn%12];
      int activeSide = samuraiUp/3;
      int activeSamurai = samuraiUp%3;
      if (dump) {
	fprintf(dump,
		"### Turn: %d; Side: %d; Samurai: %d; Actions:",
		turn, activeSide, activeSamurai);
      }
      FILE* toAI = samuraiStates[samuraiUp].toAI;
      gameState.sendTurnInfo(toAI, activeSide, activeSamurai);
      if (ifs != nullptr) {
	gameState.sendTurnInfo(ifs[samuraiUp], activeSide, activeSamurai);
	fflush(ifs[samuraiUp]);
      }
      SamuraiScanner& fromAI = samuraiStates[samuraiUp].fromAI;
      gameState.receiveActionCommands
	(fromAI, activeSide, activeSamurai, logOutput);
      if (dump) {
	fprintf(dump, "\n");
	int width = gameState.battleField.width;
	int height = gameState.battleField.height;
	FieldMap map = gameState.battleField.map;
	for (int y = 0; y != height; y++) {
	  for (int x = 0; x != width; x++) {
	    Section* sect = map.at(Coordinates(x, y));
	    fprintf(dump, " ");
	    if (sect->state < 0) {
	      fprintf(dump, ".");
	    } else {
	      fprintf(dump, "%d", sect->state);
	    }
	    if (sect->population != 0) {
	      fprintf(dump, "*");
	    } else {
	      fprintf(dump, " ");
	    }
	  }
	  fprintf(dump, "\n");
	}
	for (int a = 0; a != 2; a++) {
	  for (int w = 0; w != 3; w++) {
	    SamuraiState& ss = samuraiStates[samuraiUp];
	    fprintf(dump, "Samurai %d.%d@(%d,%d)",
		    ss.side, ss.weapon,
		    ss.position->coords.x, ss.position->coords.y);
	    if (ss.hidden) fprintf(dump, "; hiding");
	    if (ss.curePeriod != 0) {
	      fprintf(dump, "; resting: %d", ss.curePeriod);
	    }
	    fprintf(dump, "\n");
	  }
	}
	fflush(dump);
      }
    }
    for (int s = 0; s != 6; s++) {
      SamuraiState& ss = samuraiStates[s];
      fprintf(ss.toAI, "-1\n");
    }
    // Compute scores
    int scores[6];
    for (int s = 0; s != 6; s++) {
      scores[s] = 1;		// home position is a territory
    }
    for (int y = 0; y != gameState.battleField.height; y++) {
      for (int x = 0; x != gameState.battleField.width; x++) {
	Section* sect = gameState.battleField.section(x, y);
	if (sect->state >= 0) {
	  scores[sect->state]++;
	}
      }
    }
    int scoreA = scores[0]+scores[1]+scores[2];
    int scoreB = scores[3]+scores[4]+scores[5];
    int sx1, sx2;
    if (scoreA > scoreB) {
      scores[0] += 300; scores[1] += 300; scores[2] += 300;
    } else if (scoreA < scoreB) {
      scores[3] += 300; scores[4] += 300; scores[5] += 300;
    } else {
      for (int s = 0; s != 6; s++) scores[s] += 150;
    }
    if (jsonOutput) {
      fclose(logOutput);
      FILE* logInput = fopen(tempFileName, "r");
      finalReport(logInput, scores);
      remove(tempFileName);
    }
    return 0;
  } catch (ErrorReport err) {
    fprintf(stderr, "%s\n", err.msg.c_str());
    if (turn < 0) {
      fprintf(stderr, "Before game starts\n");
    } else {
      fprintf(stderr, "At turn %d\n", turn);
    }
    return -1;
  }
}
