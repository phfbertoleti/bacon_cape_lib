/* Push-button control/read module (bacon cape) */
/*
 * Author: Pedro Bertoleti
 */
#include "button_ctrl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int ret_min_button(int * SysReqs, int length)
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

int setup_button(TBUTTONCtrl * ptBUTTON)
{
    int sys_req[2];
    char cmd_gpio[100] = {0};
    int ret;

    /* If push-button GPIO has been already configured, there's nothing more to do here */
    if (ptBUTTON->button_config != 0)
    	return BUTTON_CTRL_ERROR;

    sprintf(cmd_gpio,"echo %d > /sys/class/gpio/export", ptBUTTON->gpio_num);
    sys_req[0] = system(cmd_gpio);

    sprintf(cmd_gpio,"echo in > /sys/class/gpio/gpio%d/direction", ptBUTTON->gpio_num);
    sys_req[1] = system(cmd_gpio);

    ret = ret_min_button(sys_req,2);

    if (ret > -1)
    	ptBUTTON->button_config = 1;
    else
    {
        /* Make the first push-button state read */
        read_button(ptBUTTON);
    }

    return ret;
}

int read_button(TBUTTONCtrl * ptBUTTON)
{
    FILE *fp;
    char cmd_gpio[100] = {0};
    char output_gpio_read[100] = {0};

    /* If push-button GPIO hasn't been configured yet, there's nothing more to do here */
    if (ptBUTTON->button_config == 0)
    	return BUTTON_CTRL_ERROR;

    sprintf(cmd_gpio,"cat /sys/class/gpio/gpio%d/value", ptBUTTON->gpio_num);

    fp = popen(cmd_gpio, "r");

    if (fp == NULL)
       return BUTTON_CTRL_ERROR;

    while (fgets(output_gpio_read, sizeof(output_gpio_read)-1, fp) != NULL);

    ptBUTTON->button_state = atoi(output_gpio_read);
    return BUTTON_CTRL_SUCCESS;
}