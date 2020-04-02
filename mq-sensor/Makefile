FSM=fsm
FSM_PATH=../fsm
SCRIPT_PATH=../scripts
IN_PATH=../in

include config.mk

FSM_SRC=$(FSM_PATH)/button.smudge \
		$(FSM_PATH)/mq.smudge \
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
	sed -e 's/BUTTON_NAME/1/g' -e 's/BUTTON_PIN/$(BUTTON_PIN_1)/g' $(IN_PATH)/button.cpp.in > button1.cpp
	sed -e 's/MQ_NAME/1/g' -e 's/MQ_PIN/$(MQ_PIN_1)/g' $(IN_PATH)/mq.cpp.in > mq1.cpp

$(FSM).c: $(FSM).smudge
	smudge $<

$(FSM).smudge: $(FSM_SRC) fsm_head.smudge config.mk
	cat fsm_head.smudge > $@
	sed "s/button/button1/g" $(FSM_PATH)/button.smudge >> $@
	sed "s/mq/mq1/g" $(FSM_PATH)/mq.smudge >> $@
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
	python3 $(SCRIPT_PATH)/switch_action.py $< 0 > $@

config.h: $(IN_PATH)/config.h.in config.mk
	sed -e 's/MQTT_USERNAME.*/MQTT_USERNAME "${PRODUCT_KEY}"/' \
		-e 's/MQTT_HOST.*/MQTT_HOST "${MQTT_HOST}"/' \
		-e 's/MQTT_PORT.*/MQTT_PORT ${MQTT_PORT}/' \
		-e 's/USE_SWITCH.*/USE_SWITCH 0/' \
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
	rm -f button*.cpp
	rm -f mq*.cpp
	rm -f $(FSM).smudge
	rm -f blinker.cpp
	rm -f mqtt.cpp
	rm -f config.h
	rm -f $(ALL_FILES)
