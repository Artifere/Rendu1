# implementation par défaut (est remplacée avec make CLAUSE=[AutreImplementation])
CLAUSE   = SmartClause
# niveau de verbose par défaut du release (est remplacé avec make VERBOSE=n)
VERBOSE  = 1

SRC= parser.cpp Variable.cpp satSolver.cpp
OBJ= ${SRC:.cpp=.o}
d_OBJ= ${SRC:.cpp=.o}
p_OBJ= ${SRC:.cpp=_p.o}
CXX	 = g++
LFLAGS   = -lm
CXXFLAGS = -DVERBOSE=$(VERBOSE) -Wall -Wextra -s -O2 -Wno-unused-parameter
CXXDEBUGFLAGS = -DVERBOSE=10 -Wall -Wextra -O0 -g -Wno-unused-parameter
CXXPROFILEFLAGS = -DVERBOSE=0 -Wall -Wextra -g -O2 -Wno-unused-parameter

all : release
      
release: $(OBJ) $(HDR) 
	${CXX} $(CXXFLAGS) -o $@ $(OBJ) $(LFLAGS)  $(LIB)

debug: $(d_OBJ) $(HDR) 
	${CXX} $(CXXDEBUGFLAGS) -o $@ $(d_OBJ) $(LFLAGS)  $(LIB)

profile: $(p_OBJ) $(HDR) 
	${CXX} $(CXXPROFILEFLAGS) -o $@ $(p_OBJ) $(LFLAGS)  $(LIB)

clean: 
	rm -f $(OBJ) $(d_OBJ) $(p_OBJ)

destroy: clean
	rm -f release debug profile

%.o: %.cpp
	$(CXX) -DCLAUSE=$(CLAUSE) $(CXXFLAGS) -c $< -o $@
     
%_p.o: %.cpp
	$(CXX) -DCLAUSE=$(CLAUSE) $(CXXPROFILEFLAGS) -c $< -o $@
 
