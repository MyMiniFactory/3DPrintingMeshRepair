# BUILD can be debug or release
BUILD := debug

cxxflags.release := -O3

CC := g++
OUT_EXE := ./out/filecheck
CXXFLAGS += -std=c++11 -I ./vcglib/ -I ./vcglib/eigenlib/ -I . ${cxxflags.${BUILD}}

build:
	@echo BUILD=${BUILD}
	@echo CXXFLAGS=${CXXFLAGS}

	$(CC) fileCheck.cpp $(CXXFLAGS) -o $(OUT_EXE)
