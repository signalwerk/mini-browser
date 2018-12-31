#include <stdio.h>
#include <iostream>

using namespace std;

#include "html.cpp"


int main() {


  cout << "-- start parser --" << endl;

  try {
    // test for self closing tags
    // Html html ("    <html>test <b>bold</b> text  <img href='http://'>image tag</img> text </html>");
    Html html ("    <html>test <b>bold</b> text  <img>image tag</img> text </html>");
  } catch(const std::exception& e) {
      std::cout << e.what() << '\n';
      throw;   // rethrows the exception object of type std::length_error
  }

  cout << "-- end parser --" << endl;



  return 0;
}
