#ifndef NODE_H_
#define NODE_H_

#include <ostream>
#include <list>

class Node;

typedef std::list<Node*> nodeList_t; 

class Node {
public:
  int graphId;
  int nodeId;
  nodeList_t neighbors;

  Node();
  Node(int gi, int ni);
  Node(const Node &n);
  ~Node();

  void addNeighbor(Node* n);

};

std::ostream& operator<<(std::ostream &os, const nodeList_t &nodeList);

#endif
