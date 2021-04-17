
ifeq ($(OS),Windows_NT)
	CXX = x86_64-w64-mingw32-g++
	CXX_FLAGS = -O3 -o test.exe
else
	CXX = g++-7
	CXX_FLAGS = -g -rdynamic -O3 -o test
endif

all: clean
	$(CXX) -std=c++17 main.cpp $(CXX_FLAGS)
	./test

clean:
	rm -rf test