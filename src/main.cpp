#include <iostream>
#include <stdio.h>

using namespace std;

#include "css.cpp"
#include "html.cpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

string get_file_string(string filename)
{
  std::ifstream ifs(filename);
  return string((std::istreambuf_iterator<char>(ifs)),
                (std::istreambuf_iterator<char>()));
}

static bool endsWith(const std::string &str, const std::string &suffix)
{
  return str.size() >= suffix.size() &&
         0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

int main(int argc, char *argv[])
{

  if (argc != 3)
  {
    std::cerr << "Usage: mini-browser"
              << " index.html out.json" << std::endl;
    return 1;
  }

  std::string inputFilename(argv[1]);
  std::string outputFilename(argv[2]);

  cout << "-- start parser --" << endl;

  try
  {
    // test for self closing tags
    // Html html ("    <html>test <b>bold</b> text  <img href='http://'>image
    // tag</img> text </html>");
    // Html html("    <html lang='de'>test <b>bold</b> text  <img>image
    // tag</img> "
    //           "text </html>");

    std::string characters = get_file_string(inputFilename);

    if (endsWith(inputFilename, "html"))
    {
      cout << "-- HTML Parser --" << endl;

      Html html(characters);

      ofstream myfile;
      myfile.open(outputFilename);
      html.print(myfile);
      myfile.close();
    }
    else if (endsWith(inputFilename, "css"))
    {
      cout << "-- CSS Parser --" << endl;

      Css css(characters);

      ofstream myfile;
      myfile.open(outputFilename);

      css.print(myfile);
      myfile.close();
    }

    // Css css("h1, h2, h3 { margin: auto; color: #cc0000; }");
  }
  catch (const std::exception &e)
  {
    std::cout << e.what() << '\n';
    throw; // rethrows the exception object of type std::length_error
  }

  cout << "-- end parser --" << endl;

  return 0;
}
