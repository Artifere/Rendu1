# implementation par défaut (est remplacée avec make CLAUSE=[AutreImplementation])
CLAUSE   = SmartClause
# niveau de verbose par défaut du release (est remplacé avec make VERBOSE=n)
VERBOSE  = 1

SRC= parser.cpp Variable.cpp satSolver.cpp
OBJ= ${SRC:.cpp=.o}
d_OBJ= ${SRC:.cpp=.o}
CXX	 = g++
LFLAGS   = -lm
CXXFLAGS = -DVERBOSE=$(VERBOSE) -Wall -Wextra -O2 -s -Wno-unused-parameter
CXXDEBUGFLAGS = -DVERBOSE=10 -Wall -Wextra -O0 -g -Wno-unused-parameter

all : release clean
      
release: $(OBJ) $(HDR) 
	${CXX} $(CXXFLAGS) -o $@ $(OBJ) $(LFLAGS)  $(LIB)

debug: $(d_OBJ) $(HDR) 
	${CXX} $(CXXDEBUGFLAGS) -o $@ $(d_OBJ) $(LFLAGS)  $(LIB)

clean: 
	rm -f $(OBJ) $(d_OBJ)

destroy: clean
	rm -f release debug

%.o: %.cpp
	$(CXX) -DCLAUSE=$(CLAUSE) $(CXXFLAGS) -c $< -o $@
     

