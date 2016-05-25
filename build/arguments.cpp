#include <iostream>
#include <cstdlib>
#include "arguments.hpp"

using namespace std;

Arguments::Arguments() {}

Arguments::Arguments(int argc, char *argv[]) {

  argv++;
  argc--;

  while ((argc > 0) && (argv[0][0] == '-')) {
    switch(argv[0][1]) {
    case 'i':
      input = argv[1];
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
  cerr << "Usage: GraphAnalyzer -i <input GraphAnalyzer (.aj) file> [options]"<< endl;
  cerr <<"Options:" << endl;
  cerr <<"-h : print this usage and exit" << endl;
  exit(EXIT_SUCCESS);
} 
