SRC= parser.cpp satSolver.cpp
r_OBJ= ${SRC:.cpp=_r.o}
d_OBJ= ${SRC:.cpp=_d.o}
b_OBJ= ${SRC:.cpp=_b.o}
CXX	 = g++
LFLAGS   = -lm
CXXFLAGS = -DINLINED_CLAUSE -DRELEASE -O2 -s -Wall -Wextra
CXXDEBUGFLAGS = -DINLINED_CLAUSE -DRELEASE -Wall -Wextra -O0 -g
CXXBENCHFLAGS = -DINLINED_CLAUSE  -Wall -Wextra -O2 -s

all : release

release: $(r_OBJ) $(HDR) 
	${CXX} $(CXXFLAGS) -o $@ $(r_OBJ) $(LFLAGS)  $(LIB);\
    cp release testsSatisfiable/exe;\
    cp release testsUnsatisfiable/exe

debug: $(d_OBJ) $(HDR) 
	${CXX} $(CXXDEBUGFLAGS) -o $@ $(d_OBJ) $(LFLAGS)  $(LIB)

bench  : $(b_OBJ) $(HDR)
	${CXX} $(CXXBENCHFLAGS) -o $@ $(b_OBJ) $(LFLAGS) $(LIB);\
    cp bench testsSatisfiable/bench;\
    cp bench testsUnsatisfiable/bench



clean: 
	rm -f $(r_OBJ) $(d_OBJ) $(b_OBJ) $(DEP)

destroy: clean
	rm -f release debug bench

%_r.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%_d.o: %.cpp
	$(CXX) $(CXXDEBUGFLAGS) -c $< -o $@

%_b.o: %.cpp
	$(CXX) $(CXXBENCHFLAGS) -c $< -o $@
