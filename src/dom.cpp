#include <string>
#include <iostream>
#include <map>

class Dom {
    string name;
    string _text;
    std::map<std::string, std::string> _attributes;

  public:
    Dom();
    void print(int indent);
    void tagname(string);
    string tagname();
    void text(string);
    std::vector<Dom> children; //parsed DOM (result of parser)

    std::string get_attribute(std::string key);
    bool has_attribute(std::string key);
    void add_attribute(std::string key, std::string value);
    void attributes(std::map<std::string, std::string> attributes);
};

void Dom::attributes(std::map<std::string, std::string> attributes){
  _attributes = attributes;
}


std::string Dom::get_attribute(std::string key){
	if (_attributes.count(key) > 0){
		return _attributes[key];
	} else {
		return std::string("");
	}
}


void Dom::add_attribute(std::string key, std::string value){
	_attributes[key] = value;
}


Dom::Dom () {
  name = "Element-Name";
  add_attribute("href", "http");
}

void Dom::print(int indent = 0) {
  cout << string(indent, ' ') << "{" << endl;
  cout << string(indent, ' ') << "  tagname: '" << name << "'," << endl;
  cout << string(indent, ' ') << "  text: '" << _text << "'," << endl;


  std::cout  << string(indent, ' ') << "  attributes: [" << std::endl;

  std::map<std::string, std::string>::iterator it = _attributes.begin();
  while (it != _attributes.end())
	{
		// Accessing KEY from element pointed by it.
		std::string word = it->first;

		// Accessing VALUE from element pointed by it.
		std::string count = it->second;

		std::cout  << string(indent, ' ') << "    { key: '" << word << "', value: '" << count << "' }," << std::endl;

		// Increment the Iterator to point to next entry
		it++;
	}
  std::cout  << string(indent, ' ') << "  ]" << std::endl;


  cout << string(indent, ' ') << "  children: [" << endl;
  // print now the result
  for(Dom& e : children)
  {
     e.print(indent + 4);
  }
  cout << string(indent, ' ') << "  ]" << endl;


  cout << string(indent, ' ') << "}," << endl;
}

void Dom::tagname(string newName) {
  name = newName;
}

void Dom::text(string newText) {
  _text = newText;
}


string Dom::tagname() {
  return name;
}
