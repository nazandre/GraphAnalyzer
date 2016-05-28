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
  
  Graph g(arguments.input);

  if (!g.isConnected()) {
    cerr << "WARNING: Input graph is not connected..." << endl;
  }

  if (arguments.numThreads != "") {
    int t = stoi(arguments.numThreads);
    omp_set_num_threads(t);
  }
  
  if (arguments.fastCompute) {
    g.fastCompute();
    cout << "Diameter: " << g.diameter << endl;
    cout << "Radius: " << g.radius << endl;
    return EXIT_SUCCESS;
  }

  g.compute();
  
  cout << "Diameter: " << g.diameter << endl;
  cout << "Radius: " << g.radius << endl;
  cout << "Min Farness: " << g.minFarness << endl;
  cout << "Center: " << g.center << endl;
  cout << "Centroid: " << g.centroid << endl;
  
  for (list<int>::iterator it = arguments.nodeQueries.begin();
      it != arguments.nodeQueries.end();
      it++) {
    int nodeId = *it;
    int graphId = nodeId - 1; // for now... later: should implement a mapping procedure
                              // from nodeId to graphId
    cout << "Node " << graphId << ": "
	 << "eccentricity = " << g.getEccentricity(graphId)
	 << ", "
	 << "farness = " << g.getFarness(graphId)
	 << endl;
  }
  
  return EXIT_SUCCESS;
}
