import java.io.BufferedReader;

public class CommentedIStream {
  //BufferedReader stdReader = new BufferedReader(new InputStreamReader(System.in));
  public static String[] read(BufferedReader stdReader) {
    String line = "";
    try {
      for (line = stdReader.readLine(); line.startsWith("#"); line = stdReader.readLine()) ;
    } catch (Exception e) {
      e.getStackTrace();
      System.exit(-1);
    }
    return line.split("\\s");
  }
}
