#ifndef ARGUMENTS_H_
#define ARGUMENTS_H_

#include <list>
#include <string>

enum computationMode_t {COMP_ECC_FAR, COMP_RAD_DIAM, COMP_BETWEENNESS};

class Arguments {
public:
  std::string input;
  std::string numThreads;
  int initialNode;

  std::list<int> nodeQueries;
  computationMode_t computationMode;
  
  Arguments();
  Arguments(int argc, char *argv[]);
  Arguments(const Arguments &a);
  ~Arguments();

  void help();

  bool hasNodeQueries();
  
};

#endif
