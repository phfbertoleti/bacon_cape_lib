/** @brief Display (7-segments) control module (bacon cape)\n
    Shift-register datasheet link: http://www.ti.com/lit/ds/symlink/sn74hc595.pdf\n
    This module was highly inspired by SPI control examples present in Chapter 8 of\n
    "Exploring BeagleBone: Tools and Techniques for Building" book (author: Derek Molloy)\n\n
    IMPORTANT: I2C-1 must be disable. Otherwise, 7-segment display won't work.
    @author Pedro Bertoleti
    @date September, 2018
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <linux/spi/spidev.h>
#include "display7seg_ctrl.h"
#include "general_functions.h"

/** @addtogroup 7-Segment_display_control
@{
*/

#define SPI_PATH             "/dev/spidev1.0" 
#define SPI_SPEED            1000000   /* SPI Clk frequency (1MHz) */
#define SPI_BITS_PER_WORD    8         /* A SPI word contains 8 bits */
#define SPI_MODE             3         /* SPI Mode 3 */

#define ENABLE_DEBUG_MESSAGES_DISP7SEG

#ifdef ENABLE_DEBUG_MESSAGES_DISP7SEG
   #define DEBUG_DISP7SEG(...) printf(__VA_ARGS__)
#else
   #define DEBUG_DISP7SEG
#endif

/* Constantes */ 
//                                        Pgfedcba
const unsigned char alfanum_map[16] = { 0b01000000,   //0
                                        0b01111001,   //1
                                        0b00100100,   //2
		                        0b00110000,   //3		
		                        0b00011001,   //4		
		                        0b00010010,   //5
		                        0b00000010,   //6
		                        0b01111000,   //7
		                        0b00000000,   //8
		                        0b00010000,   //9
		                        0b00001000,   //A
		                        0b00000011,   //b
		                        0b01000110,   //C
		                        0b00100001,   //d
		                        0b00000110,   //e
		                        0b00001110    //F
                                      };

/**SPI buffer transfer (send/receive)\n
        Send and receive buffer using SPI.
        @param[in] ptDISP7SEG - pointer to 7-segments display structure variable
        @param[in] send - buffer to send via SPI
        @param[out] receive - buffer to be filled with received bytes from SPI
        @param[in] length - length of the buffer to be sent via SPI
        @return Success (0) or Fail (!=0)
*/
int spi_transfer(TDISP7SEGCtrl * ptDISP7SEG, unsigned char send[], unsigned char receive[], int length)
{
    struct spi_ioc_transfer transfer_spi;
    int status;

    /* Fill SPI communication structure */
    transfer_spi.tx_buf = (unsigned long) send;
    transfer_spi.rx_buf = (unsigned long) receive; // the buffer for receiving data
    transfer_spi.len = length;
    transfer_spi.speed_hz = SPI_SPEED;
    transfer_spi.bits_per_word = SPI_BITS_PER_WORD;
    transfer_spi.delay_usecs = 0;

    /* Transfer informations to device (considering SPI structure informations) */
    status = ioctl(ptDISP7SEG->spi_filedescriptor, SPI_IOC_MESSAGE(1), &transfer_spi);
    return status;
}

/**Setup 7-segment display\n
        This function initializes 7-segments display communication (SPI).\n
        This function must be called before any other 7-segments display function.

        @param[in] ptDISP7SEG - pointer to 7-segments display structure variable
        @return Success or fail (DISP7SEG_CTRL_SUCCESS or DISP7SEG_CTRL_ERROR)
*/
int setup_display7seg(TDISP7SEGCtrl * ptDISP7SEG)
{
    int sys_req[50];
    char cmd_gpio[100] = {0};
    int ret = DISP7SEG_CTRL_SUCCESS;
    int idx_sys_req = 0;
    uint32_t speed = SPI_SPEED;
    uint8_t bits = SPI_BITS_PER_WORD;  
    uint8_t mode = SPI_MODE;
	
    /* If display has been already configured, there's nothing to do here */
    if (ptDISP7SEG->display7seg_config != 0)
    	return DISP7SEG_CTRL_ERROR;

    sys_req[idx_sys_req] = system("sudo sh -c \"echo BB-SPIDEV0 > /sys/devices/platform/bone_capemgr/slots\"");
    idx_sys_req++;

    /* Important: shift-register clear GPIO must be configured and set to 1 to ensure no forced clean condition is applied */
    sprintf(cmd_gpio, "echo %d > /sys/class/gpio/export", ptDISP7SEG->gpio_num_clear);
    sys_req[idx_sys_req] = system(cmd_gpio);
    idx_sys_req++;

    sprintf(cmd_gpio, "echo out > /sys/class/gpio/gpio%d/direction", ptDISP7SEG->gpio_num_clear);
    sys_req[idx_sys_req] = system(cmd_gpio);
    idx_sys_req++;

    sprintf(cmd_gpio, "echo 1 > /sys/class/gpio/gpio%d/value", ptDISP7SEG->gpio_num_clear);
    sys_req[idx_sys_req] = system(cmd_gpio);
    idx_sys_req++;

    /* Try to open SPI communication (for both write and read) and get access to SPI bus */
    if ((ptDISP7SEG->spi_filedescriptor = open(SPI_PATH, O_RDWR))<0)
    {
        sys_req[idx_sys_req] = -1;
        idx_sys_req++;
    }
    else 
    {
        sys_req[idx_sys_req] = ioctl(ptDISP7SEG->spi_filedescriptor, SPI_IOC_WR_MODE, &mode);
        idx_sys_req++;

        sys_req[idx_sys_req] = ioctl(ptDISP7SEG->spi_filedescriptor, SPI_IOC_RD_MODE, &mode);
        idx_sys_req++;

        sys_req[idx_sys_req] = ioctl(ptDISP7SEG->spi_filedescriptor, SPI_IOC_WR_BITS_PER_WORD, &bits);
        idx_sys_req++;

        sys_req[idx_sys_req] = ioctl(ptDISP7SEG->spi_filedescriptor, SPI_IOC_RD_BITS_PER_WORD, &bits);
        idx_sys_req++;

        sys_req[idx_sys_req] = ioctl(ptDISP7SEG->spi_filedescriptor, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
        idx_sys_req++;

        sys_req[idx_sys_req] = ioctl(ptDISP7SEG->spi_filedescriptor, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
        idx_sys_req++;
    }

    ret = ret_min(sys_req,idx_sys_req);

    if (ret > -1)
    	ptDISP7SEG->display7seg_config = 1;

    return ret;
}

/**Close 7-segment display\n
        This function closes 7-segments display communication (SPI).\n
        This function must be called when there's no more usage of 7-segments display.

        @param[in] ptDISP7SEG - pointer to 7-segments display structure variable
        @return Success (0) or Fail (!=0)
*/
int close_display7seg(TDISP7SEGCtrl * ptDISP7SEG)
{
    return close(ptDISP7SEG->spi_filedescriptor);
}

/**Clear 7-segment display\n
        This function clears all segments of 7-segments display

        @param[in] ptDISP7SEG - pointer to 7-segments display structure variable
        @return Success (0) or Fail (!=0)
*/
int clear_display7seg(TDISP7SEGCtrl * ptDISP7SEG)
{
    int sys_req;
    unsigned char byte_clear = 0xFE;

    /* If display hasn't been configured yet, there's nothing to do here */
    if (ptDISP7SEG->display7seg_config == 0)
        return DISP7SEG_CTRL_ERROR;

    sys_req = spi_transfer(ptDISP7SEG, &byte_clear, 0, 1);
    fflush(stdout);
    return sys_req;
}

/**Light 7-segment display\n
        This function lights all segments of 7-segments display

        @param[in] ptDISP7SEG - pointer to 7-segments display structure variable
        @return Success (0) or Fail (!=0)
*/
int light_display7seg(TDISP7SEGCtrl * ptDISP7SEG)
{
    int sys_req;
    unsigned char byte_light = 0x00;

    /* If display hasn't been configured yet, there's nothing to do here */
    if (ptDISP7SEG->display7seg_config == 0)
    	return DISP7SEG_CTRL_ERROR;

    sys_req = spi_transfer(ptDISP7SEG, &byte_light, 0, 1);
    fflush(stdout);
    return sys_req;
}

/**Write an alfanumeric char to 7-segment display\n
        This function writes an alfanumeric char to 7-segment display.\n
        Valid alfanumeric chars (so far) are: 0,1,2,3,4,5,6,7,8,9,A,b,C,d,E and F

        @param[in] ptDISP7SEG - pointer to 7-segments display structure variable
        @return Success (0) or Fail (!=0)
*/
int write_alfanum_display7seg(TDISP7SEGCtrl * ptDISP7SEG)
{
     unsigned char byte_to_write;
     int idx_alfanum_map;
     int sys_req;

    /* If display hasn't been configured yet, there's nothing to do here */
    if (ptDISP7SEG->display7seg_config == 0)
    	return DISP7SEG_CTRL_ERROR;

    /* If alfanum char is out of allowed range (from 0x00 to 0x0F), this function must end here */
    if (ptDISP7SEG->alfanum_to_write < 0x10)
    {
        idx_alfanum_map = (int)ptDISP7SEG->alfanum_to_write;
        byte_to_write = alfanum_map[idx_alfanum_map];
    }
    else
       return DISP7SEG_CTRL_ERROR;

    /* Set or clear decimal point marker on display (based on ptDISP7SEG->write_point state) */
    byte_to_write = byte_to_write|(ptDISP7SEG->write_point & 0x01);

    sys_req = spi_transfer(ptDISP7SEG, &byte_to_write, 0, 1);
    fflush(stdout);    
    return sys_req;
}
/** @} */