/** @brief Header file of display (7-segments) control module (bacon cape)\n
    Shift-register datasheet link: http://www.ti.com/lit/ds/symlink/sn74hc595.pdf\n
    This module was highly inspired by SPI control examples present in Chapter 8 of\n
    "Exploring BeagleBone: Tools and Techniques for Building" book (author: Derek Molloy)\n\n
    IMPORTANT: I2C-1 must be disable. Otherwise, 7-segment display won't work.
    @author Pedro Bertoleti
    @date September, 2018
*/
#ifndef DISP7SEGCTRL_H
#define DISP7SEGCTRL_H

#define DISP7SEG_CTRL_ERROR   -1
#define DISP7SEG_CTRL_SUCCESS  0

/** @brief 7-segments display control structure.\n\n
    This is the 7-segments display control structure, a structure that defines everything regarding to its configuration and usage.
*/
typedef struct {
/** display7seg_config is the 7-segments display config flag. It indicates if 7-segments display communication (SPI) is already configured (1) or not (0).\n
    Operations on 7-segments display will only take action when display7seg_config is equal to 1.
*/
   int display7seg_config;
/** alfanum_to_write stores the alfanumeric char to write on 7-segments display\n
    So far, only 0,1,2,3,4,5,6,7,8,9,A,b,C,d,E and F alfanumeric chars are allowed.
*/
   char alfanum_to_write;
/** write_point is a flag that indicates if the decimal point separator of 7-segments display must light (1) or not (0)
*/
   char write_point;
/** gpio_num_clear is the GPIO number that shift-register clear pin is wired to.\n
    This is defined by user program, allowing multiple 7-segments displays handling (if necessary).
*/
   int gpio_num_clear;
/** spi_filedescriptor is the file descriptor for SPI file that controls shift-register.
*/
   int spi_filedescriptor;
}TDISP7SEGCtrl;

int setup_display7seg(TDISP7SEGCtrl * ptDISP7SEG);
int close_display7seg(TDISP7SEGCtrl * ptDISP7SEG);
int clear_display7seg(TDISP7SEGCtrl * ptDISP7SEG);
int light_display7seg(TDISP7SEGCtrl * ptDISP7SEG);
int write_alfanum_display7seg(TDISP7SEGCtrl * ptDISP7SEG);
#endif