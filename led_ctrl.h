/* Header file do controle de leds (bacon cape) */
#ifndef LEDCTRL_H
#define LEDCTRL_H

#define LED_CTRL_ERROR   -1

typedef struct {
   int led_config;	
   int gpio_num;
   int led_state;
}TLEDCtrl;

#endif

int setup_led(TLEDCtrl * ptLED);
int turn_on_led(TLEDCtrl * ptLED);
int turn_off_led(TLEDCtrl * ptLED);