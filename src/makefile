CC=gcc
IDIR =../include
CFLAGS=-I$(IDIR)

ODIR=obj

#local libraries
#LDIR =../lib
#LIBS=-lm

_DEPS = shell.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o shell.o parser.o internal.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

shell: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
