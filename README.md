# Programming Assignment 2 - Yana Chala

My program consists of 4 excecutable files and 5 main source code files. Among the excecutable files there are 3 sort files - shellsort, quicksort and bubblesort and a main file, that compiles to mysorter. Mysorter can take multiple flags. The flags it takes are -d: the depth of the structure, -f: filename, -a: atribute number that the records should be sorted on (takes values 0 to 3), -o: filename that output will be directed to, -r: if included, then the ranges will be determined randmly. One of the possible invocations goes as follows: 
./mysorter -f test_data_100000.bin -a 0 -d 2

If the flag -o is not specified, then the output would be written to standard out. This includes reporting of missing signals, if none of the signals are missing nothing will be reported. If the flag -r is not included, the records will be distributed in equal sizes among the sorters. 

The program is structured in a way that the main logic is implemented in the file called main.cpp. It contains the recursive calls to split function, which decides based on the depth level whether to split further or call the sorter programs. All the helper functions are located in utility.cpp, including reading and writing to and from the files and pipes. Some additional functions are writter there as well. 





