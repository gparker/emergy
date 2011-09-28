///
/// \file   tudor_emergy.h
/// \author Gordon Rios
/// \brief  Basic class for supporting emergy calculations
///
/// Copyright (c) 2011, Gordon Rios. All rights reserved.
/// Distributed under the Simplified BSD License. See COPYING.
///

#include "emergy.h"
#include "gtest/gtest.h"

namespace {
  using namespace tudor_emergy;

  TEST(TudorEmergyTest, BasicDataTypes) {
	// define a flow for input or output
	EmNodeValue nameValue("Node1", 1000.0);
	EXPECT_EQ("Node1", nameValue.first);

	// parse a name value pair from a string
	EmNodeValue branchProportion = parseNodeValue("Node3=0.50");
	EXPECT_EQ("Node3", branchProportion.first);
	EXPECT_NEAR(0.50, branchProportion.second, 0.0001);

	// define a basic set of flows or branch proportions
	EmNodeValueMap outputFlows;
	outputFlows.insert(EmNodeValue("Node1", 2000.0));
	outputFlows["Node2"] = 3000.0;

	// can also use this for branch proportions (should sum to 1.0)
	EmNodeValueMap graphNode;
	graphNode["Node1"] = 0.10;
	graphNode["Node2"] = 0.40;
	graphNode.insert(branchProportion);

	// basic graph structure
	EmGraphMap graph;
	graph.insert(EmGraphMapEntry("Node0", graphNode));
	graph["Node1"]["Node4"] = 0.25; // branch proportion Node1 => Node4

	// convenience types
	EmNodeSet pathSet;			// std::set<string>
	pathSet.insert("Node1");
	EXPECT_EQ(1u, pathSet.size());

	EmNodeList pathList;		// std::list<string>
	pathList.push_back("Node1");
	EXPECT_EQ(1u, pathList.size());

	EmPathLists allPathsList;	// std::list<list<string> >
	allPathsList.push_back(pathList);
	allPathsList.push_back(pathList);
	EXPECT_EQ(2u, allPathsList.size());
  }

  TEST(TudorEmergyTest, VanillaCalc) {
	// setup graph
	EmGraphMap graph;
	graph["Node1"]["Node2"] = 0.30; // leaf 30.0
	graph["Node1"]["Node3"] = 0.70;
	graph["Node3"]["Node4"] = 0.90; // leaf 63.0
	graph["Node3"]["Node5"] = 0.10; // leaf  7.0

	// setup input flows
	EmParams params;
	params.savePaths = true;
	params.minBranchFlow = 0.0;	// no thresholding
	params.inputFlows.insert(parseNodeValue("Node1=100"));
	
	// this structure saves off results
	EmCalcProfile profile;

	// run the calculator
	calculateEmergy(graph, params, profile);
	EXPECT_EQ(3u, profile.pathCount);
	EXPECT_EQ(2u, profile.pathMaxLen);
	EXPECT_NEAR(30.0, profile.outputFlows["Node2"], 0.0001);
	EXPECT_NEAR(63.0, profile.outputFlows["Node4"], 0.0001);
	EXPECT_NEAR(7.0, profile.outputFlows["Node5"], 0.0001);
	EXPECT_EQ(3u, profile.allPaths.size());
  }
} // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
