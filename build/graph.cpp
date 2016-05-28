#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstring>
#include <climits>

#include "graph.hpp"

using namespace std;

//#define INPUT_GRAPH_READING_DEBUG
//#define APSP_DEBUG
#define DEBUG_FAST_COMPUTE

#define MAX_DISTANCE 9999
#define MIN_DISTANCE -9999

Graph::Graph() {
  graph = NULL;
  numNodes = 0;
  numEdges = 0;
  computed = false;
  diameter = INT_MIN;
  radius = INT_MAX;
  eccentricity = NULL;
  farness = NULL;
}

Graph::Graph(std::string i) {
  input = i;
  graph = NULL;
  numNodes = 0;
  numEdges = 0;
  computed = false;
  diameter = INT_MIN;
  radius = INT_MAX;
  eccentricity = NULL;
  farness = NULL;
  read();
}

Graph::Graph(const Graph &g) {
  // NOT IMPLEMENTED YET
  cerr << "ERROR: Graph copy constructor not implemented yet." << endl;
  exit(EXIT_FAILURE);
}

Graph::~Graph() {
  delete[] graph;
  delete[] eccentricity;
  delete[] farness;
}

void Graph::read() {
  string line;
  ifstream file(input);
  
  file >> numNodes;
  cerr << "Graph: " << numNodes << " graph" << endl;
  graph = new Node[numNodes];

  getline(file,line); // skip current line
  
  for (int i = 0; i < numNodes; i++) {
    if(getline(file,line)) {
      int numNeighbors, nodeId, graphId;
      istringstream iss(line);
      iss >> nodeId;
      graph[i].graphId = i;
      graph[i].nodeId = nodeId;
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
	  addEdge(i,graphId-1);
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

void Graph::addEdge(int i, int j) {
  Node *ni = &graph[i];
  Node *nj = &graph[j];

  ni->addNeighbor(nj);
  nj->addNeighbor(ni);
  numEdges++;
}

void Graph::breadthFirstSearch(Node *root,
			       distanceMap_t distances,
			       parentMap_t parents) {
  int i, distance;
  Node *parent = NULL, *neighbor = NULL;
  bool finished = false;
  nodeList_t::iterator it;
  
  if (distances == NULL) {
    cerr << "ERROR: distances parameters of Graph::breadthFirstSearch should not be NULL" << endl;
  }

  memset(distances, -1, numNodes*sizeof(distance_t));
  
  distance = 0;
  distances[root->graphId] = distance;
 
  while (!finished) {
    finished = true;
    for (i = 0; i < numNodes; i++) {
      if (distances[i] == distance) {
	finished = false;
	parent = &graph[i];
	it = parent->neighbors.begin();
	while (it != parent->neighbors.end()) {
	  neighbor = *it;
	  if (distances[neighbor->graphId] != -1) {
	    it++;
	    continue;
	  }
	  distances[neighbor->graphId] = distance + 1;
	  if (parents != NULL) {
	    parents[neighbor->graphId] = parent;
	  }
	  it++;
	}
      }
    }
    distance++;
  }
}

distance_t Graph::maxDistance(distanceMap_t d) {
  distance_t m = d[0];
  for (int i = 1; i < numNodes; i++) {
    m = max(m,d[i]);
  }
  return m;
}

distance_t Graph::minDistance(distanceMap_t d) {
  distance_t m = d[0];
  for (int i = 1; i < numNodes; i++) {
    m = min(m,d[i]);
  }
  return m;
}

void Graph::compute() {
  cerr << "Compute: " << endl;
  cerr << "Breadth-First Search (BFS) based algorithm..." << endl;
#ifdef PARALLEL_APSP
  cerr << "Parallel optimization using OpenMP..." << endl;
#pragma omp parallel for
#endif
  for(int i = 0; i < numNodes; i++) {
    distance_t eccentricity = 0;
    distanceMap_t distances = new distance_t[numNodes];
    breadthFirstSearch(&graph[i],distances,NULL);
    eccentricity = maxDistance(distances);
    diameter = max(diameter, eccentricity);
    radius = min(radius, eccentricity);
    delete[] distances;
  }
}

void Graph::compute(int index) {
  
}

void Graph::fastCompute() { // compute only diameter+radius
  Node *current;
  int next;
  distance_t eccentricity = 0;
  distanceMap_t eccentricityU = new distance_t[numNodes];
  distanceMap_t eccentricityL = new distance_t[numNodes];
  distanceMap_t distances = new distance_t[numNodes];
  distanceMap_t sum =  new distance_t[numNodes]; // to break the ties in next node selection
  bool *alreadyDone = new bool[numNodes]; 
  bool selectInLower = true;
  bool notConnectedDetected = false;
  int round = 1;
  
  cerr << "Compute diameter and radius computation...: " << endl;

  current = &graph[0]; // start with node 0
  memset(eccentricityU, MAX_DISTANCE, numNodes*sizeof(distance_t));
  memset(eccentricityL, 0, numNodes*sizeof(distance_t));
  memset(alreadyDone, false, numNodes*sizeof(bool));
   
  while(true) {
#ifdef DEBUG_FAST_COMPUTE
    cerr << "Computing BFS from node " << current->graphId << endl; 
#endif
    breadthFirstSearch(current,distances,NULL);
    alreadyDone[current->graphId] = true;
    
    eccentricity = maxDistance(distances);

    eccentricityU[current->graphId] = eccentricity;
    eccentricityL[current->graphId] = eccentricity;
    
    for (int i = 0; i < numNodes; i++) {
      if (i != current->graphId) {
	eccentricityU[i] = min(eccentricityU[i],distances[i] + eccentricity);
	eccentricityL[i] = max(eccentricityL[i],distances[i]);
	if (!notConnectedDetected && distances[i] <= 0) {
	  notConnectedDetected = true;
	  cerr << "WARNING: Not a connected configuration!" << endl;
	}
	sum[i] += distances[i];
      }
    }

    if (round < 4) {
      next = 0;
      for (int i = 1; i < numNodes; i++) {
	if (distances[i] > distances[next]) {
	  next = i;
	}
      }
      current = &graph[next];
      round++;
      continue;
    }
	
    // X : set of nodes v such that eL[v] < eU[v]
    // Y : set of nodes v such that eL[v] == eU[v]
    
    int minEccYid = -1;
    int minEccLXid = -1;
    int maxEccUXid = -1;
    int maxEccYid = -1;

    for (int i = 0; i < numNodes; i++) {
      if (eccentricityL[i] < eccentricityU[i]) { // X
	
	if (minEccLXid == -1 && maxEccUXid == -1) {
	  minEccLXid = i;
	  maxEccUXid = i;
	  continue;
	}
	
	if (eccentricityL[i] < eccentricityL[minEccLXid] ||
	    (eccentricityL[i] == eccentricityL[minEccLXid] && sum[i] < sum[minEccLXid])
	    ) {
	  minEccLXid = i;
	}
	
	if (eccentricityU[i] > eccentricityL[maxEccUXid] ||
	    (eccentricityU[i] == eccentricityU[maxEccUXid] && sum[i] > sum[minEccLXid])
	    ) {
	  maxEccUXid = i;
	}
	
      } else { // Y
	if (minEccYid == -1 && maxEccYid == -1) {
	  minEccYid = i;
	  maxEccYid = i;
	  continue;
	}
	if (eccentricityL[i] < eccentricityL[minEccYid]) {
	  minEccYid = i;
	}
	if (eccentricityU[i] > eccentricityL[maxEccYid]) {
	  maxEccYid = i;
	}
      }
    }
    
    if (eccentricityL[minEccYid] <= eccentricityL[minEccLXid] && // radius comp. has converged
	eccentricityL[maxEccYid] >= eccentricityL[maxEccUXid] // diameter comp. has converged
	) {
      radius = eccentricityL[minEccYid];
      diameter = eccentricityL[maxEccYid];
      break;
    }
    
    if(selectInLower) {
      next = minEccLXid;
    } else {      
      next = maxEccUXid;
    }
    
    selectInLower = !selectInLower;
    current = &graph[next];
    round++;
  }

  cout << "Diameter and radius computed in " << round << " BFSes" << endl;
  delete[] eccentricityU;
  delete[] eccentricityL;
  delete[] distances;
}

distance_t Graph::getEccentricity(int index) {
  return eccentricity[index];
}

distance_t Graph::getFarness(int index) {
  return farness[index];
}

nodeList_t& Graph::getCenter() {
  return center;
}

nodeList_t& Graph::getCentroid() {
  return centroid;
}

#if 0
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
#endif
