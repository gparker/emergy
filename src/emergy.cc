/// -*- c++ -*-
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
  const double GRAPH_SUM_MAX = 1.001; // tolerance for graph flow violations

  EmCalcProfile::EmCalcProfile() : pathCount(0), maxBranchFlows(0), pathMinflowCount(0), pathLoopCount(0), flowLostToMinflow(0.0), flowLostToLoops(0.0) {
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
	size_t num_errors = 0; 		// error out if this is > 0 at the end
	string srcNode, dstNode;
	double value;
	std::ifstream infile(filename.c_str());
	assert(infile.is_open());
	while ((infile >> srcNode) && (infile >> dstNode) && (infile >> value)) {
	  if (value < 0.0 || value > 1.0) {
		std::cerr << "ERROR: " << srcNode << " -> " << dstNode
				  << " split " << value << " not in [0,1.0]" << std::endl;
		num_errors++;
	  } else if (value == 0.0)
		std::cerr << "WARN: "  << srcNode << " -> " << dstNode
				  << " split " << value << " tests equal to 0.0" << std::endl;
	  g[srcNode][dstNode] = value;
	  ++count;
	}
	infile.close();
	std::cerr << "graph: " << filename << std::endl;
	if (num_errors > 0) {
	  std::cerr << "FATAL: Found " << num_errors << " graph errors, exiting..."
				<< std::endl;
	  exit(1);
	}
	return count;
  }

  // read inputs and accumulate to previous entry
  void readInputArgsFromFile(const std::string& filename, EmNodeValueMap& inputs) {
	std::cerr << "reading input parameters from " << filename << "..." << std::endl;
	size_t numEntries = 0;
	double totalInput = 0.0;
	std::string paramStr;
	std::ifstream infile(filename.c_str());
	assert(infile.is_open());
	while ((infile >> paramStr)) {
	  EmNodeValue newEntry = parseNodeValue(paramStr); // @TODO: error checking
	  if (inputs.find(newEntry.first) == inputs.end())
		inputs.insert(newEntry);
	  else
		inputs[newEntry.first] += newEntry.second;
	  totalInput += newEntry.second;
	  numEntries++;
	}
	std::cerr << "processed " << numEntries << " node=flow pairs "
			  << "with total input = " << totalInput
			  << std::endl;
  }

  // s is a string in 'name=value' format
  EmNodeValue parseNodeValue(const string& s) {
	size_t valuePos = s.find_first_of("=");
	assert(valuePos < s.size());	// = not found
	string name = s.substr(0, valuePos);
	double value = atof(s.substr(valuePos+1, s.size() - valuePos).c_str());
	return EmNodeValue(name, value);
  }

  /// legacy implementation only @TODO replace with non-recursive version
  /// \note pathlist is *NOT* a reference and is passed by value
  void pathBuild(const string& node, const EmGraphMap& g, EmNodeSet& pathset, EmNodeValueMap& outputs, double flow, EmNodeList pathlist, double minflow, EmCalcProfile& profile, const EmParams& params) {
	if (g.find(node) == g.end()) { // no child so aggregate flow
	  outputs[node] += flow;
	  if (pathset.size() > profile.maxBranchFlows)
		profile.maxBranchFlows = pathset.size();
	  ++profile.pathCount;
	  if (params.savePaths) {
		pathlist.push_back(node);
		pathlist.push_front(doubleToString(flow));
		profile.allPaths.push_back(pathlist);
	  }
	}
	else if (flow < minflow) { // bail out of calculations with too small flow
	  profile.flowLostToMinflow += flow;
	  ++profile.pathMinflowCount;
	  return;
	}
	else {						// update path and recurse
	  pathset.insert(node);		// update path for recursion
	  if (params.savePaths)
		pathlist.push_back(node);
	  EmGraphMap::const_iterator gcit = g.find(node);
	  for (EmNodeValueMap::const_iterator cit = gcit->second.begin(); cit != gcit->second.end(); cit++) {
		if (pathset.find(cit->first) != pathset.end()) {
		  ++profile.pathLoopCount;
		  profile.flowLostToLoops += cit->second * flow;
		}	// end path because it loops back to previous point in its path
		else {
		  pathBuild(cit->first, g, pathset, outputs, cit->second * flow, pathlist, minflow, profile, params);
		}
	  }
	  pathset.erase(node);		// return path to previous state
	}
  }

  void calculateEmergy(const EmGraphMap& graph, const EmParams& params, EmCalcProfile& profile) {
	// process all inputs
	const EmNodeValueMap& inputs = params.inputFlows;
	EmGraphMap& inputOutputs = profile.inputOutputFlows;
	EmNodeSet pathSet;
	EmNodeList pathList;
	for (ENVM_cit cit = inputs.begin(); cit != inputs.end(); cit++) {
	  EmNodeValueMap outputMap;
	  // pathBuild(cit->first, graph, pathSet, profile.outputFlows, cit->second, pathList, params.minBranchFlow * cit->second, profile, params);
	  pathBuild(cit->first, graph, pathSet, outputMap, cit->second, pathList, params.minBranchFlow * cit->second, profile, params);
	  inputOutputs.insert(EmGraphMapEntry(cit->first, outputMap));
	}

	// now aggregate all the input-output flows
	EmNodeValueMap& outputs = profile.outputFlows;
	for (EGM_cit cit = inputOutputs.begin(); cit != inputOutputs.end(); cit++)
	  for (ENVM_cit mcit = cit->second.begin(); mcit != cit->second.end(); mcit++) {
		if (outputs.find(mcit->first) == outputs.end())
		  outputs.insert(*mcit);
		else
		  outputs[mcit->first] += mcit->second;
	  }
  }
} // tudor_emergy
