
/**
 * Created by capri_000 on 2016/01/02.
 */
public class Merits {
  public final double self;
  public final double kill;
  public final double hide;
  public final double terr;
  public final double safe;
  public final double usur;
  public final double depl;
  public final double cent;

  private Merits(MeritsBuilder mb) {
    this.self = mb.self;
    this.kill = mb.kill;
    this.hide = mb.hide;
    this.terr = mb.terr;
    this.safe = mb.safe;
    this.usur = mb.usur;
    this.depl = mb.depl;
    this.cent = mb.cent;
  }

  public static class MeritsBuilder implements Builder<Merits> {
    private double self = 1;
    private double kill = 1;
    private double hide = 1;
    private double terr = 1;
    private double safe = 1;
    private double usur = 1;
    private double depl = 1;
    private double cent = 1;

    public MeritsBuilder setCent(double cent) {
      this.cent = cent;
      return this;
    }

    public MeritsBuilder setDepl(double depl) {
      this.depl = depl;
      return this;
    }

    public MeritsBuilder setUsur(double usur) {
      this.usur = usur;
      return this;
    }

    public MeritsBuilder setHide(double hide) {
      this.hide = hide;
      return this;
    }

    public MeritsBuilder setKill(double kill) {
      this.kill = kill;
      return this;
    }

    public MeritsBuilder setSelf(double self) {
      this.self = self;
      return this;
    }

    public MeritsBuilder setTerr(double terr) {
      this.terr = terr;
      return this;
    }

    public MeritsBuilder setSafe(double safe) {
      this.safe = safe;
      return this;
    }

    public Merits build() {
      return new Merits(this);
    }
  }
}
