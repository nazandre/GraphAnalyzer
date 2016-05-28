#include "node.hpp"

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
