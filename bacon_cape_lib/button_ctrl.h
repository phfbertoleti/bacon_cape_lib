/* Header file og push-button control/read module (bacon cape) */
#ifndef BUTTONCTRL_H
#define BUTTONCTRL_H

#define BUTTON_CTRL_ERROR    -1
#define BUTTON_CTRL_SUCCESS   0

typedef struct {
   int button_config;	
   int gpio_num;
   int button_state;
}TBUTTONCtrl;

#endif

int setup_button(TBUTTONCtrl * ptBUTTON);
int read_button(TBUTTONCtrl * ptBUTTON);