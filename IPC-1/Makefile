CC = g++
CFLAGS = -std=c++17 -O2 -Wall

all: signal-echo

signal-echo: signal-echo.o
	$(CC) $(CFLAGS) signal-echo.o -o signal-echo

signal-echo.o: signal-echo.cpp
	$(CC) $(CFLAGS) -c signal-echo.cpp

clean:
	rm -f signal-echo *.o
