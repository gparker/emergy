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
#include "emergy.h"

using tudor_emergy::EmNodeValueMap;
using tudor_emergy::EmNodeList;
using tudor_emergy::ENVM_cit;
using tudor_emergy::EmGraphMap;
using tudor_emergy::readGraphFromFile;
using tudor_emergy::readInputArgsFromFile;
using tudor_emergy::parseNodeValue;
using tudor_emergy::EmParams;
using tudor_emergy::EmCalcProfile;
using tudor_emergy::parseNodeValue;
using tudor_emergy::EmNodeValue;

void print_usage(const char* progname) {
	std::cerr << "USAGE: " << progname << " <graph file> <input file> [flow multiplier=0.0] [--print-sources]" << std::endl; 
}

int main(int argc, char **argv) {

  // check basic arg structure
  if (argc < 3) {
	print_usage(argv[0]);
	exit(1);
  }

  // read graph
  std::string graphFilename = argv[1];
  EmGraphMap graph;
  std::cout << std::endl;
  std::cout << "read " << readGraphFromFile(graphFilename, graph)
	   << " lines from " << graphFilename << std::endl;

  // setup parameters
  EmParams params;
  params.savePaths = false;
  if (argc > 3)
	params.minBranchFlow = atof(argv[3]);

  // get last parameter
  if (argc > 4)
	if (std::string(argv[4]) == "--print-sources") {
	  params.printSources = true;
	} else {
	  std::cerr << "ERROR: unrecognized arg '" << argv[4] << "'" << std::endl;
	  print_usage(argv[0]);
	  exit(1);
	}

  // read inputs node=flow
  readInputArgsFromFile(argv[2], params.inputFlows);

  // bail out if no inputs
  if (params.inputFlows.empty()) {
	std::cerr << "error: must specify at least input in node=flow form" << std::endl;
	exit(1);
  }

  // print out parameters
  std::cerr << "minFlow = " << params.minBranchFlow << std::endl;
  std::cerr << "found " << params.inputFlows.size() << " unique inputs" << std::endl;

  // save off results in this structure
  EmCalcProfile profile;
  
  // run the calculator
  calculateEmergy(graph, params, profile);

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

  // print out the sources of variation
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
