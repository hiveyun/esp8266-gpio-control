all: fsm fsm.pdf multism.c

fsm: fsm.smudge
	smudge $<

fsm.pdf: fsm.gv
	dot -Tpdf fsm.gv > $@

fsm.gv: fsm

multism.c: fsm.smudge
	python3 smudge.py $< > $@

clean:
	rm -f fsm.c
	rm -f fsm.h
	rm -f fsm.pdf
	rm -f fsm_ext.h
	rm -f fsm.gv
	rm -f multism.c
