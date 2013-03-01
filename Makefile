SRC= BasicClause.cpp parser.cpp satSolver.cpp
r_OBJ= ${SRC:.cpp=_r.o}
d_OBJ= ${SRC:.cpp=_d.o}
CXX	 = g++
LFLAGS   = -lm
CXXFLAGS = -O2 -s -Wall -Wextra
CXXDEBUGFLAGS = -Wall -Wextra -O0 -g


all : release

release: $(r_OBJ) $(HDR) 
	${CXX} $(CXXFLAGS) -o $@ $(r_OBJ) $(LFLAGS)  $(LIB)

debug: $(d_OBJ) $(HDR) 
	${CXX} $(CXXDEBUGFLAGS) -o $@ $(d_OBJ) $(LFLAGS)  $(LIB)

clean: 
	rm -f $(r_OBJ) $(d_OBJ) $(DEP)

destroy: clean
	rm -f release debug

%_r.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%_d.o: %.cpp
	$(CXX) $(CXXDEBUGFLAGS) -c $< -o $@
