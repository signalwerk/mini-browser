#ifndef _mb_dom_
#define _mb_dom_

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <fstream> // std::ofstream

enum class DomType
{
  ELEMENT,
  TEXT
};

class Dom
{
  int _type;
  string name;
  string _text;
  std::map<std::string, std::string> _attributes;

public:
  Dom();
  void print(std::ofstream &, int, bool);
  void tagname(string);
  string tagname();

  void type(int);
  int type();

  void text(string);
  std::vector<Dom> children; // parsed DOM (result of parser)

  std::string get_attribute(std::string name);
  bool has_attribute(std::string name);
  void add_attribute(std::string name, std::string value);
  void attributes(std::map<std::string, std::string> attributes);
};

void Dom::attributes(std::map<std::string, std::string> attributes)
{
  _attributes = attributes;
}

std::string Dom::get_attribute(std::string name)
{
  if (_attributes.count(name) > 0)
  {
    return _attributes[name];
  }
  else
  {
    return std::string("");
  }
}

void Dom::add_attribute(std::string name, std::string value)
{
  _attributes[name] = value;
}

Dom::Dom()
{
  // add_attribute("href", "http");
}

void Dom::print(std::ofstream &oFile, int indent = 0, bool last = false)
{
  oFile << string(indent, ' ') << "{" << endl;

  if (_type == static_cast<int>(DomType::TEXT))
  {
    oFile << string(indent, ' ') << "  \"type\": \"TEXT\"," << endl;
    oFile << string(indent, ' ') << "  \"text\": \"" << _text << "\"" << endl;
  }
  else if (_type == static_cast<int>(DomType::ELEMENT))
  {
    oFile << string(indent, ' ') << "  \"type\": \"ELEMENT\"," << endl;
    oFile << string(indent, ' ') << "  \"tagname\": \"" << name << "\","
          << endl;

    oFile << string(indent, ' ') << "  \"attributes\": [" << std::endl;

    std::map<std::string, std::string>::iterator it = _attributes.begin();
    while (it != _attributes.end())
    {
      // Accessing KEY from element pointed by it.
      std::string word = it->first;

      // Accessing VALUE from element pointed by it.
      std::string count = it->second;

      oFile << string(indent, ' ') << "    { \"name\": \"" << word
            << "\", \"value\": \"" << count << "\" }";

      if (it != --_attributes.end())
      {
        oFile << ",";
      }

      oFile << std::endl;

      // Increment the Iterator to point to next entry
      it++;
    }

    oFile << string(indent, ' ') << "  ]," << std::endl;

    oFile << string(indent, ' ') << "  \"children\": [" << endl;

    for (Dom &e : children)
    {

      if (&e == &children.back())
      {
        e.print(oFile, indent + 4, true);
      }
      else
      {
        e.print(oFile, indent + 4);
      }
    }

    oFile << string(indent, ' ') << "  ]";

    oFile << endl;
  }

  oFile << string(indent, ' ') << "}";

  if (!last)
  {
    oFile << ",";
  }

  oFile << endl;
}

void Dom::type(int newType) { _type = newType; }

int Dom::type() { return _type; }

void Dom::tagname(string newName) { name = newName; }

string Dom::tagname() { return name; }

void Dom::text(string newText) { _text = newText; }

#endif
