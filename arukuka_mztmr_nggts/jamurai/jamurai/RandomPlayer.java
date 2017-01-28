import java.util.Random;

public class RandomPlayer extends Player {
  public final int[] cost = {0, 4, 4, 4, 4, 2, 2, 2, 2, 1, 1};
  public final int maxPower = 7;
  public Random rnd;

  public RandomPlayer() {
    this.rnd = new Random();
  }

  public void play(GameInfo info) {
    int power = this.maxPower;
    int action;

    while (power >= 2) {
      action = this.rnd.nextInt(10) + 1;

      if (cost[action] <= power && info.isValid(action)) {
        power -= cost[action];
        info.doAction(action);
      }
    }

    if (power != 0) {
      if (rnd.nextInt(11) > 8) {
        action = info.samuraiInfo[info.weapon].hidden == 1 ? 10 : 9;
        if (info.isValid(action)) {
          info.doAction(action);
        }
      }
    }
  }
}