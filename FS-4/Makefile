CXX = g++

TARGET = redirect-cin
SRC = redirect-cin.cpp
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^

%.o: %.cpp
	$(CXX) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
