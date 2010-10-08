# You can use either the shiny new clang compiler, or the more traditional gcc
#COMPILER=/Developer/usr/bin/clang
COMPILER=gcc
FLAGS=-Wall -O2
LIBRARIES=-lncurses

all: Seitunes

clean:
	rm *.o Seitunes

distclean: clean
	rm *~

Seitunes:Seitunes.o libseitunes.o
	$(COMPILER) -o Seitunes Seitunes.o libseitunes.o $(LIBRARIES)

Seitunes.o:Seitunes.c
	$(COMPILER) $(FLAGS) -c Seitunes.c -o Seitunes.o

libseitunes.o:libseitunes.c
	$(COMPILER) $(FLAGS) -c libseitunes.c -o libseitunes.o

install:
	cp Seitunes /usr/bin

