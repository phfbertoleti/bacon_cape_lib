lib_validator: lib_validator.c \
                 led_ctrl.c \
                 slider_ctrl.c \
                 button_ctrl.c \
                 eeprom_ctrl.c \
                 display7seg_ctrl.c 
	gcc lib_validator.c led_ctrl.c slider_ctrl.c button_ctrl.c eeprom_ctrl.c display7seg_ctrl.c -o lib_validator -lpaho-mqtt3c -Wall
