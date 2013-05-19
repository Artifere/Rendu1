SRC= main.cpp
OUT= smt
OBJ= ${SRC:.cpp=.o}
CXX	 = g++
LFLAGS   = -lm
CXXFLAGS = -Wall -Wextra -g

all : release

release: $(OBJ) $(HDR)
	${CXX} $(CXXFLAGS) -o $(OUT) $(OBJ) $(LFLAGS)  $(LIB);

clean: 
	rm -f $(OBJ) $(d_OBJ) $(p_OBJ)

destroy: clean
	rm -f ../release ../debug ../profile release debug profile

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

