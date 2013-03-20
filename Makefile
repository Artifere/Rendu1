SRC= parser.cpp Variable.cpp satSolver.cpp
r_OBJ= ${SRC:.cpp=_r.o}
d_OBJ= ${SRC:.cpp=_d.o}
b_OBJ= ${SRC:.cpp=_b.o}
CXX	 = g++ -DCLAUSE=SmartClause
LFLAGS   = -lm
CXXFLAGS = -DVERBOSE=1 -Wall -Wextra -O2 -s -Wno-unused-parameter
CXXDEBUGFLAGS = -DVERBOSE=10 -Wall -Wextra -O0 -g -Wno-unused-parameter
CXXBENCHFLAGS = -DVERBOSE=0 -Wall -Wextra -O2 -s -Wno-unused-parameter

all : release

release: $(r_OBJ) $(HDR) 
	${CXX} $(CXXFLAGS) -o $@ $(r_OBJ) $(LFLAGS)  $(LIB) ;\
    cp release testsSatisfiable/exe;\
    cp release testsUnsatisfiable/exe

debug: $(d_OBJ) $(HDR) 
	${CXX} $(CXXDEBUGFLAGS) -o $@ $(d_OBJ) $(LFLAGS)  $(LIB)

bench  : $(b_OBJ) $(HDR)
	${CXX} $(CXXBENCHFLAGS) -o $@ $(b_OBJ) $(LFLAGS) $(LIB);\
    cp bench testsSatisfiable/;\
    cp bench testsUnsatisfiable/;\


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
