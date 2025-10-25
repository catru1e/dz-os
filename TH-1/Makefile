CC = g++
TARGET = array-summary
SRC = array-summary.cpp
CFLAGS = -std=c++17 -pthread

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
