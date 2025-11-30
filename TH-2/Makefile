CXX = g++
CXXFLAGS = -std=c++17 -pthread -O2 -fPIC
LDFLAGS = -pthread

all: libparallel_scheduler.so demo-application

parallel_scheduler.o: parallel_scheduler.cpp parallel_scheduler.hpp
	$(CXX) $(CXXFLAGS) -c parallel_scheduler.cpp

libparallel_scheduler.so: parallel_scheduler.o
	$(CXX) -shared -o libparallel_scheduler.so parallel_scheduler.o

demo-application: demo.cpp libparallel_scheduler.so
	$(CXX) $(CXXFLAGS) demo.cpp -L. -lparallel_scheduler $(LDFLAGS) -o demo-application

clean:
	rm -f *.o *.so demo-application
