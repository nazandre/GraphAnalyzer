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

  if (g.numNodes < 2) {
    cerr << "ERROR: Input graph is empty or contains only two nodes..." << endl;
  }
  
  if (!g.isConnected()) {
    cerr << "WARNING: Input graph is not connected..." << endl;
  }

  if (arguments.numThreads != "") {
    int t = stoi(arguments.numThreads);
    omp_set_num_threads(t);
  }

  switch(arguments.computationMode) {
  case COMP_ECC_FAR:
    {
      
      if (arguments.hasNodeQueries()) {
	
	for (list<int>::iterator it = arguments.nodeQueries.begin();
	     it != arguments.nodeQueries.end();
	     ++it) {
	  
	  int nodeId = *it;
	  int graphId = nodeId - 1; // for now... later: should implement a mapping procedure
	  nodeProperty_t res = g.compute(graphId);
	  distance_t eccentricity = res.first;
	  distance_t farness = res.second;
	  
	  // from nodeId to graphId
	  cout << "Node " << nodeId << ": "
	       << "eccentricity = " << eccentricity
	       << ", "
	       << "farness = " << farness
	       << endl;
	}
	return EXIT_SUCCESS;	  
      }
      
      g.compute();
      
      cout << "Diameter: " << g.diameter << endl;
      cout << "Radius: " << g.radius << endl;
      cout << "Min Farness: " << g.minFarness << endl;
      cout << "Extremities: " << g.extremities << endl;
      cout << "Center: " << g.center << endl;
      cout << "Centroid: " << g.centroid << endl; 
    }
    break;
  case COMP_RAD_DIAM:
    {
      g.fastCompute(arguments.initialNode);
      cout << "Diameter: " << g.diameter << endl;
      cout << "Radius: " << g.radius << endl;
      return EXIT_SUCCESS;
    }
    break;
  case COMP_BETWEENNESS:
    {
      g.computeBetweenness();
      cout << "Max betweenness: " << g.maxBetweenness << endl;
      cout << "Betweenness center: " << g.betweennessCenter << endl;

      if (arguments.hasNodeQueries()) {
    
	for (list<int>::iterator it = arguments.nodeQueries.begin();
	     it != arguments.nodeQueries.end(); ++it) {
    
	  int nodeId = *it;
	  int graphId = nodeId - 1; // for now... later: should implement a mapping procedure
	  betweenness_t bet = g.getBetweenness(graphId);
	  cout << "Node " << nodeId << ": "
	       << "betweenness = " << bet
	       << endl;
	}	  
      }
    }
    break;
  }
  return EXIT_SUCCESS;
}
