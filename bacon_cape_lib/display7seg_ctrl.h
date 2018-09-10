/* Header file of display (7-segments) control module (bacon cape) */
/* Shift-register datasheet link: http://www.ti.com/lit/ds/symlink/sn74hc595.pdf */

#ifndef DISP7SEGCTRL_H
#define DISP7SEGCTRL_H

#define DISP7SEG_CTRL_ERROR   -1

typedef struct {
   int display7seg_config;	   
   char alfanum_to_write;   
   char write_point;
   int gpio_num_clear;
   int spi_filedescriptor;
}TDISP7SEGCtrl;

#endif

int setup_display7seg(TDISP7SEGCtrl * ptDISP7SEG);
int close_display7seg(TDISP7SEGCtrl * ptDISP7SEG);
int clear_display7seg(TDISP7SEGCtrl * ptDISP7SEG);
int light_display7seg(TDISP7SEGCtrl * ptDISP7SEG);
int write_alfanum_display7seg(TDISP7SEGCtrl * ptDISP7SEG);