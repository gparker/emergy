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

using namespace tudor_emergy;

EmCalc::EmCalc() : pathCount(0), pathMaxLen(0), pathMinflowCount(0),
				   pathLoopCount(0), flowLostToMinflow(0.0), 
				   flowLostToCycles(0.0) { /*empty */ }

string tudor_emergy::doubleToString(double x) {
  std::ostringstream strs;
  strs << x;
  return strs.str();
}

size_t tudor_emergy::readGraphFromFile(const string& filename, EmGraphMap& g) {
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
EmNodeValue tudor_emergy::parseNodeValue(const string& s) {
  size_t valuePos = s.find_first_of("=");
  assert(valuePos < s.size());	// = not found
  string name = s.substr(0, valuePos);
  double value = atof(s.substr(valuePos+1, s.size() - valuePos).c_str());
  return EmNodeValue(name, value);
}

static size_t pathcount = 0;		// complete paths examined
static size_t maxpathlen = 0;
static size_t minflowcount = 0;
static double minflowflow = 0.0;
static size_t loopcount = 0;
static double loopflow = 0.0;
static EmPathLists allpaths;
void tudor_emergy::pathBuild(const string& node, const EmGraphMap& g, EmNodeSet& path, EmNodeValueMap& outputs, double flow, EmNodeList& pathsteps, double minflow) {
  if (g.find(node) == g.end()) { // no child so aggregate flow
	outputs[node] += flow;
	if (path.size() > maxpathlen)
	  maxpathlen = path.size();
	++pathcount;
	pathsteps.push_back(node);
	pathsteps.push_front(doubleToString(flow));
	allpaths.push_back(pathsteps);
  }
  else if (flow < minflow) { // bail out of calculations with too small flow
	minflowflow += flow;
	++minflowcount;
	return;
  }
  else {						// update path and recurse
	path.insert(node);
	pathsteps.push_back(node);
	EmGraphMap::const_iterator gcit = g.find(node);
	for (EmNodeValueMap::const_iterator cit = gcit->second.begin(); cit != gcit->second.end(); cit++) {
	  if (path.find(cit->first) != path.end()) {
		++loopcount;
		loopflow += cit->second * flow;
	  }	// end path because it loops back to previous point in its path
	  else {
		pathBuild(cit->first, g, path, outputs, cit->second * flow, pathsteps, minflow);
	  }
	}
	path.erase(node);
  }
}
