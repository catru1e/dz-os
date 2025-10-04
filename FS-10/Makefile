CXX = g++

TARGET = copy
SRC = copy.cpp
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^

%.o: %.cpp
	$(CXX) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
