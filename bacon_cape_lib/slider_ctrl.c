/** @brief Slider read/control module (bacon cape)
    @author Pedro Bertoleti
    @date September, 2018
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "slider_ctrl.h"


/** @addtogroup Slider_control
@{
*/

/* General defines */
#define ADC_MAX_VALUE    4095 /* BBB ADC is a 11-bit ADC, therefore: Max ADC value = 2^11 - 1 = 4095 */

/**Setup accelerometer\n
        This function initializes slider ADC input (channel 5). This function must be called before any other slider function.

        @param[in] ptSLIDER - pointer to slider structure variable
        @return Success or fail (SLIDER_CTRL_SUCCESS or SLIDER_CTRL_ERROR)
*/
int setup_slider(TSLIDERCtrl * ptSLIDER)
{
    int sys_req;

    /* If slider has already configured, there's nothing to do here */
    if (ptSLIDER->slider_config != 0)
    	return SLIDER_CTRL_ERROR;

    sys_req = system("sudo sh -c \"echo 'BB-ADC' > /sys/devices/platform/bone_capemgr/slots\"");

    if (sys_req > -1)
    	ptSLIDER->slider_config = 1;
    else
    {
        /* Make first slider reading */
        read_slider(ptSLIDER);
    }

    return sys_req;
}

/**Read raw value from slider\n
        This function reads the raw value from slider ADC.

        @param[in] ptSLIDER - pointer to slider structure variable
        @return Success or fail (SLIDER_CTRL_SUCCESS or SLIDER_CTRL_ERROR)
*/
int read_slider(TSLIDERCtrl * ptSLIDER)
{
    FILE *fp;
    char cmd_analog_in[100] = {0};
    char output_analog_read[100] = {0};

    /* If slider hasn't been configured yet, there's nothing to do here */
    if (ptSLIDER->slider_config == 0)
    	return SLIDER_CTRL_ERROR;

    sprintf(cmd_analog_in, "cat /sys/bus/iio/devices/iio:device0/in_voltage%d_raw", ptSLIDER->slider_analog_input);
    fp = popen(cmd_analog_in, "r");

    if (fp == NULL)
       return SLIDER_CTRL_ERROR;

    while (fgets(output_analog_read, sizeof(output_analog_read)-1, fp) != NULL);
    pclose(fp);

    ptSLIDER->slider_value = atoi(output_analog_read);
    return SLIDER_CTRL_SUCCESS;
}

/**Read slider percentage\n
        This function reads the percentagem of slider.

        @param[in] ptSLIDER - pointer to slider structure variable
        @return Success or fail (SLIDER_CTRL_SUCCESS or SLIDER_CTRL_ERROR)
*/
int read_slider_percentage(TSLIDERCtrl * ptSLIDER)
{
    if (read_slider(ptSLIDER) == -1)
        return SLIDER_CTRL_ERROR;
    else
        ptSLIDER->slider_percentage_value = (int)(((float)ptSLIDER->slider_value/ADC_MAX_VALUE)*100);

    return SLIDER_CTRL_SUCCESS;
}
/** @} */