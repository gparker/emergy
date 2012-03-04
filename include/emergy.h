/// -*- c++ -*-
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
  typedef EmGraphMap::const_iterator EGM_cit;
  typedef pair<string, EmNodeValueMap> EmGraphMapEntry;
  typedef set<string> EmNodeSet;
  typedef list<string> EmNodeList;
  typedef list<EmNodeList> EmPathLists;
  typedef EmPathLists::const_iterator EPL_cit;

  /// \struct EmCalcProfile 
  /// \brief profile a run of the calculator
  /// \note maxBranchFlows = maxPathLen-1
  struct EmCalcProfile {
	size_t pathCount;		/// number of complete paths examined
	size_t maxBranchFlows;	/// longest sequence of branch steps
	size_t pathMinflowCount;
	size_t pathLoopCount;
	double flowLostToMinflow;
	double flowLostToLoops;
	EmPathLists allPaths;		/// only populate this if requested in EmParams
	EmNodeValueMap outputFlows;
	EmGraphMap inputOutputFlows; /// [input => (output=value)]
	EmCalcProfile();
  };

  /// \struct EmParams
  /// \brief parameters for a run of the calculator
  /// \param minBranchFlow sets a cutoff for whether to branch flow to
  /// child nodes
  struct EmParams {
	bool savePaths;				// should all the paths be saved?
	double minBranchFlow;
	EmNodeValueMap inputFlows;
	EmParams() : savePaths(false), minBranchFlow(0.0) { /* empty */ }
  };

  /// calculate the emergy of a system in graph and populate a run profile
  /// \param graph a graphical model of the system
  /// \param params a set of parameters controlling the inputs and the run
  /// \param profile a set of results and the outputs of the run
  void calculateEmergy(const EmGraphMap& graph, const EmParams& params, EmCalcProfile& profile);

  /// read a graph from a file
  /// \brief read the graph in form N1 N2 split in [0.0, 1.0]
  /// \param filename a file with format: parent child branch
  /// \param g an empty graph
  size_t readGraphFromFile(const string& filename, EmGraphMap& g);

  /// read a series of node=value inputs from a file
  /// \param filename a file with format: parent child branch
  /// \param inputs are a unique mapping of nodes to input flows
  void readInputArgsFromFile(const std::string& filename, EmNodeValueMap& inputs);

  /// parse a name value pair from a string
  /// \param s a string in the form "name=value"
  EmNodeValue parseNodeValue(const string& s);
}

#endif	// TUDOR_EMERGY_H
