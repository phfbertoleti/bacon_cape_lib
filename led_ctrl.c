/* Modulo para controle dos leds (bacon cape) */
#include "led_ctrl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//#define ENABLE_DEBUG_MESSAGES_LED

#ifdef ENABLE_DEBUG_MESSAGES_LED
   #define DEBUG_LED(...) printf(__VA_ARGS__)
#else
   #define DEBUG_LED
#endif

int ret_min_led(int * SysReqs, int length)
{
    int	i;
    int min = SysReqs[0];

    for(i=1; i<length; i++)
    {
         if (SysReqs[i] < min)
         	min = SysReqs[i];
    }

    return min;
}

int setup_led(TLEDCtrl * ptLED)
{
    int sys_req[2];    
    char cmd_gpio[100] = {0};
    int ret;

    /* Se GPIO ja foi configurado, nao ha nada a ser feito */
    if (ptLED->led_config != 0)
    	return LED_CTRL_ERROR;

    sprintf(cmd_gpio, "echo %d > /sys/class/gpio/export", ptLED-> gpio_num);
    sys_req[0] = system(cmd_gpio);

    sprintf(cmd_gpio, "echo out > /sys/class/gpio/gpio%d/direction", ptLED-> gpio_num);
    sys_req[1] = system(cmd_gpio);
        
    DEBUG_LED("\n\r[LED] Status (setup): %d %d\n", sys_req[0], sys_req[1]);

    ret = ret_min_led(sys_req,2);

    if (ret > -1)
       ptLED->led_config = 1;
    else
    {
        /* Inicializa com o LED apagado */    
        turn_off_led(ptLED);
    }

    return ret;
}


int turn_on_led(TLEDCtrl * ptLED)
{
    int sys_req_ext_status[2];
    char cmd_gpio[100] = {0};
    int ret;

    /* Se GPIO ainda nao foi configrado, nao ha nada a ser feito */
    if (ptLED->led_config == 0)
    	return LED_CTRL_ERROR;

    sprintf(cmd_gpio, "echo 1 > /sys/class/gpio/gpio%d/value", ptLED-> gpio_num);
    sys_req_ext_status[0] = system(cmd_gpio);
    sys_req_ext_status[1] = WEXITSTATUS(sys_req_ext_status[0]);
     
    ret = ret_min_led(sys_req_ext_status,2);

    if (ret > -1)
      ptLED->led_state = 1;

    return ret;
}

int turn_off_led(TLEDCtrl * ptLED)
{
    int sys_req_ext_status[2];
    char cmd_gpio[100] = {0};
    int ret;

    /* Se GPIO ainda nao foi configrado, nao ha nada a ser feito */
    if (ptLED->led_config == 0)
    	return LED_CTRL_ERROR;

    sprintf(cmd_gpio, "echo 0 > /sys/class/gpio/gpio%d/value", ptLED-> gpio_num);
    sys_req_ext_status[0] = system(cmd_gpio);
    sys_req_ext_status[1] = WEXITSTATUS(sys_req_ext_status[0]);
       
    ret = ret_min_led(sys_req_ext_status,2);

    if (ret > -1)
      ptLED->led_state = 0;

    return ret;
}