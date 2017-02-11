#include "samurai.hpp"
#include <thread>

SamuraiScanner::SamuraiScanner() {}

SamuraiScanner::SamuraiScanner(FILE* in):
  input(in), commentChar('#') {
}

void SamuraiScanner::skipComments() {
  char c = fgetc(input);
  while (isspace(c) || c == commentChar) {
    if (c == commentChar) {
      do {
	c = fgetc(input);
      } while (c != '\n');
    } else {
      c = fgetc(input);
    }
  }
  ungetc(c, input);
}

int SamuraiScanner::get() {
  skipComments();
  int value;
  if (fscanf(input, "%d", &value) != 1) return -99999;
  return value;
}

string SamuraiScanner::gets() {
  skipComments();
  for (char c = fgetc(input); c != '"'; c = fgetc(input)) {
    if (!isspace(c)) {
      throw ErrorReport(string("Character '") + c + 
			"' found where a string literal is expected");
    }
  }
  string str = "";
  for (char c = fgetc(input); c != '"'; c = fgetc(input)) {
    str += c;
  }
  return str;
}
