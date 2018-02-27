# BUILD can be debug or release
BUILD := debug

cxxflags.release := -O3

CC := g++
OUT_EXE := ./out/filecheck
CXXFLAGS += -std=c++11 -I ./vcglib/ -I ./vcglib/eigenlib/ -I . ${cxxflags.${BUILD}}

UNITTEST_OUT_EXE := ./unittest/unittest_out/filecheck

UNITTESTCXXFLAGS := -I /home/mmf159/Documents/vcg_learning/unittest/catch \
					-D FILECHECK_TEST

FILECHECK_CPP := fileCheck.cpp

build:
	@echo BUILD=${BUILD}
	@echo CXXFLAGS=${CXXFLAGS}

	$(CC) ${FILECHECK_CPP} $(CXXFLAGS) -o $(OUT_EXE)
	@echo run it like $(OUT_EXE) path/to/stl

test:
	$(CC) ${FILECHECK_CPP} unittest/fileCheckUnittest.cpp $(CXXFLAGS) $(UNITTESTCXXFLAGS) -o $(UNITTEST_OUT_EXE)
	@echo run it like $(UNITTEST_OUT_EXE)
