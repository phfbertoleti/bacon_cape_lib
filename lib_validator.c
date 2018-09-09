/* Programa de validacao da lib (bacon cape) */
/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "led_ctrl.h"
#include "slider_ctrl.h"
#include "button_ctrl.h"
#include "eeprom_ctrl.h"
#include "display7seg_ctrl.h"

/* Defines */
#define NUMERO_TESTES_LEDS   3

int main(void)
{
   int i;
   TLEDCtrl LED_verde;
   TLEDCtrl LED_azul;
   TLEDCtrl LED_vermelho;
   TSLIDERCtrl slider;
   TBUTTONCtrl botao;
   TEEPROMCtrl eeprom;
   TDISP7SEGCtrl disp7seg;

   /* Popula as informacoes dos leds */
   LED_verde.led_config = 0;
   LED_verde.gpio_num = 50;

   LED_azul.led_config = 0;
   LED_azul.gpio_num = 51;

   LED_vermelho.led_config = 0;
   LED_vermelho.gpio_num = 7;

   /* Popula as informacoes do slider */
   slider.slider_config = 0;
   slider.slider_analog_input = 5;

   /* Popula as informacoes do botao */
   botao.button_config = 0;
   botao.gpio_num = 22;

   /* Popula as informacoes da EEPROM */
   eeprom.eeprom_config = 0;

   /* Popula as informacoes do display 7 segmentos */
   disp7seg.display7seg_config = 0;
   disp7seg.write_point = 0;
   disp7seg.gpio_num_clear = 48;

   /* Faz o setup dos leds, botao e slider */
   setup_led(&LED_verde);
   setup_led(&LED_azul);
   setup_led(&LED_vermelho);
   setup_slider(&slider);
   setup_button(&botao);
   setup_eeprom(&eeprom);
   setup_display7seg(&disp7seg);

   
   printf("\n\r[TESTE DE EEPROM]\n");

   eeprom.high_addr_byte = 0x00;
   eeprom.low_addr_byte = 0x00;
   eeprom.byte_to_write = 0xEA;

   write_byte_eeprom(&eeprom);
   read_byte_eeprom(&eeprom);

   if (eeprom.byte_read == eeprom.byte_to_write)
      printf("\n\r[TESTE DE EEPROM] Ok. Byte escrito: %02X  Byte lido: %02X\n", eeprom.byte_to_write, eeprom.byte_read);
   else
      printf("\n\r[TESTE DE EEPROM] Erro. Byte escrito: %02X  Byte lido: %02X\n", eeprom.byte_to_write, eeprom.byte_read);
   

   close_eeprom(&eeprom);
   
   printf("\n\r[TESTE DE DISPLAY 7 SEGMENTOS]\n");   

   for(i=0; i<0x10; i++)
   {
      printf("\n\r[TESTE DE DISPLAY 7 SEGMENTOS] Caracter %02X\n",i);
      disp7seg.alfanum_to_write = (char)i;
      write_alfanum_display7seg(&disp7seg);   
      usleep(500000);
   }

   close_display7seg(&disp7seg);

   /* Rotina de validacao / testes */
   for(i=0; i<NUMERO_TESTES_LEDS; i++)
   {
      turn_on_led(&LED_verde);
      sleep(1);
      turn_off_led(&LED_verde);
      sleep(1);

      read_slider(&slider);
      printf("\n\rSlider: %d\n", slider.slider_value);

      read_button(&botao);
      printf("\n\rBotao: %d\n", botao.button_state);

      turn_on_led(&LED_azul);
      sleep(1);
      turn_off_led(&LED_azul);
      sleep(1);

      read_slider(&slider);
      printf("\n\rSlider: %d\n", slider.slider_value);

      read_button(&botao);
      printf("\n\rBotao: %d\n", botao.button_state);

      turn_on_led(&LED_vermelho);
      sleep(1);
      turn_off_led(&LED_vermelho);
      sleep(1);

      read_slider(&slider);
      printf("\n\rSlider: %d\n", slider.slider_value);

      read_button(&botao);
      printf("\n\rBotao: %d\n", botao.button_state);
   }

}
