FSM=fsm
FSM_PATH=../fsm
SCRIPT_PATH=../scripts
IN_PATH=../in

include config.mk

FSM_SRC=$(FSM_PATH)/button.smudge \
		$(FSM_PATH)/switch.smudge \
		$(FSM_PATH)/blinker.smudge \
		$(FSM_PATH)/network.smudge \
		$(FSM_PATH)/mqtt.smudge \

ALL_FILES=mqtt.h \
		  multism.h \
		  network.cpp \
		  smudge.cpp \
		  storage.cpp \
		  storage.h \

all: $(FSM).c $(FSM).pdf multism.c blinker.cpp mqtt.cpp config.h $(ALL_FILES)
	@env $(ENVS) python3 $(SCRIPT_PATH)/button.py $(SWITCH_COUNT)
	@env $(ENVS) python3 $(SCRIPT_PATH)/switch.py $(SWITCH_COUNT)

$(FSM).c: $(FSM).smudge
	smudge $<

$(FSM).smudge: $(FSM_SRC) $(SCRIPT_PATH)/fsm.py config.mk
	python3 $(SCRIPT_PATH)/fsm.py $(SWITCH_COUNT) > $@
	for i in $$(seq 1 $(SWITCH_COUNT)); do \
		sed "s/button/button$$i/g" $(FSM_PATH)/button.smudge >> $@; \
		sed "s/switch/switch$$i/g" $(FSM_PATH)/switch.smudge >> $@; \
	done
	cat $(FSM_PATH)/blinker.smudge >> $@
	cat $(FSM_PATH)/network.smudge >> $@
	cat $(FSM_PATH)/mqtt.smudge >> $@

blinker.cpp: $(IN_PATH)/blinker.cpp.in
	sed 's/BLINKER_PIN/2/g' $< > $@

$(FSM).pdf: $(FSM).gv
	dot -Tpdf $(FSM).gv > $@

$(FSM).gv: $(FSM).c

multism.c: $(FSM).smudge $(SCRIPT_PATH)/smudge.py
	python3 $(SCRIPT_PATH)/smudge.py $< > $@

mqtt.cpp: $(IN_PATH)/mqtt.cpp.in $(SCRIPT_PATH)/switch_action.py
	python3 $(SCRIPT_PATH)/switch_action.py $< $(SWITCH_COUNT) > $@

config.h: $(IN_PATH)/config.h.in config.mk
	sed -e 's/MQTT_USERNAME.*/MQTT_USERNAME "${PRODUCT_KEY}"/' \
		-e 's/MQTT_HOST.*/MQTT_HOST "${MQTT_HOST}"/' \
		-e 's/MQTT_PORT.*/MQTT_PORT ${MQTT_PORT}/' \
		-e 's/KEEP_SWITCH_STATE.*/KEEP_SWITCH_STATE ${KEEP_SWITCH_STATE}/' \
		-e 's/SWITCH_STATE_ON.*/SWITCH_STATE_ON ${SWITCH_STATE_ON}/' \
		-e 's/SWITCH_STATE_OFF.*/SWITCH_STATE_OFF ${SWITCH_STATE_OFF}/' \
		-e 's/DEFAULT_SWITCH_STATE.*/DEFAULT_SWITCH_STATE ${DEFAULT_SWITCH_STATE}/' \
		-e 's/USE_SWITCH.*/USE_SWITCH 1/' \
		$< > $@

%.h:
	cp ../$@ $@

%.cpp:
	cp ../$@ $@

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
	rm -f config.h
	rm -f $(ALL_FILES)
