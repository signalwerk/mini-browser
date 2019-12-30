#ifndef _mb_css_
#define _mb_css_

#include "parserCss.cpp"
#include <string>

class Css {
  CSSParser parser;

public:
  Css(string);
  void print(std::ofstream &);
};

Css::Css(string css) { parser.feed(css); }

void Css::print(std::ofstream &oFile) { parser.print(oFile); }

#endif
