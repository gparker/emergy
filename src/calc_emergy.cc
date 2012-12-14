/// -*- c++ -*-
/// \file   calc_emergy.cc
/// \author Gordon Rios
/// \brief  Legacy calculator rewritten to use tudor_emergy library.
///
/// Copyright (c) 2011, Gordon Rios. All rights reserved.
/// Distributed under the Simplified BSD License. See COPYING.
///

#include <iostream>
#include <string>
#include "emergy.h"

int main(int argc, char **argv) {
  using tudor_emergy::EmNodeValueMap;
  using tudor_emergy::EmNodeList;
  using tudor_emergy::ENVM_cit;
  using tudor_emergy::EmGraphMap;
  using tudor_emergy::readGraphFromFile;
  using tudor_emergy::parseNodeValue;
  using tudor_emergy::EmParams;
  using tudor_emergy::EmCalcProfile;

  if (argc < 3) {
	std::cerr << "USAGE: " << argv[0] << " <graph file> <flow multiplier=0.0> <node=flow>" << std::endl; 
	exit(1);
  }

  // read graph
  std::string graphFilename = argv[1];
  EmGraphMap graph;
  std::cout << "read " << readGraphFromFile(graphFilename, graph)
	   << " lines from " << graphFilename << std::endl;

  // setup parameters
  EmParams params;
  params.savePaths = true;
  params.minBranchFlow = atof(argv[2]);

  // read inputs node=flow
  for (int i = 3; i < argc; ++i)
	params.inputFlows.insert(parseNodeValue(std::string(argv[i])));
  std::cerr << "read " << params.inputFlows.size() << " inputs" << std::endl;

  // bail out if no inputs
  if (params.inputFlows.empty()) {
	std::cerr << "error: must specify at least input in node=flow form" << std::endl;
	exit(1);
  }

  // save off results in this structure
  EmCalcProfile profile;
  
  // run the calculator
  calculateEmergy(graph, params, profile);

  // dump number of paths examined
  std::cout << "total input Flow: " << profile.totalInputFlow << std::endl;
  std::cout << "longest path: " << profile.maxBranchFlows << std::endl;
  std::cout << "complete paths: " << profile.pathCount << std::endl;
  std::cout << "loop violations: " << profile.pathLoopCount << std::endl;
  std::cout << "flow lost to loop violations: " << profile.flowLostToLoops << std::endl;
  std::cout << "minflow violations: " << profile.pathMinflowCount << std::endl;
  std::cout << "flow lost to minflow violations: " << profile.flowLostToMinflow << std::endl;
  std::cout << "total output flow: " << profile.totalOutputFlow << std::endl;

  // dump outputs
  for (ENVM_cit dcit = profile.outputFlows.begin(); dcit != profile.outputFlows.end(); dcit++)
	std::cout << "output: " << dcit->first << " = " << dcit->second << std::endl;

  using tudor_emergy::EmPathLists;
  using tudor_emergy::EPL_cit;
  std::cout << "PATHS" << std::endl;
  for (EPL_cit lcit = profile.allPaths.begin(); lcit != profile.allPaths.end(); lcit++) {
	for (EmNodeList::const_iterator slcit = lcit->begin();  slcit != lcit->end(); slcit++)
	  printf("%s\t", slcit->c_str());
	printf("\n");
  }
  exit(0);
}
