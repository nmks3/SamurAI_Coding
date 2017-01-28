import java.util.*;
import java.util.concurrent.*;
import java.util.stream.Collectors;
import java.util.stream.IntStream;
import java.util.stream.Stream;

public class OtameshiPlayer extends Player {
  public static final int[] cost = {0, 4, 4, 4, 4, 2, 2, 2, 2, 1, 1};
  public static final int maxPower = 7;
  public Random rnd;

  private static final Merits merits = new Merits.MeritsBuilder()
      .setTerr(10)
      .setSelf(3)
      .setKill(100)
      .setHide(0)
      .setSafe(200)
      .setUsur(5)
      .setDepl(1)
      .setCent(3)
      .build();

  public OtameshiPlayer() {
    this.rnd = new Random();
  }

  private static class HistoryTree {
    private GameInfo info;
    private Deque<HistoryTree> children;
    private Optional<HistoryTree> parent;
    private int action;

    public HistoryTree(Optional<HistoryTree> parent, GameInfo info, int action) {
      this.parent = parent;
      this.info = info;
      this.children = new ArrayDeque<>();
      this.action = action;
    }

    public GameInfo getInfo() {
      return info;
    }

    public void add(HistoryTree c) {
      children.add(c);
    }

    public double score() {
      return info.score(merits);
    }

    public Deque<Integer> getActions() {
      return getActions(new ArrayDeque<>());
    }

    private Deque<Integer> getActions(Deque<Integer> actions) {
      if (parent.isPresent()) {
        actions.addFirst(action);
        return parent.get().getActions(actions);
      } else {
        return actions;
      }
    }

    public List<HistoryTree> collect() {
      List<HistoryTree> list = children.stream().collect(ArrayList::new, (l, c) -> l.addAll(c.collect()), List::addAll);
      list.add(this);
      return list;
    }
  }

  public void plan(HistoryTree tree, final int power) {
    for (int i = 1; i < 11; ++i) {
      if (cost[i] <= power && tree.getInfo().isValid(i)) {
        GameInfo next = new GameInfo(tree.getInfo());
        next.doAction(i);
        HistoryTree child = new HistoryTree(Optional.of(tree), next, i);
        tree.add(child);
        plan(child, power - cost[i]);
      }
    }
  }

  int[][] fieldDup = null;
  SamuraiInfo[] samuraiDup = null;

  public void setDup(GameInfo info) {
    fieldDup = Stream.of(info.field).map(a -> Arrays.copyOf(a, a.length)).toArray(int[][]::new);
    samuraiDup = Stream.of(info.samuraiInfo).map(SamuraiInfo::new).toArray(SamuraiInfo[]::new);
  }

  public void play(GameInfo info) {
    ExecutorService pool = Executors.newSingleThreadExecutor();
    try {
      Future<Boolean> future = pool.submit(new Callable<Boolean>() {
        @Override
        public Boolean call() throws Exception {
          //System.err.println("turn : " + info.turn + ", side : " + info.side + ", weapon : " + info.weapon);
          info.occupyCount = 0;
          info.playerKill = 0;
          info.selfCount = 0;
          info.usurpCount = 0;

          if (fieldDup != null && samuraiDup != null) {
            // 20 x 20 x 3 x 7
            final int[][] ox = {
                {0, 0, 0, 0},
                {0, 0, 1, 1, 2},
                {-1, -1, -1, 0, 1, 1, 1}
            };
            final int[][] oy = {
                {1, 2, 3, 4},
                {1, 2, 0, 1, 0},
                {0, -1, 1, 1, 1, -1, 0}
            };
            Map<Integer, Set<Point>> map = new HashMap<>();
            for (int i = 3; i < 6; ++i) {
              map.put(i, new HashSet<>());
            }
            for (int y = 0; y < info.height; ++y) {
              for (int x = 0; x < info.width; ++x) {
                if (info.field[y][x] != fieldDup[y][x] && fieldDup[y][x] != 9) {
                  int v = info.field[y][x];
                  if (3 <= v && v < 6) {
                    map.get(v).add(Point.valueOf(x, y));
                  }
                }
              }
            }
            for (int i = 3; i < 6; ++i) {
              SamuraiInfo si = info.samuraiInfo[i];
              if (si.curX == -1 && si.curY == -1) {
                //System.err.println("search " + i);
                Set<Point> set = new HashSet<>();
                for (int y = 0; y < info.height; ++y) {
                  for (int x = 0; x < info.width; ++x) {
                    for (int r = 0; r < 4; ++r) {
                      boolean flag = true;
                      if (samuraiDup[i].curX != -1 && samuraiDup[i].curY != -1) {
                        flag &= Math.abs(samuraiDup[i].curX - x) + Math.abs(samuraiDup[i].curY - y) <= 1;
                      }
                      flag &= map.get(i).size() > 0;
                      boolean done = false;
                      int diffCount = 0;
                      for (int d = 0; flag && d < ox[i - 3].length; ++d) {
                        int[] pos = info.rotate(r, ox[i - 3][d], oy[i - 3][d]);
                        int px = x + pos[0];
                        int py = y + pos[1];
                        if (px < 0 || info.width <= px || py < 0 || info.height <= py) {
                          continue;
                        }
                        if (info.field[py][px] == fieldDup[py][px]) {
                          continue;
                        }
                        done |= info.field[py][px] == i;
                        flag &= info.field[py][px] == i
                            || info.field[py][px] < 3
                            || info.field[py][px] >= 8;
                        if (info.field[py][px] == i && fieldDup[py][px] != 9) {
                          ++diffCount;
                        }
                      }
                      flag &= done;
                      flag &= diffCount == map.get(i).size();
                      if (info.samuraiInfo[i].curX == -1 && info.samuraiInfo[i].curY == -1) {
                        flag &= (info.field[y][x] >= 3 && info.field[y][x] < 6) || info.field[y][x] == 9;
                      }
                      if (flag) {
                        set.add(Point.valueOf(x, y));
                      }
                    }
                  }
                }
                //set.stream().forEach(v ->
                    //System.err.println("\t? : " + v));
                if (set.size() == 1) {
                  Point p = set.iterator().next();
                  int x = p.x;
                  int y = p.y;
                  //System.err.println("\t\tgot it! : " + p);
                  si.curX = x;
                  si.curY = y;
                  info.samuraiInfo[i] = si;
                }
              }
            }

            fieldDup = null;
            samuraiDup = null;
          }

          HistoryTree root = new HistoryTree(Optional.empty(), info, 0);
          plan(root, maxPower);

          List<HistoryTree> histories = root.collect();

          double[] roulette = new double[histories.size()];
          double accum = 0.0;
          {
            int i = 0;
            for (Iterator<HistoryTree> ite = histories.iterator(); ite.hasNext(); ) {
              double v = Math.exp(ite.next().getInfo().score(merits));
              accum += v;
              roulette[i++] = accum;
            }
          }
          if (Double.isInfinite(accum)) {
            //System.err.println("accum goes infinite!");
          }
          int idx = Arrays.binarySearch(roulette, rnd.nextDouble() * accum);
          if (idx < 0) {
            idx = -idx - 1;
          }
          GameInfo best = histories.get(idx).getInfo();
          Deque<Integer> bestActions = histories.get(idx).getActions();
          System.out.println(bestActions.stream().map(v -> v.toString()).collect(Collectors.joining(" ")));

          fieldDup = Stream.of(best.field).map(a -> Arrays.copyOf(a, a.length)).toArray(int[][]::new);
          samuraiDup = Stream.of(best.samuraiInfo).map(SamuraiInfo::new).toArray(SamuraiInfo[]::new);

          debug(best.isSafe2(), best.samuraiInfo[best.weapon].curX, best.samuraiInfo[best.weapon].curY);

          return Boolean.TRUE;
        }
      });
      try {
        future.get(50, TimeUnit.MILLISECONDS);
      } catch (InterruptedException |
          ExecutionException e
          ) {
        e.printStackTrace(System.err);
      } catch (TimeoutException e) {
        //System.err.println("timed out");
        // do something...
      }
    } finally {
      pool.shutdown();
    }
  }

  void debug(Object... os) {
    //System.err.println(Arrays.deepToString(os));
  }
}
