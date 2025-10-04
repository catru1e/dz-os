CXX = g++

TARGET = manual-append
SRC = manual-append.cpp
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^

%.o: %.cpp
	$(CXX) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
