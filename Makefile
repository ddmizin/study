CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall

all: hamming

hamming: Hamming_code.cpp
	$(CXX) $(CXXFLAGS) Hamming_code.cpp -o hamming

clean:
	rm -f hamming
