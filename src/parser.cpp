#include "dom.cpp"
#include <iostream>
#include <string>
#include <vector>

class Parser {
  unsigned int pos;       // position of the parser
  string input;           // HTML input
  std::vector<Dom> nodes; // parsed DOM (result of parser)

  string buffer;

public:
  Parser();                  // constructor
  void feed(string);         // feed the HTML string to the class
  vector<Dom> parse_nodes(); // Parse the html document -> nodes = root element
  Dom parse_node();          // parse current node
  Dom parse_element();       // Parse a single element, including its open tag,
                             // contents, and closing tag.
  bool starts_with(string);  // check if stream starts with current chars
  bool eof();                // are we eof
  void consume_whitespace(); // skip whitespace
  string read();
  string consume_char();
  string parse_tag_name();
  string parse_text();
  std::map<std::string, std::string> parse_attributes();
  std::pair<string, string> parse_attr();
  string parse_attr_value();
};

Parser::Parser() { pos = 0; }

void Parser::feed(string html) {
  cout << "HTML feeded: " << html << endl;
  input = html;
  nodes = parse_nodes();

  // print now the result
  for (Dom &e : nodes) {
    std::cout << "print" << std::endl;
    e.print();
  }
}

/// Does the current input start with the given string?
bool Parser::starts_with(string s) {

  // read missing characters to compare
  for (int i = s.length() - buffer.length(); !eof() && i > 0; i--) {
    read();
  }

  // comparison
  if (buffer.length() >= s.length() && s == buffer.substr(0, s.length())) {
    cout << "starts_with found: " << s << endl;
    return true;
  }

  cout << "starts_with NOT found: " << s << endl;
  return false;
}

/// Return true if all input is consumed.
bool Parser::eof() { return pos >= input.length(); }

// read one char into the buffer
string Parser::read() {
  if (!eof()) {
    buffer += input[pos];
    pos++;
  }

  return buffer;
}

// skip whitespace
void Parser::consume_whitespace() {
  for (;;) {
    read();
    // if(buffer.length() > 0 && buffer[0] == ' ') {
    if (buffer.length() > 0 && isspace(static_cast<unsigned char>(buffer[0]))) {
      std::cout << "skip whitespace" << std::endl;
      buffer.erase(0, 1);
    } else {
      break;
    }
  }
}

/// Parse a sequence of sibling nodes.
vector<Dom> Parser::parse_nodes() {

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

Dom Parser::parse_node() {

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

string Parser::parse_tag_name() {

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

string Parser::parse_text() {

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

/// Return the current character
string Parser::consume_char() {

  // is buffer empty?
  if (buffer.length() == 0) {
    read();
  }

  if (buffer.length() > 0) {
    string returnStr(buffer, 0, 1); // get first char
    buffer.erase(0, 1);
    return returnStr;
  }

  throw std::runtime_error("buffer empty – EOF?");
}

std::map<std::string, std::string> Parser::parse_attributes() {
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

std::pair<string, string> Parser::parse_attr() {
  string name = parse_tag_name();

  if (consume_char() != "=") {
    throw std::runtime_error("after attribute-name there has to be an =");
  }
  string value = parse_attr_value();

  return {name, value};
}

string Parser::parse_attr_value() {
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
Dom Parser::parse_element() {

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
