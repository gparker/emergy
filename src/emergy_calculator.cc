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

int main(int argc, char **argv) {

  if (argc < 3) {
	std::cerr << "USAGE: " << argv[0] << " <graph file> <input file> [flow multiplier=0.0]" << std::endl; 
	exit(1);
  }

  // read graph
  std::string graphFilename = argv[1];
  EmGraphMap graph;
  std::cout << "read " << readGraphFromFile(graphFilename, graph)
	   << " lines from " << graphFilename << std::endl;

  // setup parameters
  EmParams params;
  params.savePaths = false;
  if (argc == 4)
	params.minBranchFlow = atof(argv[3]);

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
  std::cout << "longest path: " << profile.maxBranchFlows << std::endl;
  std::cout << "complete paths: " << profile.pathCount << std::endl;
  std::cout << "loop violations: " << profile.pathLoopCount << std::endl;
  std::cout << "flow lost to loop violations: " << profile.flowLostToLoops << std::endl;
  std::cout << "minflow violations: " << profile.pathMinflowCount << std::endl;
  std::cout << "flow lost to minflow violations: " << profile.flowLostToMinflow << std::endl;

  // dump outputs
  for (ENVM_cit dcit = profile.outputFlows.begin(); dcit != profile.outputFlows.end(); dcit++)
	std::cout << "output: " << dcit->first << " = " << dcit->second << std::endl;

  
  using tudor_emergy::EmPathLists;
  using tudor_emergy::EPL_cit;
  if (params.savePaths) {
	std::cout << "PATHS" << std::endl;
	for (EPL_cit lcit = profile.allPaths.begin(); lcit != profile.allPaths.end(); lcit++) {
	  for (EmNodeList::const_iterator slcit = lcit->begin();  slcit != lcit->end(); slcit++)
		printf("%s\t", slcit->c_str());
	  printf("\n");
	}
  }
  exit(0);
}
