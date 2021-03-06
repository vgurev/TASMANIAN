include Config/AltBuildSystems/Makefile.in

IADD = -I./include $(CommonIADD)
LADD = -L./ $(CommonLADD)
LIBS = ./libtasmaniandream.a ./libtasmaniansparsegrid.a $(CommonLIBS)
FFLIBS90 = ./libtasmanianfortran90.a ./libtasmaniansparsegrid.a ./libtasmaniandream.a $(CommonLIBS)

TSG_SOURCE = $(wildcard ./SparseGrids/*.cpp) $(wildcard ./SparseGrids/*.hpp) $(wildcard ./SparseGrids/*.h)
TDR_SOURCE = $(wildcard ./DREAM/*.cpp) $(wildcard ./DREAM/*.hpp)

# headers to be ignored in this build .in., .windows., executables, config files, etc.
CMAKE_IN_HEADERS = $(wildcard ./DREAM/*.in.hpp) \
                   $(wildcard ./DREAM/tasdream*.hpp) \
                   $(wildcard ./SparseGrids/*.*.hpp) \
                   $(wildcard ./SparseGrids/*.*.h) \
                   $(wildcard ./SparseGrids/tasgrid*.hpp) \

# header names with ./include as opposed to ./SparseGrids
HEADERS = $(patsubst ./DREAM/%,./include/%,$(filter-out $(CMAKE_IN_HEADERS),$(wildcard ./DREAM/*.hpp))) \
          $(patsubst ./SparseGrids/%,./include/%,$(filter-out $(CMAKE_IN_HEADERS),$(wildcard ./SparseGrids/*.h*))) \
          ./include/TasmanianConfig.hpp

ALL_TARGETS = GaussPattersonRule.table TasmanianSG.py example_sparse_grids.py InterfacePython/testConfigureData.py testTSG.py \
              sandbox.py example_sparse_grids.cpp \
              $(wildcard ./DREAM/Examples/example_dream*.cpp) \
              libtasmaniansparsegrid.so libtasmaniansparsegrid.a libtasmaniandream.so libtasmaniandream.a tasgrid dreamtest gridtest $(HEADERS)

DREAM_EXAMPLES_OBJ = $(patsubst ./DREAM/Examples/%,%,$(patsubst %.cpp,%.o,$(wildcard ./DREAM/Examples/example_dream*.cpp)))

CONFIGURED_HEADERS = ./SparseGrids/TasmanianConfig.hpp

# all target
.PHONY: all
all: $(ALL_TARGETS)
	@echo ""
	@echo "TASMANIAN Simple GNU Make engine:"
	@echo "Complete build for C/C++, CLI and Python interfaces"
	@echo "MATLAB interface: use 'make matlab'"
	@echo ""
	@echo "      or alternatively manually edit ./InterfaceMATLAB/tsgGetPaths.m"
	@echo "      and use MATLAB command addpath('$(shell pwd)/InterfaceMATLAB/')"
	@echo ""

# There are seemingly superfluous dependencies, but those allow for the use of make -j command without conflict
# most of the work is done by the make in the subfolders and
# the extra dependencies ensure only one subfoler make is called at a time

# Common headers
SparseGrids/TasmanianConfig.hpp: ./Config/AltBuildSystems/TasmanianConfig.hpp
	cp ./Config/AltBuildSystems/TasmanianConfig.hpp ./SparseGrids/TasmanianConfig.hpp

include/TasmanianConfig.hpp: ./SparseGrids/TasmanianConfig.hpp
	mkdir -p ./include
	cp ./SparseGrids/TasmanianConfig.hpp ./include/

# Sparse Grids
libtasmaniansparsegrid.so: ./SparseGrids/libtasmaniansparsegrid.so
	cp ./SparseGrids/libtasmaniansparsegrid.so .

SparseGrids/libtasmaniansparsegrid.so: $(TSG_SOURCE) $(CONFIGURED_HEADERS)
	cd SparseGrids; make

libtasmaniansparsegrid.a: ./SparseGrids/libtasmaniansparsegrid.so ./SparseGrids/libtasmaniansparsegrid.a
	cp ./SparseGrids/libtasmaniansparsegrid.a .

SparseGrids/libtasmaniansparsegrid.a: ./SparseGrids/libtasmaniansparsegrid.so $(TSG_SOURCE) $(CONFIGURED_HEADERS)
	cd SparseGrids; make

tasgrid: libtasmaniansparsegrid.a libtasmaniansparsegrid.so ./SparseGrids/tasgrid
	cp ./SparseGrids/tasgrid .

SparseGrids/tasgrid: ./SparseGrids/libtasmaniansparsegrid.so $(TSG_SOURCE) $(CONFIGURED_HEADERS)
	cd SparseGrids; make

gridtest: libtasmaniansparsegrid.a libtasmaniansparsegrid.so ./SparseGrids/gridtest
	cp ./SparseGrids/gridtest .

SparseGrids/gridtest: ./SparseGrids/libtasmaniansparsegrid.so $(TSG_SOURCE) $(CONFIGURED_HEADERS)
	cd SparseGrids; make

# DREAM
libtasmaniandream.so: ./DREAM/libtasmaniandream.so
	cp ./DREAM/libtasmaniandream.so .

DREAM/libtasmaniandream.so: libtasmaniansparsegrid.so $(TDR_SOURCE) libtasmaniansparsegrid.so $(CONFIGURED_HEADERS)
	cd DREAM; make

libtasmaniandream.a: libtasmaniandream.so ./DREAM/libtasmaniandream.a
	cp ./DREAM/libtasmaniandream.a .

DREAM/libtasmaniandream.a: ./DREAM/libtasmaniandream.so $(TDR_SOURCE) libtasmaniansparsegrid.a $(CONFIGURED_HEADERS)
	cd DREAM; make

dreamtest: libtasmaniandream.a libtasmaniandream.so ./DREAM/dreamtest
	cp ./DREAM/dreamtest .

DREAM/dreamtest: ./DREAM/libtasmaniandream.so libtasmaniansparsegrid.a $(TDR_SOURCE) $(CONFIGURED_HEADERS)
	cd DREAM; make

# Headers
# many calls to mkdir, consider reducing
include/TasmanianDREAM.hpp: ./DREAM/TasmanianDREAM.hpp
	mkdir -p ./include
	cp ./DREAM/TasmanianDREAM.hpp ./include/

include/tasdream%.hpp: ./DREAM/tasdream%.hpp
	mkdir -p ./include
	cp ./$< ./$@

include/tsg%.hpp: ./DREAM/tsg%.hpp
	mkdir -p ./include
	cp ./$< ./$@

include/TasmanianSparseGrid.h: ./SparseGrids/TasmanianSparseGrid.h
	mkdir -p ./include
	cp ./SparseGrids/TasmanianSparseGrid.h ./include/

include/TasmanianSparseGrid.hpp: ./SparseGrids/TasmanianSparseGrid.hpp
	mkdir -p ./include
	cp ./SparseGrids/TasmanianSparseGrid.hpp ./include/

include/tasgrid%.hpp: ./SparseGrids/tasgrid%.hpp
	mkdir -p ./include
	cp ./$< ./$@

include/tsg%.hpp: ./SparseGrids/tsg%.hpp
	mkdir -p ./include
	cp ./$< ./$@

# Python, tables, etc.
GaussPattersonRule.table: ./SparseGrids/GaussPattersonRule.table
	cp ./SparseGrids/GaussPattersonRule.table .

TasmanianSG.py: ./Config/AltBuildSystems/TasmanianSG.py
	cp ./Config/AltBuildSystems/TasmanianSG.py .

example_sparse_grids.py: ./Config/AltBuildSystems/example_sparse_grids.py
	cp ./Config/AltBuildSystems/example_sparse_grids.py .

InterfacePython/testConfigureData.py: ./Config/AltBuildSystems/testConfigureData.py
	cp ./Config/AltBuildSystems/testConfigureData.py InterfacePython/

testTSG.py: ./InterfacePython/testTSG.py
	cp ./InterfacePython/testTSG.py .

sandbox.py: ./InterfacePython/sandbox.py
	cp ./InterfacePython/sandbox.py .

example_sparse_grids.cpp: ./SparseGrids/Examples/example_sparse_grids.cpp
	cp ./SparseGrids/Examples/example_sparse_grids.cpp .

example_dre%.cpp: ./DREAM/Examples/example_dre%.cpp
	cp ./$< ./$@

# Matlab
.PHONY: matlab
matlab:
	cp Config/AltBuildSystems/tsgGetPaths.m InterfaceMATLAB/tsgGetPaths.m
	mkdir -p tsgMatlabWorkFolder
	sed -i -e 's|ENTER\ THE\ PATH\ TO\ MATLAB\ WORK\ FOLDER|'`pwd`/tsgMatlabWorkFolder/'|g' InterfaceMATLAB/tsgGetPaths.m
	sed -i -e 's|ENTER\ THE\ PATH\ TO\ tasgrid\ EXECUTABLE|'`pwd`/tasgrid'|g' InterfaceMATLAB/tsgGetPaths.m
	@echo ""
	@echo "Setting MATLAB work forlder to "`pwd`"/tsgMatlabWorkFolder/"
	@echo "use MATLAB command: addpath('"`pwd`"/InterfaceMATLAB/');"
	@echo ""

# Python 3
.PHONY: python3
python3: TasmanianSG.py testTSG.py example_sparse_grids.py
	cp ./Config/AltBuildSystems/TasmanianSG.py .
	cp ./Config/AltBuildSystems/example_sparse_grids.py .
	cp ./InterfacePython/testTSG.py .
	sed -i -e 's|\#\!\/usr\/bin\/env\ python|\#\!\/usr\/bin\/env\ python3|g' example_sparse_grids.py
	sed -i -e 's|\#\!\/usr\/bin\/env\ python|\#\!\/usr\/bin\/env\ python3|g' testTSG.py

# Fortran
.PHONY: fortran
fortran: example_sparse_grids_f90 fortester90 libtasmanianfortran90.a libtasmanianfortran90.so tasmaniansg.mod
	./fortester90

fortester90: libtasmanianfortran90.a libtasmanianfortran90.so tasmaniansg.mod
	cp InterfaceFortran/fortester.f90 .
	$(FF) $(OPTF) $(IADD) -c fortester.f90 -o fortester90.o
	$(FF) $(OPTLFF) $(LADD) fortester90.o -o fortester90 $(FFLIBS90) -lstdc++

example_sparse_grids_f90: libtasmanianfortran90.a libtasmanianfortran90.so tasmaniansg.mod
	cp ./InterfaceFortran/Examples/example_sparse_grids.f90 .
	$(FF) $(OPTF) $(IADD) -c example_sparse_grids.f90 -o example_sparse_grids_f90.o
	$(FF) $(OPTLFF) $(LADD) example_sparse_grids_f90.o -o example_sparse_grids_f90 $(FFLIBS90) -lstdc++

tasmaniansg.mod: InterfaceFortran/tasmaniansg.mod
	cp InterfaceFortran/tasmaniansg.mod .

InterfaceFortran/tasmaniansg.mod: libtasmanianfortran90.a libtasmanianfortran90.so
	cd InterfaceFortran/; make

libtasmanianfortran90.so: InterfaceFortran/libtasmanianfortran90.so
	cp InterfaceFortran/libtasmanianfortran90.so .

libtasmanianfortran90.a: InterfaceFortran/libtasmanianfortran90.a
	cp InterfaceFortran/libtasmanianfortran90.a .

InterfaceFortran/libtasmanianfortran90.so: libtasmanianfortran90.a
	cd InterfaceFortran/; make

InterfaceFortran/libtasmanianfortran90.a:
	cd InterfaceFortran/; make

# Testing and examples
.PHONY: test
test: $(ALL_TARGETS)
	./gridtest
	./dreamtest
	PYTHONPATH=$(PYTHONPATH):./InterfacePython ./testTSG.py && { echo "SUCCESS: Test completed successfully"; }

.PHONY: examples
examples: $(ALL_TARGETS) example_dream
	$(CC) $(OPTC) $(IADD) -c example_sparse_grids.cpp -o example_sparse_grids.o
	$(CC) $(OPTL) $(LADD) example_sparse_grids.o -o example_sparse_grids $(LIBS)

example_dream: $(ALL_TARGETS) $(DREAM_EXAMPLES_OBJ)
	$(CC) $(OPTL) $(LADD) $(DREAM_EXAMPLES_OBJ) -o example_dream $(LIBS)

%.o: %.cpp
	$(CC) $(OPTC) $(IADD) -c $<

# Clean
.PHONY: clean
clean:
	rm -fr libtasmaniansparsegrid.so
	rm -fr libtasmaniansparsegrid.a
	rm -fr libtasmaniandream.so
	rm -fr libtasmaniandream.a
	rm -fr libtasmanianfortran90.so
	rm -fr libtasmanianfortran90.a
	rm -fr tasmaniansg.mod
	rm -fr fortester*
	rm -fr tasgrid
	rm -fr gridtest
	rm -fr tasdream
	rm -fr TasmanianSG.py
	rm -fr example_sparse_grids.py
	rm -fr GaussPattersonRule.table
	rm -fr *.pyc
	rm -fr ./InterfacePython/*.pyc
	rm -fr __pycache__
	rm -fr ./InterfacePython/__pycache__
	rm -fr example_sparse_grids
	rm -fr example_sparse_grids.o
	rm -fr example_sparse_grids.cpp
	rm -fr example_dream*
	rm -fr example_sparse_grids.f90
	rm -fr example_sparse_grids_fortran.o
	rm -fr example_sparse_grids_fortran
	rm -fr testSave
	rm -fr testTSG.py
	rm -fr sandbox.py
	rm -fr include
	rm -fr ./SparseGrids/TasmanianConfig.hpp
	rm -fr ./InterfacePython/testConfigureData.py
	cd SparseGrids; make clean
	cd DREAM; make clean
	cd InterfaceFortran; make clean
