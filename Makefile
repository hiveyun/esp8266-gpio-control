OBJS=relay.o queue.o multism.o

all: relay relay.pdf

relay: relay.smudge
	smudge $<

relay.pdf: relay.gv
	dot -Tpdf relay.gv > $@

relay.gv: relay

clean:
	rm -f relay.c
	rm -f relay.h
	rm -f relay.pdf
	rm -f relay_ext.h
	rm -f relay.gv
	rm -f $(OBJS)
