#ifndef ARGUMENTS_H_
#define ARGUMENTS_H_

#include <list>
#include <string>

class Arguments {
public:
  std::string input;
  std::string numThreads;
  bool fastCompute;
  std::list<int> nodeQueries;
  
  Arguments();
  Arguments(int argc, char *argv[]);
  Arguments(const Arguments &a);
  ~Arguments();

  void help();

  bool hasNodeQueries();
  
};

#endif
