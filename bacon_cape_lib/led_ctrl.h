/** @brief Header file from LED write/control module (bacon cape)
    @author Pedro Bertoleti
    @date September, 2018
*/
#ifndef LEDCTRL_H
#define LEDCTRL_H

#define LED_CTRL_ERROR    -1
#define LED_CTRL_SUCCESS   0

/** @brief LED control structure.

    This is the LED control structure, a structure that defines everything regarding to its configuration and usage.
*/
typedef struct {
/** led_config is the LED config flag. It indicates if LED GPIO is already configured (1) or not (0).\n
    Operations on LED will only take action when led_config is equal to 1.
*/
   int led_config;

/** gpio_num is the GPIO number that LED is wired. It indicates if LED GPIO is already configured (1) or not (0).\n
    This GPIO number is informed by the user program, what makes this lib suitable to control one or more LEDs.
*/
   int gpio_num;

/** led_state contains the current LED state: 1 for LED on and 0 for LED off
*/
   int led_state;
}TLEDCtrl;


int setup_led(TLEDCtrl * ptLED);
int turn_on_led(TLEDCtrl * ptLED);
int turn_off_led(TLEDCtrl * ptLED);
#endif