


all: main


main: main.cpp
	g++ -std=c++17 -Wall main.cpp -o main

clean:
	-rm main
