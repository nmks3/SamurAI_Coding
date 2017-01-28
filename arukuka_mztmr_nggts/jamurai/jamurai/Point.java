/**
 * Created by capri_000 on 2016/01/07.
 */
public class Point {
  final int x;
  final int y;

  private static final Point[][] TABLE;
  static {
    Point[][] table = new Point[20][20];
    for (int i = 0; i < 20; ++i) {
      for (int j = 0; j < 20; ++j) {
        table[i][j] = new Point(j, i);
      }
    }
    TABLE = table;
  }

  private Point(int x, int y) {
    this.x = x;
    this.y = y;
  }

  public static Point valueOf(int x, int y) {
    return TABLE[y][x];
  }

  @Override
  public int hashCode() {
    return y * 20 + x;
  }

  @Override
  public boolean equals(Object r) {
    return this == r;
  }

  @Override
  public String toString() {
    return "(" + x + ", " + y + ")";
  }
}
