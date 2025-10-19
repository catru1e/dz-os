# Makefile
CXX := g++

SRCS := main.cpp
OBJS := $(SRCS:.cpp=.o)
TARGET := interactive-shell

.PHONY: all clean

all: $(TARGET)

# compile step (multi-stage: compile .o then link)
%.o: %.cpp
	$(CXX) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) 

clean:
	$(RM) $(OBJS) $(TARGET)
