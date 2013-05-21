SRC= main.cpp Term.cpp
OUT= smt
OBJ= ${SRC:.cpp=.o}
CXX	 = g++
LFLAGS   = -lm
CXXFLAGS = -Wall -Wextra -O2 -s -Wno-sign-compare

all : release

release: $(OBJ) $(HDR)
	${CXX} $(CXXFLAGS) -o $(OUT) $(OBJ) $(LFLAGS)  $(LIB);

clean: 
	rm -f $(OBJ) $(d_OBJ) $(p_OBJ)

destroy: clean
	rm -f smt

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

