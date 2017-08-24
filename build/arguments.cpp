#include <iostream>
#include <cstdlib>
#include <omp.h>

#include "arguments.hpp"

using namespace std;

Arguments::Arguments() {
  computationMode = COMP_ECC_FAR;
  input = "";
  numThreads = "";
  initialNode = -1;
}

Arguments::Arguments(int argc, char *argv[]) {
  computationMode = COMP_ECC_FAR;
  input = "";
  numThreads = "";
  initialNode = -1;
  
  argv++;
  argc--;
  
  while ((argc > 0) && (argv[0][0] == '-')) {
    switch(argv[0][1]) {
    case 'i':
      input = argv[1];
      argc--;
      argv++;
      break;
    case 't':
      numThreads = argv[1];
      argc--;
      argv++;
      break;
    case 'n':
      nodeQueries.push_back(stoi(argv[1]));
      argc--;
      argv++;
      break;
    case 'm' :
      computationMode = computationMode_t(stoi(argv[1]));
      argc--;
      argv++;
      break;
    case 's' :
      initialNode = stoi(argv[1]);
      argc--;
      argv++;
      break;
    case 'h':
      help();
      break;
    default:
      cerr << "ERROR: Unknown argument -" <<  argv[0][1] << endl;
      help();
    }
    argc--;
    argv++;
  }

  if (input == "") {
    cerr << "ERROR: No input file" << endl;
    help();
  }

  size_t lastDot = input.find_last_of(".");
  string ext = input.substr(lastDot + 1);
  if (ext != "aj") {
    cerr << "ERROR: Input file should be a GraphAnalyzer (.aj) file." << endl;
    help();
  }
  
  /*if (output == "") {
    size_t lastSlash = input.find_last_of("/\\");
    string n =  input.substr(0, lastDot).substr(lastSlash+1);
    output = n + ".aj";
    }*/
}

Arguments::Arguments(const Arguments &a) {
  input = a.input;
}

Arguments::~Arguments() { }

void Arguments::help() {
  //cerr << "VisibleSim Configuration File To Graph Analyzer File:" << endl;
  cerr << endl;
  cerr << "Usage: GraphAnalyzer -i <input GraphAnalyzer (.aj) file> [options]" << endl;
  cerr <<"Options:" << endl;
  cerr << "-t <# threads>:     max number of threads to use (default: " << omp_get_max_threads()
       << ")" << endl;
  cerr << "-c <mode>:          computation mode " << endl;
  cerr << "                       <mode>:" << endl;

  cerr << "                          " << COMP_ECC_FAR << " (by default): computation of the ecccentricity and farness of all nodes (Complexity: O(n) BFSes, i.e., O(n(n+m) time and  O(n*#threads) memory space)" << endl;
  cerr << "                          " << COMP_RAD_DIAM << ": fast computation of the radius and diameter of the graph. Does not support parallelism. (Complexity: O(n) BFSes, i.e., O(n(n+m) time and  O(n) memory space)" << endl;
  cerr << "                          " << COMP_BETWEENNESS << ": computation of the betweenness of the graph. (Complexity: O(n) BFSes, i.e., O(n(n+m) time and  O(n^2) memory space)" << endl;
   
  cerr << "-n <node id>:       print the eccentricity and farness of the node with the" << endl;
  cerr << "                      id \"node id\". Support multiple node queries (e.g.," << endl;
  cerr << "                      \"-n 4 -n 6 -n 8\" to display information about nodes" << endl;
  cerr << "                      4, 6 and 8" << endl;
  cerr << "-s:                 initial node (e.g., with -d)" << endl;
  cerr << "-h:                 print this usage and exit" << endl;
  exit(EXIT_SUCCESS);
}

bool Arguments::hasNodeQueries() {
  return !nodeQueries.empty();
}
