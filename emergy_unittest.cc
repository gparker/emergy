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
	EmNodeValue nameValue("N1", 1000.0);
	EXPECT_EQ("N1", nameValue.first);

	// parse a name value pair from a string
	EmNodeValue branchProportion = parseNodeValue("N3=0.50");
	EXPECT_EQ("N3", branchProportion.first);
	EXPECT_NEAR(0.50, branchProportion.second, 0.0001);

	// define a basic set of flows or branch proportions
	EmNodeValueMap outputFlows;
	outputFlows.insert(EmNodeValue("N1", 2000.0));
	outputFlows["N2"] = 3000.0;

	// can also use this for branch proportions (should sum to 1.0)
	EmNodeValueMap graphNode;
	graphNode["N1"] = 0.10;
	graphNode["N2"] = 0.40;
	graphNode.insert(branchProportion);

	// basic graph structure
	EmGraphMap graph;
	graph.insert(EmGraphMapEntry("N0", graphNode));
	graph["N1"]["N4"] = 0.25; // branch proportion Node1 => Node4

	// convenience types
	EmNodeSet pathSet;			// std::set<string>
	pathSet.insert("N1");
	EXPECT_EQ(1u, pathSet.size());

	EmNodeList pathList;		// std::list<string>
	pathList.push_back("N1");
	EXPECT_EQ(1u, pathList.size());

	EmPathLists allPathsList;	// std::list<list<string> >
	allPathsList.push_back(pathList);
	allPathsList.push_back(pathList);
	EXPECT_EQ(2u, allPathsList.size());
  }

  // N1 --> N2
  // N1 --> N3 --> N4
  //           |-> N5
  TEST(TudorEmergyTest, CalculateEmergy) {
	// setup graph
	EmGraphMap graph;
	graph["N1"]["N2"] = 0.30; // leaf 30.0
	graph["N1"]["N3"] = 0.70;
	graph["N3"]["N4"] = 0.90; // leaf 63.0
	graph["N3"]["N5"] = 0.10; // leaf  7.0

	// setup input flows
	EmParams params;
	params.savePaths = true;
	params.minBranchFlow = 0.0;	// no thresholding
	params.inputFlows.insert(parseNodeValue("N1=100"));
	
	// this structure saves off results
	EmCalcProfile profile;

	// run the calculator
	calculateEmergy(graph, params, profile);
	EXPECT_EQ(3u, profile.pathCount);
	EXPECT_EQ(2u, profile.maxBranchFlows);
	EXPECT_NEAR(30.0, profile.outputFlows["N2"], 0.0001);
	EXPECT_NEAR(63.0, profile.outputFlows["N4"], 0.0001);
	EXPECT_NEAR(7.0, profile.outputFlows["N5"], 0.0001);
	EXPECT_EQ(3u, profile.allPaths.size());
  }

  // N1 --> N2
  // N1 --> N3 --> N4 --> N1 (loop)
  //                  |-> N5
  //           |-> N6 --> N7
  //                      N7 --> N8 (minflow)
  //                  |-> N5
  TEST(TudorEmergyTest, LoopsAndMinflow) {
	// setup graph in a sparse fashion and ignore 0.0 entries in the matrix
	EmGraphMap graph;
	graph["N1"]["N2"] = 0.40; // [0.40] leaf
	graph["N1"]["N3"] = 0.60; // (0.60) interior
	graph["N3"]["N4"] = 0.90; // (0.54) interior
	graph["N3"]["N6"] = 0.10; // (0.06) interior
	graph["N4"]["N1"] = 0.50; // <0.27> lost to a loop
	graph["N4"]["N5"] = 0.50; // [0.27] leaf
	graph["N6"]["N7"] = 0.10; // (0.006)
	graph["N6"]["N5"] = 0.90; // [0.054] add to previous leaf
	graph["N7"]["N8"] = 1.00; // <0.006> lost to minflow @0.01

	// setup input flows
	EmParams params;
	params.savePaths = false;
	params.minBranchFlow = 0.01; // thresholding loses N1->N3->N6->N7 (0.006)

	// input of 1.0 produces branch probabilities as outputs
	params.inputFlows.insert(parseNodeValue("N1=1.0"));
	
	// this structure saves off results
	EmCalcProfile profile;

	// run the calculator
	calculateEmergy(graph, params, profile);
	EXPECT_EQ(3u, profile.pathCount);
	EXPECT_EQ(3u, profile.maxBranchFlows);
	EXPECT_EQ(2u, profile.outputFlows.size());
	EXPECT_NEAR(0.006, profile.flowLostToMinflow, 0.0001);
	EXPECT_NEAR(0.27, profile.flowLostToLoops, 0.0001);
	EXPECT_EQ(1u, profile.pathMinflowCount);
	EXPECT_EQ(1u, profile.pathLoopCount);
	EXPECT_NEAR(0.40, profile.outputFlows["N2"], 0.0001);
	EXPECT_NEAR(0.324, profile.outputFlows["N5"], 0.0001);
	EXPECT_TRUE(profile.allPaths.empty());
  }
} // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
