# EMERGY

Emergy is a simple API and command line utilty for calculating emergy
via the modified track summing algorithm developed at Henri Tudor.

## SETUP

Setup the google test framework in the project directory:

	svn checkout http://googletest.googlecode.com/svn/trunk/ gtest-svn

Build and run tests:

	make
	./emergy_unittest

## REFERENCES

Emergy is discussed in detail in [Wikipedia's entry on Emergy](http://en.wikipedia.org/wiki/Emergy). The basic track summing algorithm and an example is found in [Odum, 1996](http://books.google.com/books?id=j1PHFoVb7rYC&lpg=PA99&ots=0pPQZkP2BF&dq=track%20summing%20odum%201996&pg=PA99#v=onepage&q&f=false).

A paper [Marvuglia, Benetto, Rugani, Rios] [1] presenting the method and algorithm will be presented at Enviroinfo 2011.

  [1]: "A scalable implementation of the track summing algorithm for Emergy calculation with Life Cycle Inventory databases"

## AUTHOR

Emergy was written by Gordon Rios (gparker at gmail) and is released
under the simplified 2-clause BSD license.
