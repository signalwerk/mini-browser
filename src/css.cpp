#ifndef _mb_css_
#define _mb_css_

#include "parserCss.cpp"
#include <string>

class Css {
  CSSParser parser;

public:
  Css(string);
};


Css::Css(string css) { parser.feed(css); }


#endif
