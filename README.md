# EMERGY

Emergy is a simple API and command line utilty for calculating emergy
via the modified track summing algorithm developed at Henri Tudor.

## SETUP

Setup the google test framework in the project directory:

	svn checkout http://googletest.googlecode.com/svn/trunk/ gtest-svn

Build and run tests:

	make
	make tests

## EXAMPLES

The command line calculator (calc_emergy) is a very simple but usable
example of a tool that uses the emergy library. It takes a process
graph file in format: NODEA NODEB FLOW where FLOW is a fraction of
energy leading out from NODEA to NODEB with the fractions summing to
one.

Here's the usage specification for calc_emergy:

	USAGE: ./calc_emergy <graph file> <flow multiplier=0.0> <input=flow>

Run the command line calculator from the project directory:

	./calc_emergy test-files/odum96-figure6.8.dat 0.0 A=3000 B=7000 C=20000
	reading graph from test-files/odum96-figure6.8.dat...
	graph: test-files/odum96-figure6.8.dat
	read 9 lines from test-files/odum96-figure6.8.dat
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
	750		A	D	E	Y	
	3000	A	D	Z	
	1750	B	D	E	Y	
	7000	B	D	Z	
	5000	C	A	D	E	Y	
	20000	C	A	D	Z	

## REFERENCES

Emergy is discussed in detail in [Wikipedia's entry on Emergy](http://en.wikipedia.org/wiki/Emergy). The basic track summing algorithm and an example is found in [Odum, 1996](http://books.google.com/books?id=j1PHFoVb7rYC&lpg=PA99&ots=0pPQZkP2BF&dq=track%20summing%20odum%201996&pg=PA99#v=onepage&q&f=false).

A paper [Marvuglia, Benetto, Rugani, Rios] [1] presenting the method and algorithm will be presented at Enviroinfo 2011.

  [1]: "A scalable implementation of the track summing algorithm for Emergy calculation with Life Cycle Inventory databases"

## AUTHOR

Emergy was written by Gordon Rios (gparker at gmail) and is released
under the simplified 2-clause BSD license.
