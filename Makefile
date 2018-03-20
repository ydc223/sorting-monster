all: mysorter shellsort quicksort bubblesort

mysorter: main.o utility.o
	g++ main.o utility.o -o mysorter

main.o: main.cpp
	g++ -g -c main.cpp 

shellsort: shellsort.o utility.o
	g++ shellsort.o utility.o -o shellsort

quicksort: quicksort.o utility.o
	g++ quicksort.o utility.o -o quicksort

bubblesort: bubblesort.o utility.o
	g++ bubblesort.o utility.o -o bubblesort

shellsort.o: shellsort.cpp
	g++ -g -c shellsort.cpp 

quicksort.o: quicksort.cpp
	g++ -g -c quicksort.cpp 

bubblesort.o: bubblesort.cpp
	g++ -c bubblesort.cpp 

utility.o: utility.cpp
	g++ -c utility.cpp 

clean:
	rm -rf *o mysorter shellsort quicksort bubblesort

$(OBJECTS): utility.h

