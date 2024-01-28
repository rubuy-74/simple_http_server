IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj
#LDIR=../lib

LIBS=

_DEPS = request_parser.h response_creator.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = server.o request_parser.o response_creator.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o : %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~