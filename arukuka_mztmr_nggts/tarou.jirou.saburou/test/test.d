import std.stdio;
import std.range;
import std.algorithm;
import std.container;
import std.typecons;
import std.random;

alias Tuple!(int, "x", int, "y") Point;

auto iterateArr()
{
  Point[] arr;
  for (int i = 0; i < 10; ++i) {
    arr ~= Point(uniform(0, 10), uniform(0, 10));
  }
  arr = arr.sort.uniq.array;
  Point[] dst;
  dst = arr.dup;
  return dst;
}

auto iterateByKey()
{
  bool[Point] set;
  for (int i = 0; i < 10; ++i) {
    set[Point(uniform(0, 10), uniform(0, 10))] = true;
  }
  Point[] arr;
  foreach (p; set.byKey) {
    arr ~= p;
  }
  return arr;
}

void test()
{
  for (int i = 0; i < 1000; ++i) {
    auto a = iterateByKey();
    auto b = iterateArr();
    auto c = a ~ b;
  }
}

void main()
{
  /+
  5.iota.each!(i => {
    writeln = i; // nothing happen
  });
  10.iota.each!((i){
    writeln = i;
  });
  15.iota.tee!(i => {
    writeln = i; // nothing happen
  });

  int[][] f = [[0, 1], [2, 3]];
  int[][] g = f.dup;
  int[][] h = f.map!(a => a.dup).array;
  g[0][0] = 9;
  f.writeln;  // modified
  h[0][0] = 0;
  f.writeln;  // unmodified

  auto rbt4 = redBlackTree!"a > b"(0, 1, 5, 7);
  while (rbt4.length) {
    rbt4.front.writeln;
    rbt4.removeFront;
  }
  +/
  test();
}

