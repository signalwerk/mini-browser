#ifndef _mb_parser_css_
#define _mb_parser_css_

#include "dom.cpp"
#include "parser.cpp"
#include <iostream>
#include <string>
#include <vector>

struct selector
{
  string tag_name;
  string id;
  vector<string> _class;
};

struct declaration
{
  string name;
  string value;
};

struct rule
{
  vector<selector> selectors;
  vector<declaration> declarations;
};

struct stylesheet
{
  vector<rule> rules;
};

// enum selector {
//     Simple(Simpleselector),
// }

class CSSParser : public Parser
{

  stylesheet stylesheet; // parsed DOM (result of parser)

public:
  CSSParser();                             // constructor
  void print(ofstream &oFile);             // std out print
  void print(ofstream &oFile, rule &rule); // std out print

  void feed(string); // feed the CSS string to the class
  vector<rule> parse_rules();
  rule parse_rule(); // Parse a single rule
  vector<selector> parse_selectors();

  selector parse_selector();
  vector<declaration> parse_declarations();

  declaration parse_declaration();
  string parse_identifier();
  string parse_attr();
  string parse_value();
};

CSSParser::CSSParser() { pos = 0; }

void CSSParser::feed(string css)
{
  cout << "CSS feeded" << endl;
  input = css;
  stylesheet.rules = parse_rules();

  cout << "end css parser " << endl;
}

void CSSParser::print(ofstream &oFile)
{
  oFile << "{" << endl;
  oFile << "  \"total_rules\": " << stylesheet.rules.size() << "," << endl;
  oFile << "  \"rules\": [" << endl;

  for (size_t i = 0; i < stylesheet.rules.size(); i++)
  {
    oFile << "    "; // Indent each rule
    print(oFile, stylesheet.rules[i]);
    if (i < stylesheet.rules.size() - 1)
    {
      oFile << "," << endl;
    }
    else
    {
      oFile << endl;
    }
  }

  oFile << "  ]" << endl;
  oFile << "}" << endl;
}

void CSSParser::print(ofstream &oFile, rule &rule)
{
  vector<selector> &s = rule.selectors;
  vector<declaration> &d = rule.declarations;

  oFile << "{" << endl;

  // Print selectors
  oFile << "      \"selectors\": [" << endl; // Additional indentation
  for (size_t i = 0; i < s.size(); i++)
  {
    selector &sel = s[i];
    oFile << "        {" << endl;
    oFile << "          \"tag_name\": \"" << sel.tag_name << "\"," << std::endl;
    oFile << "          \"id\": \"" << sel.id << "\"," << std::endl;
    oFile << "          \"_class\": [";
    for (size_t j = 0; j < sel._class.size(); j++)
    {
      oFile << "\"" << sel._class[j] << "\"";
      if (j < sel._class.size() - 1)
        oFile << ", ";
    }
    oFile << "]";
    oFile << (i < s.size() - 1 ? "        }," : "        }") << std::endl; // Close selector object
  }
  oFile << "      ]," << endl;

  // Print declarations
  oFile << "      \"declarations\": [" << endl;
  for (size_t i = 0; i < d.size(); i++)
  {
    declaration &dec = d[i];
    oFile << "        {\"name\": \"" << dec.name << "\", \"value\": \"" << dec.value << "\"}";
    oFile << (i < d.size() - 1 ? "," : "") << std::endl;
  }
  oFile << "      ]" << endl;

  oFile << "    }"; // No newline here to handle comma placement in the parent print function
}

/// Parse a sequence of rules.
vector<rule> CSSParser::parse_rules()
{

  vector<rule> rules;

  for (;;)
  {
    consume_whitespace();
    if (eof())
    {
      break;
    }
    rules.push_back(parse_rule());
  }

  return rules;
}

/// Parse a rule set: `<selectors> { <declarations> }`.
rule CSSParser::parse_rule()
{
  std::cout << "parse_rule css" << std::endl;

  rule rule;

  rule.selectors = parse_selectors();
  rule.declarations = parse_declarations();
  return rule;
}

/// Parse a comma-separated list of selectors.
vector<selector> CSSParser::parse_selectors()
{
  vector<selector> selectors;

  for (;;)
  {
    std::cout << "consume selectors" << std::endl;
    selectors.push_back(parse_selector());
    consume_whitespace();

    if (buffer[0] == ',')
    {
      consume_char();
      consume_whitespace();
    }

    if (buffer[0] == '{')
    {

      consume_char();
      consume_whitespace();
      break;

      // throw std::runtime_error("Unexpected character {} in selector list");
    }

    if (eof())
    {
      break;
    }
  }

  // // Return selectors with highest specificity first, for use in matching.
  // selectors.sort_by(| a, b | b.specificity().cmp(&a.specificity()));

  return selectors;
}

selector CSSParser::parse_selector()
{

  selector selector;

  std::cout << "consume selector" << std::endl;

  consume_whitespace();

  if (buffer[0] == '#')
  {

    consume_char();
    selector.id = parse_identifier();
  }
  else if (buffer[0] == '.')
  {

    consume_char();
    selector._class.push_back(parse_identifier());
  }
  else if (buffer[0] == '*')
  {
    // universal selector
    consume_char();
  }
  else
  {

    // todo valid_identifier_char(c)
    // throw std::runtime_error("Unexpected character {} in selector list");
    selector.tag_name = parse_identifier();
  }

  return selector;
}

string CSSParser::parse_identifier()
{
  std::cout << "parse identifier" << std::endl;

  string identifier;

  for (;;)
  {

    if (buffer.length() == 0)
    {
      read();
    }

    if (isdigit(buffer[0]) || isalpha(buffer[0]) || buffer[0] == '-' ||
        buffer[0] == '_')
    {
      identifier += consume_char();
    }
    else
    {
      break;
    }
  }
  std::cout << "   " << identifier << std::endl;

  return identifier;
}

string CSSParser::parse_attr()
{
  std::cout << "parse attr" << std::endl;

  string attr;

  for (;;)
  {

    if (buffer.length() == 0)
    {
      read();
    }

    if (isdigit(buffer[0]) || isalpha(buffer[0]) || buffer[0] == '-' ||
        buffer[0] == '_')
    {
      attr += consume_char();
    }
    else
    {
      break;
    }
  }
  std::cout << "  " << attr << std::endl;

  return attr;
}

string CSSParser::parse_value()
{
  std::cout << "parse val" << std::endl;

  string val;

  for (;;)
  {

    if (buffer.length() == 0)
    {
      read();
    }

    if (buffer[0] != ';')
    {
      val += consume_char();
    }
    else
    {
      break;
    }
  }
  std::cout << " val:" << val << std::endl;

  return val;
}

vector<declaration> CSSParser::parse_declarations()
{

  std::cout << " parse declarations" << std::endl;

  vector<declaration> declarations;

  for (;;)
  {
    consume_whitespace();
    if (buffer[0] == '}')
    {
      consume_char();
      break;
    }
    declarations.push_back(parse_declaration());

    if (eof())
    {
      break;
    }
  }

  return declarations;
}

declaration CSSParser::parse_declaration()
{
  declaration dec;
  dec.name = parse_attr();

  consume_whitespace();
  if (consume_char() != ":")
  {
    throw std::runtime_error("attribute needs to end with :");
  }

  consume_whitespace();
  dec.value = parse_value();
  consume_whitespace();

  if (consume_char() != ";")
  {
    throw std::runtime_error("value needs to end with ;");
  }

  return dec;
}

#endif
