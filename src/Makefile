all:test appendlog.o  merge.o
test:test.c
	gcc -Wall -g -o test test.c
appendlog.o:appendlog.c
	gcc -Wall -g -O2 -c appendlog.c 
merge.o:merge.c
	gcc -Wall -g -O2 -c  merge.c
clean:
	rm -rf test
