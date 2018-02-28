# BUILD can be debug or release
BUILD := debug

cxxflags.release := -O3

CC := g++
OUT_EXE := ./out/filecheck
CXXFLAGS += -std=c++11 -I ./vcglib/ -I ./vcglib/eigenlib/ -I . ${cxxflags.${BUILD}}

EM_OUT_JS := ./upload_form/filecheck.js

UNITTEST_OUT_EXE := ./unittest/unittest_out/filecheck
EM_UNITTEST_OUT_HTML := ./unittest/unittest_out/filecheck.html

EM_EXTRA_FLAGS := -s DEMANGLE_SUPPORT=1

EM_CXXFLAGS := -s EXPORTED_FUNCTIONS='["_file_check", "_file_check_repair"]' -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "FS_createDataFile", "FS_readFile"]' -s TOTAL_MEMORY=268435456

UNITTESTCXXFLAGS := -I ./unittest/catch \
					-D FILECHECK_TEST

EM_UNITTESTCXXFLAGS := -s DEMANGLE_SUPPORT=1 --embed-file ./unittest/meshes/@./unittest/meshes/

FILECHECK_CPP := fileCheck.cpp
UNITTEST_CPP := unittest/fileCheckUnittest.cpp

EMCC := em++

build:
	@echo BUILD=${BUILD}
	@echo CXXFLAGS=${CXXFLAGS}

	${CC} ${FILECHECK_CPP} ${CXXFLAGS} -o ${OUT_EXE}
	@echo run it like ${OUT_EXE} path/to/stl

test:
	${CC} ${FILECHECK_CPP} ${UNITTEST_CPP} ${CXXFLAGS} ${UNITTESTCXXFLAGS} -o ${UNITTEST_OUT_EXE}
	@echo run it like ${UNITTEST_OUT_EXE}

em:
	@echo BUILD=${BUILD}
	@echo CXXFLAGS=${CXXFLAGS}
	${EMCC} ${FILECHECK_CPP} ${CXXFLAGS} ${EM_CXXFLAGS} ${EM_EXTRA_FLAGS} -o ${EM_OUT_JS}

emtest:
	${EMCC} ${FILECHECK_CPP} ${UNITTEST_CPP} ${CXXFLAGS} ${UNITTESTCXXFLAGS} ${EM_UNITTESTCXXFLAGS} ${EM_EXTRA_FLAGS} -o ${EM_UNITTEST_OUT_HTML}
	@echo run it like google-chrome ${EM_UNITTEST_OUT_HTML} or nodejs ./unittest/unittest_out/filecheck.js
