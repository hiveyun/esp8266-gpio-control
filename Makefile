FSM=fsm

include config.mk

FSM_SRC= fsm/button.smudge \
		fsm/switch.smudge \
		fsm/blinker.smudge \
		fsm/network.smudge \
		fsm/mqtt.smudge \


all: $(FSM).c $(FSM).pdf multism.c blinker.cpp mqtt.cpp config.h
	@env $(ENVS) python3 scripts/switch_button.py $(SWITCH_COUNT)

$(FSM).c: $(FSM).smudge
	smudge $<

$(FSM).smudge: $(FSM_SRC) Makefile scripts/fsm.py
	python3 scripts/fsm.py $(SWITCH_COUNT) > $@
	for i in $$(seq 1 $(SWITCH_COUNT)); do \
		sed "s/button/button$$i/g" fsm/button.smudge >> $@; \
		sed "s/switch/switch$$i/g" fsm/switch.smudge >> $@; \
	done
	cat fsm/blinker.smudge >> $@
	cat fsm/network.smudge >> $@
	cat fsm/mqtt.smudge >> $@

blinker.cpp: in/blinker.cpp.in
	sed 's/BLINKER_PIN/2/g' $< > $@

$(FSM).pdf: $(FSM).gv
	dot -Tpdf $(FSM).gv > $@

$(FSM).gv: $(FSM).c

multism.c: $(FSM).smudge scripts/smudge.py
	python3 scripts/smudge.py $< > $@

mqtt.cpp: in/mqtt.cpp.in scripts/switch_action.py
	python3 scripts/switch_action.py $< $(SWITCH_COUNT) > $@

config.h: in/config.h.in Makefile
	sed -e 's/MQTT_USERNAME.*/MQTT_USERNAME "${PRODUCT_KEY}"/' \
		-e 's/MQTT_HOST.*/MQTT_HOST "${MQTT_HOST}"/' \
		-e 's/MQTT_PORT.*/MQTT_PORT ${MQTT_PORT}/' \
		-e 's/KEEP_SWITCH_STATE.*/KEEP_SWITCH_STATE ${KEEP_SWITCH_STATE}/' \
		-e 's/SWITCH_STATE_ON.*/SWITCH_STATE_ON ${SWITCH_STATE_ON}/' \
		-e 's/SWITCH_STATE_OFF.*/SWITCH_STATE_OFF ${SWITCH_STATE_OFF}/' \
		-e 's/DEFAULT_SWITCH_STATE.*/DEFAULT_SWITCH_STATE ${DEFAULT_SWITCH_STATE}/' \
		$< > $@

clean:
	rm -f $(FSM).c
	rm -f $(FSM).h
	rm -f $(FSM).pdf
	rm -f $(FSM)_ext.h
	rm -f $(FSM).gv
	rm -f multism.c
	rm -f switch*.cpp
	rm -f button*.cpp
	rm -f $(FSM).smudge
	rm -f blinker.cpp
	rm -f mqtt.cpp
	rm -r config.h
