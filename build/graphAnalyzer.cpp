#include <cstdlib>
#include <iostream>
#include <fstream>

#include "arguments.hpp"
#include "graph.hpp"

using namespace std;

int main(int argc, char *argv[]) {

  Arguments arguments(argc,argv);
  cerr << "Input: " << arguments.input << endl;

  Graph g(arguments.input);
  g.computeAllPairShortestPaths();

  int diameter = g.getDiameter();
  
  cout << "Diameter: " << diameter << endl;
  
  return EXIT_SUCCESS;
}
