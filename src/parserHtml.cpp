#ifndef _mb_parser_html_
#define _mb_parser_html_

#include "dom.cpp"
#include "parser.cpp"
#include <iostream>
#include <string>
#include <vector>

class HTMLParser : public Parser {

  std::vector<Dom> nodes; // parsed DOM (result of parser)

public:
  HTMLParser();                // constructor
  void print(ofstream &oFile); // std out print
  void feed(string);           // feed the HTML string to the class
  vector<Dom> parse_nodes(); // Parse the html document -> nodes = root element
  Dom parse_node();          // parse current node
  Dom parse_element();       // Parse a single element, including its open tag,
                             // contents, and closing tag.
  string parse_tag_name();
  string parse_text();
  std::map<std::string, std::string> parse_attributes();
  std::pair<string, string> parse_attr();
  string parse_attr_value();
};

HTMLParser::HTMLParser() { pos = 0; }

void HTMLParser::feed(string html) {
  cout << "HTML feeded: " << html << endl;
  input = html;
  nodes = parse_nodes();
}

void HTMLParser::print(ofstream &oFile) {

  // print now the result
  // for (Dom &e : nodes) {
  //   std::cout << "print" << std::endl;
  //   e.print(oFile);
  // }

  // print first node
  nodes.front().print(oFile, 0, true);
}

/// Parse a sequence of sibling nodes.
vector<Dom> HTMLParser::parse_nodes() {

  vector<Dom> lNodes;

  for (;;) {

    std::cout << "consume" << std::endl;

    // consume leading whitespace
    consume_whitespace();

    if (eof() || starts_with("</")) {
      break;
    }

    std::cout << "parse node" << std::endl;

    // ok there is still something to parse
    lNodes.push_back(parse_node());
  }

  return lNodes;
}

Dom HTMLParser::parse_node() {

  if (buffer.length() == 0) {
    read();
  }

  Dom dom;
  if (buffer[0] == '<') {
    dom = parse_element();
    dom.type(static_cast<int>(DomType::ELEMENT));

  } else {
    std::cout << "start parse text" << std::endl;
    dom.type(static_cast<int>(DomType::TEXT));
    dom.text(parse_text());
  }

  return dom;
}

string HTMLParser::parse_tag_name() {

  string tagname;

  for (;;) {

    if (buffer.length() == 0) {
      read();
    }

    // if(buffer.length() > 0 && buffer[0] == ' ') {
    if (isdigit(buffer[0]) || isalpha(buffer[0])) {
      tagname += consume_char();
    } else {
      break;
    }
  }

  return tagname;
}

string HTMLParser::parse_text() {

  string text;

  for (;;) {

    if (buffer.length() == 0) {
      read();
    }

    // if(buffer.length() > 0 && buffer[0] == ' ') {
    if (buffer[0] != '<') {
      text += consume_char();
    } else {
      break;
    }
  }

  cout << "text parsed: '" << text << "'" << endl;
  return text;
}

std::map<std::string, std::string> HTMLParser::parse_attributes() {
  std::map<std::string, std::string> attributes;

  for (;;) {

    consume_whitespace();

    if (buffer[0] == '>') {
      break;
    }

    std::pair<string, string> data = parse_attr();
    attributes[std::get<0>(data)] = std::get<1>(data);
  }
  return attributes;
}

std::pair<string, string> HTMLParser::parse_attr() {
  string name = parse_tag_name();

  if (consume_char() != "=") {
    throw std::runtime_error("after attribute-name there has to be an =");
  }
  string value = parse_attr_value();

  return {name, value};
}

string HTMLParser::parse_attr_value() {
  string open_quote = consume_char();

  if (open_quote[0] != '"' && open_quote[0] != '\'') {
    throw std::runtime_error("attribute values have to be quoted");
  }

  string text;

  for (;;) {

    if (buffer.length() == 0) {
      read();
    }

    if (buffer[0] != open_quote[0]) {
      text += consume_char();
    } else {
      break;
    }
  }

  if (consume_char() != open_quote) {
    throw std::runtime_error(
        "attribute values have to start and end with the same quote-style");
  }

  return text;
}

// Parse a single element, including its open tag, contents, and closing tag.
Dom HTMLParser::parse_element() {

  Dom dom;

  if (consume_char() != "<") {
    throw std::runtime_error("Tags should start with <");
  }

  dom.tagname(parse_tag_name());

  // let attrs = self.parse_attributes();
  dom.attributes(parse_attributes());

  // todo
  // attributes should be parsed here!!!

  consume_whitespace();

  if (consume_char() != ">") {
    std::cout << "buffer: '" << buffer << "'" << std::endl;
    throw std::runtime_error("Tags should end with >");
  }

  dom.children = parse_nodes();

  if (consume_char() != "<") {
    std::cout << "missing closing tag: '" << dom.tagname() << "'" << std::endl;
    throw std::runtime_error("please close tag");
  }
  consume_whitespace();

  if (consume_char() != "/") {
    std::cout << "missing closing tag: '" << dom.tagname() << "'" << std::endl;
    throw std::runtime_error("please close tag");
  }
  consume_whitespace();

  if (parse_tag_name() != dom.tagname()) {
    std::cout << "closing tag not matching with open tag: '" << dom.tagname()
              << "'" << std::endl;
    throw std::runtime_error("closing tag not matching with open tag");
  }

  consume_whitespace();

  if (consume_char() != ">") {
    std::cout << "missing closing tag: '" << dom.tagname() << "'" << std::endl;
    throw std::runtime_error("please close tag");
  }

  return dom;
}
#endif
