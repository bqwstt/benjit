CXX = clang++
CFLAGS = -Wall -std=c++23 -O3 -stdlib=libc++ -Ibase -Isrc
EXE = jit

SRC = main.cc $(wildcard src/*.cc)

.PHONY: build run clean

build:
	$(CXX) $(CFLAGS) $(SRC) -o $(EXE)

run: build
	./$(EXE)

clean:
	rm -f $(EXE)
	rm -rf $(EXE).dSYM
