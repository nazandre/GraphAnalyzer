#include <cstdlib>
#include <iostream>
#include <fstream>
#include <omp.h>

#include "arguments.hpp"
#include "graph.hpp"

using namespace std;

int main(int argc, char *argv[]) {

  Arguments arguments(argc,argv);
  cerr << "Input: " << arguments.input << endl;

  omp_set_num_threads(4);
  
  Graph g(arguments.input);
  //g.computeAllPairShortestPaths();

  //g.compute();

  g.fastCompute();
  
  int &diameter = g.diameter;
  int &radius = g.radius;
  cout << "Diameter: " << diameter << endl;
  cout << "Radius: " << radius << endl;
  
  return EXIT_SUCCESS;
}
