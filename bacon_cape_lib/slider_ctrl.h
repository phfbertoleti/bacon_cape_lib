/** @brief Header file from slider read/control module (bacon cape)
    @author Pedro Bertoleti
    @date September, 2018
*/
#ifndef SLIDERCTRL_H
#define SLIDERCTRL_H

#define SLIDER_CTRL_ERROR    -1
#define SLIDER_CTRL_SUCCESS   0

/** @brief Slider control structure.

    This is the slider control structure, a structure that defines everything regarding to its configuration and usage.
*/
typedef struct {
/** slider_config is the slider config flag. It indicates if ADC is already configured (1) or not (0).\n
    Operations on slider will only take action when slider_config is equal to 1.
*/
   int slider_config;

/** slider_analog_input contains the analog input number that slider is wired.\n
    This analog input number is informed by the user program, what makes this lib suitable for one or more ADC sliders.
*/
   int slider_analog_input;

/** slider_value stands for the raw value of ADC that slider is wired.
*/
   int slider_value;

/** slider_percentage_value stands for the perentagem of the slider level is applied.\n
    This is calculated considering the maximum ADC value allowed in the ADC slider is wired.
*/
   int slider_percentage_value;
}TSLIDERCtrl;

int setup_slider(TSLIDERCtrl * ptSLIDER);
int read_slider(TSLIDERCtrl * ptSLIDER);
int read_slider_percentage(TSLIDERCtrl * ptSLIDER);
#endif