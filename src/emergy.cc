///
/// \file   emergy.cc
/// \author Gordon Rios
/// \brief  Basic class for supporting emergy calculations
///
/// Copyright (c) 2011, Gordon Rios. All rights reserved.
/// Distributed under the Simplified BSD License. See COPYING.
///

#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>

#include "emergy.h"

namespace tudor_emergy {

  EmCalcProfile::EmCalcProfile() : pathCount(0), pathMaxLen(0), pathMinflowCount(0), pathLoopCount(0), flowLostToMinflow(0.0), flowLostToLoops(0.0) {
	/*empty */
  }

  string doubleToString(double x) {
	std::ostringstream strs;
	strs << x;
	return strs.str();
  }

  size_t readGraphFromFile(const string& filename, EmGraphMap& g) {
	std::cerr << "reading graph from " << filename << "..." << std::endl;
	size_t count = 0;
	string srcNode, dstNode;
	double value;
	std::ifstream infile(filename.c_str());
	assert(infile.is_open());
	while ((infile >> srcNode) && (infile >> dstNode) && (infile >> value)) {
	  g[srcNode][dstNode] = value;
	  ++count;
	}
	infile.close();
	std::cerr << "graph: " << filename << std::endl;
	return count;
  }

  // s is a string of name=value format
  EmNodeValue parseNodeValue(const string& s) {
	size_t valuePos = s.find_first_of("=");
	assert(valuePos < s.size());	// = not found
	string name = s.substr(0, valuePos);
	double value = atof(s.substr(valuePos+1, s.size() - valuePos).c_str());
	return EmNodeValue(name, value);
  }

  // implementation only @TODO clean this up
  void pathBuild(const string& node, const EmGraphMap& g, EmNodeSet& path, EmNodeValueMap& outputs, double flow, EmNodeList& pathsteps, double minflow, EmCalcProfile& profile, const EmParams& params) {
	if (g.find(node) == g.end()) { // no child so aggregate flow
	  outputs[node] += flow;
	  if (path.size() > profile.pathMaxLen)
		profile.pathMaxLen = path.size();
	  ++profile.pathCount;
	  pathsteps.push_back(node);
	  pathsteps.push_front(doubleToString(flow));
	  if (params.savePaths)
		profile.allPaths.push_back(pathsteps);
	}
	else if (flow < minflow) { // bail out of calculations with too small flow
	  profile.flowLostToMinflow += flow;
	  ++profile.pathMinflowCount;
	  return;
	}
	else {						// update path and recurse
	  path.insert(node);
	  pathsteps.push_back(node);
	  EmGraphMap::const_iterator gcit = g.find(node);
	  for (EmNodeValueMap::const_iterator cit = gcit->second.begin(); cit != gcit->second.end(); cit++) {
		if (path.find(cit->first) != path.end()) {
		  ++profile.pathLoopCount;
		  profile.flowLostToLoops += cit->second * flow;
		}	// end path because it loops back to previous point in its path
		else {
		  pathBuild(cit->first, g, path, outputs, cit->second * flow, pathsteps, minflow, profile, params);
		}
	  }
	  path.erase(node);
	}
  }

  void calculateEmergy(const EmGraphMap& graph, const EmParams& params, EmCalcProfile& profile) {
  // process all inputs
	const EmNodeValueMap& inputs = params.inputFlows;
	EmNodeSet pathSet;
	EmNodeList pathList;
	for (ENVM_cit cit = inputs.begin(); cit != inputs.end(); cit++)
	  pathBuild(cit->first, graph, pathSet, profile.outputFlows, cit->second, pathList, params.minBranchFlow * cit->second, profile, params);
  }
} // tudor_emergy
