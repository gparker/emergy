# Plain vanilla google test Makefile
#
# SYNOPSIS:
#
#   make [all]  - makes everything.
#   make TARGET - makes the given target.
#   make clean  - removes all files generated by make.

# Please tweak the following variable definitions as needed by your
# project, except GTEST_HEADERS, which you can use in your own targets
# but shouldn't modify.

# Points to the root of Google Test, relative to where this file is.
# Remember to tweak this if you move this file.
GTEST_DIR = gtest-svn

# Where to find user code.
USER_DIR = .
USER_SRC = $(USER_DIR)/src
USER_INC = $(USER_DIR)/include

# Flags passed to the preprocessor.
CPPFLAGS += -I$(GTEST_DIR) -I$(GTEST_DIR)/include -I$(USER_INC)

# Flags passed to the C++ compiler.
#CXXFLAGS += -g -Wall -Wextra
CXXFLAGS += -O3 -Wall -Wextra

# All tests produced by this Makefile.  Remember to add new tests you
# created to the list.
TESTS = emergy_unittest

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

# House-keeping build targets.

all : $(TESTS) calc_emergy emergy_calculator

clean :
	rm -f $(TESTS) gtest.a gtest_main.a *.o

# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

# Builds a sample test.  A test should link with either gtest.a or
# gtest_main.a, depending on whether it defines its own main()
# function.

emergy.o : $(USER_SRC)/emergy.cc $(USER_INC)/emergy.h $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_SRC)/emergy.cc

emergy_unittest.o : $(USER_DIR)/emergy_unittest.cc $(USER_INC)/emergy.h \
	$(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/emergy_unittest.cc

emergy_calculator.o : $(USER_SRC)/emergy_calculator.cc $(USER_INC)/emergy.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_SRC)/emergy_calculator.cc

calc_emergy.o : $(USER_SRC)/calc_emergy.cc $(USER_INC)/emergy.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_SRC)/calc_emergy.cc

emergy_unittest : emergy.o emergy_unittest.o gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@

calc_emergy	: emergy.o calc_emergy.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

emergy_calculator : emergy.o emergy_calculator.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

tests	: emergy_unittest
	./emergy_unittest
	@echo "Ran all $@ ..."
