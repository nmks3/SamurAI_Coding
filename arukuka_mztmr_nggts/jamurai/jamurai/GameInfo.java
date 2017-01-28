import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Arrays;
import java.util.stream.Stream;

public class GameInfo {
  public static final int PLAYER_NUM = 6;
  public static BufferedReader stdReader;
  public int turns;
  public int side;
  public int weapon;
  public int width, height;
  public int maxCure;
  public SamuraiInfo[] samuraiInfo;
  public int turn, curePeriod;
  public int[][] field;

  public GameInfo(GameInfo info) {
    this.turns = info.turns;
    this.side = info.side;
    this.weapon = info.weapon;
    this.width = info.width;
    this.height = info.height;
    this.maxCure = info.maxCure;
    this.samuraiInfo = Stream.of(info.samuraiInfo).map(SamuraiInfo::new).toArray(SamuraiInfo[]::new);
    this.turn = info.turn;
    this.curePeriod = info.curePeriod;
//    this.field = Stream.of(info.field).map(
//        a -> Arrays.copyOf(a, a.length)
//    ).toArray(int[][]::new);
    this.field = info.field;

    this.occupyCount = info.occupyCount;
    this.playerKill = info.playerKill;
    this.selfCount = info.selfCount;
    this.usurpCount = info.usurpCount;
  }

  public GameInfo() {
    GameInfo.stdReader = new BufferedReader(new InputStreamReader(System.in));

    String[] res = this.read();

    this.turns = Integer.parseInt(res[0]);
    this.side = Integer.parseInt(res[1]);
    this.weapon = Integer.parseInt(res[2]);
    this.width = Integer.parseInt(res[3]);
    this.height = Integer.parseInt(res[4]);
    this.maxCure = Integer.parseInt(res[5]);
    this.samuraiInfo = new SamuraiInfo[GameInfo.PLAYER_NUM];
    for (int i = 0; i < GameInfo.PLAYER_NUM; ++i) {
      this.samuraiInfo[i] = new SamuraiInfo();
    }
    for (int i = 0; i < GameInfo.PLAYER_NUM; ++i) {
      res = this.read();
      this.samuraiInfo[i].homeX = Integer.parseInt(res[0]);
      this.samuraiInfo[i].homeY = Integer.parseInt(res[1]);
    }
    for (int i = 0; i < GameInfo.PLAYER_NUM; ++i) {
      res = this.read();
      this.samuraiInfo[i].rank = Integer.parseInt(res[0]);
      this.samuraiInfo[i].score = Integer.parseInt(res[1]);
    }
    this.turn = 0;
    this.curePeriod = 0;
    this.field = new int[this.height][this.width];
    System.out.println("0");
  }

  public String[] read() {
    String line = "";
    try {
      for (line = GameInfo.stdReader.readLine(); line.startsWith("#"); line = GameInfo.stdReader.readLine()) ;
    } catch (Exception e) {
      e.getStackTrace();
      System.exit(-1);
    }
    return line.split("\\s");
  }

  public void readTurnInfo() {
    String[] res = this.read();

    if (res.length == 0) {
      System.exit(-1);
    }

    this.turn = Integer.parseInt(res[0]);

    if (this.turn < 0) {
      System.exit(-1);
    }

    res = this.read();
    this.curePeriod = Integer.parseInt(res[0]);

    for (int i = 0; i < GameInfo.PLAYER_NUM; ++i) {
      res = this.read();
      this.samuraiInfo[i].curX = Integer.parseInt(res[0]);
      this.samuraiInfo[i].curY = Integer.parseInt(res[1]);
      this.samuraiInfo[i].hidden = Integer.parseInt(res[2]);
    }

    for (int i = 0; i < this.height; ++i) {
      Arrays.fill(this.field[i], 0);
    }

    for (int i = 0; i < this.height; ++i) {
      res = this.read();

      for (int j = 0; j < this.width; ++j) {
        this.field[i][j] = Integer.parseInt(res[j + 1]);
      }
    }
  }

  public boolean isValid(int action) {
    SamuraiInfo myself = this.samuraiInfo[this.weapon];
    int curX = myself.curX;
    int curY = myself.curY;

    if (action >= 0 && action <= 4) {
      return myself.hidden == 0;
    }

    if (action >= 5 && action <= 8) {
      if (action == 5) {
        curX = curX;
        curY = curY + 1;
      }
      if (action == 6) {
        curX = curX + 1;
        curY = curY;
      }
      if (action == 7) {
        curX = curX;
        curY = curY - 1;
      }
      if (action == 8) {
        curX = curX - 1;
        curY = curY;
      }
      if (curX < 0 || this.width <= curX || curY < 0 || this.height <= curY) {
        return false;
      }
      if (myself.hidden == 1 && this.field[curY][curX] >= 3) {
        return false;
      }
      for (int i = 0; i < GameInfo.PLAYER_NUM; ++i) {
        if (curX == this.samuraiInfo[i].curX && curY == this.samuraiInfo[i].curY) {
          return false;
        }
        if (i != this.weapon && (curX == this.samuraiInfo[i].homeX && curY == this.samuraiInfo[i].homeY)) {
          return false;
        }
      }
      return true;
    }

    if (action == 9) {
      if (myself.hidden == 1) {
        return false;
      }
      if (this.field[curY][curX] >= 3) {
        return false;
      }
      return true;
    }

    if (action == 10) {
      if (myself.hidden != 1) {
        return false;
      }
      for (int i = 0; i < GameInfo.PLAYER_NUM; ++i) {
        SamuraiInfo other = this.samuraiInfo[i];
        if (other.hidden != 1 && (other.curX == curX && other.curY == curY)) {
          return false;
        }
      }
      return true;
    }

    System.exit(-1);
    return false;
  }

  public int[] rotate(int direction, int x0, int y0) {
    int[] res = {0, 0};
    if (direction == 0) {
      res[0] = x0;
      res[1] = y0;
    }
    if (direction == 1) {
      res[0] = y0;
      res[1] = -x0;
    }
    if (direction == 2) {
      res[0] = -x0;
      res[1] = -y0;
    }
    if (direction == 3) {
      res[0] = -y0;
      res[1] = x0;
    }
    return res;
  }

  public int occupyCount = 0;
  public int playerKill = 0;
  public int selfCount = 0;
  public int usurpCount = 0;

  public void occupy(int direction) {
    this.field = Stream.of(this.field).map(
        a -> Arrays.copyOf(a, a.length)
    ).toArray(int[][]::new);

    SamuraiInfo myself = this.samuraiInfo[this.weapon];
    int curX = myself.curX;
    int curY = myself.curY;
    int[] size = {4, 5, 7};
    int[][] ox = {
        {0, 0, 0, 0},
        {0, 0, 1, 1, 2},
        {-1, -1, -1, 0, 1, 1, 1}
    };
    int[][] oy = {
        {1, 2, 3, 4},
        {1, 2, 0, 1, 0},
        {0, -1, 1, 1, 1, -1, 0}
    };

    for (int i = 0; i < size[this.weapon]; ++i) {
      int[] pos = this.rotate(direction, ox[this.weapon][i], oy[this.weapon][i]);
      pos[0] += curX;
      pos[1] += curY;
      if (0 <= pos[0] && pos[0] < this.width && 0 <= pos[1] && pos[1] < this.height) {
        boolean isHome = false;
        for (int j = 0; j < GameInfo.PLAYER_NUM; ++j) {
          if (this.samuraiInfo[j].homeX == pos[0] && this.samuraiInfo[j].homeY == pos[1]) {
            isHome = true;
          }
        }
        if (!isHome) {
          if (this.field[pos[1]][pos[0]] != this.weapon) {
            if (this.field[pos[1]][pos[0]] >= 3) {
              if (this.field[pos[1]][pos[0]] < 6) {
                ++usurpCount;
              }
              ++occupyCount;
            }
            ++selfCount;
            this.field[pos[1]][pos[0]] = this.weapon;
          }
          for (int j = 3; j < GameInfo.PLAYER_NUM; ++j) {
            SamuraiInfo si = this.samuraiInfo[j];
            if (si.curX == pos[0] && si.curY == pos[1]) {
              si.curX = si.homeX;
              si.curY = si.homeY;
              si.hidden = 0;
              this.samuraiInfo[j] = si;
              ++playerKill;
            }
          }
        }
      }
    }
  }

  public double score(Merits m) {
    return this.samuraiInfo[this.weapon].hidden * m.hide
        + this.selfCount * m.self
        + this.playerKill * m.kill
        + this.occupyCount * m.terr
        + (this.isSafe2() ? 1 : 0) * m.safe
        + this.usurpCount * m.usur
        + this.deployLevel2() * m.depl
        + this.centerLevel3() * m.cent;
  }

  @Deprecated
  public boolean isSafe() {
    int min = 1 << 28;

    SamuraiInfo me = this.samuraiInfo[this.weapon];
    for (int i = 3; i < GameInfo.PLAYER_NUM; ++i) {
      SamuraiInfo si = this.samuraiInfo[i];
      if (si.curX == -1 || si.curY == -1) {
        continue;
      }
      min = Math.min(min, Math.abs(si.curX - me.curX));
      min = Math.min(min, Math.abs(si.curY - me.curY));
    }

    return min >= 2;
  }

  public boolean isSafe2() {
    boolean flag = true;
    SamuraiInfo me = this.samuraiInfo[this.weapon];
    // 3
    {
      SamuraiInfo si = this.samuraiInfo[3];
      if (si.curX != -1 && si.curY != -1) {
        int dx = Math.abs(si.curX - me.curX);
        int dy = Math.abs(si.curY - me.curY);
        flag &= (dx + dy > 5 || Math.min(dx, dy) >= 2);
      }
    }
    // 4
    {
      SamuraiInfo si = this.samuraiInfo[4];
      if (si.curX != -1 && si.curY != -1) {
        int dx = Math.abs(si.curX - me.curX);
        int dy = Math.abs(si.curY - me.curY);
        flag &= dx + dy > 3;
      }
    }
    // 5
    {
      SamuraiInfo si = this.samuraiInfo[5];
      if (si.curX != -1 && si.curY != -1) {
        int dx = Math.abs(si.curX - me.curX);
        int dy = Math.abs(si.curY - me.curY);
        flag &= dx + dy > 3 || Math.max(dx, dy) > 2;
      }
    }
    return flag;
  }

  @Deprecated
  public double deployLevel() {
    SamuraiInfo me = this.samuraiInfo[this.weapon];
    double min = 1 << 28;
    for (int i = 0; i < 3; ++i) {
      if (this.weapon == i) continue;
      SamuraiInfo si = this.samuraiInfo[i];
      min = Math.min(min, Math.sqrt(Math.pow(me.curX - si.curX, 2) + Math.pow(me.curY - si.curY, 2)));
    }
    return min;
  }

  public double deployLevel2() {
    SamuraiInfo me = this.samuraiInfo[this.weapon];
    double min = 1 << 28;
    for (int i = 0; i < 3; ++i) {
      if (this.weapon == i) continue;
      SamuraiInfo si = this.samuraiInfo[i];
      min = Math.min(min, Math.abs((me.curX - si.curX) + Math.abs(me.curY - si.curY)));
    }
    return min;
  }

  @Deprecated
  public double centerLevel() {
    SamuraiInfo me = this.samuraiInfo[this.weapon];
    double dist = Math.pow(me.curX - this.width / 2, 2) + Math.pow(me.curY - this.height / 2, 2);
    return Math.sqrt((this.width * this.height) / (1 + dist));
  }

  @Deprecated
  public double centerLevel2() {
    SamuraiInfo me = this.samuraiInfo[this.weapon];
    double dist = Math.pow(me.curX - this.width / 2.0, 2) + Math.pow(me.curY - this.height / 2.0, 2);
    double maxd = Math.pow(this.width / 2.0, 2) + Math.pow(this.height / 2.0, 2);
    return maxd - dist;
  }

  public double centerLevel3() {
    SamuraiInfo me = this.samuraiInfo[this.weapon];
    double dist = Math.abs(me.curX - this.width / 2) + Math.abs(me.curY - this.height / 2);
    double maxd = this.width / 2 + this.height / 2;
    return maxd - dist;
  }

  public void doAction(int action) {
    SamuraiInfo myself = this.samuraiInfo[this.weapon];
    int curX = myself.curX;
    int curY = myself.curY;

    if (action >= 1 && action <= 4) {
      this.occupy(action - 1);
    }
    if (action == 5) {
      curY += 1;
    }
    if (action == 6) {
      curX += 1;
    }
    if (action == 7) {
      curY -= 1;
    }
    if (action == 8) {
      curX -= 1;
    }
    if (action == 9) {
      myself.hidden = 1;
    }
    if (action == 10) {
      myself.hidden = 0;
    }
    myself.curX = curX;
    myself.curY = curY;
    this.samuraiInfo[this.weapon] = myself;
  }
}