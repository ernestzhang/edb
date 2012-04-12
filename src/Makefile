CC=gcc
CFLAGS=-Wall -g 
OBJECTS=appendlog.o merge.o
all:$(OBJECTS)
$(OBJECTS):%.o:%.c
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm $(OBJECTS)

