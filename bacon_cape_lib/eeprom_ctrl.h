/** @brief Header file from EEPROM control module (bacon cape)\n
    Link for EEPROM datasheet: http://learning.media.mit.edu/projects/gogo/parts_pdf/EEPROM%20-%2024LC256.pdf\n\n
    IMPORTANT:\n
    In order to make EEPROM writing data possible, short-circuit C2 capacitor is needed.\n
    Without this "hardware hack", only EEPROM reading can be done.
    @author Pedro Bertoleti
    @date September, 2018
*/
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

/** @brief EEPROM control structure.\n\n
    This is the EEPROM control structure, a structure that defines everything regarding to its configuration and usage.
*/
typedef struct {
/** eeprom_config is the EEPROM config flag. It indicates if EEPROM communication (I2C) is already configured (1) or not (0).\n
    Operations on EEPROM will only take action when eeprom_config is equal to 1.
*/
    int eeprom_config;
/** eeprom_addr is the EEPROM I2C Address. It's configured by user program, allowing multiple EEPROMs handling (if necessary)
*/
    int eeprom_addr;
/** i2c_descriptor for the EEPROM I2C file.
*/
    int i2c_descriptor;
/** high_addr_byte is the higher part (MSB) of EEPROM internal memory address (for write or read operations)
*/
    char high_addr_byte;
/** low_addr_byte is the lower part (LSB) of EEPROM internal memory address (for write or read operations)
*/
    char low_addr_byte;
/** byte_read stores the byte read on a read operation.
*/
    char byte_read;
/** byte_to_write stores the byte to write on a write operation.
*/
    char byte_to_write;
/** page_to_write stores the byte page (64 bytes) to write on a page write operation.
*/
    char page_to_write[EEPROM_PAGE_SIZE];
}TEEPROMCtrl;

int setup_eeprom(TEEPROMCtrl * ptEEPROM);
int close_eeprom(TEEPROMCtrl * ptEEPROM);
int read_byte_eeprom(TEEPROMCtrl * ptEEPROM);
int write_byte_eeprom(TEEPROMCtrl * ptEEPROM);
int write_page_eeprom(TEEPROMCtrl * ptEEPROM);
#endif