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
	
	// define a basic set of flows or branch proportions
	EmNodeValueMap outputFlows;
	outputFlows.insert(EmNodeValue("Node1", 2000.0));
	outputFlows["Node2"] = 3000.0;

	// parse a name value pair from a string
	EmNodeValue branchProportion = parseNodeValue("Node3=0.50");
	EXPECT_EQ("Node3", branchProportion.first);
	EXPECT_NEAR(0.50, branchProportion.second, 0.0001);

	// can also use this for branch proportions (should sum to 1.0)
	EmNodeValueMap graphNode;
	graphNode["Node1"] = 0.10;
	graphNode["Node2"] = 0.40;
	graphNode.insert(branchProportion);

	// basic graph structure
	EmGraphMap graph;
	graph.insert(EmGraphMapEntry("Node0", graphNode));
	graph["Node1"]["Node4"] = 0.25; // branch proportion Node1 => Node4
  }

  TEST(TudorEmergyTest, CalculateEmergy) {
	// setup graph
	EmGraphMap graph;
	graph["Node1"]["Node2"] = 0.25;
	graph["Node1"]["Node3"] = 0.75;
	
	// setup input flows
	EmNodeValueMap inputFlows;
	inputFlows.insert(parseNodeValue("Node1=100"));
	
	// prepare calcuation structures
	EmNodeValueMap outputFlows;
	EmNodeSet path;
	EmNodeList sequence;

	// run a legacy style calculation with low level method
	pathBuild(inputFlows.begin()->first, graph, path, outputFlows,
			  inputFlows.begin()->second, sequence, 0.0);
	EXPECT_NEAR(75.0, outputFlows["Node3"], 0.0001);
  }
} // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
