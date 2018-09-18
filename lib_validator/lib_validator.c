/* Program sample for lib's validation only */
/*
 * Author: Pedro Bertoleti
 */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../bacon_cape_lib/led_ctrl.h"
#include "../bacon_cape_lib/slider_ctrl.h"
#include "../bacon_cape_lib/button_ctrl.h"
#include "../bacon_cape_lib/eeprom_ctrl.h"
#include "../bacon_cape_lib/display7seg_ctrl.h"

/* Defines */
#define TEST_NUM_LEDS   3

/* Main program */
int main(void)
{
   int i;
   TLEDCtrl LED_green;
   TLEDCtrl LED_blue;
   TLEDCtrl LED_red;
   TSLIDERCtrl slider;
   TBUTTONCtrl push_button;
   TEEPROMCtrl eeprom;
   TDISP7SEGCtrl disp7seg;

   /* Fill information about all bacon cape's features covered by lib */
   LED_green.led_config = 0;
   LED_green.gpio_num = 50;

   LED_blue.led_config = 0;
   LED_blue.gpio_num = 51;

   LED_red.led_config = 0;
   LED_red.gpio_num = 7;

   slider.slider_config = 0;
   slider.slider_analog_input = 5;

   push_button.button_config = 0;
   push_button.gpio_num = 22;

   eeprom.eeprom_config = 0;

   disp7seg.display7seg_config = 0;
   disp7seg.write_point = 0;
   disp7seg.gpio_num_clear = 48;

   /* Setup all bacon cape's features covered by lib */
   setup_led(&LED_green);
   setup_led(&LED_blue);
   setup_led(&LED_red);
   setup_slider(&slider);
   setup_button(&push_button);
   setup_eeprom(&eeprom);
   setup_display7seg(&disp7seg);

   printf("\n\r[EEPROM TEST]\n");

   eeprom.high_addr_byte = 0x00;
   eeprom.low_addr_byte = 0x00;
   eeprom.byte_to_write = 0xEA;

   write_byte_eeprom(&eeprom);
   read_byte_eeprom(&eeprom);

   if (eeprom.byte_read == eeprom.byte_to_write)
      printf("\n\r[EEPROM TEST] EEPROM test Ok. Write: %02X  Read: %02X\n", eeprom.byte_to_write, eeprom.byte_read);
   else
      printf("\n\r[EEPROM TEST] Errors occured in EEPROM test. Write: %02X  Read: %02X\n", eeprom.byte_to_write, eeprom.byte_read);

   close_eeprom(&eeprom);

   printf("\n\r[DISPLAY TEST]\n");   

   for(i=0; i<0x10; i++)
   {
      printf("\n\r[DISPLAY TEST] Alfanumeric char printed: %02X\n",i);
      disp7seg.alfanum_to_write = (char)i;
      write_alfanum_display7seg(&disp7seg);
      usleep(500000);
   }

   /* Push-button, slider and LEDs validation */
   for(i=0; i<TEST_NUM_LEDS; i++)
   {
      turn_on_led(&LED_green);
      sleep(1);
      turn_off_led(&LED_green);
      sleep(1);

      read_slider(&slider);
      printf("\n\rSlider: %d\n", slider.slider_value);

      read_button(&push_button);
      printf("\n\rPush-button state: %d\n", push_button.button_state);

      turn_on_led(&LED_blue);
      sleep(1);
      turn_off_led(&LED_blue);
      sleep(1);

      read_slider(&slider);
      printf("\n\rSlider: %d\n", slider.slider_value);

      read_button(&push_button);
      printf("\n\rPush-button state: %d\n", push_button.button_state);

      turn_on_led(&LED_red);
      sleep(1);
      turn_off_led(&LED_red);
      sleep(1);

      read_slider(&slider);
      printf("\n\rSlider: %d\n", slider.slider_value);

      read_button(&push_button);
      printf("\n\rPush-button state: %d\n", push_button.button_state);
   }

   return 0;
}
