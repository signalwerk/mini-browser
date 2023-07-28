#ifndef _mb_html_
#define _mb_html_

#include "parserHtml.cpp"
#include <string>

#include <fstream> // std::ofstream

class Html
{
  int width, height;
  HTMLParser parser;

public:
  Html(string);
  void setViewport(int, int);
  void print(std::ofstream &);
};

void Html::setViewport(int w, int h)
{
  width = w;
  height = h;
}

Html::Html(string html) { parser.feed(html); }

void Html::print(std::ofstream &oFile) { parser.print(oFile); }

#endif
