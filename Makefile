FSM=fsm

FSM_SRC=fsm/fsm.smudge \
		fsm/button.smudge \
		fsm/relay.smudge \
		fsm/led.smudge \
		fsm/network.smudge \
		fsm/mqtt.smudge

all: $(FSM) $(FSM).pdf multism.c

$(FSM): $(FSM).smudge
	smudge $<

$(FSM).smudge: $(FSM_SRC)
	> $@
	cat fsm/fsm.smudge >> $@
	sed 's/button/button1/g' fsm/button.smudge >> $@
	sed 's/button/button2/g' fsm/button.smudge >> $@
	sed 's/relay/relay1/g' fsm/relay.smudge >> $@
	sed 's/relay/relay2/g' fsm/relay.smudge >> $@
	cat fsm/led.smudge >> $@
	cat fsm/network.smudge >> $@
	cat fsm/mqtt.smudge >> $@

$(FSM).pdf: $(FSM).gv
	dot -Tpdf $(FSM).gv > $@

$(FSM).gv: $(FSM)

multism.c: $(FSM).smudge
	python3 smudge.py $< > $@

clean:
	rm -f $(FSM).c
	rm -f $(FSM).h
	rm -f $(FSM).pdf
	rm -f $(FSM)_ext.h
	rm -f $(FSM).gv
	rm -f multism.c
