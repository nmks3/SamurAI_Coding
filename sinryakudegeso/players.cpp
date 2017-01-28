#include "players.hpp"
#include <cstdlib>
#include <fstream>
#include <time.h>
#include <cmath>
#include <string.h>
#include <random>

// C++で書くは今回が初めてなので、STLなどは使っていません。
// また、Cっぽいところが多々あります。

// デバック用
//#define DEBUG
bool logging = false;

// 乱数を使うかどうか
#define USERANDOM
// デバッグ時は、デバッグ表示を行う変数を true にする
// また、データをファイルから読み込むようにする
#ifdef DEBUG
bool debug = true;
string path("C:/cygwin/home/samurai/manager/dump5");
#else
bool debug = false;
#endif

// データを標準入力から読み込む部分はサンプルコードと同じ。ただし、GameInfo のフィールドのみ、Field クラスのインスタンスとする
CommentedIStream::CommentedIStream(istream &is) :
is(&is) {
}

int CommentedIStream::get() {
	if (is->eof()) exit(0);
	return is->get();
}

void CommentedIStream::skipComments() {
	char c = get();
	while (isspace(c) || c == commentChar) {
		if (c == commentChar) {
			do {
				c = get();
			} while (c != '\n');
		}
		else {
			c = get();
		}
	}
	is->unget();
}

CommentedIStream& operator>>(CommentedIStream &cs, int &i) {
	cs.skipComments();
	*cs.is >> i;
	return cs;
}

void SamuraiInfo::homePosition(CommentedIStream& is) {
	is >> homeX >> homeY;
}

void SamuraiInfo::readScoreInfo(CommentedIStream& is) {
	is >> rank >> score;
}

void SamuraiInfo::readTurnInfo(CommentedIStream& is) {
	int state;
	is >> curX >> curY >> state;
	alive = (state >= 0);
	hidden = (alive ? state : 0);
}

GameInfo::GameInfo(CommentedIStream& is) {
	is >> turns >> side >> weapon
		>> width >> height >> cureTurns;
	field = new Field(width, height);
	for (SamuraiInfo& s : samuraiInfo) {
		s.homePosition(is);
	}
	for (SamuraiInfo& s : samuraiInfo) {
		s.readScoreInfo(is);
	}
	cout << 0 << endl;
}

void GameInfo::readTurnInfo(CommentedIStream& is) {
	is >> turn;
	if (turn < 0) exit(0);
	is >> curePeriod;
	if (logging) clog << "Samurai positions before turn " << turn;
	for (SamuraiInfo& s : samuraiInfo) {
		s.readTurnInfo(is);
		if (logging) clog << " (" << s.curX << "," << s.curY << ")"
			<< (s.hidden != 0 ? "*" : "");
	}
	if (logging) clog << endl;
	int fieldvalue;
	for (int y = 0; y != height; y++) {
		for (int x = 0; x != width; x++) {
			is >> fieldvalue;
			field->set(x, y, fieldvalue);
		}
	}
}

// フィールドのコンストラクタ
Field::Field(int w, int h) : width(w), height(h) {
	field = new int[width * height];
}

// フィールドの中身を 0 でクリアする
void Field::clear() {
	clear(0);
}

// フィールドの中身を num でクリアする
void Field::clear(int num) {
	// 0 でクリアする場合は、memset を使って高速に行う
	if (num == 0) {
		memset(field, num, sizeof(int) * width * height);
	}
	else {
		for (int i = 0; i < width * height; i++) {
			field[i] = num;
		}
	}
}

// フィールドの (x, y) の位置に value をセットする
void Field::set(int x, int y, int value) {
	// 範囲のチェック
	if (isinside(x, y) == false) {
		if (debug) {
			cout << "#Field::set out of field error (x, y) = (" << x << "," << y << ")" << endl;
		}
	}
	field[x + width * y] = value;
}


// フィールドの (x, y) の位置のデータに value を足す
int Field::add(int x, int y, int value) {
	// 範囲のチェック
	if (isinside(x, y) == false) {
		if (debug) {
			cout << "#Field::add out of field error (x, y) = (" << x << "," << y << ")" << endl;
		}
	}
	field[x + width * y] += value;
	return field[x + width * y];
}

// フィールドの (x, y) の位置のデータを取得する
int Field::get(int x, int y) {
	// 範囲のチェック
	if (isinside(x, y) == false) {
		if (debug) {
			cout << "#Field::get out of field error (x, y) = (" << x << "," << y << ")" << endl;
		}
		return 0;
	}
	return field[x + y * width];
}

// (x, y) がフィールドの内部かどうかを調べる
bool Field::isinside(int x, int y) {
	if (x < 0 || x >= width || y < 0 || y >= height) {
		return false;
	}
	else {
		return true;
	}
}

// msg をコメントとして表示してから、フィールドの情報をダンプする。
void Field::dump(string msg) {
	cout << "#" << msg << endl;
	for (int y = 0; y < height; y++) {
		cout << "#";
		for (int x = 0; x < width; x++) {
			cout << field[x + y * width] << " ";
		}
		cout << endl;
	}
}

// msg をコメントとして表示してから、フィールドの情報をダンプする。
// 視界外で存在しない場所は ? を表示する
void Field::dump2(string msg, Field *f) {
	cout << "#" << msg << endl;
	for (int y = 0; y < height; y++) {
		cout << "#";
		for (int x = 0; x < width; x++) {
			if (field[x + y * width] == 0) {
				if (f->get(x, y) != 9) {
					cout << "0";
				}
				else {
					cout << "?";
				}
			}
			else {
				cout << field[x + y * width];
			}
			cout << " ";
		}
		cout << endl;
	}
}

// msg をコメントとして表示してから、フィールドの情報をダンプする。
// ただし、フィールドの値が 0 でなければ 1 を、0 であれば 0 をダンプする
void Field::dump3(string msg) {
	cout << "#" << msg << endl;
	for (int y = 0; y < height; y++) {
		cout << "#";
		for (int x = 0; x < width; x++) {
			if (field[x + y * width] > 0) {
				cout << "1 ";
			}
			else {
				cout << "0 ";
			}
		}
		cout << endl;
	}
}

// フィールド f の内容を自身にコピーする
void Field::copy(Field *f) {
	memcpy(field, f->field, sizeof(int) * width * height);
}

// 前のフィールド prevf と 現在のフィールド currentf を比較し、pnum の色に変化した場所を自身のフィールドに記録し、変化した場所の数を返す。
// 具体的には以下の作業を行う。
// 前のフィールド prevf と 現在のフィールド currentf を比較し、
// フィールドの色（値）が異なっており、なおかつ、現在のフィールド（currentf）の色が pnum の場所を探し、その場所を 1 とする
// ただし、前のフィールドで、視界外(9)の場所は、そこが変化したかどうかがわからないため除く
int Field::calcdifffield(Field *prevf, Field *currentf, int pnum) {
	// 自身のフィールドをクリアする
	clear();
	// 変化したタイルの数を数える変数の初期化
	int count = 0;
	for (int i = 0; i < width * height; i++) {
		// 前のフィールドの (x, y) のデータ
		int pd = prevf->field[i];
		// 現在のフィールドの (x, y) のデータ
		int cd = currentf->field[i];
		// 色が変化し、前のフィールドで視界外でなく、pnum に変化した場合、その場所を 1 とし、変化した数を数える変数を 1 増やす
		if (pd != cd && pd != 9 && cd == pnum) {
			field[i] = 1;
			count++;
		}
	}
	return count;
}

// num でないマスを数える
int Field::calcnotequalfieldnum(int num) {
	int count = 0;
	for (int i = 0; i < width * height; i++) {
		if (field[i] != num) {
			count++;
		}
	}
	return count;
}

// (0, 0) から右に探していき、最初に num の値になった座標を x, y に代入する
void Field::findnotequal(int num, int& x, int &y) {
	int i = 0;
	for (int y2 = 0; y2 < height; y2++) {
		for (int x2 = 0; x2 < width; x2++) {
			if (field[i++] != num) {
				x = x2;
				y = y2;
				return;
			}
		}
	}
}

// フィールドのマスの中の最大値を返す
int Field::calcmax() {
	int max = field[0];
	for (int i = 0; i < width * height; i++) {
		if (field[i] > max) {
			max = field[i];
		}
	}
	return max;
}

// プレーヤーのデータのコンストラクタ
Playerdata::Playerdata(GameInfo &info) {
	// 各フィールドのインスタンスを作成する
	difffield = new Field(info.width, info.height);
	place = new Field(info.width, info.height);
	newplace = new Field(info.width, info.height);
	placebak = new Field(info.width, info.height);
	placebak2 = new Field(info.width, info.height);
	occupiedcount = new Field(info.width, info.height);
	firstoccupiedcount = new Field(info.width, info.height);
	occupiedfield = new Field(info.width, info.height);
	attackrange = new Field(info.width, info.height);
	attackplace = new Field(info.width, info.height);
}

// num の場所に隠れられるかどうか。視界外は隠れられるとする
bool Playerdata::canhide(int num) {
	if ((side * 3 <= num && num < side * 3 + 3) || num == 9) {
		return true;
	}
	else {
		return false;
	}
}

// フィールドの最大幅、高さ
#define MAXWIDTH 20
#define MAXHEIGHT 20
// 大域変数のフィールドのポインタ。それぞれの意味は、main のインスタンスの生成の部分を参照
Field *prevfield, *prevfieldbak, *tmpfield, *checkfield, *movecostfield, *fieldchecked, *fieldchecked2, *dangerzone, *estimatefield, *homefield, *sikaifield, *naname2dangerfield;

// インデックスをそれぞれ x1, y1, x2, y2, weapon, adir1, adir2 とする
// 自分に対して2回連続行動した敵が、(x1, y1) の位置から adir1 方向に、(x2, y2) の位置から adir2 方向に weapon の武器で攻撃した場合の攻撃範囲を格納するテーブル
// 攻撃不能な場合は NULL が格納される。
// 攻撃可能な場合は、攻撃可能なマスの座標が x座標, y座標 の順で格納され、最後に -1 が格納される
int *attacktable2[MAXWIDTH][MAXHEIGHT][MAXWIDTH][MAXHEIGHT][3][4][4];
// 時間を格納する変数
clock_t prevclock;
// 各プレーヤーのデータを格納する変数
Playerdata *pdata[6];
// 各ターンにどのプレーヤーが行動可能かを表すテーブルの元。実際はプレーヤーの side によって変化するのでそれを計算して turnplayer に格納する
int turnplayerorig[12] = { 0, 3, 4, 1, 2, 5, 3, 0, 1, 4, 5, 2 };
int turnplayer[12];
// 自身が行動可能な一つ前のターン
int prevturn;
int fieldnum;
int *checkattacktable, *checkattacktable2;
int action[7];
int finalaction[7];	// 最終的に決定されたそのターンの行動
int onum[10], eonum[10]; // 占領された数、推定フィールドで占領された数
int allyfieldnum, enefieldnum, neutralfieldnum, potentialenefilednum;

// 乱数に関する変数と設定
random_device rd;
mt19937 mt(rd());
uniform_real_distribution<double> rnd(0.0, 1.0);

// 槍、刀、鉞の占領区域数
int attacknum[3] = { 4, 5, 7 };
// 槍、刀、鉞の占領区域
int attackplacex[3][4][7] = {
	// 槍
	{
		// 南、東、北、西の順
		{ 0, 0, 0, 0 },
		{ 1, 2, 3, 4 },
		{ 0, 0, 0, 0 },
		{ -1, -2, -3, -4 }
	},
	// 刀
	{
		{ 2, 1, 1, 0, 0 },
		{ 0, 0, 1, 1, 2 },
		{ -2, -1, -1, 0, 0 },
		{ 0, 0, -1, -1, -2 }
	},
	// 鉞
	{
		{ 1, 1, 1, 0, -1, -1, -1 },
		{ -1, 0, 1, 1, 1, 0, -1 },
		{ -1, -1, -1, 0, 1, 1, 1 },
		{ 1, 0, -1, -1, -1, 0, 1 }
	}
};

int attackplacey[3][4][7] = {
	// 槍
	{
		// 南、東、北、西の順
		{ 1, 2, 3, 4 },
		{ 0, 0, 0, 0 },
		{ -1, -2, -3, -4 },
		{ 0, 0, 0, 0 },
	},
	// 刀
	{
		{ 0, 0, 1, 1, 2 },
		{ -2, -1, -1, 0, 0 },
		{ 0, 0, -1, -1, -2 },
		{ 2, 1, 1, 0, 0 }
	},
	// 鉞
	{
		{ -1, 0, 1, 1, 1, 0, -1 },
		{ -1, -1, -1, 0, 1, 1, 1 },
		{ 1, 0, -1, -1, -1, 0, 1 },
		{ 1, 1, 1, 0, -1, -1, -1 }
	}
};

int main(int argc, char* argv[]) {
	if (argc >= 2 && strcmp(argv[1], "-d") == 0) {
		logging = true;
	}
	// 開始時間を記録しておく
	clock_t startclock = clock();
	prevclock = clock();

// デバッグ時は、ファイルからデータを読み込む
#ifdef DEBUG
	ifstream ifs(path);
	CommentedIStream in(ifs);
#else
	CommentedIStream in(cin);
#endif

	GameInfo info(in);
	fieldnum = info.width * info.height;
	// 各大域変数のフィールドのインスタンスの生成
	// 前に自身が行動した直後のフィールドデータ（以下前のフィールドと記す）
	prevfield = new Field(info.width, info.height);
	// バックアップ用のフィールド
	prevfieldbak = new Field(info.width, info.height);
	// 様々な用途で一時的に使用するフィールド
	tmpfield = new Field(info.width, info.height);
	// チェック用のフィールド
	checkfield = new Field(info.width, info.height);
	// 移動コストを格納するフィールド
	movecostfield = new Field(info.width, info.height);
	// 
	fieldchecked = new Field(info.width, info.height);
	fieldchecked2 = new Field(info.width, info.height);
	// 次に自分のターンが来るまでに、敵が攻撃可能な場所を表すフィールド。プレーヤー3,4,5が攻撃可能な場合、それぞれ1,2,4のビットで表現する
	dangerzone = new Field(info.width, info.height);
	// その時点での占領状況を推定するフィールド。通常のフィールドデータに加え、敵が占領した可能性がある場所を 7 で表す
	estimatefield = new Field(info.width, info.height);
	// 自分の斜め２の位置に敵がいると仮定した場合の危険地帯
	naname2dangerfield = new Field(info.width, info.height);
	// 関数 checkattack で (x, y) での攻撃を計算済の場合、その結果を格納するテーブル
	checkattacktable = new int[info.width * info.height];
	// 関数 checkattack2 で (x1, y1), (x2, y2) での攻撃を計算済の場合、その結果を格納するテーブル
	checkattacktable2 = new int[info.width * info.height * info.width * info.height];
	// 本拠地のマスは、そのマスを本拠地とするプレーヤーの番号、そうでなければ　-1 を格納するフィールド
	// (そのマスが本拠地かどうかを簡単に調べるために使われる）
	homefield = new Field(info.width, info.height);
	// 本拠地のマスのデータは後で設定するので、最初はすべて -1 を設定しておく
	homefield->clear(-1);

	sikaifield = new Field(info.width, info.height);
	estimatefield->clear(8);
	// attacktable2 を生成する関数を呼ぶ
	calcattacktable(info);

	// 各ターン（12で割った余り）の行動可能なプレーヤーの番号を表す配列の設定
	// 自身の陣営によって変わってくるので計算する
	for (int i = 0; i < 12; i++) {
		if (info.side == 0) {
			turnplayer[i] = turnplayerorig[i];
		}
		else {
			turnplayer[i] = (turnplayerorig[i] + 3) % 6;
		}
	}
	// 前のフィールドの初期化（すべて最初は占領されていない状態にする）
	prevfield->clear(8);
	// 各プレーヤーの初期情報の設定
	for (int i = 0; i < 6; i++) {
		pdata[i] = new Playerdata(info);
		// 前の位置の設定（本拠地）
		pdata[i]->px = info.samuraiInfo[i].homeX;
		pdata[i]->py = info.samuraiInfo[i].homeY;
		// 本拠地の設定
		pdata[i]->hx = pdata[i]->px;
		pdata[i]->hy = pdata[i]->py;
		// homefield に本拠地のマスの値を設定する
		homefield->set(pdata[i]->hx, pdata[i]->hy, i);
		// 陣営
		pdata[i]->side = static_cast<int>(round(i / 3));
		// 前のフィールドで、本拠地にいることにする
		prevfield->set(pdata[i]->px, pdata[i]->py, i);
		// フィールド上で存在可能な位置をクリアする
		pdata[i]->place->clear(Status::NOTEXIST);
		// 本拠地に存在することにする
		pdata[i]->place->set(pdata[i]->px, pdata[i]->py, Status::APPEARED);
		// 武器の設定
		pdata[i]->weapon = i % 3;
		// 最初は死亡していない
		pdata[i]->isdead = false;
		// 他のプレーヤーによって殺された可能性のあるターン数の初期化
		for (int j = 0; j < 6; j++) {
			pdata[i]->maybekilledturn[j] = -1;
		}
		pdata[i]->occupiedcount->clear();
		pdata[i]->x = pdata[i]->hx;
		pdata[i]->y = pdata[i]->hy;
		pdata[i]->hidden = 0;
		pdata[i]->newplace->clear();
		pdata[i]->newplace->set(pdata[i]->x, pdata[i]->y, Status::APPEARED);
	}
	// 一つ前のターンを -1 としておく
	prevturn = -1;
	// 初期設定にかかった時間を計算して表示する
	clock_t currentclock = clock();
	cout << "#first time " << (currentclock - prevclock) << "ms" << endl;
	prevclock = currentclock;

	// ここから各自分のターンに与えられたデータを解析し、自分の行動を決定する
	while (true) {
		// ターンの情報を読み込む（サンプルコードと同じ）
		info.readTurnInfo(in);
		// 途中から開始した場合の処理（デバッグ用。通常時は使わない）
#if 0
		if (prevturn == -1 && info.turn != 0) {
			prevfield->copy(info.field);
			prevturn = info.turn;
			for (int i = 0; i < 6; i++) {
				pdata[i]->x = info.samuraiInfo[i].curX;
				pdata[i]->y = info.samuraiInfo[i].curY;
				pdata[i]->place->clear();
				if (pdata[i]->x >= 0) {
					pdata[i]->place->set(pdata[i]->x, pdata[i]->y, 1);
				}
				else {
					for (int y = 0; y < pdata[i]->place->height; y++){
						for (int x = 0; x < pdata[i]->place->width; x++){
							if (info.field->get(x, y) == 9) {
								pdata[i]->place->set(x, y, 1);
							}
						}
					}
				}
			}
			continue;
		}
#endif
		// 前のターンと情報が変化しなくなったら終了（デバッグ時はこちらで終了判定する）
		if (info.turn == prevturn) {
			clock_t endclock = clock();
			cout << "#total time " << (endclock - startclock) << " ms" << endl;
			exit(0);
			break;
		}
		// ターンのデバッグ表示
		cout << "####### Turn " << info.turn << endl;
		// 前の自分の行動直後の情報と、今回得られた情報から、敵味方の行動を分析する関数を呼ぶ
		analyse(info);

		// 各プレーヤーの位置、死亡状況などをデバッグ表示
		if (debug) {
			for (int i = 0; i < 6; i++) {
				cout << "#Player " << i << " (" << pdata[i]->x << ", " << pdata[i]->y << ") hidden " << pdata[i]->hidden << ":" << info.samuraiInfo[i].hidden;
				if (pdata[i]->isdead == true) {
					cout << " dead turn = " << pdata[i]->deadturn;
				}
				cout << endl;
			}
		}

		// 死亡していた場合は、 0 を出力して終了
		if (info.curePeriod != 0) {
			cout << "0" << endl;
			// そのターンの行動を 0 とする
			finalaction[0] = 0;
		}
		else {
			// 自身の行動を計算する関数を呼ぶ
			clock_t c1 = clock();
			ai(info, pdata);
			clock_t c2 = clock();
			// 計算にかかった時間をデバッグ表示する
			cout << "#ai thinking time " << (c2 - c1) << "ms" << endl;
			if (debug) {
				cout << "#Turn " << info.turn << " command " << endl;
			}
			// 自身の行動を出力する
			int i = 0;
			while (finalaction[i] > 0) {
				cout << finalaction[i++] << " ";
			}
			cout << "0\n";
		}
		// 自身のデータを更新する
		int pnum = info.weapon;
		// 行動前の位置を記録する
		pdata[pnum]->x = info.samuraiInfo[pnum].curX;
		pdata[pnum]->y = info.samuraiInfo[pnum].curY;
		const int dx[4] = { 0, 1, 0, -1 };
		const int dy[4] = { 1, 0, -1, 0 };
		// 自身が占領したフィールドをクリアする
		pdata[pnum]->occupiedfield->clear();
		// 自身の行動をデータに反映させる
		for (int i = 0; i < 7; i++) {
			int action = finalaction[i];
			if (action == 0) {
				break;
			}
			// 移動
			if (action >= 5 && action <= 8) {
				pdata[pnum]->x += dx[action - 5];
				pdata[pnum]->y += dy[action - 5];
			}
			// 攻撃
			else if (action >= 1 && action <= 4) {
				for (int j = 0; j < attacknum[info.weapon]; j++) {
					int x2 = pdata[pnum]->x + attackplacex[info.weapon][action - 1][j];
					int y2 = pdata[pnum]->y + attackplacey[info.weapon][action - 1][j];
					if (info.field->isinside(x2, y2) == true) {
						// 占領した場所のフィールドの情報を更新する
						info.field->set(x2, y2, pnum);
						// 占領した場所の、推定フィールドの情報を更新する
						estimatefield->set(x2, y2, pnum);
						// 自身が占領した場所を表すフィールドの情報を更新する
						pdata[pnum]->occupiedfield->set(x2, y2, 1);
					}
				}
			}
		}

		// 自身が占領した場所、自身が行動した後のフィールド、推定フィールドをデバッグ表示する
		if (debug) {
			pdata[pnum]->occupiedfield->dump("occupied field");
			info.field->dump("field");
			estimatefield->dump("estimate field");
		}
		if (logging) clog.flush();
		clock_t currentclock = clock();
		// 思考にかかった時間をデバッグ表示する
		cout << "#time " << (currentclock - prevclock) << "ms" << endl;
		// 時間制限を超えていた場合のデバッグ表示
		if ((currentclock - prevclock) > 100) {
			cout << "#time over!!!!!" << endl;
		}
		prevclock = currentclock;
		cout.flush();

		// 現在のターンを前のターンとする
		prevturn = info.turn;
		// 前のフィールド情報を、現在のフィールドの情報でコピーする
		prevfield->copy(info.field);
	}
}

// 自分に対して2回連続行動した敵が、(x1, y1) の位置から adir1 方向に、(x2, y2) の位置から adir2 方向に weapon の武器で攻撃した場合の攻撃範囲を格納するテーブルを生成する
// 攻撃不能な場合は NULL が格納される。
// 攻撃可能な場合は、攻撃可能なマスの座標が x座標, y座標 の順で格納され、最後に -1 が格納される
void calcattacktable(GameInfo& info) {
	for (int y1 = 0; y1 < info.height; y1++) {
		for (int x1 = 0; x1 < info.width; x1++) {
			for (int y2 = 0; y2 < info.height; y2++) {
				for (int x2 = 0; x2 < info.width; x2++) {
					for (int w = 0; w < 3; w++) {
						for (int d1 = 0; d1 < 4; d1++) {
							for (int d2 = 0; d2 < 4; d2++) {
								attacktable2[x1][y1][x2][y2][w][d1][d2] = NULL;
								// 2回攻撃した場合、(x1, y1) と (x2, y2) の距離は最大でも 2 なのでそれ以下の場合のみ計算する
								if (abs(x1 - x2) + abs(y1 - y2) <= 2) {
									int *data;
									data = attacktable2[x1][y1][x2][y2][w][d1][d2] = new int[100];
									// 攻撃した場所を tmpfield に格納する
									tmpfield->clear();
									for (int i = 0; i < attacknum[w]; i++) {
										// (x1, y1) からの攻撃
										int x = x1 + attackplacex[w][d1][i];
										int y = y1 + attackplacey[w][d1][i];
										// 2回の攻撃で重複して攻撃する場合があるので、まだその場所に攻撃していない場合のみ記録する
										if (tmpfield->isinside(x, y) == true && tmpfield->get(x,y) == 0) {
											tmpfield->set(x, y, 1);
											// 攻撃する座標を記録する
											*(data++) = x;
											*(data++) = y;
										}
										// (x2, y2) 空の攻撃
										x = x2 + attackplacex[w][d2][i];
										y = y2 + attackplacey[w][d2][i];
										if (tmpfield->isinside(x, y) == true && tmpfield->get(x, y) == 0) {
											tmpfield->set(x, y, 1);
											*(data++) = x;
											*(data++) = y;
										}
									}
									// データの終了を表す -1 を記録する
									*data = -1;
								}
							}
						}
					}
				}
			}
		}
	}

}

// 前の自分の行動直後の情報と、今回得られた情報から、敵味方の行動を分析する
void analyse(GameInfo& info){
	// 現在のフィールド、推定フィールドでのマスの数字の数を数える変数の初期化
	for (int i = 0; i <= 9; i++) {
		onum[i] = 0;
		eonum[i] = 0;
	}
	// 味方の占領数
	allyfieldnum = 0;
	// 敵の占領数
	enefieldnum = 0;
	// 中立地帯の数
	neutralfieldnum = 0;
	// 敵に占領された可能性のあるマスの数
	potentialenefilednum = 0;
	// 上記を推定フィールドの情報で計算する
	for (int y = 0; y < info.field->height; y++) {
		for (int x = 0; x < info.field->width; x++) {
			// 本拠地は必ずその色で占領されているので、推定フィールドの本拠地の色を設定する
			if (homefield->get(x, y) >= 0) {
				estimatefield->set(x, y, homefield->get(x, y));
			}
			else if (info.field->get(x, y) != 9) {
				estimatefield->set(x, y, info.field->get(x, y));
			}
			onum[info.field->get(x, y)]++;
			eonum[estimatefield->get(x, y)]++;
			switch (estimatefield->get(x, y)) {
			case 0:
			case 1:
			case 2:
				allyfieldnum++;
				break;
			case 3:
			case 4:
			case 5:
				enefieldnum++;
				break;
			case 7:
				potentialenefilednum++;
				break;
			case 8:
				neutralfieldnum++;
				break;
			}
		}
	}
	if (debug) {
		// 前のフィールドの情報をダンプする
		cout << "#filednum " << allyfieldnum << "," << enefieldnum << "," << potentialenefilednum << "," << neutralfieldnum << endl;
		cout << "#fieldnum2 ";
		for (int i = 0; i <= 9; i++) {
			cout << eonum[i] << " ";
		}
		cout << endl;
		prevfield->dump("prev field");
		// 現在のフィールドの情報をダンプする
		info.field->dump("current field");
		// 推定フィールドの情報をダンプする
		estimatefield->dump("current estimate field");
	}
	// 失格になっているプレーヤーは死亡とみなす
	for (int i = 0; i < 6; i++) {
		if (info.samuraiInfo[i].alive == false) {
			pdata[i]->isdead = true;
			pdata[i]->deadturn = 1000000;
		}
	}
	// プレーヤーの前の時点からの行動回数を計算する
	// 行動回数、次の行動回数、計算済フラグを初期化する
	for (int i = 0; i < 6; i++) {
		pdata[i]->actionnum = 0;
		pdata[i]->nextactionnum = 0;
		pdata[i]->calculated = false;
	}
	// 前の時点からの行動回数を計算する
	for (int i = prevturn + 1; i < info.turn; i++) {
		pdata[turnplayer[i % 12]]->actionnum++;
	}
	// 次の自分のターンまでの他のプレーヤーの行動回数を計算する
	int i = info.turn + 1;
	while (i <= info.turns) {
		int pnum = turnplayer[i % 12];
		// 自分の番が回ってきたら終了
		if (pnum == info.weapon) {
			break;
		}
		else {
			// そのターンに生きていれば行動回数を増やす
			if (pdata[pnum]->isdead == false || i > pdata[pnum]->deadturn + info.cureTurns) {
				pdata[pnum]->nextactionnum++;
			}
		}
		i++;
	}
	// 前のフィールドの状態を取っておく
	prevfieldbak->copy(prevfield);
	// 死亡状態と存在場所を取っておく
	for (int i = 0; i < 6; i++){
		pdata[i]->isdeadbak = pdata[i]->isdead;
		pdata[i]->xbak = pdata[i]->x;
		pdata[i]->ybak = pdata[i]->y;
		pdata[i]->hiddenbak = pdata[i]->hidden;
	}
	// 前のターンから、現在のターンの間のプレーヤーの行動を計算する。精度を高めるために2回計算する。
	// まず 1回目の計算を行う
	for (int i = prevturn + 1; i < info.turn; i++) {
		calcplayeraction(info, prevfield, pdata, turnplayer[i % 12], i, true);
	}
	// 前のフィールドの状態を戻す
	prevfield->copy(prevfieldbak);
	// 計算済フラグをクリアする
	// 死亡状態を戻す
	// 一回目に計算した占領した場所を firstoccupiedcount にコピーする
	for (int i = 0; i < 6; i++){
		pdata[i]->calculated = false;
		pdata[i]->isdead = pdata[i]->isdeadbak;
		pdata[i]->x = pdata[i]->xbak;
		pdata[i]->y = pdata[i]->ybak;
		pdata[i]->hidden = pdata[i]->hiddenbak;
		pdata[i]->firstoccupiedcount->copy(pdata[i]->occupiedcount);
		if (debug) {
			cout << "#" << i << endl;
			pdata[i]->firstoccupiedcount->dump("first occupiedcount");
		}
	}
	// 前のターンから、現在のターンの間のプレーヤーの行動を計算する。
	// 2回目 は 1回目で得られた占領可能な場所の情報を元に、より精度の高い計算を行う
	for (int i = prevturn + 1; i < info.turn; i++) {
		calcplayeraction(info, prevfield, pdata, turnplayer[i % 12], i, false);
	}
	// 攻撃範囲を計算する
	calcattackrange(info, pdata, *dangerzone);
}

// 移動の行動を計算する。現在地が不明な場合に呼ばれるので、視界内で見えている場合は考慮しない。
// (x, y):	現在の調べている場所
// pfield:	前のフィールド, cfield: 現在のフィールド, mcostfield: 移動コストを表すフィールド
// currentcost: この位置にたどり着くまでに必要なコスト
// hidden:	隠れているかどうか
// p:		調べているプレーヤーデータ
// actionleft:	残り行動力
// turnleft:	残りのターン（行動回数）
void calcmoveaction(int x, int y, Field *pfield, Field *cfield, Field *mcostfield, int currentcost, bool hidden, Playerdata **pdata, int pnum, int actionleft, int turnleft) {
	Playerdata &p = *pdata[pnum];
	// 行動回数が残っていなければ終了
	if (turnleft <= 0) {
		return;
	}
	// 行動力が負の場合終了する
	if (actionleft < 0) {
		return;
	}
	// フィールド内でなければ終了
	if (pfield->isinside(x, y) == false) {
		return;
	}
	// 他のプレーヤーの出現位置には入れない
	for (int i = 0; i < 6; i++) {
		if (i != pnum && pdata[i]->x == x && pdata[i]->y == y && pdata[i]->hidden == 0 && hidden == 0) {
			return;
		}
	}
	// 本拠地は本人以外入れない。ここで計算するのは敵の行動なので、p.weapon + 3 が敵の番号となる
	int hnum = homefield->get(x, y);
	if (hnum >= 0 && hnum != p.weapon + 3) {
		return;
	}
	// 現在と前のフィールドの状態を取得する
	int cfnum = cfield->get(x, y);
	// 隠れている状態で、その場所が現在隠れられなければ、姿を現す必要がある
	// estimatefield でやる必要がある？（todo）
	if (hidden == true && p.canhide(cfnum) == false) {
		// 現れるための行動力がなければ終了
		if (actionleft == 0) {
			return;
		}
		// 1コスト消費して出現する
		actionleft--;
		currentcost++;
		hidden = false;
	}
	// ここにたどり着くための移動コストが、mcostfield に記録された移動コスト以上であれば、最短ルートではないので終了
	if (mcostfield->get(x, y) <= currentcost) {
		return;
	}
	// この位置にたどり着くまでに必要な移動コストを更新する
	mcostfield->set(x, y, currentcost);
	// その場所に現在の状態で隠れることができる場合
	if (p.canhide(cfnum) == true) {
		// 視界外の場合は、不明の状態で存在可能
		if (cfnum == 9) {
			p.newplace->set(x, y, Status::UNKNOWN);
		}
		// 視界内の場合は、隠れている状態で存在可能
		else if (hidden == true || actionleft > 0) {
			p.newplace->set(x, y, Status::HIDDEN);
		}
	}
	// 移動するだけの行動力が無く、行動ターン数が残っている場合は、行動ターン数を減らしてこの場所から続きを実行する
	if (actionleft < 2){
		if (turnleft > 1) {
			actionleft = 7;
			turnleft--;
		}
		else {
			return;
		}
	}
	// 移動コストを減らして上下左右に移動する
	calcmoveaction(x + 1, y, pfield, cfield, mcostfield, currentcost + 2, hidden, pdata, pnum, actionleft - 2, turnleft);
	calcmoveaction(x - 1, y, pfield, cfield, mcostfield, currentcost + 2, hidden, pdata, pnum, actionleft - 2, turnleft);
	calcmoveaction(x, y + 1, pfield, cfield, mcostfield, currentcost + 2, hidden, pdata, pnum, actionleft - 2, turnleft);
	calcmoveaction(x, y - 1, pfield, cfield, mcostfield, currentcost + 2, hidden, pdata, pnum, actionleft - 2, turnleft);
}


// (x, y) の位置から発射した場合、変化した場所をすべて網羅するかどうかを調べ、
// 網羅していた場合は、占領した場所の数値を1増やす
// prevcheck: 前のターンにそこにいたかどうかを確認する
// 網羅していた場合は true を返す
bool checkattack(int x, int y, Playerdata **pdata, int pnum, bool prevcheck, GameInfo &info, bool firstflag) {
	Playerdata &p = *pdata[pnum];
	bool retval = false;
	// フィールドの外の場合は終了
	if (p.difffield->isinside(x, y) == false) {
		return retval;
	}
	// 前の状態で、そこにいなかった場合は終了
	if (prevcheck == true && p.place->get(x, y) == Status::NOTEXIST) {
		return retval;
	}
	// 既にチェック済の場合、テーブルの値を使用する
	int index = x + y * p.place->width;
	if (checkattacktable[index] == 1) {
		return true;
	}
	else if (checkattacktable[index] == 2) {
		return false;
	}
	// 東西南北にその位置から攻撃した場合、変化した場所を何個占領するかを数える
	for (int i = 0; i < 4; i++) {
		int count = 0;
		for (int j = 0; j < attacknum[p.weapon]; j++) {
			int x2 = x + attackplacex[p.weapon][i][j];
			int y2 = y + attackplacey[p.weapon][i][j];
			// 範囲外の場合は飛ばす
			if (p.difffield->isinside(x2, y2) == false) {
				continue;
			}
			// そのマスの色が自分の色に変化していた場合
			if (p.difffield->get(x2, y2) > 0) {
				count++;
			}
			else {
				// 誰かの本拠地の場合は飛ばす
				if (homefield->get(x2, y2) >= 0) {
					continue;
				}
				// 武器で占領する場所の現在の状態が自分の色、視界外の場合はOK
				// プレーヤーの色、未占領（8)の場合は、そこに攻撃した後に、その色になることはありえないので採用しない
				// 2回目のチェックで、味方の色で、その味方がそこに攻撃不能の場合も採用しない
				int fnum = info.field->get(x2, y2);
				if (fnum != pnum && fnum != 9 && (fnum == 8 || fnum == info.weapon || (firstflag == false && fnum < 3 && pdata[fnum]->firstoccupiedcount->get(x2, y2) == 0))) {
					count = 0;
					break;
				}
			}
		}
		// 変化した場所をすべて占領していた場合は、占領した場所の回数を1増やす
		// 変化した場所がない場合は、そこに攻撃した可能性があるということで、occupiedcount を増やしておく。
		// ただし、occupiednum は増やさないし、返り値も true にはしない
		if (count == p.diffcount || p.diffcount == 0) {
			for (int j = 0; j < attacknum[p.weapon]; j++) {
				int x2 = x + attackplacex[p.weapon][i][j];
				int y2 = y + attackplacey[p.weapon][i][j];
				if (p.difffield->isinside(x2, y2)) {
					p.occupiedcount->add(x2, y2, 1);
				}
			}
			if (p.diffcount > 0) {
				p.occupiednum++;
				retval = true;
			}
		}
	}
	if (retval == true) {
		checkattacktable[index] = 1;
	}
	else {
		checkattacktable[index] = 2;
	}
	return retval;
}

// (x1, y1) と (x2, y2)の位置から発射した場合、変化した場所をすべて網羅するかどうかを調べ、
// 網羅していた場合は、占領した場所の数値を1増やす
// prevcheckdist: 前のターンに(x1, y1) から prevcheckdist の範囲内にいたかどうかを確認する
bool checkattack2(int x1, int y1, int x2, int y2, Playerdata **pdata, int pnum, int prevcheckdist, GameInfo &info, bool firstflag) {
	Playerdata &p = *pdata[pnum];
	bool retval = false;
	// フィールドの外の場合は終了
	if (p.difffield->isinside(x1, y1) == false) {
		return false;
	}
	if (p.difffield->isinside(x2, y2) == false) {
		return false;
	}
	// 前の状態で、prevcheckdist の範囲内にいなかった場合は終了
	if (prevcheckdist >= 0){
		bool existflag = false;
		for (int dx = -prevcheckdist; dx <= prevcheckdist; dx++) {
			for (int dy = -prevcheckdist; dy <= prevcheckdist; dy++) {
				int x = x1 + dx;
				int y = y1 + dy;
				if (abs(dx) + abs(dy) <= prevcheckdist && p.place->isinside(x, y) == true) {
					if (p.place->get(x1, y1) != Status::NOTEXIST) {
						existflag = true;
						dx = prevcheckdist;
						break;
					}
				}
			}
		}
		if (existflag == false) {
			return false;
		}
	}
	// (x1, y1) と (x2, y2) を入れ替えても結果は同じ
	int index = x1 + y1 * p.place->width + x2 * p.place->width * p.place->height + y2 * p.place->width * p.place->height * p.place->width;
	int index2 = x2 + y2 * p.place->width + x1 * p.place->width * p.place->height + y1 * p.place->width * p.place->height * p.place->width;
	if (checkattacktable2[index] == 1 || checkattacktable2[index2] == 1) {
		return true;
	}
	else if (checkattacktable2[index] == 2 || checkattacktable2[index2] == 2) {
		return false;
	}

	// 東西南北にその位置から攻撃した場合、変化した場所を何個占領するかを数える
	// 一回目の発射方向
	for (int i = 0; i < 4; i++) {
		// 二回目の発射方向
		for (int j = 0; j < 4; j++) {
			int *data = attacktable2[x1][y1][x2][y2][p.weapon][i][j];
			// ありえないが一応チェックしておく
			if (data == NULL) {
				cout << "#error!!!!!!!" << x1 << "," << y1 << ", " << x2 << "," << y2 << "," << p.weapon  << "," << i << "," << j << endl;
				continue;
			}
			else {
				int count = 0;
				while (*data >= 0) {
					int x = *(data++);
					int y = *(data++);
					// そのマスの色が自分の色に変化していた場合
					if (p.difffield->get(x, y) > 0) {
						count++;
					}
					else {
						// 誰かの本拠地の場合は飛ばす
						if (homefield->get(x, y) >= 0) {
							continue;
						}
						// 武器で占領する場所の現在の状態が自分の色、視界外の場合はOK
						// プレーヤーの色、未占領（8)の場合は、そこに攻撃した後に、その色になることはありえないので採用しない
						// 2回目のチェックで、味方の色で、その味方がそこに攻撃不能の場合も採用しない
						int fnum = info.field->get(x, y);
						if (fnum != pnum && fnum != 9 && (fnum == 8 || fnum == info.weapon || (firstflag == false && fnum < 3 && pdata[fnum]->firstoccupiedcount->get(x, y) == 0))) {
							count = 0;
							break;
						}
					}
				}
				// 変化した場所をすべて占領していた場合は、一回目の占領した場所の回数を1増やす
				// 変化した場所がない場合は、そこに攻撃した可能性があるということで、occupiedcount を増やしておく。
				// ただし、occupiednum は増やさないし、返り値も true にはしない
				if (count == p.diffcount || p.diffcount == 0) {
					int *dataptr = attacktable2[x1][y1][x2][y2][p.weapon][i][j];
					while (*dataptr >= 0) {
						int x = *(dataptr++);
						int y = *(dataptr++);
						p.occupiedcount->add(x, y, 1);
					}
					if (p.diffcount > 0) {
						p.occupiednum++;
						retval = true;
					}
				}
			}
		}
	}

	if (retval == true) {
		checkattacktable2[index] = 1;
		checkattacktable2[index2] = 1;
	}
	else {
		checkattacktable2[index] = 2;
		checkattacktable2[index2] = 2;
	}
	return retval;
}

// pnum 番のプレーヤーの行動を計算する。
// turn: 現在のターン数、firstflag: 一回目の計算かどうか
void calcplayeraction(GameInfo& info, Field *prevfield, Playerdata **pdata, int pnum, int turn, bool firstflag) {
	// 計算するプレーヤーデータ
	Playerdata &p = *pdata[pnum];
	// 現在位置と隠れているかどうかをセット
	p.x = info.samuraiInfo[pnum].curX;
	p.y = info.samuraiInfo[pnum].curY;
	p.hidden = info.samuraiInfo[pnum].hidden;

	// 視界内で、前のフィールドから比べてそのプレーヤーの色に変化した場所を計算する
	p.diffcount = p.difffield->calcdifffield(prevfield, info.field, pnum);

	// 死んでいた場合の復活判定
	if (p.isdead == true) {
		if (turn > p.deadturn + info.cureTurns) {
			p.isdead = false;
		}
	}

	if (debug && firstflag == false) {
		cout << "#turn " << turn << " pnum " << pnum << "(x, y) = (" << p.x << "," << p.y << ") next actionum " << p.nextactionnum << " isdead = " << p.isdead;
		if (p.isdead == true) {
			cout << " deadturn = " << p.deadturn;
		}
		cout << endl;
		prevfield->dump("prev field");
		info.field->dump("current field");
	}	
	
	// このターンの開始時に生きているかどうか
	bool isdeadatthisturn = p.isdead;
	// 既に計算済でない場合
	if (p.calculated == false) {
		// 新しい存在位置をクリアする
		p.newplace->clear(Status::NOTEXIST);
		// 占領のチェックに関するデータを初期化する
		p.occupiedcount->clear();
		p.occupiednum = 0;
		// 新しい存在位置を計算する
		// 死亡している場合
		if (p.isdead == true) {
			// 2回行動の一回目の行動で、死亡していた場合は終了
			if (p.actionnum == 2 && turn == prevturn + 1) {
				return;
			}
			// 本拠地にいなければ復活している
			if (p.x >= 0 && (p.x != p.hx || p.y != p.hy)) {
				p.isdead = false;
				// 死亡ターンが正確でない場合があるため、ここに来る可能性はある。念のため警告を表示しておく
				if (debug) {
					cout << "#warning 1 player " << pnum << " is alive at turn " << turn << " before deadturn " << p.deadturn << endl;
				}
			}
		}

		//場所が分かっている場合、その位置をセットする
		if (p.x >= 0) {
			// 新しい存在位置に現在位置をセットする
			p.newplace->set(p.x, p.y, (p.hidden == 0) ? Status::APPEARED : Status::HIDDEN);
			// 本拠地にいる場合
			if (p.x == p.hx && p.y == p.hy) {
				// 死んだ結果、本拠地にいる可能性を計算する
				if (p.isdead == false) {
					// 最大移動可能距離を計算する
					int maxmove = p.actionnum * 3;
					// 攻撃していた場合は、減らす。2回攻撃していた場合は正確ではない（check!!)2回行動していた場合のチェックを行う（todo）
					if (p.diffcount > 0) {
						maxmove -= 2;
					}
					// 前の時点での位置がわかっていた場合
					if (p.px >= 0) {
						// 前の位置と距離が最大移動数を越えていた場合は、死亡している
						if (abs(p.x - p.px) + abs(p.y - p.py) > maxmove) {
							if (debug) {
								cout << "#player " << pnum << " may be killed (case 1)." << endl;
							}
							p.isdead = true;
							// とりあえず前の自分の行動したターンの次のターンに死んだことにする
							p.deadturn = prevturn + 1;
							// 誰が何時殺したかを推測する
							calckilledplayer(pdata, pnum, info);
						}
					}
					else {
						//　すべての存在可能な位置からの距離が移動可能な距離よりも離れていた場合は死亡している
						bool isdead = true;
						for (int y = 0; y < info.height; y++) {
							for (int x = 0; x < info.width; x++) {
								if (p.place->get(x, y) > 0) {
									if (abs(x - p.x) + abs(y - p.y) <= maxmove){
										isdead = false;
										y = info.height;
										break;
									}
								}
							}
						}
						if (isdead == true) {
							if (debug) {
								cout << "#player " << pnum << " may be killed (case 2)." << endl;
							}
							p.isdead = true;
							// とりあえず前の自分の行動したターンの次のターンに死んだことにする
							p.deadturn = prevturn + 1;
							// 誰が何時殺したかを推測する
							calckilledplayer(pdata, pnum, info);
						}
					}
				}
			}
			// 現在死亡していない場合（現在死亡していた場合は、現在位置は行動の推測に使えない）
			if (p.isdead == false) {
				// 攻撃（フィールドが自分の色に変化）していた場合
				// 変化していない場合も、味方の場合は、占領した場所が他のプレーヤーで占領し返された可能性があるので、チェックするようにする
				// (checkattack で味方が攻撃した可能性によって、敵の攻撃の判定を行うので必要）
				if (p.diffcount > 0 || pnum < 3) {
					// checkattack の結果を記録しておくためのテーブルの初期化
					memset(checkattacktable, 0, sizeof(int) * info.width * info.height);
					// 1回行動の場合。今いる場所と隣接する場所で攻撃した可能性がある
					// または、2回行動の場合で、2回目の行動の場合（この場合、1回目の行動時は死亡している）
					if (p.actionnum == 1 || (p.actionnum == 2 && turn != prevturn + 1)) {
						checkattack(p.x, p.y, pdata, pnum, false, info, firstflag);
						// 隣接する場所で攻撃した場合、その場所に前の時点で存在していた可能性がなければならないので第5引数を true にする
						checkattack(p.x - 1, p.y, pdata, pnum, true, info, firstflag);
						checkattack(p.x + 1, p.y, pdata, pnum, true, info, firstflag);
						checkattack(p.x, p.y - 1, pdata, pnum, true, info, firstflag);
						checkattack(p.x, p.y + 1, pdata, pnum, true, info, firstflag);
					}
					// 2回行動する場合
					else {
						// 一回だけ攻撃した場合は、今いる場所から距離4の任意の位置で攻撃可能
						for (int dx = -4; dx <= 4; dx++) {
							for (int dy = -4; dy <= 4; dy++){
								int distance = abs(dx) + abs(dy);
								if (distance <= 4) {
									// 距離4の場所のみ前のターンにそこに存在している必要がある
									checkattack(p.x + dx, p.y + dy, pdata, pnum, (distance == 4) ? true : false, info, firstflag);
								}
							}
						}
						// checkattack2 の結果を記録しておくためのテーブルの初期化
						memset(checkattacktable2, 0, sizeof(int) * info.width * info.height * info.width * info.height);
						// 2回攻撃した場合は以下の可能性がある
						// ・今いる場所から距離２の位置で攻撃、距離１の位置で攻撃
						checkattack2(p.x - 2, p.y, p.x - 1, p.y, pdata, pnum, 0, info, firstflag);
						checkattack2(p.x - 1, p.y - 1, p.x - 1, p.y, pdata, pnum, 0, info, firstflag);
						checkattack2(p.x - 1, p.y - 1, p.x, p.y - 1, pdata, pnum, 0, info, firstflag);
						checkattack2(p.x, p.y - 2, p.x, p.y - 1, pdata, pnum, 0, info, firstflag);
						checkattack2(p.x + 1, p.y - 1, p.x + 1, p.y, pdata, pnum, 0, info, firstflag);
						checkattack2(p.x + 1, p.y - 1, p.x, p.y - 1, pdata, pnum, 0, info, firstflag);
						checkattack2(p.x + 2, p.y, p.x + 1, p.y, pdata, pnum, 0, info, firstflag);
						checkattack2(p.x + 1, p.y + 1, p.x + 1, p.y, pdata, pnum, 0, info, firstflag);
						checkattack2(p.x + 1, p.y + 1, p.x, p.y + 1, pdata, pnum, 0, info, firstflag);
						checkattack2(p.x, p.y + 2, p.x, p.y + 1, pdata, pnum, 0, info, firstflag);
						checkattack2(p.x - 1, p.y + 1, p.x - 1, p.y, pdata, pnum, 0, info, firstflag);
						checkattack2(p.x - 1, p.y + 1, p.x, p.y + 1, pdata, pnum, 0, info, firstflag);
						// ・今いる場所から距離２の位置で攻撃、距離０の位置で攻撃
						checkattack2(p.x - 2, p.y, p.x, p.y, pdata, pnum, 0, info, firstflag);
						checkattack2(p.x - 1, p.y - 1, p.x, p.y, pdata, pnum, 0, info, firstflag);
						checkattack2(p.x, p.y - 2, p.x, p.y, pdata, pnum, 0, info, firstflag);
						checkattack2(p.x + 1, p.y - 1, p.x, p.y, pdata, pnum, 0, info, firstflag);
						checkattack2(p.x + 2, p.y, p.x, p.y, pdata, pnum, 0, info, firstflag);
						checkattack2(p.x + 1, p.y + 1, p.x, p.y, pdata, pnum, 0, info, firstflag);
						checkattack2(p.x, p.y + 2, p.x, p.y, pdata, pnum, 0, info, firstflag);
						checkattack2(p.x - 1, p.y + 1, p.x, p.y, pdata, pnum, 0, info, firstflag);
						// ・今いる場所から距離１の位置で攻撃、同じ場所でもう一度攻撃(さらに、距離1の位置から距離１の位置に前のターンに存在している必要がある）
						checkattack2(p.x - 1, p.y, p.x - 1, p.y, pdata, pnum, 1, info, firstflag);
						checkattack2(p.x, p.y - 1, p.x, p.y - 1, pdata, pnum, 1, info, firstflag);
						checkattack2(p.x + 1, p.y, p.x + 1, p.y, pdata, pnum, 1, info, firstflag);
						checkattack2(p.x, p.y + 1, p.x, p.y + 1, pdata, pnum, 1, info, firstflag);
						// ・今いる場所から距離１の位置で攻撃、距離０の位置で攻撃(さらに、距離1の位置から距離１の位置に前のターンに存在している必要がある）
						checkattack2(p.x - 1, p.y, p.x, p.y, pdata, pnum, 1, info, firstflag);
						checkattack2(p.x, p.y - 1, p.x, p.y, pdata, pnum, 1, info, firstflag);
						checkattack2(p.x + 1, p.y, p.x, p.y, pdata, pnum, 1, info, firstflag);
						checkattack2(p.x, p.y + 1, p.x, p.y, pdata, pnum, 1, info, firstflag);
						// ・今いる場所で2回攻撃(さらに、今の位置から距離1の位置に前のターンに存在している必要がある）
						checkattack2(p.x, p.y, p.x, p.y, pdata, pnum, 1, info, firstflag);
					}
					// 変化した場所が存在した場合で、一つも占領されなかった場合は、死んでしまったため、場所が本拠地に移動してしまった可能性が考えられる
					if (p.occupiednum == 0 && p.diffcount > 0) {
						if (debug) {
							if (p.x == p.hx && p.y == p.hy) {
								cout << "#player " << pnum << " may be killed (case 4) in turn " << turn << endl;
								p.difffield->dump("d");
								// ここで死んだことにする
								p.isdead = true;
								// とりあえず前の自分の行動したターンの次のターンに死んだことにする
								p.deadturn = prevturn + 1;
								// 誰が何時殺したかを推測する
								calckilledplayer(pdata, pnum, info);
								// 攻撃しているのだから、このターンより前に死んでいるはずがないので、このターンより前に死んだことになっていた場合、このターンより後に死んだことにする
								if (p.deadturn <= turn) {
									for (int i = turn + 1; i < info.turn; i++) {
										if (round(turnplayer[i % 12] / 3) != round(pnum / 3)) {
											p.deadturn = i;
											break;
										}
									}
									if (debug) {
										// それでもこのターンより前に死んだことになっていた場合はおかしい
										if (p.deadturn <= turn) {
											cout << "#error 9" << p.deadturn << "," << turn << endl;
										}
										cout << "#player " << pnum << " killed in turn " << p.deadturn << endl;
									}
								}
							}
							else {
								// ここにきたらおかしい
								cout << "#error 1" << endl;
							}
						}
					}
					else {
						p.calculated = true;
					}
				}
				else {
					p.calculated = true;
				}
			}
		}
		// まだ計算されていない場合（場所がわかっていない、または場所がわかっているが、死亡した場合）
		//// 現在死んでいて2回行動の一回目の場合は飛ばす？（あってる？）
		if (p.calculated == false) { // && !(p.isdead == true && p.actionnum == 2 && turn == prevturn + 1)) {
			// 攻撃していない（変化したマスが存在しない）場合
			if (p.diffcount == 0) {
				// 場所がわかっていない場合
				if (p.x < 0) {
					// すべての前の時点で存在可能だった場所から、移動可能な場初を調べる
					for (int y = 0; y < info.height; y++) {
						for (int x = 0; x < info.width; x++) {
							int fnum = p.place->get(x, y);
							// 前の時点で、その場所に存在可能だった場合
							if (fnum != Status::NOTEXIST) {
								// フィールド上の移動コストを 100 (最大移動コストの 14 より大きな数でクリアする)
								movecostfield->clear(100);
								// その位置から移動可能な場所を計算する
								calcmoveaction(x, y, prevfield, info.field, movecostfield, 0, (fnum == Status::HIDDEN) ? true : false, pdata, pnum, 7, p.actionnum);
							}
						}
					}
				}
			}
			// 変化したマスが存在する場合。
			// 変化していない場合も、味方の場合に占領した場所が他のプレーヤーで占領し返された可能性があるので、チェックするようにする
			// (checkattack で味方が攻撃した可能性によって、敵の攻撃の判定を行うので必要）
			if (p.diffcount > 0 || pnum < 3) {
				// checkattack の結果を記録しておくためのテーブルの初期化
				memset(checkattacktable, 0, sizeof(int) * info.width * info.height);
				// 1回だけ行動する場合
				// 2回行動の場合で、2回目の行動の場合（この場合、1回目の行動時は死亡している）
				if (p.actionnum == 1 || (p.actionnum == 2 && turn != prevturn + 1)) {
					// すべての前の時点で存在可能だった場所から、移動、占領の行動を行い、変化した場所がすべて占領されていればそこに存在できるものとする
					for (int y = 0; y < info.height; y++) {
						for (int x = 0; x < info.width; x++) {
							int pstatus = p.place->get(x, y);
							// 前の時点で、その場所に存在可能だった場合
							if (pstatus != Status::NOTEXIST) {
								const int dx[4] = { 0, 1, 0, -1 };
								const int dy[4] = { 1, 0, -1, 0 };
								// その場で発射していた場合はその場所には存在可能
								if (checkattack(x, y, pdata, pnum, false, info, firstflag) == true) {
									setnewplace(p, info.field, x, y);
									// 隣に移動した場合の状態を計算する
									for (int i = 0; i < 4; i++) {
										int x2 = x + dx[i];
										int y2 = y + dy[i];
										// フィールド外の場合は飛ばす
										if (info.field->isinside(x2, y2) == false) {
											continue;
										}
										// 前の時点で隠れていた場合。出現、攻撃、移動でコストを使い果たすので、見えている状態でそこに存在可能とする
										if (pstatus == Status::HIDDEN) {
											// そこに存在しない状態の場合のみ設定する
											if (p.newplace->get(x2, y2) == Status::NOTEXIST) {
												p.newplace->set(x2, y2, Status::APPEARED);
											}
										}
										else {
											setnewplace(p, info.field, x2, y2);
										}
									}
								}
								// 隣に移動して攻撃可能かどうかのチェック
								for (int i = 0; i < 4; i++) {
									int x2 = x + dx[i];
									int y2 = y + dy[i];
									// 攻撃可能な場合
									if (info.field->isinside(x2, y2) == true && checkattack(x2, y2, pdata, pnum, false, info, firstflag) == true) {
										// 前の時点で隠れていた場合。出現、移動、攻撃でコストを使い果たすので、見えている状態でそこに存在可能とする
										if (pstatus == Status::HIDDEN) {
											// そこに存在しない状態の場合のみ設定する
											if (p.newplace->get(x2, y2) == Status::NOTEXIST) {
												p.newplace->set(x2, y2, Status::APPEARED);
											}
										}
										else {
											setnewplace(p, info.field, x2, y2);
										}
									}
								}
							}
						}
					}
				}
				// 2回攻撃した場合
				else {
					// 一回攻撃、一回移動の可能性をチェックする。(x, y)の場所で攻撃した場合の計算を行う
					for (int y = 0; y < info.height; y++) {
						for (int x = 0; x < info.width; x++) {
							int mindist = 100;
							// そのマスの何マス以内(最大4)に存在可能な場所があるかどうかを計算し、最も近い距離を計算する
							for (int dx = -4; dx <= 4; dx++) {
								for (int dy = -4; dy <= 4; dy++) {
									int x2 = x + dx;
									int y2 = y + dy;
									int dist = abs(dx) + abs(dy);
									// 範囲内で、距離が4マス以内の場合
									if (p.place->isinside(x2, y2) == true && dist <= 4) {
										if (p.place->get(x2, y2) != Status::NOTEXIST) {
											if (dist < mindist) {
												mindist = dist;
											}
										}
									}
								}
							}
							// 4マス以内に存在可能があった場合、その場所が発射可能かどうかをチェックする
							if (mindist <= 4 && checkattack(x, y, pdata, pnum, false, info, firstflag) == true) {
								int maxdist = -1;
								// そのマスに前の状態で存在可能だった場合は、そこを含む、距離４マスの位置に存在可能
								if (mindist == 0) {
									maxdist = 4;
								}
								// 前の状態で隣のマスに存在可能だった場合は、そこを含む距離３マスの位置に存在可能
								else if (mindist == 1) {
									maxdist = 3;
								}
								// そうでなければ、前の状態で距離3マス以内に存在可能だった場合は、そこを含む距離1マスの位置に存在可能
								else if (mindist <= 3) {
									maxdist = 1;
								}
								// 距離4マスに存在可能だった場合は、そこのみ存在可能
								else if (mindist == 4) {
									maxdist = 0;
								}
								// そのマスから距離 maxdist 以内の場所に存在可能
								for (int dx = -4; dx <= 4; dx++) {
									for (int dy = -4; dy <= 4; dy++) {
										int x2 = x + dx;
										int y2 = y + dy;
										int dist = abs(dx) + abs(dy);
										// 範囲内で、距離が maxdist マス以内の場合
										if (p.place->isinside(x2, y2) == true && dist <= maxdist) {
											setnewplace(p, info.field, x2, y2);
										}
									}
								}
							}
						}
					}
					// 二回攻撃の可能性をチェックする
					// checkattack2 の結果を記録しておくためのテーブルの初期化
					memset(checkattacktable2, 0, sizeof(int) * info.width * info.height * info.width * info.height);
					for (int y = 0; y < info.height; y++) {
						for (int x = 0; x < info.width; x++) {
							// その場所に前の時点で存在していた場合
							if (p.place->get(x, y) != Status::NOTEXIST) {
								// 同じ場所で2回攻撃可能な場合は、その場所の距離1で存在可能
								if (checkattack2(x, y, x, y, pdata, pnum, -1, info, firstflag) == true) {
									setnewplace2(p, info.field, x, y);
								}
								// 今の場所と隣で撃可能な場合は、隣の場所から距離１で存在可能
								if (checkattack2(x, y, x - 1, y, pdata, pnum, -1, info, firstflag) == true) {
									setnewplace2(p, info.field, x - 1, y);
								}
								if (checkattack2(x, y, x + 1, y, pdata, pnum, -1, info, firstflag) == true) {
									setnewplace2(p, info.field, x + 1, y);
								}
								if (checkattack2(x, y, x, y - 1, pdata, pnum, -1, info, firstflag) == true) {
									setnewplace2(p, info.field, x, y - 1);
								}
								if (checkattack2(x, y, x, y + 1, pdata, pnum, -1, info, firstflag) == true) {
									setnewplace2(p, info.field, x, y + 1);
								}
								// 今の場所と距離2の場所で攻撃可能な場合は、距離２の位置に存在可能
								for (int dx = -2; dx <= 2; dx++) {
									for (int dy = -2; dy <= 2; dy++) {
										if (abs(dx) + abs(dy) == 2) {
											if (checkattack2(x, y, x + dx, y + dy, pdata, pnum, -1, info, firstflag) == true) {
												setnewplace(p, info.field, x + dx, y + dy);
											}
										}
									}
								}
								// 今の場所の隣の場所と、その場所で攻撃可能な場合は、その場所から距離１に存在可能。
								// 今の場所の隣の場所と、その隣の場所で攻撃可能な場合は、その隣の場所に存在可能
								for (int dx = -1; dx <= 1; dx++) {
									for (int dy = -1; dy <= 1; dy++) {
										if (abs(dx) + abs(dy) == 1) {
											for (int dx2 = -1; dx2 <= 1; dx2++) {
												for (int dy2 = -1; dy2 <= 1; dy2++) {
													if (abs(dx2) + abs(dy2) <= 1) {
														if (checkattack2(x + dx, y + dy, x + dx + dx2, y + dy + dy2, pdata, pnum, -1, info, firstflag) == true) {
															if (dx2 == 0 && dy2 == 0) {
																setnewplace2(p, info.field, x + dx + dx2, y + dy + dy2);
															}
															else {
																setnewplace(p, info.field, x + dx + dx2, y + dy + dy2);
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
				// 一つも占領されなかった場合は、占領した場所すべてが、その後別のプレーヤーによって塗りつぶされて元の状態になった可能性がある。
				// その場合、プレーヤー視点では、元の状態に塗りつぶしたプレーヤーは何も行動していないように見えてしまい、その結果
				// checkattack の判定で実際には攻撃可能なのに false となってしまう場合がある。
				// これを厳密に処理するのは困難なので、この場合は、変化した場所を occupiedplace とする
				if (p.occupiednum == 0 && p.diffcount > 0) {
					p.occupiedcount->copy(p.difffield);
					p.occupiednum = 1;
					if (debug) {
						cout << "#warning 6 turn " << turn << " pnum " << pnum << endl;
					}
				}
			}
			p.calculated = true;
		}
	}
	// 既に計算済の場合(2回目の行動）は、変化した場所を occupiedplace とする（それ以上の細かいことは計算してもわからないので）
	else {
		p.occupiedcount->copy(p.difffield);
		if (p.diffcount > 0) {
			p.occupiednum = 1;
		}
		else {
			p.occupiednum = 0;
		}
	}

	// 変化していた場合、100％占領された場所を計算する
	// 2回行動の最初の行動時に死んでいた場合は、自分の行動の次の行動なので、その前に他のプレーヤーによって
	// 死んでいることはありえないので、死んでいるものとして計算しない
	if (p.diffcount > 0 && !(isdeadatthisturn == true && p.actionnum == 2 && turn == prevturn + 1)) {
		calcoccupation(pdata, pnum, turn, true);
	}

	// 死亡している場合の処理
	if (p.isdead == true) {
		p.newplace->clear();
		// 本拠地で出現しているものとする
		p.newplace->set(p.hx, p.hy, Status::APPEARED);
		p.x = p.hx;
		p.y = p.hy;
		p.hidden = 0;
		// 死んだはずなのに、本拠地で見えていない場合はおかしいのでチェックしておく
		// ただし、2回行動の場合、初回の行動時は無視する（2回目で復活している可能性があるため）
		if (!(p.actionnum == 2 && turn == prevturn + 1)) {
			if (info.field->get(p.hx, p.hy) != 9 && p.x < 0) {
				if (debug) {
					cout << "error 8 " << info.field->get(p.hx, p.hy) << " pnum " << pnum << "," << p.actionnum << "," << p.calculated << endl;
				}
			}
		}
	}
	// 死亡していない場合、新しい存在可能な場所で、現在の視界と矛盾している場所の矛盾を解消する
	else {
		// 敵の場合で、場所が不明な場合のみ必要な処理
		if (pnum >= 3 && p.x < 0) {
			if (debug) {
				p.newplace->dump("nplace old");
			}
			for (int y = 0; y < p.newplace->height; y++) {
				for (int x = 0; x < p.newplace->width; x++) {
					int cfnum = info.field->get(x, y);
					int pstatus = p.newplace->get(x, y);
					// 視界外の場合は、隠れている場合は UNKNOWN に変更する
					if (cfnum == 9) {
						if (pstatus == Status::HIDDEN) {
							p.newplace->set(x, y, Status::UNKNOWN);
						}
						continue;
					}
					// 視界内の隠れられない場所に存在していた場合か見えている場合は、（場所が不明なはずなので）矛盾しているので存在しないものとする
					// この場合、攻撃によって死亡している可能性がある
					if (cfnum <= 2 || cfnum == 8 || pstatus == Status::APPEARED) {
						if (pstatus != Status::NOTEXIST) {
							p.newplace->set(x, y, Status::NOTEXIST);
							p.maybekilledturn[cfnum] = turn;
							if (debug && firstflag == false) {
								cout << "#player " << pnum << " may be killed (case 3)." << endl;
							}
						}
					}
				}
			}
			if (debug) {// && firstflag == false) {
				p.newplace->dump("nplace new");
			}
			// 矛盾を解消した結果、存在可能位置が 0 になった場合は死亡している
			if (p.newplace->calcnotequalfieldnum(Status::NOTEXIST) == 0) {
				if (debug && firstflag == false) {
					cout << "#player " << pnum << " is killed (case 3)." << endl;
				}
				// この後誰が殺したか判定する
				p.isdead = true;
				p.deadturn = prevturn + 1;
				calckilledplayer(pdata, pnum, info);
				p.newplace->clear();
				// 本拠地で出現しているものとする
				p.newplace->set(p.hx, p.hy, Status::APPEARED);
			}
			// 死んだ可能性がある場合で、本拠地が見えていない場合は、本拠地にもいるかもしれない
			else if (p.maybekilledturn[0] >= 0 || p.maybekilledturn[1] >= 0 || p.maybekilledturn[2] >= 0) {
				if (info.field->get(p.hx, p.hy) == 9) {
					p.newplace->set(p.hx, p.hy, Status::APPEARED);
				}
			}
		}
		// 現在の場所がわかっている場合はセットしなおす
		else if (p.x >= 0){
			// 新しい存在位置に現在位置をセットする
			p.newplace->clear();
			p.newplace->set(p.x, p.y, (info.samuraiInfo[pnum].hidden == 0) ? Status::APPEARED : Status::HIDDEN);
		}
	}

	// 1回目の処理はここで終了
	if (firstflag == true) {
		return;
	}
	// 他のプレーヤーによって殺された可能性のあるターン数の初期化
	for (int i = 0; i < 6; i++) {
		p.maybekilledturn[i] = -1;
	}
	// estimatefield の更新
	if (pnum >= 3 && isdeadatthisturn == false) {
		for (int y = 0; y < estimatefield->height; y++) {
			for (int x = 0; x < estimatefield->width; x++) {
				// 見えていない場所で占領した可能性がある場所を 7 とする
				if (p.occupiednum > 0) {
					if (p.occupiedcount->get(x, y) > 0 && info.field->get(x, y) == 9) {
						estimatefield->set(x, y, 7);
					}
				}
				else {
					if (p.attackrange->get(x, y) > 0 && info.field->get(x, y) == 9) {
						estimatefield->set(x, y, 7);
					}
				}
			}
		}
	}
	if (debug) {
		if (p.diffcount > 0) {
			cout << "#diffcount " << p.diffcount << " occupied num = " << p.occupiednum << endl;
		}
		p.difffield->dump("diff field");
		p.place->dump("old place");
		p.newplace->dump2("new place", info.field);
		if (p.diffcount > 0) {
			p.occupiedcount->dump("occupied count");
			p.occupiedfield->dump("occupied field");
		}
	}
	// 存在可能な場所が1か所しかなければそこにいるものとする
	if (p.x < 0 && p.newplace->calcnotequalfieldnum(Status::NOTEXIST) == 1) {
		p.newplace->findnotequal(Status::NOTEXIST, p.x, p.y);
		if (debug) {
			cout << "#player " << pnum << " must be in (" << p.x << "," << p.y << ")" << endl;
		}
	}
	// 現在の状態を前の状態にコピーする
	p.px = p.x;
	p.py = p.y;
	p.place->copy(p.newplace);
}

// 占領したことによる敵の殺害をチェックする。changeisdeadflag が true の場合は、殺害した場合はそのプレーヤーの isdead を変化させる
void calcoccupation(Playerdata **pdata, int pnum, int turn, bool changeisdeadflag) {
	Playerdata &p = *pdata[pnum];
	// 何らかのバグによって占領されている場所が存在しない場合でここに来た場合は終了する。
	if (p.occupiednum == 0) {
		if (debug) {
			cout << "#error 10" << endl;
		}
		return;
	}
	// 全ての攻撃で占領された場所が 100% 占領されている
	// 各プレーヤーのフィールド上に存在可能な場所の数、存在可能な場所に攻撃された回数
	int placenum[6], hitcount[6];
	// 死んだかどうかをチェックするための初期化
	for (int i = 0; i < 6; i++) {
		placenum[i] = pdata[i]->place->calcnotequalfieldnum(Status::NOTEXIST);
		hitcount[i] = 0;
		p.killflag[i] = 0;
	}
	p.occupiedfield->clear();
	for (int y = 0; y < p.occupiedcount->height; y++) {
		for (int x = 0; x < p.occupiedcount->width; x++){
			// 本拠地は占領できないので飛ばす
			if (homefield->get(x, y) >= 0) {
				continue;
			}
			if (p.occupiedcount->get(x, y) == p.occupiednum) {
				p.occupiedfield->set(x, y, 1);
				prevfield->set(x, y, pnum);
				for (int i = 0; i < 6; i++) {
					if (pdata[i]->place->get(x, y) != Status::NOTEXIST) {
						hitcount[i]++;
					}
				}
			}
		}
	}
	// 各プレーヤーについて、殺害の可能性を調べる
	for (int i = 0; i < 6; i++) {
		// 既に死んでいる場合はチェックしない
		if (pdata[i]->isdead == true) {
			continue;
		}
		// 敵で、存在可能な場所すべてに攻撃された場合は死亡している
		if (round(pnum / 3) != round(i / 3)) {
			if (placenum[i] == hitcount[i]) {
				if (changeisdeadflag == true) {
					if (debug) {
						cout << "#player " << pnum << " killed player " << i << " case 4 " << endl;
					}
					pdata[i]->isdead = true;
					pdata[i]->deadturn = turn;
				}
				pdata[pnum]->killflag[i] = 1;
			}
			// 殺した可能性がある場合はそのターン数を記録する
			else if (hitcount[i] > 0) {
				pdata[i]->maybekilledturn[pnum] = turn;
				pdata[pnum]->killflag[i] = 2;
			}
		}
	}
}

// pnum を殺したプレーヤーと殺したターンを計算する
void calckilledplayer(Playerdata **pdata, int pnum, GameInfo &info) {
	Playerdata &p = *pdata[pnum];
	// 殺された可能性のある最小ターン
	int minturn = -1;
	// 殺した可能性のあるプレーヤー番号
	int killedplayer = -1;
	for (int i = 0; i < 6; i++) {
		if (p.maybekilledturn[i] >= 0) {
			if (killedplayer == -1 || minturn < p.maybekilledturn[i]) {
				killedplayer = i;
				minturn = p.maybekilledturn[i];
			}
		}
	}
	if (killedplayer >= 0) {
		if (debug) {
			cout << "#player " << pnum << " may be killed by player (case 1) " << killedplayer << " in turn " << minturn << endl;
		}
		p.deadturn = minturn;
	}
	else {
		// 存在可能な位置の色が敵の色に変化していた場合は、その敵に殺された可能性がある
		// 最も早いターンに殺された可能性がある場合を採用する
		killedplayer = -1;
		minturn = -1;
		for (int y = 0; y < p.place->height; y++) {
			for (int x = 0; x < p.place->width; x++) {
				if (p.place->get(x, y) != Status::NOTEXIST) {
					int fnum = info.field->get(x, y);
					if (fnum < 6 && round(fnum / 3) != round(pnum / 3)) {
						int killedturn = -1;
						for (int i = prevturn + 1; i < info.turn; i++) {
							if (turnplayer[i % 12] == fnum) {
								killedturn = i;
								break;
							}
						}
						if (killedplayer == -1 || (minturn > killedturn && killedturn != -1)) {
							killedplayer = fnum;
							minturn = killedturn;
						}
					}
				}
			}
		}
		if (killedplayer >= 0) {
			if (killedplayer == info.weapon) {
				minturn = prevturn;
			}
			if (debug) {
				cout << "#player " << pnum << " may be killed by player (case 2) " << killedplayer << " in turn " << minturn << endl;
			}
			p.deadturn = minturn;
		}
		else {
			// 前の時の後で、最初に殺せた可能性のあるターンを採用する
			for (int i = prevturn + 1; i < info.turn; i++) {
				if (round(turnplayer[i % 12] / 3) != round(pnum / 3)) {
					p.deadturn = i;
					break;
				}
			}
			if (debug) {
				cout << "#player " << pnum << " killed by unknown player in turn " << p.deadturn << endl;
			}
		}
	}
}

// x, y の newplace をセットする
void setnewplace(Playerdata &p, Field *field, int x, int y) {
	// フィールドの外の場合は終了
	if (field->isinside(x, y) == false) {
		return;
	}
	int fnum = field->get(x, y);
	// その場所に隠れることができる場合
	if (p.canhide(fnum) == true) {
		// 視界外の場合は、不明の状態で存在可能
		if (fnum == 9) {
			p.newplace->set(x, y, Status::UNKNOWN);
		}
		// 視界内の場合は、隠れている状態で存在可能
		else {
			p.newplace->set(x, y, Status::HIDDEN);
		}
	}
	// 隠れることができない場合で視界内の場合は、見えている状態でそこに存在可能とする
	// もしここにいた場合は、視界内で隠れられないので、必ず攻撃されて死亡しているはず。そのチェックは最後に行う
	else {
		p.newplace->set(x, y, Status::APPEARED);
	}
}

// x, y から距離1以内の場所で setplace を実行する
void setnewplace2(Playerdata &p, Field *field, int x, int y) {
	setnewplace(p, field, x, y);
	setnewplace(p, field, x - 1, y);
	setnewplace(p, field, x + 1, y);
	setnewplace(p, field, x, y - 1);
	setnewplace(p, field, x, y + 1);
}

void calcattackrange(GameInfo& info, Playerdata **pdata, Field& dzone) {
	// 次の自分のターンまでの他のプレーヤーの攻撃範囲を計算する
	// 攻撃範囲のクリア
	int checknum[6];
	for (int i = 0; i < 6; i++) {
		// place の内容が壊れる可能性があるので取っておく
		pdata[i]->placebak->copy(pdata[i]->place);
		pdata[i]->attackrange->clear();
		pdata[i]->attackplace->clear();
		checknum[i] = 0;
	}
	dzone.clear();
	int i = info.turn + 1;
	while (i < info.turns) {
		int pnum = turnplayer[i % 12];
		if (pnum == info.weapon) {
			break;
		}
		else {
			checknum[pnum]++;
			// そのターンに生きていれば攻撃可能な場所を計算する
			// >= の = は必要？？（check!!)
			if (pdata[pnum]->isdead == false || i >= pdata[pnum]->deadturn + info.cureTurns) {
				for (int y = 0; y < info.height; y++) {
					for (int x = 0; x < info.width; x++) {
						if (pdata[pnum]->place->get(x, y) != Status::NOTEXIST) {
							for (int dx = -1; dx <= 1; dx++) {
								for (int dy = -1; dy <= 1; dy++) {
									int x2 = x + dx;
									int y2 = y + dy;
									int hnum;
									// 自分以外の本拠地には入れない
									if (abs(dx) + abs(dy) <= 1 && info.field->isinside(x2, y2) == true && ((hnum = homefield->get(x2, y2)) < 0 || hnum == pnum)) {
										// 同じマスにまだ行動していない、現れている状態のプレーヤーがいた場合は入れない
										bool playerflag = false;
										for (int i = 0; i < 6; i++) {
											// 自分または、チェック済の場合はそこにいないかもしれないので飛ばす
											if (i == pnum || checknum[i] > 0) {
												continue;
											}
											if (pdata[i]->x == x2 && pdata[i]->y == y2 && pdata[i]->hidden == 0) {
												playerflag = true;
											}
										}
										if (playerflag == false) {
											pdata[pnum]->attackplace->add(x2, y2, 1);
										}									
									}
								}
							}
						}
					}
				}
				// 2回行動する場合は、存在可能な場所を3歩分増やす
				if (checknum[pnum] == 1 && pdata[pnum]->nextactionnum == 2) {
					tmpfield->copy(pdata[pnum]->place);
					// その場所までの調べた最短距離を表す表すフィールド。
					checkfield->clear(100);
					for (int y = 0; y < info.height; y++) {
						for (int x = 0; x < info.width; x++) {
							if (pdata[pnum]->place->get(x, y) != Status::NOTEXIST) {
								calcmoveplace(x, y, pdata, pnum, *tmpfield, *checkfield, 0, checknum);
							}
						}
					}
					pdata[pnum]->place->copy(tmpfield);
				}
				// 攻撃範囲の計算
				for (int y = 0; y < info.height; y++) {
					for (int x = 0; x < info.width; x++) {
						int anum;
						if ((anum = pdata[pnum]->attackplace->get(x, y)) > 0) {
							for (int j = 0; j < 4; j++) {
								for (int k = 0; k < attacknum[pdata[pnum]->weapon]; k++) {
									int x2 = x + attackplacex[pdata[pnum]->weapon][j][k];
									int y2 = y + attackplacey[pdata[pnum]->weapon][j][k];
									if (info.field->isinside(x2, y2) == true && homefield->get(x2, y2) < 0) {
										pdata[pnum]->attackrange->add(x2, y2, anum);
									}
								}
							}
						}
					}
				}
				// 味方の攻撃範囲内で、隠れられない場所に敵の攻撃場所が存在した場合、自分が死んでも味方が殺すと判断して、その攻撃場所と攻撃範囲を減らす
				// ただし、自分が死んだときの評価値と、殺す相手の評価値の合計が負の場合は損なので、減らさない
				if (pnum < 3) {
					for (int y = 0; y < info.height; y++) {
						for (int x = 0; x < info.width; x++) {
							int fnum;
							if (pdata[pnum]->attackrange->get(x, y) > 0 && ((fnum = info.field->get(x,y)) < 3 || fnum == 8)) {
								for (int l = 3; l < 6; l++) {
									if (info.turn < info.turns * HYOUKA_LATTERHALF) {
										if (HYOUKA_KILL[l - 3] + HYOUKA_KILLED[info.weapon] < 0) {
											continue;
										}
									}
									else {
										if (HYOUKA_KILL2[l - 3] + HYOUKA_KILLED2[info.weapon] < 0) {
											continue;
										}
									}
									// その場所にいられないものとする
									pdata[l]->place->set(x, y, 0);
									// その場所で攻撃していた場合、その場所から攻撃できる場所の攻撃回数を減らす（０になったら攻撃不可）
									int anum;
									if ((anum = pdata[l]->attackplace->get(x, y)) > 0) {
										pdata[l]->attackplace->set(x, y, -1);
										for (int j = 0; j < 4; j++) {
											for (int k = 0; k < attacknum[pdata[l]->weapon]; k++) {
												int x2 = x + attackplacex[pdata[l]->weapon][j][k];
												int y2 = y + attackplacey[pdata[l]->weapon][j][k];
												if (info.field->isinside(x2, y2) == true && homefield->get(x2, y2) < 0) {
													// 負になったらおかしいので念のためエラーチェック（デバッグ時のみ）
													if (debug) {
														if (pdata[l]->attackrange->add(x2, y2, -anum) < 0) {
															cout << "#error 11" << endl;
														}
													}
													else {
														pdata[l]->attackrange->add(x2, y2, -anum);
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		i++;
	}
	// 危険範囲の計算
	for (int i = 3; i < 6; i++) {
		for (int y = 0; y < info.height; y++) {
			for (int x = 0; x < info.width; x++) {
				if (pdata[i]->attackrange->get(x, y) > 0) {
					dzone.set(x, y, dzone.get(x, y) | (1 << (i - 3)));
				}
			}
		}
	}
	// place の情報を戻す
	// マスの最大攻撃値を計算する
	for (int i = 0; i < 6; i++) {
		pdata[i]->place->copy(pdata[i]->placebak);
		pdata[i]->maxattackrange = pdata[i]->attackrange->calcmax();
	}
	if (debug) {
		cout << "#Turn " << info.turn << endl;
		for (int i = 0; i < 6; i++) {
			if (i == info.weapon) {
				continue;
			}
			cout << "#player " << i << " (" << pdata[i]->x << ", " << pdata[i]->y << ") hidden " << pdata[i]->hidden;
			if (pdata[i]->isdead == true) {
				cout << " dead turn = " << pdata[i]->deadturn;
			}
			cout << endl;
			pdata[i]->attackplace->dump("attack place");
			pdata[i]->attackrange->dump("attack range");
			pdata[i]->attackrange->dump3("attack range 2");
			pdata[i]->newplace->dump("place");
		}
		cout << "#Turn " << info.turn << " ";
		dzone.dump("danger zone");
	}
}

// 思考ルーチン
void ai(GameInfo& info, Playerdata **pdata) {
	Playerdata &p = *pdata[info.weapon];
	p.x = info.samuraiInfo[info.weapon].curX;
	p.y = info.samuraiInfo[info.weapon].curY;
	p.hidden = info.samuraiInfo[info.weapon].hidden;
	// 何もしない行動のデフォルト評価値を 0 とする
	double maxhyouka = -10000000;

	// 自分が攻撃した場所をクリアする
	p.occupiedcount->clear();
	// 各プレーヤーが殺されたか、特定のプレーヤーを殺したかどうかなどのフラグをクリアする
	int killflag[6];
	for (int i = 0; i < 6; i++) {
		p.killflag[i] = 0;
		killflag[i] = 0;
		// 一番最初に x, y が設定されていない場合があるので、入れておく
		if (pdata[i]->x < -1) {
			pdata[i]->x = info.samuraiInfo[i].curX;
			pdata[i]->y = info.samuraiInfo[i].curY;
		}
	}
	// 最終的に決定された行動をクリアする
	finalaction[0] = 0;
	// その場所を攻撃していない状態で既にチェックしたかどうかを表すフィールドをクリアする（1: 出現している状態でチェックした, 2: 隠れている状態でチェックした)
	p.occupiedfield->clear();
	// 味方の現在の視界を計算する
	sikaifield->clear();
	int sikainum = 0;
	for (int i = 0; i < 3; i++) {
		if (i == info.weapon) {
			continue;
		}
		for (int dx = -5; dx <= 5; dx++) {
			for (int dy = -5; dy <= 5; dy++) {
				int x = pdata[i]->x + dx;
				int y = pdata[i]->y + dy;
				if (abs(dx) + abs(dy) <= 5 && sikaifield->isinside(x, y) == true && sikaifield->get(x,y) == 0) {
					sikainum++;
					sikaifield->set(x, y, 1);
				}
			}
		}
	}
	if (debug) {
		cout << "#sikainum " << sikainum << endl;
	}
	// 自身の行動を決める関数を呼ぶ
	// 最初に渡す行動は何もしていない状態とする
	action[0] = 0;
	// 危険地帯をコピーする
	tmpfield->copy(dangerzone);
	// 斜め２の危険地帯を計算する
	naname2dangerfield->clear();
	// このターンの最初に現れている場合は、自分の斜め２の位置に敵が潜んでいる可能性がある。その場合の危険地帯を計算する
	if (p.hidden == 0) {
		for (int dx = -2; dx <= 2; dx += 2) {
			for (int dy = -2; dy <= 2; dy += 2) {
				int x = p.x + dx;
				int y = p.y + dy;
				if (abs(dx) == 2 && abs(dy) == 2 && info.field->isinside(x, y) == true) {
					bool eneexistflag = false;
					for (int i = 3; i < 6; i++) {
						// 敵が次に行動可能で、そこに存在している可能性がある場合
						if (pdata[i]->nextactionnum > 0 && pdata[i]->place->get(x, y) != Status::NOTEXIST) {
							eneexistflag = true;
							break;
						}
					}
					if (eneexistflag == true) {
						naname2dangerfield->set(p.x + dx / 2, p.y, 1);
						naname2dangerfield->set(p.x, p.y + dy / 2, 1);
					}
				}
			}
		}
	}
	if (debug) {
		naname2dangerfield->dump("nanme2 danger field");
	}

	calcaction(info, pdata, p.x, p.y, p.hidden, 7, action, 0, finalaction, maxhyouka, killflag, false, *tmpfield, sikainum);
	if (debug) {
		cout << "#maxhyouka " << maxhyouka << endl;
	}
	for (int i = 3; i < 6; i++) {
		if (killflag[i] == 1) {
			if (debug) {
				cout << "#AI killed player " << i << " in turn " << info.turn << endl;
			}
			pdata[i]->isdead = true;
			pdata[i]->deadturn = info.turn;
			pdata[i]->x = pdata[i]->hx;
			pdata[i]->y = pdata[i]->hy;
			pdata[i]->hidden = 0;
		}
	}
}

// 再起的に行動の評価を行う
// (x, y) 自分の位置, hidden 隠れているかどうか, actionleft 残り行動回数, action これまでに行った行動, actionnum これまでに行った行動の回数, finalaction これまでの最高評価の行動
// maxhyouka: これまでの最高の評価値, killflag: 殺した敵を表すフラグ（ビットで表す）, attackflag: これまで攻撃したかどうか
// currentdangerzone: 現時点での危険地帯, 味方の視界の合計値
void calcaction(GameInfo &info, Playerdata **pdata, int x, int y, int hidden, int actionleft, int *action, int actionnum, int *finalaction, double& maxhyouka, int *killflag, bool attackflag, Field& currentdangerzone, int sikainum) {
	Playerdata &p = *pdata[info.weapon];
	// 盤外の場合は終了
	if (info.field->isinside(x, y) == false) {
		return;
	}
	int fnum = homefield->get(x, y);
	// 自分以外の本拠地の場合は入れないので終了
	if (fnum >= 0 && fnum != info.weapon) {
		return;
	}
	// 隠れており、味方の占領地でなければ、隠れられないので終了
	if (hidden == 1 && p.occupiedcount->get(x,y) == 0 && info.field->get(x, y) >= 3 ) {
		return;
	}
	// 現れており、誰かがそこに現れていればそこに行けないので終了
	if (hidden == 0) {
		for (int i = 0; i < 6; i++) {
			if (i == info.weapon) {
				continue;
			}
			if (pdata[i]->x == x && pdata[i]->y == y && pdata[i]->hidden == 0) {
				return;
			}
		}
	}
	// その場所を攻撃していない状態で既にチェックしたかどうかを調べる
	int ofnum = p.occupiedfield->get(x, y);
	// 攻撃していない場合で、チェック済の場合は終了
	if (!(attackflag == false && (ofnum & (1 << hidden)) > 0)) {
		// 攻撃していない場合はチェック済とする
		if (attackflag == false) {
			p.occupiedfield->set(x, y, ofnum | (1 << hidden));
		}
		//	cout << "# " << x << "," << y << " ofnum " << ofnum << " attack " << attackflag << " hidden " << hidden << endl;
		// 評価する
		// 最終ターンかどうか（最終ターンの場合は、殺されても構わないという評価を行うために調べる）
		bool islastturn = ((info.turns - info.turn) <= 6) ? true : false;
		// 評価値のクリア
		double hyouka = 0;
		// 乱数による評価
		double randomhyouka = rnd(mt) * HYOUKA_RANDOM;
		// デバッグ時は、乱数があると再現性がなくなってしまい困るので、乱数を排除できるようにしておく
#ifndef USERANDOM
		randomhyouka = 0;
#endif
		hyouka += randomhyouka;

		// 敵撃破による評価
		int killhyouka = 0;
		// 自分の攻撃の結果、誰が生きているかを表すフラグ
		int aliveflag = 7;
		for (int i = 3; i < 6; i++) {
			//　敵を殺していた場合。
			if (p.killflag[i] == 1) {
				aliveflag -= 1 << (i - 3);
				// ただし、自分が最後のターンでその敵が次に行動しない場合は殺しても意味がないので評価には加算しない
				if (!(islastturn == true && pdata[i]->nextactionnum == 0)) {
					if (info.turn < info.turns * HYOUKA_LATTERHALF) {
						killhyouka += HYOUKA_KILL[(i - 3)];
					}
					else {
						killhyouka += HYOUKA_KILL2[(i - 3)];
					}
				}
			}
		}
		hyouka += killhyouka;

		// 占領に関する評価
		int occupynum = 0;			// 占領したマスの数
		int eneoccupynum = 0;		// 敵の領地を占領したマスの数
		int allyoccupynum = 0;		// 味方の領地を占領したマスの数
		int neutraloccupynum = 0;	// 中立地帯を占領したマスの数
		int eneattackoccupynum = 0; // 敵が攻撃可能な場所を占領したマスの数
		for (int y2 = 0; y2 < info.height; y2++) {
			for (int x2 = 0; x2 < info.width; x2++) {
				if (p.occupiedcount->get(x2, y2) > 0) {
					int fnum = info.field->get(x2, y2);
					// 自分以外の領地の場合、占領できる
					if (fnum != info.weapon) {
						occupynum++;
						// 味方の領地
						if (fnum < 3) {
							allyoccupynum++;
						}
						else if (fnum < 6) {
							eneoccupynum++;
						}
						else if (fnum == 8) {
							neutraloccupynum++;
						}
						// 敵の領地、または未占領地で、敵が攻撃可能な場所
						// 自分の攻撃の結果、死んでしまう敵は除く
						if (fnum >= 3 && (currentdangerzone.get(x2, y2) & aliveflag) > 0) {
							eneattackoccupynum++;
						}
					}
				}
			}
		}

		// 占領による評価
		double occupyhyouka = eneoccupynum * HYOUKA_ENEOCCUPY + neutraloccupynum * HYOUKA_NEUTRALOCCUPY + allyoccupynum * HYOUKA_ALLYOCCUPY + eneattackoccupynum * HYOUKA_ENECANATTACKOCCUPY;
		hyouka += occupyhyouka;

		// 危険地帯かどうかの評価
		double dangerhyouka = 0;
		int dnum = currentdangerzone.get(x, y);
		// それぞれの敵について、その位置を攻撃できるかどうかを調べる
		for (int i = 0; i < 3; i++) {
			// 攻撃可能で、生きている場合
			if ((dnum & (1 << i)) > 0 && (aliveflag & (1 << i)) > 0) {
				double tmphyouka;
				// 敵の位置が確定している場合（見えていない場合も含む）
				if (pdata[i + 3]->x >= 0) {
					// 自分が隠れていない場合
					if (hidden == 0) {
						if (info.turn < info.turns * HYOUKA_LATTERHALF) {
							tmphyouka = HYOUKA_KILLED[info.weapon] * HYOUKA_DANGER_APPEARED;
						}
						else {
							tmphyouka = HYOUKA_KILLED2[info.weapon] * HYOUKA_DANGER_APPEARED;
						}
					}
					// 隠れている場合
					else {
						if (info.turn < info.turns * HYOUKA_LATTERHALF) {
							tmphyouka = HYOUKA_KILLED[info.weapon] * HYOUKA_DANGER_HIDDEN;
						}
						else {
							tmphyouka = HYOUKA_KILLED2[info.weapon] * HYOUKA_DANGER_HIDDEN;
						}
					}
					if (dangerhyouka > tmphyouka) {
						dangerhyouka = tmphyouka;
					}
				}
				// 敵の位置が不明な場合
				else {
					// 自分が隠れていない場合
					if (hidden == 0) {
						if (info.turn < info.turns * HYOUKA_LATTERHALF) {
							tmphyouka = HYOUKA_KILLED[info.weapon] * HYOUKA_WARNING_APPEARED;
						}
						else {
							tmphyouka = HYOUKA_KILLED2[info.weapon] * HYOUKA_WARNING_APPEARED;
						}
					}
					else {
						// 自分が隠れている場合。あまり臆病にふるまうと、何もできなくなってしまうので、敵の攻撃の可能性が高いかどうかによって判断する
						// また、自分が攻撃していた場合、その攻撃により相手に自分の位置のヒントを与えることになってしまうので、それも考慮する
						if ((double)pdata[i + 3]->attackrange->get(x, y) / (double)pdata[i + 3]->maxattackrange > (attackflag == false ? HYOUKA_WARNING_NONATTACK_HIDDEN_MINRATIO : HYOUKA_WARNING_ATTACKED_HIDDEN_MINRATIO)) {
							if (info.turn < info.turns * HYOUKA_LATTERHALF) {
								tmphyouka = HYOUKA_KILLED[info.weapon] * HYOUKA_WARNING_HIDDEN2;
							}
							else {
								tmphyouka = HYOUKA_KILLED2[info.weapon] * HYOUKA_WARNING_HIDDEN2;
							}
						}
						else{
							if (info.turn < info.turns * HYOUKA_LATTERHALF) {
								tmphyouka = HYOUKA_KILLED[info.weapon] * HYOUKA_WARNING_HIDDEN;
							}
							else {
								tmphyouka = HYOUKA_KILLED2[info.weapon] * HYOUKA_WARNING_HIDDEN;
							}
						}
					}
					if (dangerhyouka > tmphyouka) {
						dangerhyouka = tmphyouka;
					}
				}
			}
			else {
				if (hidden == 0) {
					dangerhyouka += HYOUKA_SAFETY_APPEARED;
				}
			}
		}
		// 最後のターンは死んでも川まないので評価しない
		if (islastturn == true) {
			dangerhyouka = 0;
		}
		hyouka += dangerhyouka;

		// 次のターンの占領に関する評価
		double nextturnoccupyhyouka = 0;
		double tmphyouka;
		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				if (abs(dx) + abs(dy) <= 1) {
					int x2 = x + dx;
					int y2 = y + dy;
					if (info.field->isinside(x2, y2) == true) {
						for (int i = 0; i < 4; i++) {
							tmphyouka = 0;
							for (int j = 0; j < attacknum[info.weapon]; j++) {
								int x3 = x2 + attackplacex[info.weapon][i][j];
								int y3 = y2 + attackplacey[info.weapon][i][j];
								if (info.field->isinside(x3, y3) == true && homefield->get(x3, y3) < 0) {
									int fnum = info.field->get(x3, y3);
									// 今回の攻撃でそこを占領していた場合は除く
									int onum = p.occupiedcount->get(x3, y3);
									if (onum == 0) {
										int esnum = estimatefield->get(x3, y3);
										// 見えない場所で、確定している場合は、 fnum を変化させる
										if (fnum == 9 && esnum != 7) {
											fnum = esnum;
										}
										if (fnum == 9) {
											tmphyouka += HYOUKA_NEXT_UNKNOWNOCCUPY;
										}
										else if (fnum == 8) {
											tmphyouka += HYOUKA_NEXT_NEUTRALOCCUPY;
										}
										else if (fnum >= 3) {
											tmphyouka += HYOUKA_NEXT_ENEOCCUPY;
										}
										else if (fnum != info.weapon) {
											tmphyouka += HYOUKA_NEXT_ALLYOCCUPY;
										}
									}
								}
							}
							if (tmphyouka > nextturnoccupyhyouka) {
								nextturnoccupyhyouka = tmphyouka;
							}
						}
					}
				}
			}
		}
		// 最後のターンの場合は、次はないので評価しない
		if (islastturn == true) {
			nextturnoccupyhyouka = 0;
		}
		hyouka += nextturnoccupyhyouka;

		// 次のターンの付近の敵の占領地域の量による評価
		double eneplacehyouka = 0;
		for (int y2 = 0; y2 < info.field->height; y2++) {
			for (int x2 = 0; x2 < info.field->width; x2++) {
				if (abs(x - x2) + abs(y - y2) <= HYOUKA_ESTIMATE_RANGE) {
					fnum = info.field->get(x2, y2);
					int esnum = estimatefield->get(x2, y2);
					// 見えない場所で、確定している場合は、 fnum を変化させる
					if (fnum == 9 && esnum != 7) {
						fnum = esnum;
					}
					if (fnum == 9) {
						tmphyouka += HYOUKA_ESTIMATE_UNKNOWNOCCUPY;
					}
					else if (fnum == 8) {
						eneplacehyouka += HYOUKA_ESTIMATE_NEUTRALOCCUPY;
					}
					else if (fnum >= 3) {
						eneplacehyouka += HYOUKA_ESTIMATE_ENEOCCUPY;
					}
				}
			}
		}
		// 最後のターンの場合は、次はないので評価しない
		if (islastturn == true) {
			eneplacehyouka = 0;
		}
		hyouka += eneplacehyouka;

		// 2回行動でライバル（自分と同じ武器の敵）を殺しに行けるかどうかの評価
		int nextrivalkillhyouka = 0;
		Playerdata& rp = *pdata[info.weapon + 3];
		// ライバルが次のターン行動不可能な場合で、現在死亡していない場合で、本拠地にいない場合は殺しに行ける可能性がある
		if (rp.actionnum == 2 && rp.isdead == false && !(rp.x == rp.hx && rp.y == rp.hy)) {
			// 敵の存在可能な位置が一回（次の攻撃）で攻撃可能な場所以下の場合、100%殺せる可能性がある
			int rivalcount = rp.place->calcnotequalfieldnum(Status::NOTEXIST);
			if (rivalcount <= attacknum[info.weapon]) {
				for (int dx = -1; dx <= 1; dx++) {
					for (int dy = -1; dy <= 1; dy++) {
						int x2 = x + dx;
						int y2 = y + dy;
						int hitcount = 0;
						int hnum;
						if (abs(dx) + abs(dy) <= 1 && rp.place->isinside(x2, y2) == true && ((hnum = homefield->get(x2, y2)) < 0 || hnum == info.weapon)) {
							for (int i = 0; i < 4; i++) {
								for (int j = 0; j < attacknum[info.weapon]; j++) {
									int x3 = x2 + attackplacex[info.weapon][i][j];
									int y3 = y2 + attackplacey[info.weapon][i][j];
									if (rp.place->isinside(x3, y3) == true && rp.place->get(x3, y3) != Status::NOTEXIST) {
										hitcount++;
									}
								}
							}
						}
						if (hitcount == rivalcount) {
							nextrivalkillhyouka = HYOUKA_NEXT_CANKILLRIVAL;
							dy = 1;
							dx = 1;
						}
					}
				}
			}
		}
		// 最後のターンの場合は、次が無いので評価しない
		if (islastturn == true) {
			nextrivalkillhyouka = 0;
		}
		hyouka += nextrivalkillhyouka;

		// 視界に関する評価
		double sikaihyouka = sikainum * HYOUKA_SIKAI;
		for (int dx = -5; dx <= 5; dx++) {
			for (int dy = -5; dy <= 5; dy++) {
				int x2 = x + dx;
				int y2 = y + dy;
				if (abs(dx) + abs(dy) <= 5 && sikaifield->isinside(x2, y2) == true && sikaifield->get(x2, y2) == 0) {
					sikaihyouka += HYOUKA_SIKAI;
				}
			}
		}
		// 最後のターンの場合は評価しない
		if (islastturn == true) {
			sikaihyouka = 0;
		}
		hyouka += sikaihyouka;

		// 斜め2の位置の評価
		double naname2hyouka = 0;
		// 2回行動しない相手の場合、斜めに2マスの位置は絶対安全な上、次に相手がこちら側に動いてきた場合は殺せるので良い位置取りと考えられる
		for (int i = 3; i < 6; i++) {
			// ライバルは、2回行動または0回行動なので、当てはまらないので飛ばす
			// 相手の次の行動回数が 0 の場合は死んでいるので飛ばす
			if (i == info.weapon + 3 || pdata[i]->nextactionnum == 0) {
				continue;
			}
			// 相手の場所が確実にわかっている場合のみ計算する
			if (pdata[i]->x >= 0) {
				if (abs(x - pdata[i]->x) == 2 && abs(y - pdata[i]->y) == 2) {
					if (hidden == 0) {
						naname2hyouka += HYOUKA_NANAME2_APPEARED;
					}
					else {
						naname2hyouka += HYOUKA_NANAME2_HIDDEN;
					}
				}
			}
		}
		// 後半は占領を優先するので、評価しない
		if (islastturn == true || info.turn >= info.turns * HYOUKA_LATTERHALF) {
			naname2hyouka = 0;
		}
		hyouka += naname2hyouka;


		// このターンの最初に自分が見えている状態で、斜め2の位置に敵が潜んでいる可能性があった場合の危険地帯にいるかどうか
		double naname2enehyouka = 0;
		if (p.hidden == 0 && naname2dangerfield->get(x, y) > 0 && islastturn != true) {
			naname2enehyouka += HYOUKA_NANAME2_DANGER;
		}
		hyouka += naname2enehyouka;

		// 敵の視界の範囲内に現れている
		double enesikaihyouka = 0;
		// 現在見えている状態の場合、敵の視界の範囲内にいるかどうか
		if (hidden == 0 && islastturn != true) {
			bool enesikai = false;
			for (int i = 3; i < 6; i++) {
				if (pdata[i]->x >= 0 && abs(x - pdata[i]->x) + abs(y - pdata[i]->y) <= 5) {
					enesikai = true;
				}
			}
			if (enesikai == true) {
				enesikaihyouka = HYOUKA_ENESIKAIAPPEARED;
			}
		}
		hyouka += enesikaihyouka;

		if (debug) {
			cout << "#action ";
			for (int i = 0; i < actionnum; i++) {
				cout << action[i] << " ";
			}
			cout << "0" << endl << "#  o " << occupynum << " a " << allyoccupynum << " e " << eneoccupynum << " n " << neutraloccupynum;
			cout << " randomhyouka "  << randomhyouka << " ohyouka " << occupyhyouka << " nextturnoccupyhyouka " << nextturnoccupyhyouka << " eneplacehyouka " << eneplacehyouka << " killhyouka " << killhyouka << " nextrivalkillhyouka " << nextrivalkillhyouka << " dangerhyouka " << dangerhyouka << " sikaihyouka " << sikaihyouka << " naname2hyouka " << naname2hyouka << " naname2enehyouka " << naname2enehyouka << " enesikaihyouka " << enesikaihyouka;
			cout << " actionleft " << actionleft << " hyouka " << hyouka << endl;
		}

		// 最高評価の場合、データを更新する
		if (hyouka > maxhyouka) {
			maxhyouka = hyouka;
			int i;
			for (i = 0; i < actionnum; i++) {
				finalaction[i] = action[i];
			}
			finalaction[i] = 0;
			for (i = 3; i < 6; i++) {
				killflag[i] = 0;
				if (p.killflag[i] == 1) {
					killflag[i] = 1;
				}
			}
		}
	}

	const int dx[4] = { 0, 1, 0, -1 };
	const int dy[4] = { 1, 0, -1, 0 };
	// 次の行動を 1 から 10 までの間から順番に試していく
	for (int i = 1; i <= 10; i++) {
		switch (i) {
		case 1:
		case 2:
		case 3:
		case 4:
			// 攻撃。行動回数が足りないか、隠れている場合は攻撃できないので飛ばす
			if (actionleft < 4 || hidden == 1) {
				break;
			}
			// 攻撃範囲を occupiedcount に反映させる。一回の行動で最大1回しか攻撃できないので、ここでクリアして良い。
			p.occupiedcount->clear();
			p.occupiednum = 0;
			// 敵の攻撃可能な場所、攻撃可能な範囲をとっておく
			pdata[3]->placebak->copy(pdata[3]->attackplace);
			pdata[4]->placebak->copy(pdata[4]->attackplace);
			pdata[5]->placebak->copy(pdata[5]->attackplace);
			pdata[3]->placebak2->copy(pdata[3]->attackrange);
			pdata[4]->placebak2->copy(pdata[4]->attackrange);
			pdata[5]->placebak2->copy(pdata[5]->attackrange);
			for (int j = 0; j < attacknum[info.weapon]; j++) {
				int x2 = x + attackplacex[info.weapon][i - 1][j];
				int y2 = y + attackplacey[info.weapon][i - 1][j];
				// 誰かの本拠地は占領不可
				if (p.occupiedcount->isinside(x2, y2) == true && homefield->get(x2, y2) == -1) {
					p.occupiedcount->set(x2, y2, 1);
					p.occupiednum = 1;
					// 占領した場所に敵がいた可能性があれば殺しているので、そこからの攻撃範囲を減らす
					for (int k = 3; k < 6; k++) {
						int dist = pdata[k]->nextactionnum * 3 - 2;
						if (pdata[k]->place->get(x2, y2) > 0) {
							for (int dx2 = -dist; dx2 <= dist; dx2++) {
								for (int dy2 = -dist; dy2 <= dist; dy2++) {
									int x3 = x2 + dx2;
									int y3 = y2 + dy2;
									// これだと2回行動の1回目が消しきれないのでは？（todo）
									if (abs(dx2) + abs(dy2) <= dist && pdata[k]->place->isinside(x3, y3) == true && pdata[k]->placebak->add(x3, y3, -1) == 0) {
										for (int m = 0; m < 4; m++){
											for (int l = 0; l < attacknum[k - 3]; l++){
												int x4 = x3 + attackplacex[k - 3][m][l];
												int y4 = y3 + attackplacey[k - 3][m][l];
												if (pdata[k]->placebak->isinside(x4, y4) == true) {
													if (pdata[k]->placebak2->add(x4, y4, -1) == 0) {
														currentdangerzone.add(x4, y4, -(1 << (k - 3)));
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			if (p.occupiednum > 0) {
				calcoccupation(pdata, info.weapon, info.turn, false);
			}
			action[actionnum] = i;
			calcaction(info, pdata, x, y, hidden, actionleft - 4, action, actionnum + 1, finalaction, maxhyouka, killflag, true, currentdangerzone, sikainum);
			// 攻撃範囲をクリアする
			if (p.occupiednum > 0) {
				currentdangerzone.copy(dangerzone);
			}
			// 殺したフラグをクリアする
			for (int j = 0; j < 6; j++) {
				p.killflag[j] = 0;
			}
			p.occupiedcount->clear();
			break;
		case 5:
		case 6:
		case 7:
		case 8:
			if (actionleft < 2) {
				break;
			}
			action[actionnum] = i;
			calcaction(info, pdata, x + dx[i - 5], y + dy[i - 5], hidden, actionleft - 2, action, actionnum + 1, finalaction, maxhyouka, killflag, attackflag, currentdangerzone, sikainum);
			// 移動
			break;
		case 9:
			// 隠れる
			if (actionleft < 1 || hidden == 1) {
				break;
			}
			action[actionnum] = i;
			calcaction(info, pdata, x, y, 1, actionleft - 1, action, actionnum + 1, finalaction, maxhyouka, killflag, attackflag, currentdangerzone, sikainum);
			break;
		case 10:
			// 出現する
			if (actionleft < 1 || hidden == 0) {
				break;
			}
			action[actionnum] = i;
			calcaction(info, pdata, x, y, 0, actionleft - 1, action, actionnum + 1, finalaction, maxhyouka, killflag, attackflag, currentdangerzone, sikainum);
			break;
		default:
			break;
		}
	}
}

void debugprinttime(string str, int time) {
	if (debug) {
		cout << "#" << str << " " << time << " ms" << endl;
	}
}


// dist が 3 以下の場合、(x, y) が移動可能な場所であれば f のその座標の値を 1 にする
void calcmoveplace(int x, int y, Playerdata **pdata, int pnum, Field &f, Field &checkfield, int dist, int *checknum) {
	if (dist > 3) {
		return;
	}
	if (f.isinside(x, y) == false) {
		return;
	}
	int cnum = checkfield.get(x, y);
	if (cnum <= dist) {
		return;
	}
	int hnum = homefield->get(x, y);
	if (hnum >= 0 && hnum != pnum) {
		checkfield.set(x, y, -1);
		return;
	}
	for (int i = 0; i < 6; i++) {
		if (i == pnum || checknum[i] > 0) {
			continue;
		}
		if (pdata[i]->x == x && pdata[i]->y == y && pdata[i]->hidden == 0) {
			checkfield.set(x, y, -1);
			return;
		}
	}
	checkfield.set(x, y, dist);
	f.set(x, y, 1);
	calcmoveplace(x - 1, y, pdata, pnum, f, checkfield, dist + 1, checknum);
	calcmoveplace(x, y - 1, pdata, pnum, f, checkfield, dist + 1, checknum);
	calcmoveplace(x + 1, y, pdata, pnum, f, checkfield, dist + 1, checknum);
	calcmoveplace(x, y + 1, pdata, pnum, f, checkfield, dist + 1, checknum);
}