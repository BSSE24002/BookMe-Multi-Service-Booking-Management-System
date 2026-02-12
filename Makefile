all: ./a.out

compRun:
	g++ -std=c++11 main.cpp bsse24074.cpp bsse24046.cpp bsse24030.cpp train.cpp movie.cpp -o r.out

run: clean compRun; ./r.out

clean:
	rm -f *.out
