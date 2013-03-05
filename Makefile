SRC= BasicClause.cpp parser.cpp satSolver.cpp
r_OBJ= ${SRC:.cpp=_r.o}
d_OBJ= ${SRC:.cpp=_d.o}
b_OBJ= ${SRC:.cpp=_b.o}
CXX	 = g++
LFLAGS   = -lm
CXXFLAGS = -O2 -s -Wall -Wextra
CXXDEBUGFLAGS = -Wall -Wextra -O0 -g


all : release

release: $(r_OBJ) $(HDR) 
	${CXX} $(CXXFLAGS) -o $@ $(r_OBJ) $(LFLAGS)  $(LIB);\
    cp release testsSatisfiable/exe;\
    cp release testsUnsatisfiable/exe

debug: $(d_OBJ) $(HDR) 
	${CXX} $(CXXDEBUGFLAGS) -o $@ $(d_OBJ) $(LFLAGS)  $(LIB)

bench : $(b_OBJ) $(HDR) 
	${CXX} $(CXXFLAGS) -o $@ $(b_OBJ) $(LFLAGS)  $(LIB);\
    cp bench testsSatisfiable/bench;\
    cp bench testsUnsatisfiable/bench



clean: 
	rm -f $(r_OBJ) $(d_OBJ) $(b_OBJ) $(DEP)

destroy: clean
	rm -f release debug

%_r.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ -DRELEASE=1

%_d.o: %.cpp
	$(CXX) $(CXXDEBUGFLAGS) -c $< -o $@ -DRELEASE=1

%_b.o: %.cpp
	$(CXX) $(CXXLAGS) -c $< -o $@
