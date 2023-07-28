#ifndef _mb_parser_css_
#define _mb_parser_css_

#include "dom.cpp"
#include "parser.cpp"
#include <iostream>
#include <string>
#include <vector>

const string INDENT = "  "; // 2 spaces for indentation

// Modified selector structure to support compound selectors
struct selector
{
  string tag_name;
  string id;
  vector<string> _class;
  vector<selector> descendants; // For supporting compound selectors
};

struct declaration
{
  string name;
  string value;
};

// Base class to represent any CSS entity (rule or atrule)
struct CSSEntity
{
  virtual ~CSSEntity() = default; // virtual destructor for safe deletion
};

struct rule : CSSEntity
{
  vector<selector> selectors;
  vector<declaration> declarations;
};

struct atrule : CSSEntity
{
  string keyword;
  string value;
  vector<CSSEntity *> innerEntities; // Store nested rules and atrules
};

struct stylesheet
{
  vector<CSSEntity *> entities; // Will contain both rules and atrules
};

class CSSParser : public Parser
{
  stylesheet stylesheet; // parsed DOM (result of parser)

public:
  CSSParser();         // constructor
  ~CSSParser();        // destructor
  bool skip_comment(); // Skip over comments
  void consume_void(); // Skip over comments and whitespace.
  void print(ofstream &oFile, const string &prefix);
  void print(ofstream &oFile, rule &rule, const string &prefix);
  void print_atrule(ofstream &oFile, const atrule &a, const string &prefix);
  // void print_descendant(ofstream &oFile, vector<SimpleSelector> &descendants, const string &prefix);

  void feed(string); // feed the CSS string to the class
  void parse_rules();
  rule parse_rule(); // Parse a single rule
  atrule parse_atrule();

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
  parse_rules();
  cout << "end css parser " << endl;
}

// Comment handling
bool CSSParser::skip_comment()
{
  if (starts_with("/*"))
  {
    consume_char();
    consume_char();
    while (!starts_with("*/"))
    {
      consume_char();
    }
    consume_char(); // consume '*'
    consume_char(); // consume '/'
    return true;
  }
  else
  {
    return false;
  }
}

// consume comments and whitespaces
void CSSParser::consume_void()
{
  while (true)
  {
    consume_whitespace();
    if (!skip_comment())
    {
      break;
    }
  }
}

void CSSParser::print(ofstream &oFile, const string &prefix = "")
{
  size_t ruleCount = 0;
  for (auto entity : stylesheet.entities)
  {
    if (dynamic_cast<rule *>(entity))
    {
      ++ruleCount;
    }
  }

  oFile << prefix << "{" << endl;
  oFile << prefix << INDENT << "\"total_rules\": " << ruleCount << "," << endl;
  oFile << prefix << INDENT << "\"entities\": [" << endl;

  for (size_t i = 0; i < stylesheet.entities.size(); ++i)
  {
    if (i != 0)
      oFile << "," << endl;

    if (auto r = dynamic_cast<rule *>(stylesheet.entities[i]))
    {
      print(oFile, *r, prefix + INDENT + INDENT);
    }
    else if (auto a = dynamic_cast<atrule *>(stylesheet.entities[i]))
    {
      print_atrule(oFile, *a, prefix + INDENT + INDENT);
    }
  }

  oFile << endl
        << prefix << INDENT << "]" << endl;
  oFile << prefix << "}" << endl;
}

void CSSParser::print(ofstream &oFile, rule &rule, const string &prefix = "")
{
  vector<selector> &s = rule.selectors;
  vector<declaration> &d = rule.declarations;

  oFile << prefix << "{" << endl;

  // Print selectors
  oFile << prefix << INDENT << "\"selectors\": [" << endl; // Additional indentation
  for (size_t i = 0; i < s.size(); i++)
  {
    selector &sel = s[i];
    oFile << prefix << INDENT << INDENT << "{" << endl;
    oFile << prefix << INDENT << INDENT << INDENT << "\"tag_name\": \"" << sel.tag_name << "\"," << std::endl;
    oFile << prefix << INDENT << INDENT << INDENT << "\"id\": \"" << sel.id << "\"," << std::endl;
    oFile << prefix << INDENT << INDENT << INDENT << "\"_class\": [";

    for (size_t j = 0; j < sel._class.size(); j++)
    {
      oFile << "\"" << sel._class[j] << "\"";
      if (j < sel._class.size() - 1)
      {
        oFile << prefix << ",";
      }
    }
    oFile << "]" << endl;
    oFile << prefix << INDENT << INDENT << (i < s.size() - 1 ? "}," : "}") << std::endl; // Close selector object
  }
  oFile << prefix << INDENT << "]," << endl;

  // Print declarations
  oFile << prefix << INDENT << "\"declarations\": [" << endl;
  for (size_t i = 0; i < d.size(); i++)
  {
    declaration &dec = d[i];
    oFile << prefix << INDENT << INDENT << "{" << endl
          << prefix << INDENT << INDENT << INDENT << "\"name\": \"" << dec.name << "\"," << endl
          << prefix << INDENT << INDENT << INDENT << "\"value\": \"" << dec.value << "\"" << endl
          << prefix << INDENT << INDENT << "}" << (i < d.size() - 1 ? "," : "") << std::endl;
  }
  oFile << prefix << INDENT << "]" << endl;

  oFile << prefix << "}"; // No newline here to handle comma placement in the parent print function
}

void CSSParser::print_atrule(ofstream &oFile, const atrule &a, const string &prefix = "")
{
  oFile << prefix << "{" << endl;

  oFile << prefix << INDENT << "\"keyword\": \"" << a.keyword << "\"," << endl;
  oFile << prefix << INDENT << "\"value\": \"" << a.value << "\"";

  // Check if the atrule has inner content
  if (!a.innerEntities.empty())
  {
    oFile << "," << endl;
    oFile << prefix << INDENT << "\"content\": [" << endl;
    for (size_t i = 0; i < a.innerEntities.size(); ++i)
    {
      if (i != 0)
        oFile << "," << endl;

      if (auto r = dynamic_cast<rule *>(a.innerEntities[i]))
      {
        print(oFile, *r, prefix + INDENT + INDENT);
      }
      else if (auto innerA = dynamic_cast<atrule *>(a.innerEntities[i]))
      {
        print_atrule(oFile, *innerA, prefix + INDENT + INDENT);
      }
    }
    oFile << endl;
    oFile << prefix << INDENT << "]" << endl;

    oFile << prefix << "}";
  }
}

/// Parse a sequence of rules.
void CSSParser::parse_rules()
{
  for (;;)
  {
    consume_void();
    if (buffer[0] == '@')
    {
      stylesheet.entities.push_back(new atrule(parse_atrule()));
      continue;
    }
    else if (eof())
    {
      break;
    }
    stylesheet.entities.push_back(new rule(parse_rule()));
  }
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
    consume_void();

    if (buffer[0] == ',')
    {
      consume_char();
      consume_void();
    }

    if (buffer[0] == '{')
    {

      consume_char();
      consume_void();
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

atrule CSSParser::parse_atrule()
{
  atrule a;
  // Consume '@'
  consume_char();

  // Parse the keyword
  a.keyword = parse_identifier();
  consume_void();

  // Parse the value
  while (next() != '{')
  {
    a.value += consume_char();
  }

  consume_char(); // Consume the '{'

  // Parse inner entities while the buffer doesn't reach a closing '}'

  while (next() != '}')
  {

    consume_void();

    if (buffer[0] == '@')
    {
      a.innerEntities.push_back(new atrule(parse_atrule()));
    }
    else
    {
      a.innerEntities.push_back(new rule(parse_rule()));
    }
    consume_void();
  }

  // consume the '}'
  consume_char();

  return a;
}

selector CSSParser::parse_selector()
{
  selector currentSelector;

  consume_void();
  while (true)
  {
    if (buffer[0] == '#')
    {
      consume_char();
      currentSelector.id = parse_identifier();
    }
    else if (buffer[0] == '.')
    {
      consume_char();
      currentSelector._class.push_back(parse_identifier());
    }
    else if (buffer[0] == '*')
    {
      // universal selector
      consume_char();
    }
    else if (isspace(buffer[0]))
    {
      consume_void();
      currentSelector.descendants.push_back(parse_selector());
    }
    else
    {
      currentSelector.tag_name = parse_identifier();
    }

    // Check if a compound selector follows
    char nextChar = next();
    if (nextChar == ',' || nextChar == '{')
    {
      break;
    }
  }

  return currentSelector;
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

  while (next() != ';')
  {
    if (!skip_comment())
    {
      val += consume_char();
    }
  }

  std::cout << " val:" << val << std::endl;

  return val;
}

vector<declaration> CSSParser::parse_declarations()
{

  std::cout << " parse declarations" << std::endl;

  vector<declaration> declarations;

  consume_void();

  while (next() != '}')
  {

    declarations.push_back(parse_declaration());
    consume_void();
  }

  // consume the '}'
  consume_char();

  return declarations;
}

declaration CSSParser::parse_declaration()
{
  declaration dec;
  dec.name = parse_attr();

  consume_void();
  if (consume_char() != ":")
  {
    throw std::runtime_error("attribute needs to end with :");
  }

  consume_void();
  dec.value = parse_value();
  consume_void();

  if (consume_char() != ";")
  {
    throw std::runtime_error("value needs to end with ;");
  }

  return dec;
}

CSSParser::~CSSParser()
{
  for (auto entity : stylesheet.entities)
  {
    delete entity;
  }
}

#endif
