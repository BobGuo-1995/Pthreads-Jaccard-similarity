jaccard: jaccard1.cpp
	g++ -g -Wall -std=c++11 -o jaccard1 jaccard1.cpp etime.c -lpthread
clean:
	rm -f jaccard1 *.o