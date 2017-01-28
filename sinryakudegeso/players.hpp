#include <iostream>
#include <string>
#include <sstream>

using namespace std;
#define MAXWIDTH 20
#define MAXHEIGHT 20

struct CommentedIStream {
	istream* is;
	char commentChar = '#';
	CommentedIStream() {};
	CommentedIStream(istream &is);
	int get();
	void skipComments();
};

CommentedIStream& operator>>(CommentedIStream& cs, int& i);

void rotate(int direction, int x0, int y0, int& x, int& y);

struct SamuraiInfo {
	int homeX, homeY;
	int rank, score;
	int curX, curY;
	bool alive;
	int hidden;
	void homePosition(CommentedIStream& is);
	void readScoreInfo(CommentedIStream& is);
	void readTurnInfo(CommentedIStream& is);
};


struct Field {
	int width, height;
	int *field;
	Field(int width, int height);
	void clear();
	void clear(int num);
	void set(int x, int y, int value);
	int add(int x, int y, int value);
	int get(int x, int y);
	bool isinside(int x, int y);
	void dump(string msg);
	void dump2(string msg, Field *f);
	void dump3(string msg);
	void copy(Field *f);
	int calcdifffield(Field *f1, Field *f2, int pnum);
	int calcnotequalfieldnum(int num);
	int calcmax();
	void findnotequal(int num, int& x, int &y);
};

struct GameInfo {
	// Game information
	int turns;
	int side;
	int weapon;
	int width, height;
	int cureTurns;
	SamuraiInfo samuraiInfo[6];
	// Turn information
	int turn, curePeriod;

	//	int* field;

	Field *field;
	GameInfo(CommentedIStream& is);
	void readTurnInfo(CommentedIStream& is);
};

struct Player {
	virtual void play(GameInfo& info) = 0;
	int a[5];
	int anum = 0;
};

struct Playerdata {
	int hx, hy;		// 本拠地の位置
	int weapon;		// 武器の種類
	int side;		// 陣営

	int x, y;		// 現在位置（不明な場合は -1）
	int xbak, ybak;	// 現在位置のバックアップ 

	int px, py;		// 前の位置（不明な場合は -1)

	int hidden;		// 隠れているかどうか
	int hiddenbak;	// バックアップ

	bool isdead;		// 死んでいるかどうか（不明な場合は false）
	bool isdeadbak;	// バックアップ
	int deadturn;	// 死んだターン

	int diffplacex[14];	// 前のターンと比べて変化した場所
	int diffplacey[14];	// 前のターンと比べて変化した場所
	int diffcount;		// 変化したマスの数
	Field *difffield;	// 前のターンから変化したフィールドの場所


	Field *place;		// フィールド上の存在可能な位置(0: 存在不可能、1:存在して見えている、2:存在した場合必ず隠れている、3:存在した場合隠れているかどうか不明)
	Field *newplace;	// フィールド上の存在可能な新しい位置（上に同じ）
	Field *placebak, *placebak2;

	int actionnum;		// 前のターンから何回行動したか
	int nextactionnum;	// 次の行動回数
	bool calculated;	// すでに行動を計算済かどうか

	Field *occupiedcount;	// 変化した場所をすべて網羅する攻撃によって占領された回数(1以上の場合は、そこに攻撃する可能性がある）
	Field *firstoccupiedcount; // 一回目の占領された回数
	int occupiednum;		// 占領をチェックした回数
	Field *occupiedfield;	// 確実に占領した場所

	Field *attackrange;	// 攻撃可能範囲
	Field *attackplace; // 攻撃可能な場所
	int maxattackrange;

	int maybekilledturn[6];	// インデックス番号の他のプレーヤーによって殺された可能性があるターン（殺した可能性がなければ -1)。
	int killflag[6];		// 特定のプレーヤーを殺害したかどうか。0: 殺害していない、1: 100%殺害した、2: 殺害した可能性がある

	Playerdata(GameInfo &info);
	bool canhide(int num);
};

enum Status {
	NOTEXIST,		// 絶対に存在しない
	APPEARED,		// 存在し、見えている
	HIDDEN,			// 存在した場合は必ず隠れている
	UNKNOWN,		// 存在した場合は、隠れているかどうか不明（視界外）
};

void calcattacktable(GameInfo& info);
void analyse(GameInfo& info);
void calcplayeraction(GameInfo& info, Field *prevfield, Playerdata **pdata, int pnum, int turn, bool firstflag);
void calcmoveaction(int x, int y, Field *pfield, Field *cfield, Field *mcostfield, int currentcost, bool hidden, Playerdata **pdata, int pnum, int actionleft, int turnleft);
bool checkattack(int x, int y, Playerdata **p, int pnum, bool prevcheck, GameInfo &info, bool firstflag);
bool checkattack2(int x1, int y1, int x2, int y2, Playerdata **p, int pnum, int prevcheckdist, GameInfo &info, bool firstflag);
void calcoccupation(Playerdata **pdata, int pnum, int turn, bool changeisdeadflag);
void calckilledplayer(Playerdata **pdata, int pnum, GameInfo &info);
void setnewplace(Playerdata &p, Field *prevfield, int x, int y);
void setnewplace2(Playerdata &p, Field *prevfield, int x, int y);
void calcattackrange(GameInfo& info, Playerdata **pdata, Field& dzone);
void ai(GameInfo& info, Playerdata **pdata);
void calcaction(GameInfo &info, Playerdata **pdata, int x, int y, int hidden, int actionleft, int *action, int actionnum, int *finalaction, double& maxhyouka, int *killflag, bool attackflag, Field& currentdangerzone, int sikainum);
void debugprinttime(string str, int time);
void calcmoveplace(int x, int y, Playerdata **pdata, int pnum, Field &f, Field &checkfield, int dist, int *checknum);

extern bool logging;

// 評価値
#define HYOUKA_ENEOCCUPY 6			// 敵地を占領した
#define HYOUKA_ALLYOCCUPY 0.1		// 味方の占領地を占領した
#define HYOUKA_NEUTRALOCCUPY 3		// 中立地帯を占領した
#define HYOUKA_ENECANATTACKOCCUPY -0.5	// 敵が攻撃可能な場所を占領した
#define HYOUKA_NEXT_ENEOCCUPY 3		// 次のターンに敵地を占領できる
#define HYOUKA_NEXT_ALLYOCCUPY 0	// 次のターンの味方の占領地を占領できる
#define HYOUKA_NEXT_NEUTRALOCCUPY 2	// 次のターンに中立地帯を占領できる
#define HYOUKA_NEXT_UNKNOWNOCCUPY 1.5	// 次のターンに不明地帯を占領できる
const int HYOUKA_KILL[3] = { 150, 125, 100 };	// 敵を殺した場合の評価値
const int HYOUKA_KILL2[3] = { 150, 150, 150 };	// 敵を殺した場合の評価値（後半）
const int HYOUKA_KILLED[3] = { -150, -125, -100 }; // 敵に殺された場合の評価値の基本値
const int HYOUKA_KILLED2[3] = { -150, -150, -150 }; // 敵に殺された場合の評価値の基本値（後半）
#define HYOUKA_LATTERHALF	0.75			// 後半のターン
#define HYOUKA_NEXT_CANKILLRIVAL 50		// 次のターンにライバルを殺せる位置に移動できる
#define HYOUKA_ESTIMATE_ENEOCCUPY 0.2
#define HYOUKA_ESTIMATE_NEUTRALOCCUPY 0.1
#define HYOUKA_ESTIMATE_UNKNOWNOCCUPY 0.1
#define HYOUKA_ESTIMATE_RANGE 7
#define HYOUKA_DANGER_APPEARED 1
#define HYOUKA_DANGER_HIDDEN 0.5
#define HYOUKA_WARNING_APPEARED 0.5		// 敵の位置がわかっていない場合に、敵の攻撃可能地点に現れた状態でいる
#define HYOUKA_WARNING_HIDDEN 0.0		// 敵の位置がわかっていない場合に、敵の攻撃可能地点に隠れた状態でいる（下記参照）
#define HYOUKA_WARNING_HIDDEN2 0.3		// 敵の位置がわかっていない場合に、敵の攻撃可能地点に隠れた状態でいる（下記参照）
#define HYOUKA_WARNING_NONATTACK_HIDDEN_MINRATIO 0.1	// attackrange の値 / attackrange の最大値 がこの値を超えた場合に HIDDEN2 の値を使う
#define HYOUKA_WARNING_ATTACKED_HIDDEN_MINRATIO 0.05	// attackrange の値 / attackrange の最大値 がこの値を超えた場合に HIDDEN2 の値を使う
#define HYOUKA_SAFETY_APPEARED 0		// 安全な場所で隠れていない状態の評価値
#define HYOUKA_SIKAI 0.3				// 視界１に対する評価
#define HYOUKA_NANAME2_APPEARED	0		// 敵の斜め2の位置に出現している
#define HYOUKA_NANAME2_HIDDEN	20		// 敵の斜め2の位置に隠れている
#define HYOUKA_NANAME2_DANGER	-20		// 敵が隠れている可能性のある場所の斜め２の位置に現れている場合で、隠れている場所から攻撃される可能性の高い場所にいる
#define HYOUKA_ENESIKAIAPPEARED -5		// 敵の視界範囲内で出現している
#define HYOUKA_RANDOM 0.1				// 乱数による評価の最大値
