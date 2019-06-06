all: relay relay.pdf multism.c

relay: relay.smudge
	smudge $<

relay.pdf: relay.gv
	dot -Tpdf relay.gv > $@

relay.gv: relay

multism.c: relay.smudge
	python3 smudge.py $< > $@

clean:
	rm -f relay.c
	rm -f relay.h
	rm -f relay.pdf
	rm -f relay_ext.h
	rm -f relay.gv
	rm -f multism.c
