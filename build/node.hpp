#ifndef NODE_H_
#define NODE_H_

class Node {
public:
  int graphId;
  int nodeId;

  Node();
  Node(int gi, int ni);
  Node(const Node &n);
  ~Node();
};

#endif
