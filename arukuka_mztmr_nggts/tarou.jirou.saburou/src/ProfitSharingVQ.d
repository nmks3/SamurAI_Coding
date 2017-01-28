module samurai.ProfitSharingVQ;
import samurai;

import std.string;
import std.stdio;
import std.conv;
import std.typecons;
import std.file;
import std.algorithm;

class ProfitSharingVQ {
  private:
    int[5][43200] Q;
  
    alias Tuple!(int, "s", int, "a") SA;
    SA[2] queue;
    immutable int weapon;
    immutable int side;
    
    enum IS_LEARNING = false;
    enum USE_FILE_FLAG = false;
  public:
    this(int weapon, int side, string[] args) {
      this.weapon = weapon;
      this.side = side;
      static if(USE_FILE_FLAG) {
        string filename = args.length == 1 ? format("Q%d.csv", weapon)
                : args[weapon + 1];
        if (exists(filename)) {
          stderr.writeln("[DEBUG]: reading Q value setting..." ~ filename);
          auto fp = new File(filename, "r");
          for (int i = 0; i < 43200; ++i) {
            string[] acts = fp.readln.chomp.split(",");
            for (int j = 0; j < 5; ++j) {
              Q[i][j] = acts[j].to!int;
            }
          }
        }
      }
    }
    
    static int encodeState(const GameInfo src, const GameInfo next) pure @safe nothrow {
      const SamuraiInfo mesrc = src.samuraiInfo[src.weapon];
      const SamuraiInfo menext = next.samuraiInfo[src.weapon];
      
      int code = 0;
      code += menext.curX;
      code += menext.curY * 15;
      code += src.haveEnemyIdea(3) * 15 * 15;
      code += src.haveEnemyIdea(4) * 15 * 15 * 2;
      code += src.haveEnemyIdea(5) * 15 * 15 * 2 * 2;
      code += ((src.turn * 12) / src.turns) * 15 * 15 * 2 * 2 * 2;
      code += src.side * 15 * 15 * 2 * 2 * 2 * 12;
      
      return code;
    }
    
    static int encodeAction(int[] actions) pure @safe nothrow {
      foreach (act; actions) {
        if (1 <= act && act <= 4) {
          return act;
        }
      }
      return 0;
    }
    
    void evapolate() pure @safe nothrow {
      static if(IS_LEARNING) {
        /+
        for (int i = 0; i < 90000; ++i) {
          for (int j = 0; j < 5; ++j) {
            Q[i + side * 90000][j] -= Q[i + side * 90000][j] / 50;
          }
        }
        +/
      }
    }
    
    void enqueue(int state, int action) pure @safe nothrow{
      static if(IS_LEARNING) {
        SA sa = SA(state, action);
        queue[0] = queue[1];
        queue[1] = sa;
      }
    }
    
    int get(int state, int action) const pure @safe nothrow {
      return Q[state][action];
    }
    
    void reward() pure @safe nothrow {
      static if(IS_LEARNING) {
        int reward_value = 10;
        foreach (sa; queue) {
          Q[sa.s][sa.a] = min(200, Q[sa.s][sa.a] + reward_value);
          reward_value /= 5;
        }
      }
    }
    
    void save() const {
      static if(IS_LEARNING) {
        string filename = format("Q%d.csv", weapon);
        auto fp = new File(filename, "w");
        for (int i = 0; i < 43200; i ++) {
          fp.writeln = format("%(%d,%)", Q[i]);
        }
        fp.close();
      }
    }
}

