/** @brief LED write/control module (bacon cape)
    @author Pedro Bertoleti
    @date September, 2018
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "led_ctrl.h"
#include "general_functions.h"

/** @addtogroup LED_control
@{
*/

/**Setup LED\n
        This function initializes LED GPIO direction to output. This function must be called before any other LED function.

        @param[in] ptLED - pointer to LED structure variable
        @return Success or fail (LED_CTRL_SUCCESS or LED_CTRL_ERROR)
*/
int setup_led(TLEDCtrl * ptLED)
{
    int sys_req[2];
    char cmd_gpio[100] = {0};
    int ret= LED_CTRL_SUCCESS;

    /* If LED GPIO has been already configured, there's nothing more to do here. */
    if (ptLED->led_config != 0)
        return LED_CTRL_ERROR;

    sprintf(cmd_gpio, "echo %d > /sys/class/gpio/export", ptLED-> gpio_num);
    sys_req[0] = system(cmd_gpio);

    sprintf(cmd_gpio, "echo out > /sys/class/gpio/gpio%d/direction", ptLED-> gpio_num);
    sys_req[1] = system(cmd_gpio);

    ret = ret_min(sys_req,2);

    if (ret > -1)
       ptLED->led_config = 1;
    else
    {
        /* Inicializa com o LED apagado */
        turn_off_led(ptLED);
    }

    return ret;
}

/**Turn the LED on\n
        This function turns the LED on.

        @param[in] ptLED - pointer to LED structure variable
        @return Success or fail (LED_CTRL_SUCCESS or LED_CTRL_ERROR)
*/
int turn_on_led(TLEDCtrl * ptLED)
{
    int sys_req_ext_status[2];
    char cmd_gpio[100] = {0};
    int ret = LED_CTRL_SUCCESS;

    /* If LED GPIO hasn't been configured yet, there's nothing more to do here. */
    if (ptLED->led_config == 0)
        return LED_CTRL_ERROR;

    sprintf(cmd_gpio, "echo 1 > /sys/class/gpio/gpio%d/value", ptLED-> gpio_num);
    sys_req_ext_status[0] = system(cmd_gpio);
    sys_req_ext_status[1] = WEXITSTATUS(sys_req_ext_status[0]);

    ret = ret_min(sys_req_ext_status,2);

    if (ret > -1)
      ptLED->led_state = 1;

    return ret;
}

/**Turn the LED off\n
        This function turns the LED off.

        @param[in] ptLED - pointer to LED structure variable
        @return Success or fail (LED_CTRL_SUCCESS or LED_CTRL_ERROR)
*/
int turn_off_led(TLEDCtrl * ptLED)
{
    int sys_req_ext_status[2];
    char cmd_gpio[100] = {0};
    int ret = LED_CTRL_SUCCESS;

    /* If LED GPIO hasn't been configured yet, there's nothing more to do here. */
    if (ptLED->led_config == 0)
        return LED_CTRL_ERROR;

    sprintf(cmd_gpio, "echo 0 > /sys/class/gpio/gpio%d/value", ptLED-> gpio_num);
    sys_req_ext_status[0] = system(cmd_gpio);
    sys_req_ext_status[1] = WEXITSTATUS(sys_req_ext_status[0]);

    ret = ret_min(sys_req_ext_status,2);

    if (ret > -1)
      ptLED->led_state = 0;

    return ret;
}
/** @} */