


all: main


main: main.cpp
	g++ -std=c++17 -g3 -Wall -Wpedantic main.cpp -o main

clean:
	-rm main
