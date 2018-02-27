# BUILD can be debug or release
BUILD := debug

cxxflags.release := -O3

CC := g++
OUT_EXE := ./out/filecheck
CXXFLAGS += -std=c++11 -I ./vcglib/ -I ./vcglib/eigenlib/ -I . ${cxxflags.${BUILD}}

UNITTEST_OUT_EXE := ./unittest/unittest_out/filecheck
EM_UNITTEST_OUT_HTML := ./unittest/unittest_out/filecheck.html

UNITTESTCXXFLAGS := -I ./unittest/catch \
					-D FILECHECK_TEST
EM_UNITTESTCXXFLAGS := -s DEMANGLE_SUPPORT=1 --embed-file ./unittest/meshes/@./unittest/meshes/

FILECHECK_CPP := fileCheck.cpp
UNITTEST_CPP := unittest/fileCheckUnittest.cpp

EMCC := em++

build:
	@echo BUILD=${BUILD}
	@echo CXXFLAGS=${CXXFLAGS}

	$(CC) ${FILECHECK_CPP} $(CXXFLAGS) -o $(OUT_EXE)
	@echo run it like $(OUT_EXE) path/to/stl

test:
	$(CC) ${FILECHECK_CPP} ${UNITTEST_CPP} $(CXXFLAGS) $(UNITTESTCXXFLAGS) -o $(UNITTEST_OUT_EXE)
	@echo run it like $(UNITTEST_OUT_EXE)

emtest:
	$(EMCC) ${FILECHECK_CPP} ${UNITTEST_CPP} $(CXXFLAGS) $(UNITTESTCXXFLAGS) $(EM_UNITTESTCXXFLAGS) -o $(EM_UNITTEST_OUT_HTML)
	@echo run it like google-chrome $(EM_UNITTEST_OUT_HTML) or nodejs ./unittest/unittest_out/filecheck.js
