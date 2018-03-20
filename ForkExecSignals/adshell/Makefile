SOURCE		=  adsh
CFLAGS		= -g -c -ansi 
CC		= gcc
OUT		= adsh
LIBS		= 
INCS		= 

all:	$(SOURCE).o 
	gcc -g -o $(OUT) $(SOURCE).o 

$(SOURCE).o:	$(SOURCE).c
	$(CC) $(CFLAGS) $(SOURCE).c

clean:	
	rm -f $(SOURCE).o $(OUT)
	
