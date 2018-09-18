/** @brief Button read/control module (bacon cape)
    @author Pedro Bertoleti
    @date September, 2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "button_ctrl.h"
#include "general_functions.h"

/** @addtogroup Button_control
@{
*/

/**Setup button\n
        This function confgures the button GPIO as input.\n
        This function must be called before any other button function.

        @param[in] ptBUTTON - pointer to button structure variable
        @return Success or fail (BUTTON_CTRL_SUCCESS or BUTTON_CTRL_ERROR)
*/
int setup_button(TBUTTONCtrl * ptBUTTON)
{
    int sys_req[2];
    char cmd_gpio[100] = {0};
    int ret = BUTTON_CTRL_SUCCESS;

    /* If push-button GPIO has been already configured, there's nothing more to do here */
    if (ptBUTTON->button_config != 0)
    	return BUTTON_CTRL_ERROR;

    sprintf(cmd_gpio,"echo %d > /sys/class/gpio/export", ptBUTTON->gpio_num);
    sys_req[0] = system(cmd_gpio);

    sprintf(cmd_gpio,"echo in > /sys/class/gpio/gpio%d/direction", ptBUTTON->gpio_num);
    sys_req[1] = system(cmd_gpio);

    ret = ret_min(sys_req,2);

    if (ret > -1)
    	ptBUTTON->button_config = 1;
    else
    {
        /* Make the first push-button state read */
        read_button(ptBUTTON);
    }

    return ret;
}

/**Read button state\n
        This function reads the button state

        @param[in] ptBUTTON - pointer to button structure variable
        @return Success or fail (BUTTON_CTRL_SUCCESS or BUTTON_CTRL_ERROR)
*/
int read_button(TBUTTONCtrl * ptBUTTON)
{
    FILE *fp;
    char cmd_gpio[100] = {0};
    char output_gpio_read[100] = {0};
    int ret = BUTTON_CTRL_SUCCESS;

    /* If push-button GPIO hasn't been configured yet, there's nothing more to do here */
    if (ptBUTTON->button_config == 0)
    	return BUTTON_CTRL_ERROR;

    sprintf(cmd_gpio,"cat /sys/class/gpio/gpio%d/value", ptBUTTON->gpio_num);
    fp = popen(cmd_gpio, "r");

    if (fp == NULL)
       ret = BUTTON_CTRL_ERROR;

    if (ret == BUTTON_CTRL_SUCCESS)
        while (fgets(output_gpio_read, sizeof(output_gpio_read)-1, fp) != NULL);

    ptBUTTON->button_state = atoi(output_gpio_read);    
    return ret;
}
/** @} */