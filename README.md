# EMERGY

Emergy is a simple API and command line utilty for calculating emergy
via the modified track summing algorithm developed at Henri Tudor.

## SETUP

Setup the google test framework in the project directory:

	svn checkout http://googletest.googlecode.com/svn/trunk/ gtest-svn

Build and run tests:

	make
	make tests

Run the calculator on test files:
	
	make odum-test

## EXAMPLES

### calc_emergy

The first command line calculator is a _very_ simple but usable example of a tool that uses the emergy library.

Here's the usage specification for `calc_emergy`:

	USAGE: ./calc_emergy <graph file> <flow multiplier=0.0> <node=flow>

It takes a process graph file in format: NODEA NODEB FLOW where FLOW is a *multiplier* of emergy that _is greater than 0.0 and less than or equal to 1.0_ leading out from NODEA to NODEB.

Flow multiplier defaults to 0.0 which means that flows are treated exactly and calculations can be very slow with this setting. Start with 0.01 to get a quick calculation done as a first pass. Getopt is not used so you have to specify something.

Input flows in the form of `node=flow` refer how much total flow is input to `node` (e.g. `N3=10.0`). The nodes are assumed unique so multiple flows to a single input should be summed prior to using the command. SEE BELOW: `emergy_calculator` accepts raw flows and then sums them prior to running the calculation.

Run the command line calculator from the project directory:

```
./calc_emergy test-files/odum96-figure6.8.graph.dat 0.0 A=3000 B=7000 C=20000
reading graph from test-files/odum96-figure6.8.graph.dat...
graph: test-files/odum96-figure6.8.graph.dat
read 9 lines from test-files/odum96-figure6.8.graph.dat
read 3 inputs
longest path: 4
complete paths: 6
loop violations: 9
flow lost to loop violations: 37500
minflow violations: 0
flow lost to minflow violations: 0
output: Y = 7500
output: Z = 30000
PATHS
750	A	D	E	Y	
3000	A	D	Z	
1750	B	D	E	Y	
7000	B	D	Z	
5000	C	A	D	E	Y	
20000	C	A	D	Z	
```

### emergy_calculator

The next example is uses the slightly more sophisticated calculator and has been used for research publications see for example [1]. It uses a file to store inputs (either inline or one input per line) in the same `node=value` format but sums up nonunique entries into total inputs.

Here's the usage specification for `emergy_calculator`:

```USAGE: ./emergy_calculator <graph file> <input file> [flow multiplier=0.0] [--print-source]```

Using an input file with *inline* format:

```
cat test-files/odum96-figure6.8.inputs.dat
A=1000 A=2000 B=3000 B=4000 C=10000 C=10000
```

We can run the calculator on the same data as the previous example:

```
./emergy_calculator test-files/odum96-figure6.8.graph.dat test-files/odum96-figure6.8.inputs.dat 0.0 --print-sources

reading graph from test-files/odum96-figure6.8.graph.dat...
graph: test-files/odum96-figure6.8.graph.dat
read 9 lines from test-files/odum96-figure6.8.graph.dat
reading input parameters from test-files/odum96-figure6.8.inputs.dat...
processed 6 node=flow pairs with total input = 30000
minFlow = 0
found 3 unique inputs

STATISTICS:
longest path: 4
complete paths: 6
loop violations: 9
flow lost to loop violations: 37500
minflow violations: 0
flow lost to minflow violations: 0

OUTPUTS:
output: Y = 7500
output: Z = 30000

OUTPUT BY SOURCE:
A	Y=750.00	Z=3000.00
B	Y=1750.00	Z=7000.00
C	Y=5000.00	Z=20000.00
```

## REFERENCES

Emergy is discussed in detail in [Wikipedia's entry on Emergy](http://en.wikipedia.org/wiki/Emergy). The basic track summing algorithm and an example is found in [Odum, 1996](http://books.google.com/books?id=j1PHFoVb7rYC&lpg=PA99&ots=0pPQZkP2BF&dq=track%20summing%20odum%201996&pg=PA99#v=onepage&q&f=false).

A paper [Marvuglia, Benetto, Rugani, Rios] [1] presenting the method and algorithm will be presented at Enviroinfo 2011.

  [1]: "A scalable implementation of the track summing algorithm for Emergy calculation with Life Cycle Inventory databases"

## AUTHOR

Emergy was written by Gordon Rios (gparker at gmail) and is released
under the simplified 2-clause BSD license.
