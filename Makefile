CC	= gcc

CFILES  = main.c parser.c infocs.c dcc.c db.c wiki.c google.c

OBJS	= main.o parser.o infocs.o dcc.o db.o wiki.o google.o

LINKER = pthread

all: cfiles compile clean

cfiles: $(CFILES)
	$(CC) -c $(CFILES)

compile: $(OBJS)
	$(CC) $(OBJS) -l $(LINKER) -o xape

clean:
	rm -f $(OBJS)

