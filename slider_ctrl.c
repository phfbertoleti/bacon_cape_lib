/* Modulo para leitura do slider (bacon cape) */
#include "slider_ctrl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//#define ENABLE_DEBUG_MESSAGES_SLIDER

#ifdef ENABLE_DEBUG_MESSAGES_SLIDER
   #define DEBUG_SLIDER(...) printf(__VA_ARGS__)
#else
   #define DEBUG_SLIDER
#endif

int setup_slider(TSLIDERCtrl * ptSLIDER)
{
	int sys_req;
	
    /* Se o slider ja foi configurado, nao ha nada a fazer */
    if (ptSLIDER->slider_config != 0)
    	return SLIDER_CTRL_ERROR;
    
    sys_req = system("sudo sh -c \"echo 'BB-ADC' > /sys/devices/platform/bone_capemgr/slots\"");
    
    DEBUG_SLIDER("\r\n[SLIDER] Status (setup): %d\n", sys_req);

    if (sys_req > -1)
    	ptSLIDER->slider_config = 1;
    else
    {
        /* Popula leitura do slider */
        read_slider(ptSLIDER);
    }

    return sys_req;
}


int read_slider(TSLIDERCtrl * ptSLIDER)
{
    FILE *fp;
    char cmd_analog_in[100] = {0};
    char output_analog_read[100] = {0};

    /* Se o slider ainda nao foi configurado, nao ha nada a fazer */
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
