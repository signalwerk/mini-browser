#include "parser.cpp"
#include <string>

class Html {
  int width, height;
  Parser parser;

public:
  Html(string);
  void set_values(int, int);
};

void Html::set_values(int x, int y) {
  width = x;
  height = y;
}

Html::Html(string html) { parser.feed(html); }
