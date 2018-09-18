/** @brief Header file from Button read/control module (bacon cape)
    @author Pedro Bertoleti
    @date September, 2018
*/
#ifndef BUTTONCTRL_H
#define BUTTONCTRL_H

#define BUTTON_CTRL_ERROR    -1
#define BUTTON_CTRL_SUCCESS   0

/** @brief Button control structure.\n\n
    This is the button control structure, a structure that defines everything regarding to its configuration and usage.
*/
typedef struct {
/** button_config is the button config flag. It indicates if button GPIO is already configured (1) or not (0).\n
    Operations on button will only take action when button_config is equal to 1.
*/
   int button_config;
/** gpio_num stores the GPIO number that button is wired to.\n
    This is defined by user program, allowing multiple buttons handling (if necessary)
*/
   int gpio_num;
/** button_state stores the state of the GPIO that button is wired to.
*/
   int button_state;
}TBUTTONCtrl;

int setup_button(TBUTTONCtrl * ptBUTTON);
int read_button(TBUTTONCtrl * ptBUTTON);

#endif