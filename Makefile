FSM=fsm

FSM_SRC=fsm/fsm.smudge \
		fsm/button.smudge \
		fsm/switch.smudge \
		fsm/blinker.smudge \
		fsm/network.smudge \
		fsm/mqtt.smudge \

all: $(FSM).c $(FSM).pdf multism.c switch1.cpp button1.cpp blinker.cpp

$(FSM).c: $(FSM).smudge
	smudge $<

$(FSM).smudge: $(FSM_SRC)
	> $@
	cat fsm/fsm.smudge >> $@
	sed 's/button/button1/g' fsm/button.smudge >> $@
	sed 's/switch/switch1/g' fsm/switch.smudge >> $@
	cat fsm/blinker.smudge >> $@
	cat fsm/network.smudge >> $@
	cat fsm/mqtt.smudge >> $@

switch1.cpp: fsm/switch.cpp.in
	sed -e 's/SWITCH_NAME/1/g' -e 's/SWITCH_PIN/12/g' $< > $@

button1.cpp: fsm/button.cpp.in
	sed -e 's/BUTTON_NAME/1/g' -e 's/BUTTON_PIN/16/g' $< > $@

blinker.cpp: fsm/blinker.cpp.in
	sed 's/BLINKER_PIN/2/g' $< > $@

$(FSM).pdf: $(FSM).gv
	dot -Tpdf $(FSM).gv > $@

$(FSM).gv: $(FSM).c

multism.c: $(FSM).smudge scripts/smudge.py
	python3 scripts/smudge.py $< > $@

clean:
	rm -f $(FSM).c
	rm -f $(FSM).h
	rm -f $(FSM).pdf
	rm -f $(FSM)_ext.h
	rm -f $(FSM).gv
	rm -f multism.c
	rm -f switch1.cpp
	rm -f button1.cpp
	rm -f $(FSM).smudge
	rm -f blinker.cpp
