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
  typedef map<string, EmNodeValueMap> EmGraphMap;
  typedef pair<string, EmNodeValueMap> EmGraphMapEntry;
  typedef set<string> EmNodeSet;
  typedef list<string> EmNodeList;
  typedef list<EmNodeList> EmPathLists;
  
  struct EmCalc {
	size_t pathCount;		// complete paths examined
	size_t pathMaxLen;		// longest path encountered
	size_t pathMinflowCount;
	size_t pathLoopCount;
	double flowLostToMinflow;
	double flowLostToCycles;
	EmCalc();
  };

  size_t readGraphFromFile(const string& filename, EmGraphMap& g);

  EmNodeValue parseNodeValue(const string& s);

  void pathBuild(const string& node, const EmGraphMap& g, EmNodeSet& path, EmNodeValueMap& outputs, double flow, EmNodeList& pathsteps, double minflow);

  string doubleToString(double x);
}

#endif	// TUDOR_EMERGY_H
