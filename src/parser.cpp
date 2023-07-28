#ifndef _mb_parser_
#define _mb_parser_

#include "dom.cpp"
#include <iostream>
#include <string>
#include <vector>

#define DEBUG_BASICS

class Parser
{

protected:
  unsigned int pos; // position of the parser
  string input;     // input
  string buffer;

public:
  Parser();                  // constructor
                             // contents, and closing tag.
  bool starts_with(string);  // check if stream starts with current chars
  bool eof();                // are we eof
  void consume_whitespace(); // skip whitespace
  string read();
  string consume_char();
};

Parser::Parser() { pos = 0; }

/// Does the current input start with the given string?
bool Parser::starts_with(string s)
{

  // read missing characters to compare
  for (int i = s.length() - buffer.length(); !eof() && i > 0; i--)
  {
    read();
  }

  // comparison
  if (buffer.length() >= s.length() && s == buffer.substr(0, s.length()))
  {
#ifndef DEBUG_BASICS
    cout << "starts_with found: " << s << endl;
#endif

    return true;
  }
#ifndef DEBUG_BASICS
  cout << "starts_with not found: " << s << endl;
#endif

  return false;
}

/// Return true if all input is consumed.
bool Parser::eof() { return pos >= input.length(); }

// skip whitespace
void Parser::consume_whitespace()
{
  for (;;)
  {
    read();
    // if(buffer.length() > 0 && buffer[0] == ' ') {
    if (buffer.length() > 0 && isspace(static_cast<unsigned char>(buffer[0])))
    {

#ifndef DEBUG_BASICS
      std::cout << "skip whitespace" << std::endl;
#endif

      buffer.erase(0, 1);
    }
    else
    {
      break;
    }
  }
}

// read one char into the buffer
string Parser::read()
{
  if (!eof())
  {
    buffer += input[pos];
    pos++;
  }

  return buffer;
}

/// Return the current character
string Parser::consume_char()
{

  // is buffer empty?
  if (buffer.length() == 0)
  {
    read();
  }

  if (buffer.length() > 0)
  {
    string returnStr(buffer, 0, 1); // get first char
    buffer.erase(0, 1);
    return returnStr;
  }

  throw std::runtime_error("buffer empty – EOF?");
}

#endif
