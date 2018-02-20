


all: main


main: main.cpp
	g++ -std=c++17 -o3 -Wall -Wpedantic main.cpp -o main

clean:
	-rm main
