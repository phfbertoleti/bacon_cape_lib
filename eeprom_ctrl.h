/* Header file do controle de eeprom (bacon cape) */
/* Documentação da EEPROM: http://learning.media.mit.edu/projects/gogo/parts_pdf/EEPROM%20-%2024LC256.pdf */
#ifndef EEPROMCTRL_H
#define EEPROMCTRL_H

#define EEPROM_BASE_ADDR         0x50
#define ADDR_A0                  0b00000000
#define ADDR_A1                  0b00000000
#define ADDR_A2                  0b00000000
#define EEPROM_ADDR              EEPROM_BASE_ADDR|ADDR_A0|ADDR_A2|ADDR_A2
#define EEPROM_PAGE_SIZE         64

#define EEPROM_CTRL_ERROR    -1
#define EEPROM_CTRL_SUCCESS  0

typedef struct {
    int eeprom_config;
    int eeprom_addr;
    int i2c_descriptor;
    char high_addr_byte;
    char low_addr_byte;    
    
    /* Dados a serem lidos ou escritos */
    char byte_read;
    char byte_to_write;
    char page_to_write[EEPROM_PAGE_SIZE];
}TEEPROMCtrl;

#endif

int setup_eeprom(TEEPROMCtrl * ptEEPROM);
int close_eeprom(TEEPROMCtrl * ptEEPROM);
int read_byte_eeprom(TEEPROMCtrl * ptEEPROM);
int write_byte_eeprom(TEEPROMCtrl * ptEEPROM);
int write_page_eeprom(TEEPROMCtrl * ptEEPROM);