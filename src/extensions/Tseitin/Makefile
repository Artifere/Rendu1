SRC= Parser.cpp Tseitin.cpp main.cpp
OUT= tseitin
OBJ= ${SRC:.cpp=.o}
CXX	 = g++
LFLAGS   = -lm
CXXFLAGS = -Wall -Wextra -s -O2

all : release

release: $(OBJ) $(HDR)
	${CXX} $(CXXFLAGS) -o $(OUT) $(OBJ) $(LFLAGS)  $(LIB);

clean: 
	rm -f $(OBJ) $(d_OBJ) $(p_OBJ)

destroy: clean
	rm -f ../release ../debug ../profile release debug profile

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

