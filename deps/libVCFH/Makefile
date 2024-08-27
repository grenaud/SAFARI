CXX:=g++
CXXFLAGS:=-O3 -std=c++11
EXE:=example.exe
LD_INC_PATH:=-I./
LD_LIB_PATH:=-L./

lib: vcfheader.o
	ar -rs libvcfh.a $<

test: main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LD_INC_PATH) $(LD_LIB_PATH) -lvcfh

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LD_INC_PATH) $(LD_LIB_PATH)

vcfheader.o: vcfheader.cpp vcfheader.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LD_INC_PATH) $(LD_LIB_PATH)

.PHONY: clean

clean:
	$(RM) -f *.o
	$(RM) -f $(EXE)
	$(RM) -f *.a
