/// -*- c++ -*-
/// \file   emergy_calculator.cc
/// \author Gordon Rios
/// \brief  New calculator to use tudor_emergy library.
///
/// Copyright (c) 2011, Gordon Rios. All rights reserved.
/// Distributed under the Simplified BSD License.
/// See https://github.com/gparker/emergy/blob/master/COPYING.
///

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

//#include <cstdlib>

#include <unistd.h>

#include "emergy.h"

using tudor_emergy::EmNodeValueMap;
using tudor_emergy::EmNodeList;
using tudor_emergy::ENVM_cit;
using tudor_emergy::EmGraphMap;
using tudor_emergy::readGraphFromFile;
using tudor_emergy::readInputArgsFromFile;
using tudor_emergy::readSourcedInputsFromFile;
using tudor_emergy::parseNodeValue;
using tudor_emergy::EmParams;
using tudor_emergy::EmCalcProfile;
using tudor_emergy::parseNodeValue;
using tudor_emergy::EmNodeValue;
using tudor_emergy::EmGraphMapEntry;
void print_usage(const char* progname) {
	std::cerr << "USAGE: " << progname << " -g <graph file> -i <input file> -m [flow multiplier=0.0] -p[rint sources]" << std::endl; 
}

int main(int argc, char **argv) {

  std::string graphFilename = "None";
  std::string inputFilename = "None";

  // setup parameters
  EmParams params;
  params.savePaths = false;

  int c = -1;
  opterr = 0;
  while ((c = getopt(argc, argv, "+g:i:m:ph")) != -1) {
	switch(c) {
	case 'h':
	  print_usage(argv[0]);
	  exit(0);
	  break;
	case 'g':
	  graphFilename = std::string(optarg);
	  break;
	case 'i':
	  inputFilename = std::string(optarg);
	  break;
	case 'm':
	  params.minBranchFlow = atof(optarg);
	  break;
	case 'p':
	  params.printSources = true;
	  break;
	case '?':
	  if (optopt == 'c')
		std::cerr << "Option -" << optopt << " requires an argument" << std::endl;
	  else
		std::cerr << "Unknown option -" << optopt << std::endl;
	  return 1;
	default:
	  abort();
	}
  }

  // find garbage parameters
  for (int index = optind; index < argc; index++)
	std::cerr << "Warn: non-option argument " << argv[index] << std::endl;

  // bail if filenames not found
  if (graphFilename == "None" || inputFilename == "None") {
	print_usage(argv[0]);
	exit(2);
  }

  // read graph
  EmGraphMap graph;
  std::cout << std::endl;
  std::cout << "read " << readGraphFromFile(graphFilename, graph)
	   << " lines from " << graphFilename << std::endl;

  // read inputs node=flow
  if (params.printSources == false) {
	EmNodeValueMap inputFlows;
	readInputArgsFromFile(inputFilename, inputFlows);
	params.sourceInputFlows.insert(EmGraphMapEntry("ALL", inputFlows));
  }
  else
  	readSourcedInputsFromFile(inputFilename, params.sourceInputFlows);

  // bail out if no inputs
  if (params.sourceInputFlows.empty()) {
	std::cerr << "error: must specify at least input in node=flow form" << std::endl;
	exit(1);
  }

  // print out parameters
  std::cerr << "minFlow = " << params.minBranchFlow << std::endl;

  // save off results in this structure
  EmCalcProfile profile;
  
  // run the calculator
  calculateEmergyWithSources(graph, params, profile);

  // dump number of paths examined
  std::cout << std::endl << "STATISTICS:" << std::endl;
  std::cout << "longest path: " << profile.maxBranchFlows << std::endl;
  std::cout << "complete paths: " << profile.pathCount << std::endl;
  std::cout << "loop violations: " << profile.pathLoopCount << std::endl;
  std::cout << "flow lost to loop violations: " << profile.flowLostToLoops << std::endl;
  std::cout << "minflow violations: " << profile.pathMinflowCount << std::endl;
  std::cout << "flow lost to minflow violations: " << profile.flowLostToMinflow << std::endl;

  // dump outputs
  std::cout << std::endl << "OUTPUTS:" << std::endl;
  for (ENVM_cit dcit = profile.outputFlows.begin(); dcit != profile.outputFlows.end(); dcit++)
	std::cout << "output: " << dcit->first << " = " << dcit->second << std::endl;
  
  using tudor_emergy::EmPathLists;
  using tudor_emergy::EPL_cit;
  if (params.savePaths) {
	std::cout << std::endl << "PATHS:" << std::endl;
	for (EPL_cit lcit = profile.allPaths.begin(); lcit != profile.allPaths.end(); lcit++) {
	  for (EmNodeList::const_iterator slcit = lcit->begin();  slcit != lcit->end(); slcit++)
		printf("%s\t", slcit->c_str());
	  printf("\n");
	}
  }

  // print out the sources of emergy
  using tudor_emergy::EGM_cit;
  if (params.printSources) {
	std::cout << std::endl << "OUTPUT BY SOURCE:" << std::endl;
	for (EGM_cit cit = profile.inputOutputFlows.begin(); cit != profile.inputOutputFlows.end(); cit++) {
	  printf("%s", cit->first.c_str());
	  for (ENVM_cit mcit = cit->second.begin(); mcit != cit->second.end(); mcit++)
		printf("\t%s=%.4f", mcit->first.c_str(), mcit->second);
	  printf("\n");
	}
  }

  exit(0);
}
