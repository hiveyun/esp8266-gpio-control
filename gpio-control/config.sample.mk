SWITCH_COUNT=2

ENVS =SWITCH_PIN_1=12
ENVS+=SWITCH_PIN_2=13

ENVS+=BUTTON_PIN_1=16
ENVS+=BUTTON_PIN_2=15

# mqtt server config
PRODUCT_KEY=8a7b722f5d671136231b
MQTT_HOST=gw.huabot.com
MQTT_PORT=11883

# switch state config
# lose power will keep the state
KEEP_SWITCH_STATE=1
SWITCH_STATE_ON=1
SWITCH_STATE_OFF=0
# set default switch state when started if no keep switch state
DEFAULT_SWITCH_STATE=SWITCH_STATE_OFF
