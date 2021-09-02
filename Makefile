CXX = g++
CFLAGS = -std=c++11 -Wall -Wextra
DBGFLAGS = -g -Og
RELFLAGS = -march=native -mtune=native -flto -O3

.PHONY: all debug release clean

all: kdtree

debug: 
	@$(MAKE) CFLAGS="$(CFLAGS) $(DBGFLAGS)" --no-print-directory
	
release: 
	@$(MAKE) CFLAGS="$(CFLAGS) $(RELFLAGS)" --no-print-directory
	
kdtree: kdtree.cpp
	$(CXX) kdtree.cpp -o kdtree $(CFLAGS)

clean:
	rm kdtree

