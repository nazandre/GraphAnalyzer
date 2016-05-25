#ifndef GRAPH_H_
#define GRAPH_H_

#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/exterior_property.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>
#include <boost/graph/closeness_centrality.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/property_maps/constant_property_map.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>

#include "node.hpp"

using namespace boost;

typedef adjacency_list<listS, vecS, undirectedS, no_property,  property< edge_weight_t, int> > Graph_t;

typedef graph_traits<Graph_t>::vertex_descriptor Vertex;
typedef graph_traits<Graph_t>::edge_descriptor Edge;

typedef property_map<Graph_t, boost::vertex_index_t>::type NameMap;

// Declare a matrix type and its corresponding property map that
// will contain the distances between each pair of vertices.
typedef exterior_vertex_property<Graph_t, int> DistanceProperty;
typedef DistanceProperty::matrix_type DistanceMatrix;
typedef DistanceProperty::matrix_map_type DistanceMatrixMap;

// Declare the weight map so that each edge returns the same value.
typedef constant_property_map<Edge, int> WeightMap;

typedef exterior_vertex_property<Graph_t, float> ClosenessProperty;
typedef ClosenessProperty::container_type ClosenessContainer;
typedef ClosenessProperty::map_type ClosenessMap;

//#define FLOYD_WARSHALL_APSP
//#define JOHNSON_APSP
//#define DIJKSTRA_BASED_APSP
#define BFS_BASED_APSP
#define PARALLEL_APSP

class Graph {

public:
  std::string input;

  Graph_t graph;
  Node *nodes;
  NameMap nameMap;

  DistanceMatrix *distances;
  DistanceMatrixMap *distanceMap;

  int size;
  int diameter;
  
  Graph();
  Graph(std::string i);
  Graph(const Graph &g);
  ~Graph();

  void read();

  void computeAllPairShortestPaths();
  void printDistances();

  void computeDiameter();
  int getDiameter();
  
};


#endif
