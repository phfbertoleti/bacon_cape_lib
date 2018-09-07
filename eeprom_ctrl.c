/* Modulo para controle da EEPROM (bacon cape) */
/* Documentação da EEPROM: http://learning.media.mit.edu/projects/gogo/parts_pdf/EEPROM%20-%2024LC256.pdf */
/*
*    IMPORTANTE:
* Para a escrita na EEPROm funcionar, eh preciso que o capacitor C2 seja curto-
* circuitado. Sem fazer isso, somente a leitura será possivel.
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

#define ENABLE_DEBUG_MESSAGES_EEPROM

#ifdef ENABLE_DEBUG_MESSAGES_EEPROM
   #define DEBUG_EEPROM(...) printf(__VA_ARGS__)
#else
   #define DEBUG_EEPROM
#endif

int setup_eeprom(TEEPROMCtrl * ptEEPROM)
{
    int ret;

    /* Se a EEPROM ja foi configurada/checada, nao ha nada a ser feito */
    if (ptEEPROM->eeprom_config != 0)
        return EEPROM_CTRL_ERROR;

    /* Verifica se o arquivo de acesso a EEPROM existe*/
    if ((ptEEPROM->i2c_descriptor = open(EEPROM_FILE, O_RDWR)) < 0)
    {
        /* Arquivo nao existe */
        DEBUG_EEPROM("\n\r[EEPROM] arquivo %s nao existe\n",EEPROM_FILE);
        ptEEPROM->eeprom_config = 0;
        ret = -1;
    }
    else
    {
        ptEEPROM->eeprom_addr = EEPROM_ADDR;
        ret = 0;

        /* Tenta obter acesso ao barramento I2C */
        if (ioctl(ptEEPROM->i2c_descriptor, I2C_SLAVE, ptEEPROM->eeprom_addr) < 0)
        {
            DEBUG_EEPROM("\n\r[EEPROM] impossivel obter acesso ao barramento i2c. Errno: %d\n", errno);
            ret = -1;
        }
        else
            ptEEPROM->eeprom_config = 1;
    }

    return ret;
}

int close_eeprom(TEEPROMCtrl * ptEEPROM)
{
    close(ptEEPROM->i2c_descriptor);
    DEBUG_EEPROM("\n\r[EEPROM] Fechada a comunicação com a eeprom.\n");
    return 0;
}

int read_byte_eeprom(TEEPROMCtrl * ptEEPROM)
{
    int ret = 0;
    char buffer_read[10]={0};

    /* Se e EEPROM ainda nao foi configurada, nao ha nada a ser feito */
    if (ptEEPROM->eeprom_config == 0)
        return EEPROM_CTRL_ERROR;

    /* Faz a escrita do endereço que deseja ser lido  */
    buffer_read[0] = ptEEPROM->high_addr_byte;
    buffer_read[1] = ptEEPROM->low_addr_byte;

    /* Aguarda recepcao dos dados da EEPROM */
    usleep(WAIT_TIME_EEPROM);

    if (write(ptEEPROM->i2c_descriptor, buffer_read, 2) != 2)
    {
        DEBUG_EEPROM("\n\r[EEPROM READ] Nao foi possivel escrever no bus i2c. Errno: %d\n", errno);
        ret = -1;
    }

    /* Aguarda recepcao dos dados da EEPROM */
    usleep(WAIT_TIME_EEPROM);

    /* Faz a leitura do dado final */
    if (ret == 0)
    {
        if (read(ptEEPROM->i2c_descriptor, buffer_read, 1) != 1)
        {
            DEBUG_EEPROM("\n\r[EEPROM READ] Nao foi possivel ler byte do bus i2c. Errno: %d\n", errno);
            ret = -1;
        }
    }

    ptEEPROM->byte_read = buffer_read[0];
    return ret;
}

int write_byte_eeprom(TEEPROMCtrl * ptEEPROM)
{
    int ret = 0;
    int bytes_written = 0;
    char buffer_write[3]={0};

    /* Se e EEPROM ainda nao foi configurada, nao ha nada a ser feito */
    if (ptEEPROM->eeprom_config == 0)
        return EEPROM_CTRL_ERROR;

    /* Faz a escrita do endereço que deseja ser lido  */
    buffer_write[0] = ptEEPROM->high_addr_byte;
    buffer_write[1] = ptEEPROM->low_addr_byte;
    buffer_write[2] = ptEEPROM->byte_to_write;

    if (ret == 0)
    {
        bytes_written = write(ptEEPROM->i2c_descriptor, buffer_write, 3);
        if (bytes_written != 3)
        {
            DEBUG_EEPROM("\n\r[EEPROM WRITE] Nao foi possivel escrever no bus i2c. Errno: %d Bytes escritos: %d\n", errno, bytes_written);
            ret = -1;
        }
    }

    return ret;
}

int write_page_eeprom(TEEPROMCtrl * ptEEPROM)
{
    int ret = 0;
    int bytes_written = 0;
    char buffer_write[2+EEPROM_PAGE_SIZE]={0};

    /* Se e EEPROM ainda nao foi configurada, nao ha nada a ser feito */
    if (ptEEPROM->eeprom_config == 0)
        return EEPROM_CTRL_ERROR;

    /* Faz a escrita do endereço que deseja ser lido  */
    buffer_write[0] = ptEEPROM->high_addr_byte;
    buffer_write[1] = ptEEPROM->low_addr_byte;
    memcpy(buffer_write+2, ptEEPROM->page_to_write, EEPROM_PAGE_SIZE);

    if (ret == 0)
    {
        bytes_written = write(ptEEPROM->i2c_descriptor, buffer_write, 2+EEPROM_PAGE_SIZE);
        if (bytes_written != (2+EEPROM_PAGE_SIZE))
        {
            DEBUG_EEPROM("\n\r[EEPROM WRITE] Nao foi possivel escrever no bus i2c. Errno: %d Bytes escritos: %d\n", errno, bytes_written);
            ret = -1;
        }
    }

    return ret;
}
