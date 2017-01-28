public class SamuraiInfo {
  public int homeX, homeY;
  public int curX, curY;
  public int rank, score, hidden;

  public SamuraiInfo() {
    this.homeX = 0;
    this.homeY = 0;
    this.curX = 0;
    this.curY = 0;
    this.rank = 0;
    this.score = 0;
    this.hidden = 0;
  }

  public SamuraiInfo(SamuraiInfo info) {
    this.homeX = info.homeX;
    this.homeY = info.homeY;
    this.curX = info.curX;
    this.curY = info.curY;
    this.rank = info.rank;
    this.score = info.score;
    this.hidden = info.hidden;
  }
}