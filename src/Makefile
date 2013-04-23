# implementation par défaut (est remplacée avec make CLAUSE=[AutreImplementation])
CLAUSE    = SmartClause
# heuristique utilisée pour le programme pour choisir une variable non affectée
# (est remplacée avec  make CHOOSE=[choix de l'heuristique])
CHOOSE    = MOMS
# faire ou non un tri à l'initialisation (approximation de DLIS pour BASIC)
INIT_SORT = 0
# niveau de verbose par défaut du release (est remplacé avec make VERBOSE=n)
VERBOSE  = 1
#interaction avec l'utilisateur ou non
INTERACT = 0

SRC= parser.cpp Variable.cpp satSolver.cpp resolveLog.cpp
OBJ= ${SRC:.cpp=.o}
d_OBJ= ${SRC:.cpp=_d.o}
p_OBJ= ${SRC:.cpp=_p.o}
CXX	 = g++
LFLAGS   = -lm
IMPLFLAGS= -DCLAUSE=$(CLAUSE) -DCHOOSE=$(CHOOSE) -DVERBOSE=$(VERBOSE) -DINIT_SORT=$(INIT_SORT) -DINTERACT=$(INTERACT)
CXXFLAGS = $(IMPLFLAGS) -Wall -Wextra -s -O2 -Wno-unused-parameter
CXXDEBUGFLAGS = $(IMPLFLAGS) -Wall -Wextra -O0 -g -Wno-unused-parameter
CXXPROFILEFLAGS = $(IMPLFLAGS) -DVERBOSE=0 -Wall -Wextra -g -O2 -Wno-unused-parameter -fno-inline

all : release
      
release: $(OBJ) $(HDR) 
	${CXX} $(CXXFLAGS) -o $@ $(OBJ) $(LFLAGS)  $(LIB);\
    cp release ../release

debug: $(d_OBJ) $(HDR) 
	${CXX} $(CXXDEBUGFLAGS) -o $@ $(d_OBJ) $(LFLAGS)  $(LIB);\
    cp debug ../debug

profile: $(p_OBJ) $(HDR) 
	${CXX} $(CXXPROFILEFLAGS) -o $@ $(p_OBJ) $(LFLAGS)  $(LIB);\
    cp profile ../profile

clean: 
	rm -f $(OBJ) $(d_OBJ) $(p_OBJ)

destroy: clean
	rm -f ../release ../debug ../profile release debug profile

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
     
%_p.o: %.cpp
	$(CXX) $(CXXPROFILEFLAGS) -c $< -o $@

%_d.o: %.cpp
	$(CXX) $(CXXDEBUGFLAGS) -c $< -o $@
