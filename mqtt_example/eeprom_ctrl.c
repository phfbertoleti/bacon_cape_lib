/* EEPROM control module  (bacon cape) */
/*
 * Author: Pedro Bertoleti
 */
/* Link for EEPROM datasheet: http://learning.media.mit.edu/projects/gogo/parts_pdf/EEPROM%20-%2024LC256.pdf */
/*
*    IMPORTANT:
* In order to make EEPROM writing data possible, short-circuit C2 capacitor is needed.
* Without this "hardware hack", only EEPROM reading can be done.
*/
#include "eeprom_ctrl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <errno.h>

#define EEPROM_FILE         "/dev/i2c-2"
#define WAIT_TIME_EEPROM    100000  /* 100000us = 100ms */

int setup_eeprom(TEEPROMCtrl * ptEEPROM)
{
    int ret;

    /* If EEPROM has been already configured, there's nothing to do here */
    if (ptEEPROM->eeprom_config != 0)
        return EEPROM_CTRL_ERROR;

    /* Check EEPROM access (considering read and write operations) */
    if ((ptEEPROM->i2c_descriptor = open(EEPROM_FILE, O_RDWR)) < 0)
    {
        ptEEPROM->eeprom_config = 0;
        ret = -1;
    }
    else
    {
        ptEEPROM->eeprom_addr = EEPROM_ADDR;
        ret = 0;

        /* Try to obtain I2C bus access */
        if (ioctl(ptEEPROM->i2c_descriptor, I2C_SLAVE, ptEEPROM->eeprom_addr) < 0)
            ret = -1;
        else
            ptEEPROM->eeprom_config = 1;
    }

    return ret;
}

int close_eeprom(TEEPROMCtrl * ptEEPROM)
{
    return close(ptEEPROM->i2c_descriptor);
}

int read_byte_eeprom(TEEPROMCtrl * ptEEPROM)
{
    int ret = 0;
    char buffer_read[10]={0};

    /* If EEPROM hasn't been configured yet, there's nothing to do here */
    if (ptEEPROM->eeprom_config == 0)
        return EEPROM_CTRL_ERROR;

    /* Prepare buffer for reading operation */
    buffer_read[0] = ptEEPROM->high_addr_byte;
    buffer_read[1] = ptEEPROM->low_addr_byte;

    usleep(WAIT_TIME_EEPROM);

    if (write(ptEEPROM->i2c_descriptor, buffer_read, 2) != 2)
        ret = -1;

    /* Wait a little time for receiving EEPROM requested data */
    usleep(WAIT_TIME_EEPROM);

    /* Read requested data */
    if (ret == 0)
    {
        if (read(ptEEPROM->i2c_descriptor, buffer_read, 1) != 1)
            ret = -1;
    }

    ptEEPROM->byte_read = buffer_read[0];
    return ret;
}

int write_byte_eeprom(TEEPROMCtrl * ptEEPROM)
{
    int ret = 0;
    int bytes_written = 0;
    char buffer_write[3]={0};

    /* If EEPROM hasn't been configured yet, there's nothing to do here */
    if (ptEEPROM->eeprom_config == 0)
        return EEPROM_CTRL_ERROR;

    /* Prepare buffer for writing operation  */
    buffer_write[0] = ptEEPROM->high_addr_byte;
    buffer_write[1] = ptEEPROM->low_addr_byte;
    buffer_write[2] = ptEEPROM->byte_to_write;

    if (ret == 0)
    {
        bytes_written = write(ptEEPROM->i2c_descriptor, buffer_write, 3);
        if (bytes_written != 3)
            ret = -1;
    }

    return ret;
}

int write_page_eeprom(TEEPROMCtrl * ptEEPROM)
{
    int ret = 0;
    int bytes_written = 0;
    char buffer_write[2+EEPROM_PAGE_SIZE]={0};

    /* If EEPROM hasn't been configured yet, there's nothing to do here */
    if (ptEEPROM->eeprom_config == 0)
        return EEPROM_CTRL_ERROR;

    /* Prepare buffer for writing operation  */
    buffer_write[0] = ptEEPROM->high_addr_byte;
    buffer_write[1] = ptEEPROM->low_addr_byte;
    memcpy(buffer_write+2, ptEEPROM->page_to_write, EEPROM_PAGE_SIZE);

    if (ret == 0)
    {
        bytes_written = write(ptEEPROM->i2c_descriptor, buffer_write, 2+EEPROM_PAGE_SIZE);
        if (bytes_written != (2+EEPROM_PAGE_SIZE))
            ret = -1;
    }

    return ret;
}