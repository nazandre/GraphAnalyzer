#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "graph.hpp"

using namespace std;

//#define INPUT_GRAPH_READING_DEBUG
//#define APSP_DEBUG

#define NOT_COMPUTED_YET -1

Graph::Graph() {
  nodes = NULL;
  distances = NULL;
  distanceMap = NULL;
  diameter = NOT_COMPUTED_YET;
}

Graph::Graph(std::string i) : nameMap(get(vertex_index, graph)) {
  input = i;
  nodes = NULL;
  distances = NULL;
  distanceMap = NULL;
  diameter = NOT_COMPUTED_YET;
  read();
}

Graph::Graph(const Graph &g) {
  input = g.input;
  nameMap = g.nameMap;
  nodes = NULL;
  distances = NULL;
  distanceMap = NULL;
  diameter = NOT_COMPUTED_YET;
  read();
}

Graph::~Graph() {
  delete[] nodes;
  delete distances;
  delete distanceMap;
}

void Graph::read() {
  string line;
  ifstream file(input);
  
  file >> size;
  cerr << "Graph: " << size << " nodes" << endl;
  nodes = new Node[size];

  getline(file,line); // skip current line
  
  for (int i = 0; i < size; i++) {
    if(getline(file,line)) {
      int numNeighbors, nodeId, graphId;
      istringstream iss(line);
      iss >> nodeId;
      nodes[i].graphId = i;
      nodes[i].nodeId = nodeId;
      iss >> numNeighbors;
#ifdef INPUT_GRAPH_READING_DEBUG
      cout << "Line: " << i+1 << ": \"" <<  line << "\"" << endl;
      cout << "\t nodeId: " << nodeId << endl;
      cout << "\t numNeighbors: " << numNeighbors << endl;
      cout << "\t neighbors:";
#endif
      for (int j = 0; j < numNeighbors; j++) {
	if (iss >> graphId) {
#ifdef INPUT_GRAPH_READING_DEBUG
	  cout << " " << graphId;
#endif
	  add_edge(i,graphId-1,1,graph);
	} else {
	  cerr << "ERROR: " << input << " file corrupted." << endl;
	}
      }
#ifdef INPUT_GRAPH_READING_DEBUG
      cout << endl;
#endif
    } else {
      cerr << "ERROR: " << input << " file corrupted." << endl;
    }
  }
  file.close();
}

/*********** All Pair Shortest Path Computation ***********/

void Graph::computeAllPairShortestPaths() {
  distances = new DistanceMatrix(size);
  //distanceMap = new DistanceMatrixMap(*distances, graph);
  
  DistanceMatrix &d = *distances;
  distanceMap = new DistanceMatrixMap(d, graph);
  DistanceMatrixMap dm = *distanceMap;
  
  cerr << "Computing all pairs shortest paths..." << endl;
  
#ifdef FLOYD_WARSHALL_APSP
  // All pairs shortest paths using FW
  cerr << "Floyd Warshall algorithm..." << endl;
  floyd_warshall_all_pairs_shortest_paths(graph,dm);
#endif
#ifdef JOHNSON_APSP
  cerr << "Johnson algorithm..." << endl;
  johnson_all_pairs_shortest_paths(graph,dm);
#endif

#ifdef DIJKSTRA_BASED_APSP
  cerr << "Dijkstra-based algorithm..." << endl;
#ifdef PARALLEL_APSP
  cerr << "Parallel optimization using OpenMP..." << endl;
#pragma omp parallel for
  for(int i = 0; i < size; i++) {
    //Vertex v = vertex(i,graph);
    dijkstra_shortest_paths(graph, i, distance_map(dm[i]));
  }
#else
  graph_traits<Graph_t>::vertex_iterator i, end;
  for(boost::tie(i, end) = vertices(graph); i != end; ++i) {
    dijkstra_shortest_paths(graph, *i, distance_map(dm[*i]));
  }
#endif
#endif
  
#ifdef BFS_BASED_APSP
  cerr << "Breadth-First Search (BFS) based algorithm..." << endl;
#ifdef PARALLEL_APSP
  cerr << "Parallel optimization using OpenMP..." << endl;
#pragma omp parallel for
  for(int i = 0; i < size; i++) {
    //Vertex v = vertex(i,graph);
    breadth_first_search(graph, i,
			 visitor(make_bfs_visitor(record_distances(dm[i],on_tree_edge()))));
  }
#else
  graph_traits<Graph_t>::vertex_iterator i, end;
  for(boost::tie(i, end) = vertices(graph); i != end; ++i) {
    breadth_first_search(graph, *i,
			 visitor(make_bfs_visitor(record_distances(dm[*i],on_tree_edge()))));
  }
#endif
#endif
  
#ifdef APSP_DEBUG
  printDistances();
#endif
}

void Graph::computeDiameter() {
  diameter = NOT_COMPUTED_YET;
  if (distances != NULL) {
    // later version will use eccentricity definition,
    // just for now
    DistanceMatrix &D = *distances;
    for (int j = 0; j < size; ++j) {
      for (int i = 0; i < size; ++i) {
	diameter = max(diameter,D[i][j]);
      }
    }
  }
}

int Graph::getDiameter() {
  if (diameter == NOT_COMPUTED_YET) {
    computeDiameter();
  }
  return diameter;
}

void Graph::printDistances() {
  DistanceMatrix &D = *distances;
  //cout << "       ";
  cout << setw(7) << " ";
  for (int i = 0; i < size; ++i) {
    cout << setw(5) << i;
  }
  cout << endl;
  for (int i = 0; i < size; ++i) {
    cout << setw(3) << i << " -> ";
    for (int j = 0; j < size; ++j) {
      if (D[i][j] == (numeric_limits<int>::max)())
	cout << setw(5) << "inf";
      else
	cout << setw(5) << D[i][j];
    }
    cout << endl;
  }
}
