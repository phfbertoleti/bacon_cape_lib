/** @brief EEPROM control module (bacon cape)\n
    Link for EEPROM datasheet: http://learning.media.mit.edu/projects/gogo/parts_pdf/EEPROM%20-%2024LC256.pdf\n\n
    IMPORTANT:\n
    In order to make EEPROM writing data possible, short-circuit C2 capacitor is needed.\n
    Without this "hardware hack", only EEPROM reading can be done.
    @author Pedro Bertoleti
    @date September, 2018
*/
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
#include "eeprom_ctrl.h"

/** @addtogroup EEPROM_control
@{
*/

/* General defines */
#define EEPROM_FILE         "/dev/i2c-2"
#define WAIT_TIME_EEPROM    100000  /* 100000us = 100ms */

/**Setup EEPROM\n
        This function initializes EEPROM communication (I2C). This function must be called before any other EEPROM function.

        @param[in] ptEEPROM - pointer to EEPROM structure variable
        @return Success or fail (EEPROM_CTRL_SUCCESS or EEPROM_CTRL_ERROR)
*/
int setup_eeprom(TEEPROMCtrl * ptEEPROM)
{
    int ret = EEPROM_CTRL_SUCCESS;

    /* If EEPROM has been already configured, there's nothing to do here */
    if (ptEEPROM->eeprom_config != 0)
        return EEPROM_CTRL_ERROR;

    /* Check EEPROM access (considering read and write operations) */
    if ((ptEEPROM->i2c_descriptor = open(EEPROM_FILE, O_RDWR)) < 0)
    {
        ptEEPROM->eeprom_config = 0;
        ret = EEPROM_CTRL_ERROR;
    }
    else
    {
        ptEEPROM->eeprom_addr = EEPROM_ADDR;
        ret = EEPROM_CTRL_SUCCESS;

        /* Try to obtain I2C bus access */
        if (ioctl(ptEEPROM->i2c_descriptor, I2C_SLAVE, ptEEPROM->eeprom_addr) < 0)
            ret = EEPROM_CTRL_ERROR;
        else
            ptEEPROM->eeprom_config = 1;
    }

    return ret;
}

/**Close EEPROM communication\n
        This function closes EEPROM communication. This function must be called when EEPROM usage ends.

        @param[in] ptEEPROM - pointer to EEPROM structure variable
        @return Success (0) or fail (!=0)
*/
int close_eeprom(TEEPROMCtrl * ptEEPROM)
{
    return close(ptEEPROM->i2c_descriptor);
}

/**Read a single-byte from EEPROM\n
        This function reads a single-byte from EEPROM.\n
        The read byte will be stored in a variable from EEPROM structure variable

        @param[in] ptEEPROM - pointer to EEPROM structure variable
        @return Success or fail (EEPROM_CTRL_SUCCESS or EEPROM_CTRL_ERROR)
*/
int read_byte_eeprom(TEEPROMCtrl * ptEEPROM)
{
    int ret = EEPROM_CTRL_SUCCESS;
    char buffer_read[10]={0};

    /* If EEPROM hasn't been configured yet, there's nothing to do here */
    if (ptEEPROM->eeprom_config == 0)
        return EEPROM_CTRL_ERROR;

    /* Prepare buffer for reading operation */
    buffer_read[0] = ptEEPROM->high_addr_byte;
    buffer_read[1] = ptEEPROM->low_addr_byte;

    usleep(WAIT_TIME_EEPROM);

    if (write(ptEEPROM->i2c_descriptor, buffer_read, 2) != 2)
        ret = EEPROM_CTRL_ERROR;

    /* Wait a little time for receiving EEPROM requested data */
    usleep(WAIT_TIME_EEPROM);

    /* Read requested data */
    if (ret == EEPROM_CTRL_SUCCESS)
    {
        if (read(ptEEPROM->i2c_descriptor, buffer_read, 1) != 1)
            ret = EEPROM_CTRL_ERROR;
    }

    ptEEPROM->byte_read = buffer_read[0];
    return ret;
}

/**Write a single-byte to EEPROM\n
        This function writes a single-byte to EEPROM.\n
        The byte to write is in the EEPROM structure variable

        @param[in] ptEEPROM - pointer to EEPROM structure variable
        @return Success or fail (EEPROM_CTRL_SUCCESS or EEPROM_CTRL_ERROR)
*/
int write_byte_eeprom(TEEPROMCtrl * ptEEPROM)
{
    int ret = EEPROM_CTRL_SUCCESS;
    int bytes_written = 0;
    char buffer_write[3]={0};

    /* If EEPROM hasn't been configured yet, there's nothing to do here */
    if (ptEEPROM->eeprom_config == 0)
        return EEPROM_CTRL_ERROR;

    /* Prepare buffer for writing operation  */
    buffer_write[0] = ptEEPROM->high_addr_byte;
    buffer_write[1] = ptEEPROM->low_addr_byte;
    buffer_write[2] = ptEEPROM->byte_to_write;

    if (ret == EEPROM_CTRL_SUCCESS)
    {
        bytes_written = write(ptEEPROM->i2c_descriptor, buffer_write, 3);
        if (bytes_written != 3)
            ret = EEPROM_CTRL_ERROR;
    }

    return ret;
}

/**Write a page to EEPROM\n
        This function writes a page (64 bytes) to EEPROM.\n
        The page to write is in the EEPROM structure variable

        @param[in] ptEEPROM - pointer to EEPROM structure variable
        @return Success or fail (EEPROM_CTRL_SUCCESS or EEPROM_CTRL_ERROR)
*/
int write_page_eeprom(TEEPROMCtrl * ptEEPROM)
{
    int ret = EEPROM_CTRL_SUCCESS;
    int bytes_written = 0;
    char buffer_write[2+EEPROM_PAGE_SIZE]={0};

    /* If EEPROM hasn't been configured yet, there's nothing to do here */
    if (ptEEPROM->eeprom_config == 0)
        return EEPROM_CTRL_ERROR;

    /* Prepare buffer for writing operation  */
    buffer_write[0] = ptEEPROM->high_addr_byte;
    buffer_write[1] = ptEEPROM->low_addr_byte;
    memcpy(buffer_write+2, ptEEPROM->page_to_write, EEPROM_PAGE_SIZE);

    if (ret == EEPROM_CTRL_SUCCESS)
    {
        bytes_written = write(ptEEPROM->i2c_descriptor, buffer_write, 2+EEPROM_PAGE_SIZE);
        if (bytes_written != (2+EEPROM_PAGE_SIZE))
            ret = EEPROM_CTRL_ERROR;
    }

    return ret;
}
/** @} */