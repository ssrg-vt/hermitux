CXX=g++
CXXFLAGS=-I../../dyninst-9.2.0/prefix/include
LDFLAGS=-lpatchAPI -lsymtabAPI -linstructionAPI \
		-ldyninstAPI -lparseAPI -L../../dyninst-9.2.0/prefix/lib \
		-lpthread
SRC=$(shell ls *.cpp)
OBJ=$(SRC:.cpp=.o)
PROG=rewrite_syscalls

all: $(PROG)

$(PROG): $(OBJ)
	$(CXX) $^ -o $(PROG) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

clean:
	rm -rf *.o $(PROG)
