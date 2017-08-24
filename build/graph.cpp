#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstring>
#include <climits>
#include <omp.h>
#include <list>

#include <stack>
#include <queue>

#include <cassert>

#include "graph.hpp"

using namespace std;

//#define INPUT_GRAPH_READING_DEBUG
//#define APSP_DEBUG

//#define DEBUG_FAST_COMPUTE

#define MAX_DISTANCE INT_MAX
#define MIN_DISTANCE INT_MIN

//typedef std::pair<distance_t,distance_t> eccentricityLU_t;
//typedef eccentricityLU_t* eccentricityLUMap_t;

Graph::Graph() {
  graph = NULL;
  numNodes = 0;
  numEdges = 0;
  computed = false;
  diameter = MIN_DISTANCE;
  radius = MAX_DISTANCE;
  eccentricity = NULL;
  farness = NULL;
  betweenness = NULL;
}

Graph::Graph(std::string i) {
  input = i;
  graph = NULL;
  numNodes = 0;
  numEdges = 0;
  computed = false;
  diameter = MIN_DISTANCE;
  radius = MAX_DISTANCE;
  eccentricity = NULL;
  farness = NULL;
  betweenness = NULL;
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
  delete[] betweenness;
}

void Graph::read() {
  string line;
  ifstream file(input);
  
  file >> numNodes;
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
  cerr << "Graph: " << numNodes << " nodes, " << numEdges << " edges" << endl;
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

  for (int i = 0; i < numNodes; i++) {
    distances[i] = MAX_DISTANCE;
  }
  
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
	  if (distances[neighbor->graphId] != MAX_DISTANCE) {
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

bool Graph::isConnected() {
  distanceMap_t distances = new distance_t[numNodes];
  breadthFirstSearch(&graph[0],distances,NULL);
  for (int i = 0; i < numNodes; i++) {
    if (distances[i] == MAX_DISTANCE) {
      return false;
    }
  }
  return true;
}

void Graph::printDistance(distanceMap_t d) {
  for (int i = 0; i < numNodes; i++) {
    cerr << d[i] << ", ";
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

distance_t Graph::sumDistance(distanceMap_t d) {
  distance_t s = 0;
  for (int i = 0; i < numNodes; i++) {
    s += d[i];
  }
  return s;
}

void Graph::compute() {
  eccentricity = new distance_t[numNodes];
  farness = new distance_t[numNodes];
  
  cerr << "Compute: " << endl;
  cerr << "Breadth-First Search (BFS) based algorithm..." << endl;
#ifdef PARALLEL_APSP
  cerr << "Parallel optimization using OpenMP. "
       << "Max number of threads: " << omp_get_max_threads() << endl;
#pragma omp parallel for
#endif
  for(int i = 0; i < numNodes; i++) {
    distanceMap_t distances = new distance_t[numNodes];
    breadthFirstSearch(&graph[i],distances,NULL);
    eccentricity[i] = maxDistance(distances);
    farness[i] = sumDistance(distances);
    delete[] distances;
  }

  diameter = eccentricity[0];
  radius = eccentricity[0];
  minFarness = farness[0];
  for (int i = 1; i < numNodes; i++) {
    diameter = max(diameter, eccentricity[i]);
    radius = min(radius, eccentricity[i]);
    minFarness = min(minFarness, farness[i]);
  }

  center.clear();
  centroid.clear();
  extremities.clear();
  
  for (int i = 0; i < numNodes; i++) {
    if (eccentricity[i] == radius) {
      center.push_back(&graph[i]);
    }
    if(farness[i] == minFarness) {
      centroid.push_back(&graph[i]);
    }
    if(eccentricity[i] == diameter) {
      extremities.push_back(&graph[i]);
    }
  }
}

nodeProperty_t Graph::compute(int index) {
  distance_t eccentricity = 0;
  distance_t farness = 0;
  distanceMap_t distances = new distance_t[numNodes];
  breadthFirstSearch(&graph[index],distances,NULL);
  eccentricity = maxDistance(distances);
  farness = sumDistance(distances);
  delete[] distances;
  return make_pair(eccentricity,farness);
}


betweenness_t Graph::getBetweenness(int index) {
  assert(betweenness);
  return betweenness[index];
}

void Graph::minMaxDistance(nodeList_t list,
			   distanceMap_t value1,
			   distanceMap_t value2,
			   distanceMap_t tieBreaker,
			   Node* &minNode,
			   Node* &maxNode) {
  
  nodeList_t::iterator it;
  Node *node;
  int minNodeId, maxNodeId, nodeId;
  
  if (list.size() == 0) {
    minNode = NULL;
    maxNode = NULL;
    return;
  }

  if (value2 == NULL) {
    value2 = value1;
  }
  if (tieBreaker == NULL) {
    tieBreaker = value1;
  }
  
  it = list.begin();
  node = *it;
  minNode = node;
  maxNode = node;
  it++;
  
  for (;it != list.end(); it++) {
    node = *it;
    
    nodeId = node->graphId;
    minNodeId = minNode->graphId;
    maxNodeId = maxNode->graphId;
    
    if (value1[nodeId] < value1[minNodeId] ||
	(value1[nodeId] == value1[minNodeId] &&
	 tieBreaker[nodeId] < tieBreaker[minNodeId])
	) {
      minNode = node;
    }
    if (value2[nodeId] > value2[maxNodeId] ||
	(value2[nodeId] == value2[maxNodeId] &&
	 tieBreaker[nodeId] > tieBreaker[maxNodeId])
	) {
      maxNode = node;
    }
  }
}
			   

void Graph::fastCompute(int initialNodeGraphId) { // compute only diameter+radius
  Node *current, *next;
  bool selectInLower = true;
  int round = 1;
  distance_t ecc = 0;
  distanceMap_t eccentricityU = new distance_t[numNodes];
  distanceMap_t eccentricityL = new distance_t[numNodes];
  distanceMap_t eccentricity = eccentricityL; // to be used with nodes that have converged!
  distanceMap_t distances = new distance_t[numNodes];
  distanceMap_t sum =  new distance_t[numNodes]; // to break the ties in next node selection

  list<Node*> converged;
  list<Node*> notConverged;
  Node *minEccNode, *maxEccNode;
  Node *minEccLNode, *maxEccUNode;
  
  bool diameterHasConverged = false;
  bool radiusHasConverged = false;
  
  cerr << "Fast diameter and radius computation..." << endl;

  if (eccentricityU == NULL ||
      eccentricityL == NULL ||
      distances == NULL ||
      sum == NULL) {
    cerr << "ERROR: Memory allocation error." << endl;
  }
  
  // initizialize data structures
  for (int i = 0; i < numNodes; i++) {
    eccentricityU[i] = MAX_DISTANCE;
    eccentricityL[i] = MIN_DISTANCE;
    sum[i] = 0;
    notConverged.push_back(&graph[i]);
  }

  if (initialNodeGraphId < 0) { 
    srand(time(NULL));
    initialNodeGraphId = rand() % numNodes; // start random node
  }  

  cerr << "Starting with node: " << initialNodeGraphId << endl;
  
  current = &graph[initialNodeGraphId];
  
  while(true) {
    
#ifdef DEBUG_FAST_COMPUTE
    cerr << "Computing BFS from node " << current->graphId << endl; 
#endif
    // BFS from node "current"
    
    breadthFirstSearch(current,distances,NULL);
        
    ecc = maxDistance(distances);

    // set current node ecc
    eccentricityL[current->graphId] = ecc;
    eccentricityU[current->graphId] = ecc;
    
#ifdef DEBUG_FAST_COMPUTE
    cerr << "Eccentricity: " << ecc << endl;
#endif
    
    for (int i = 0; i < numNodes; i++) {
      eccentricityL[i] = max(eccentricityL[i],distances[i]);
      eccentricityU[i] = min(eccentricityU[i],distances[i] + ecc);
      sum[i] += distances[i];
    }

    if (round < 4) {
      next = &graph[0];
      for (int i = 1; i < numNodes; i++) {
	if (distances[i] > distances[next->graphId] &&
	    sum[i] > sum[next->graphId]) {
	  next = &graph[i];
	}
      }
      current = next;
      round++;
      continue;
    }
	
    // NotConverged : set of nodes v such that eL[v] < eU[v]
    // converged : set of nodes v such that eL[v] == eU[v]
    for (list<Node*>::iterator it = notConverged.begin();
	 it != notConverged.end(); it++) {
      Node* n = *it;
      if (eccentricityL[n->graphId] == eccentricityU[n->graphId]) {
	converged.push_back(n);
	notConverged.erase(it++);
      }
    }    
    
#ifdef DEBUG_FAST_COMPUTE
    /*printDistance(eccentricityL);
    cerr << endl;
    printDistance(eccentricityU);
    cerr << endl;*/
#endif
    
    minMaxDistance(converged, eccentricity, NULL, NULL, minEccNode, maxEccNode);

    if (notConverged.size() == 0) {
      // Algorithm has converged
      cerr << "Set of not converged nodes is empty..." << endl;
      break;
    }

    minMaxDistance(notConverged, eccentricityL, eccentricityU, sum, minEccLNode, maxEccUNode);
    
#ifdef DEBUG_FAST_COMPUTE
    int minEccLId = minEccLNode->graphId;
    int maxEccUId = maxEccUNode->graphId;
    int minConvergedId = minEccNode->graphId;
    int maxConvergedId = maxEccNode->graphId; 
    cerr << "Min EccL (in X (not converged)): "
	 << "id= " << minEccLId
	 << " => " << eccentricityL[minEccLId] << endl;
    cerr << "Max EccU (in X (not converged)): "
	 << "id= " << maxEccUId
	 << " => " << eccentricityU[maxEccUId] << endl;
    cerr << "Min Ecc (in Y (converged)): "
	 << "id= " << minConvergedId
	 << " => " << eccentricity[minConvergedId] << endl;
    cerr << "Max Ecc (in Y (converged)): "
	 << "id= " << maxConvergedId
	 << " => " << eccentricity[maxConvergedId] << endl;
#endif

    if (!diameterHasConverged) {
      diameterHasConverged = (eccentricity[maxEccNode->graphId] >=
			      eccentricityU[maxEccUNode->graphId]);
      
      if (diameterHasConverged) {
	cerr << "Diameter = " << eccentricity[maxEccNode->graphId] << endl;
	cerr << "Diameter has converged in " << round << " BFSes" << endl;
      }
    }

    if (!radiusHasConverged) {
      radiusHasConverged = (eccentricity[minEccNode->graphId] <=
			  eccentricityL[minEccLNode->graphId]);
      if (radiusHasConverged) {
	cerr << "Radius = " << eccentricity[minEccNode->graphId] << endl;
	cerr << "Radius has converged in " << round << " BFSes" << endl;
      }
    }
    
    if (diameterHasConverged && radiusHasConverged) {
      break;
    }
    
    if(selectInLower) {
      next = minEccLNode;
    } else {      
      next = maxEccUNode;
    }
    selectInLower = !selectInLower;
    round++;
    current = next;
  }
  
  cerr << "Convergence in " << round << " BFSes" << endl;
  
  radius = eccentricity[minEccNode->graphId];
  diameter = eccentricity[maxEccNode->graphId];
  
  delete[] eccentricityU;
  delete[] eccentricityL;
  delete[] distances;
  delete[] sum;
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

nodeList_t& Graph::getBetweennessCenter() {
  return betweennessCenter;
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

void Graph::computeBetweenness() {
  //https://www.cs.purdue.edu/homes/agebreme/Networks/papers/brandes01centrality.pdf
  distance_t apsp[numNodes][numNodes];
  betweenness_t delta[numNodes];
  distance_t distance[numNodes];
  betweenness_t sigma[numNodes];
  list<int> p[numNodes];
  Node *nodeV = NULL;
  
  betweenness = new betweenness_t[numNodes];
  maxBetweenness = 0;

  cerr << "Compute: " << endl;
  cerr << "Breadth-First Search (BFS) based algorithm..." << endl;
#ifdef PARALLEL_APSP
  cerr << "Parallel optimization using OpenMP. "
       << "Max number of threads: " << omp_get_max_threads() << endl;
#pragma omp parallel for
#endif
  for(int i = 0; i < numNodes; i++) {
    distanceMap_t distances = &(apsp[i][0]);
    breadthFirstSearch(&graph[i],distances,NULL);
  }

  for (int i = 0; i < numNodes; i++) {
    betweenness[i] = 0;
  }

  for (int i = 0; i < numNodes; i++) {
    stack<int> s;
    queue<int> q;
    
    for (int j = 0; j < numNodes; j++) {
      distance[j] = -1;
      sigma[j] = 0.0;
      p[j].clear();
      delta[j] = 0.0;
    }
    
    distance[i] = 0;
    sigma[i] = 1.0;
    
    q.push(i);
         
    while(!q.empty()) {
      int v = q.front();
      q.pop();            
      s.push(v);
      
      nodeV = &graph[v];
      for (nodeList_t::iterator it = nodeV->neighbors.begin();
	    it != nodeV->neighbors.end(); it++) {
	int w = (*it)->graphId;
	
	assert(apsp[v][w] == 1);

	if (distance[w] < 0) {
	  q.push(w);
	  distance[w] = distance[v] + 1;
	}
	
	if (distance[w] == (distance[v] + 1)){
	  sigma[w] += sigma[v];
	  p[w].push_back(v);
	}
      }
    } // while
    
    while (!s.empty()) {
      int w = s.top();
      s.pop();
      for (list<int>::iterator it=p[w].begin(); it != p[w].end(); ++it) {
	int v = *it;
	delta[v] += sigma[v]/sigma[w] * (1.0 + delta[w]);
      }
      if (w != i) {
	betweenness[w] += delta[w];
      }
    }
  } // global for
      
      	  
  for (int i = 0; i < numNodes; i++) {
    maxBetweenness=max(betweenness[i],maxBetweenness);
  }
  
  for (int i = 0; i < numNodes; i++) {
    if (betweenness[i] == maxBetweenness) {
      betweennessCenter.push_back(&graph[i]);
    }
  }
}
