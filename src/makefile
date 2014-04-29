IDIR = ../include
CC = gcc
CFLAGS = -I$(IDIR)

_DEPS = wlog.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

ODIR = obj

BINDIR = ../bin

_CLIENTOBJ = wlog.o rivertalk.o
CLIENTOBJ = $(patsubst %,$(ODIR)/%,$(_CLIENTOBJ))

_SERVEROBJ = wlog.o rivertalkd.o
SERVEROBJ = $(patsubst %,$(ODIR)/%,$(_SERVEROBJ))

_ALLOBJ = wlog.o rivertalkd.o rivertalk.o
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

$(ODIR)/wlog.o: wlog.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/rivertalkd.o: rivertalkd.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(ODIR)/*.o
