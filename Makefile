SRC= BasicClause.cpp parser.cpp satSolver.cpp
OBJ= ${SRC:.cpp=.o}
CXX	 = g++
LFLAGS   = -lm
CXXFLAGS = -O2 -s

exe: $(OBJ) $(HDR) 
	${CXX} $(CXXFLAGS) -o $@ $(OBJ) $(LFLAGS)  $(LIB)

clean: 
	rm -f $(OBJ) $(DEP)

destroy: clean
	rm -f exe 


%.o: %.c
	$(CXX) $(CXXFLAGS) -c $<
