CC = gcc
CFLAGS = -lncurses

ODIR = obj

BINDIR = ../bin

_CLIENTOBJ = rivertalk.o
CLIENTOBJ = $(patsubst %,$(ODIR)/%,$(_CLIENTOBJ))

_SERVEROBJ = rivertalkd.o
SERVEROBJ = $(patsubst %,$(ODIR)/%,$(_SERVEROBJ))

_ALLOBJ =  rivertalkd.o rivertalk.o
ALLOBJ = $(patsubst %,$(ODIR)/%,$(_ALLOBJ))

_EXECUTABLES = rivertalk rivertalkd
EXECUTABLES = $(patsubst %,$(BINDIR)/%,$(_EXECUTABLES))

all: $(EXECUTABLES)

$(BINDIR)/rivertalk: $(CLIENTOBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(BINDIR)/rivertalkd: $(SERVEROBJ)
	$(CC) -o $@ $^ $(CFLAGS)


$(ODIR)/rivertalk.o: rivertalk.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/rivertalkd.o: rivertalkd.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(ODIR)/*.o
