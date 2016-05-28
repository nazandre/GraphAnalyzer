#ifndef GRAPH_H_
#define GRAPH_H_

#include <utility>
#include "node.hpp"

typedef Node* graph_t; 
typedef int distance_t;
typedef distance_t* distanceMap_t;
typedef Node** parentMap_t;
typedef std::pair<distance_t,distance_t> nodeProperty_t; // eccentricity, farness

#define BFS_BASED_APSP
#define PARALLEL_APSP

class Graph {

public:
  std::string input;

  graph_t graph;

  int numNodes;
  int numEdges;

  bool computed; // true, if everything has been computed
  
  distance_t diameter;
  distance_t radius;
  distance_t minFarness;
  
  distanceMap_t eccentricity;
  distanceMap_t farness;
  
  nodeList_t center;
  nodeList_t centroid;
  
  Graph();
  Graph(std::string i);
  Graph(const Graph &g);
  ~Graph();

  void read();
  void addEdge(int i, int j);
  void breadthFirstSearch(Node *root,
			  distanceMap_t distances,
			  parentMap_t parents);

  void printDistance(distanceMap_t d);
  distance_t maxDistance(distanceMap_t d);
  distance_t minDistance(distanceMap_t d);
  distance_t sumDistance(distanceMap_t d); 

  // Check whether or not the configuration is connected (i.e., any node can reach any other)
  // Perform a BFS from node 0.
  // Complexity: O(n+m) time, O(n) memory
  bool isConnected();
  
  // Compute diameter, radius, center and centroid
  // Complexity: O(n) BFSes (O(n(n+m) time, O(n*#threads) memory)
  //    BFSes run in parallel
  void compute();

  // Compute the eccentricity and farness of a single node of index "index"
  // Complexity: O(1) BFS (O(n+m) time, O(n) memory)
  nodeProperty_t compute(int index); // compute values for a single node!

  // Compute only diameter and radius.
  // Complexity: O(n) BFSes (O(n(n+m) time, O(n) memory)
  //    In practice uses O(1) BFSes...
  // Inspired by: Borassi, M., Crescenzi, P., Habib, M., Kosters, W., Marino, A.,
  // & Takes, F. (2014, July). On the solvability of the six degrees of kevin bacon game.
  // In Fun with Algorithms (pp. 52-63). Springer International Publishing.
  void fastCompute();
  
  distance_t getEccentricity(int index);
  distance_t getFarness(int index);

  nodeList_t& getCenter();
  nodeList_t& getCentroid();
  
};


#endif
