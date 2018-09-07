/* Header file do slider (bacon cape) */
#ifndef SLIDERCTRL_H
#define SLIDERCTRL_H

#define SLIDER_CTRL_ERROR    -1
#define SLIDER_CTRL_SUCCESS   0

typedef struct {
   int slider_config;	
   int slider_analog_input;
   int slider_value;
}TSLIDERCtrl;

#endif

int setup_slider(TSLIDERCtrl * ptSLIDER);
int read_slider(TSLIDERCtrl * ptSLIDER);
