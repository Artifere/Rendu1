SRC= BasicClause.cpp parser.cpp satSolver.cpp
r_OBJ= ${SRC:.cpp=_r.o}
d_OBJ= ${SRC:.cpp=_d.o}
b_OBJ= ${SRC:.cpp=_b.o}
CXX	 = g++
LFLAGS   = -lm
CXXFLAGS = -DRELEASE=1 -O2 -s -Wall -Wextra
CXXDEBUGFLAGS = -DRELEASE=1 -Wall -Wextra -O0 -g
CXXBENCHFLAGS = -DRELEASE=0 -O2 -s

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
	$(CXX) $(CXXFLAGS) -DRELEASE=1 -c $< -o $@

%_d.o: %.cpp
	$(CXX) $(CXXDEBUGFLAGS) -DRELEASE=1 -c $< -o $@

%_b.o: %.cpp
	$(CXX) $(CXXBENCHFLAGS) -c $< -o $@
