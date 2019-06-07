FSM=fsm

FSM_SRC=fsm/fsm.smudge \
		fsm/button.smudge \
		fsm/relay.smudge \
		fsm/led.smudge \
		fsm/network.smudge \
		fsm/mqtt.smudge

all: $(FSM).c $(FSM).pdf multism.c relay1.cpp relay2.cpp button1.cpp button2.cpp

$(FSM).c: $(FSM).smudge
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

relay1.cpp: fsm/relay.cpp.in
	sed 's/RELAY_NAME/1/g' $< | sed 's/RELAY_PIN/12/g' > $@

relay2.cpp: fsm/relay.cpp.in
	sed 's/RELAY_NAME/2/g' $< | sed 's/RELAY_PIN/13/g' > $@

button1.cpp: fsm/button.cpp.in
	sed 's/BUTTON_NAME/1/g' $< | sed 's/BUTTON_PIN/16/g' > $@

button2.cpp: fsm/button.cpp.in
	sed 's/BUTTON_NAME/2/g' $< | sed 's/BUTTON_PIN/14/g' > $@

$(FSM).pdf: $(FSM).gv
	dot -Tpdf $(FSM).gv > $@

$(FSM).gv: $(FSM).c

multism.c: $(FSM).smudge
	python3 smudge.py $< > $@

clean:
	rm -f $(FSM).c
	rm -f $(FSM).h
	rm -f $(FSM).pdf
	rm -f $(FSM)_ext.h
	rm -f $(FSM).gv
	rm -f multism.c
	rm -f relay1.cpp
	rm -f relay2.cpp
	rm -f button1.cpp
	rm -f button2.cpp
