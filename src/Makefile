all:test merge.o
test:test.c
	gcc -Wall -g -o test test.c
merge.o:merge.c
	gcc -Wall -g -O2 -o merge.o merge.c
clean:
	rm -rf test
