#include "node.hpp"

using namespace std;

Node::Node() {}

Node::Node(int gi, int ni) {
  graphId = gi;
  nodeId = ni;
}

Node::Node(const Node &n) {
  graphId = n.graphId;
  nodeId = n.nodeId;
}

Node::~Node() {}

void Node::addNeighbor(Node *n) {
  neighbors.push_back(n);
}

ostream& operator<<(std::ostream &os, const nodeList_t &nodeList) {
  nodeList_t::const_iterator it = nodeList.begin();
  for (; it != nodeList.end(); it++) {
    if (it != nodeList.begin()) {
      os << ", ";
    }
    os << (*it)->nodeId; 
  }
  return os;
}
