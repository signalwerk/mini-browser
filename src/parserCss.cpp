#ifndef _mb_parser_css_
#define _mb_parser_css_

#include "dom.cpp"
#include "parser.cpp"
#include <iostream>
#include <string>
#include <vector>

struct selector {
  string tag_name;
  string id;
  vector<string> _class;
};

struct declaration {
  string name;
  string value;
};

struct rule {
  vector<selector> selectors;
  vector<declaration> declarations;
};

struct stylesheet {
  vector<rule> rules;
};

// enum selector {
//     Simple(Simpleselector),
// }

class CSSParser : public Parser {

  stylesheet stylesheet; // parsed DOM (result of parser)

public:
  CSSParser();       // constructor
  void feed(string); // feed the CSS string to the class
  vector<rule> parse_rules();
  rule parse_rule(); // Parse a single rule
  vector<selector> parse_selectors();

  selector parse_selector();
  vector<declaration> parse_declarations();

  declaration parse_declaration();
  string parse_identifier();
};

CSSParser::CSSParser() { pos = 0; }

void CSSParser::feed(string css) {
  cout << "CSS feeded: " << css << endl;
  input = css;
  stylesheet.rules = parse_rules();

  cout << "end css parser " << endl;

  // print now the result
  // for (Dom &e : nodes) {
  //   std::cout << "print" << std::endl;
  //   e.print();
  // }
}

/// Parse a sequence of rules.
vector<rule> CSSParser::parse_rules() {

  vector<rule> rules;

  for (;;) {
    std::cout << "consume css" << std::endl;
    consume_whitespace();
    if (eof()) {
      break;
    }
    rules.push_back(parse_rule());
  }

  return rules;
}

/// Parse a rule set: `<selectors> { <declarations> }`.
rule CSSParser::parse_rule() {

  rule rule;

  rule.selectors = parse_selectors();
  rule.declarations = parse_declarations();
  return rule;
}

/// Parse a comma-separated list of selectors.
vector<selector> CSSParser::parse_selectors() {
  vector<selector> selectors;

  for (;;) {
    std::cout << "consume selectors" << std::endl;
    selectors.push_back(parse_selector());
    consume_whitespace();

    if (buffer[0] == ',') {

      consume_char();
      consume_whitespace();
    } else if (buffer[0] == '{') {
      throw std::runtime_error("Unexpected character {} in selector list");
    }

    if (eof()) {
      break;
    }
  }

  // // Return selectors with highest specificity first, for use in matching.
  // selectors.sort_by(| a, b | b.specificity().cmp(&a.specificity()));

  return selectors;
}

selector CSSParser::parse_selector() {

  selector selector;
  // selector.tag_name = NULL;
  // selector.id = NULL;
  // selector.class = NULL;

  for (;;) {
    std::cout << "consume selector" << std::endl;

    consume_whitespace();

    if (buffer[0] == '#') {

      consume_char();
      selector.id = parse_identifier();

    } else if (buffer[0] == '.') {

      consume_char();
      selector._class.push_back(parse_identifier());

    } else if (buffer[0] == '*') {

      // universal selector
      consume_char();

    } else {

      // todo valid_identifier_char(c)
      // throw std::runtime_error("Unexpected character {} in selector list");
      selector.tag_name = parse_identifier();
    }

    if (eof()) {
      break;
    }
  }

  return selector;
}

string CSSParser::parse_identifier() {

  string identifier;

  for (;;) {

    if (buffer.length() == 0) {
      read();
    }

    if (isdigit(buffer[0]) || isalpha(buffer[0]) || buffer[0] == '-' ||
        buffer[0] == '_') {
      identifier += consume_char();
    } else {
      break;
    }
  }

  return identifier;
}

vector<declaration> CSSParser::parse_declarations() {
  string open_declaration = consume_char();

  if (open_declaration[0] != '{') {
    throw std::runtime_error("declaration needs to open with {");
  }

  vector<declaration> declarations;

  for (;;) {
    consume_whitespace();
    if (buffer[0] == '}') {
      consume_char();
      break;
    }
    declarations.push_back(parse_declaration());

    if (eof()) {
      break;
    }
  }

  return declarations;
}

declaration CSSParser::parse_declaration() {
  declaration dec;
  dec.name = parse_identifier();

  consume_whitespace();
  if (consume_char() != ":") {
    throw std::runtime_error("declaration needs to open with {");
  }

  consume_whitespace();
  dec.value = parse_identifier();
  consume_whitespace();

  if (consume_char() != ";") {
    throw std::runtime_error("declaration needs to open with {");
  }

  return dec;
}

#endif
