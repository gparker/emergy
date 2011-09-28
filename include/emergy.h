///
/// \file   emergy.h
/// \author Gordon Rios
/// \brief  Basic class for supporting emergy calculations
///
/// Copyright (c) 2011, Gordon Rios. All rights reserved.
/// Distributed under the Simplified BSD License. See COPYING.
///

#ifndef TUDOR_EMERGY_H
#define TUDOR_EMERGY_H

#include <string>
#include <map>
#include <list>
#include <set>

namespace tudor_emergy {
  using std::string;
  using std::pair;
  using std::map;
  using std::list;
  using std::set;

  typedef pair<string, double> EmNodeValue;
  typedef map<string, double> EmNodeValueMap;
  typedef EmNodeValueMap::const_iterator ENVM_cit;
  typedef map<string, EmNodeValueMap> EmGraphMap;
  typedef pair<string, EmNodeValueMap> EmGraphMapEntry;
  typedef set<string> EmNodeSet;
  typedef list<string> EmNodeList;
  typedef list<EmNodeList> EmPathLists;

  /// profile a run of the calculator
  struct EmCalcProfile {
	size_t pathCount;		// complete paths examined
	size_t pathMaxLen;		// longest path encountered
	size_t pathMinflowCount;
	size_t pathLoopCount;
	double flowLostToMinflow;
	double flowLostToLoops;
	EmPathLists allPaths;		// only populate this if requested in EmParams
	EmNodeValueMap outputFlows;
	EmCalcProfile();
  };

  /// parameters for a run of the calculator
  struct EmParams {
	bool savePaths;				// should all the paths be saved?
	double minBranchFlow;
	EmNodeValueMap inputFlows;
	EmParams() : savePaths(false), minBranchFlow(0.0) { /* empty */ }
  };

  /// read a graph from a file
  /// \param filename a file with format: parent child branch
  /// \param g an empty graph
  /// 'branch' is in [0..1] and sum of all child entries for any
  /// given parent = 1.0
  size_t readGraphFromFile(const string& filename, EmGraphMap& g);

  string doubleToString(double x);

  EmNodeValue parseNodeValue(const string& s);

  void calculateEmergy(const EmGraphMap& graph, const EmParams& params, EmCalcProfile& profile);
}

#endif	// TUDOR_EMERGY_H
