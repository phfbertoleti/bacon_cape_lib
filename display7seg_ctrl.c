/* Modulo para controle do display 7 segmentos (bacon cape) */
/* Link para datasheet: http://www.ti.com/lit/ds/symlink/sn74hc595.pdf */
/* Estre modulo foi altamente inspirado no codigo do controle SPI do  */ 
/* Cap. 8 do livro "Exploring BeagleBone: Tools and Techniques for Building */
/* with Embedded Linux", de Derek Molloy  */
/* 
*    IMPORTANTE:
* Para o display funcionar, deve-se desabilidar a I2C-1!  
*/
#include "display7seg_ctrl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <linux/spi/spidev.h>

#define SPI_PATH             "/dev/spidev1.0" 
#define SPI_SPEED            1000000   /* 1MHz de frequencia do barramento */
#define SPI_BITS_PER_WORD    8         /* palavra de 8 bits */
#define SPI_MODE             3         /* SPI - Mode 3 */

#define ENABLE_DEBUG_MESSAGES_DISP7SEG

#ifdef ENABLE_DEBUG_MESSAGES_DISP7SEG
   #define DEBUG_DISP7SEG(...) printf(__VA_ARGS__)
#else
   #define DEBUG_DISP7SEG
#endif

/* Constantes */ 
//                                        Pgfedcba        
const unsigned char alfanum_map[16] = { 0b01000000,   //0
	                                    0b01111001,   //1
                                        0b00100100,   //2
		                                0b00110000,   //3		                                
		                                0b00011001,   //4		                                
		                                0b00010010,   //5
		                                0b00000010,   //6
		                                0b01111000,   //7
		                                0b00000000,   //8
		                                0b00010000,   //9
		                                0b00001000,   //A
		                                0b00000011,   //b
		                                0b01000110,   //C
		                                0b00100001,   //d
		                                0b00000110,   //e
		                                0b00001110    //F
		                                };

int ret_min_display7seg(int * SysReqs, int length)
{
    int	i;
    int min = SysReqs[0];

    for(i=1; i<length; i++)
    {
         if (SysReqs[i] < min)
         	min = SysReqs[i];
    }

    return min;
}

int spi_transfer(TDISP7SEGCtrl * ptDISP7SEG, unsigned char send[], unsigned char receive[], int length)
{
    struct spi_ioc_transfer transfer_spi;
    int status;

    /* Popula estrutura de spi */
    transfer_spi.tx_buf = (unsigned long) send;
    transfer_spi.rx_buf = (unsigned long) receive; // the buffer for receiving data
    transfer_spi.len = length;
    transfer_spi.speed_hz = SPI_SPEED;
    transfer_spi.bits_per_word = SPI_BITS_PER_WORD;
    transfer_spi.delay_usecs = 0;

    /* Envia o as infromacoes para o device SPI */
    status = ioctl(ptDISP7SEG->spi_filedescriptor, SPI_IOC_MESSAGE(1), &transfer_spi);
    return status;
}

int setup_display7seg(TDISP7SEGCtrl * ptDISP7SEG)
{
	int sys_req[50];
	char cmd_gpio[100] = {0};
	int ret;
	int idx_sys_req = 0;
	uint32_t speed = SPI_SPEED;
    uint8_t bits = SPI_BITS_PER_WORD;  
    uint8_t mode = SPI_MODE;
	
    /* Se o display ja foi configurado, nao ha nada a fazer */
    if (ptDISP7SEG->display7seg_config != 0)
    	return DISP7SEG_CTRL_ERROR;
    
    sys_req[idx_sys_req] = system("sudo sh -c \"echo BB-SPIDEV0 > /sys/devices/platform/bone_capemgr/slots\"");
    idx_sys_req++;

    /* Configura GPIO do CLear e garante que este esteja em nivel alto */
    sprintf(cmd_gpio, "echo %d > /sys/class/gpio/export", ptDISP7SEG->gpio_num_clear);
    sys_req[idx_sys_req] = system(cmd_gpio);
    idx_sys_req++;

    sprintf(cmd_gpio, "echo out > /sys/class/gpio/gpio%d/direction", ptDISP7SEG->gpio_num_clear);
    sys_req[idx_sys_req] = system(cmd_gpio);
    idx_sys_req++;
   
    sprintf(cmd_gpio, "echo 1 > /sys/class/gpio/gpio%d/value", ptDISP7SEG->gpio_num_clear);
    sys_req[idx_sys_req] = system(cmd_gpio);
    idx_sys_req++;

    /* ABre cominucacao com a SPI e solicita acesso ao barramento */
    if ((ptDISP7SEG->spi_filedescriptor = open(SPI_PATH, O_RDWR))<0)
    {
        sys_req[idx_sys_req] = -1;
        idx_sys_req++;
    }
    else 
    {
        sys_req[idx_sys_req] = ioctl(ptDISP7SEG->spi_filedescriptor, SPI_IOC_WR_MODE, &mode);
        idx_sys_req++;
    
        sys_req[idx_sys_req] = ioctl(ptDISP7SEG->spi_filedescriptor, SPI_IOC_RD_MODE, &mode);
        idx_sys_req++;

        sys_req[idx_sys_req] = ioctl(ptDISP7SEG->spi_filedescriptor, SPI_IOC_WR_BITS_PER_WORD, &bits);
        idx_sys_req++;

        sys_req[idx_sys_req] = ioctl(ptDISP7SEG->spi_filedescriptor, SPI_IOC_RD_BITS_PER_WORD, &bits);
        idx_sys_req++;

        sys_req[idx_sys_req] = ioctl(ptDISP7SEG->spi_filedescriptor, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
        idx_sys_req++;

        sys_req[idx_sys_req] = ioctl(ptDISP7SEG->spi_filedescriptor, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
        idx_sys_req++;
    }

    ret = ret_min_display7seg(sys_req,idx_sys_req);

    if (ret > -1)
    	ptDISP7SEG->display7seg_config = 1;

    return ret;
}

int close_display7seg(TDISP7SEGCtrl * ptDISP7SEG)
{
    return close(ptDISP7SEG->spi_filedescriptor);
}

int clear_display7seg(TDISP7SEGCtrl * ptDISP7SEG)
{
    int sys_req;
    unsigned char byte_clear = 0xFE;

    /* Se o display ainda nao foi configurado, nao ha nada a fazer */
    if (ptDISP7SEG->display7seg_config == 0)
        return DISP7SEG_CTRL_ERROR;

    sys_req = spi_transfer(ptDISP7SEG, &byte_clear, 0, 1);
    fflush(stdout);
    return sys_req;
}

int light_display7seg(TDISP7SEGCtrl * ptDISP7SEG)
{
    int sys_req;
    unsigned char byte_light = 0x00;

    /* Se o display ainda nao foi configurado, nao ha nada a fazer */
    if (ptDISP7SEG->display7seg_config == 0)
    	return DISP7SEG_CTRL_ERROR;

    sys_req = spi_transfer(ptDISP7SEG, &byte_light, 0, 1);
    fflush(stdout);
    return sys_req;
}

int write_alfanum_display7seg(TDISP7SEGCtrl * ptDISP7SEG)
{
     unsigned char byte_to_write;
     int idx_alfanum_map;
     int sys_req;

     /* Se os GPIOs do display ainda nao foram configurados, nao ha nada a ser feito */
    if (ptDISP7SEG->display7seg_config == 0)
    	return DISP7SEG_CTRL_ERROR;

    /* Se o caracter alfanumerico esta fora do range (0x00 - 0x0F), nao ha nada a ser feito */
    if (ptDISP7SEG->alfanum_to_write < 0x10)
    {
        idx_alfanum_map = (int)ptDISP7SEG->alfanum_to_write;
        byte_to_write = alfanum_map[idx_alfanum_map];
    }
    else
       return DISP7SEG_CTRL_ERROR;
   
    /* seta ou reseta ponto decimal, de acordo com a escolha em ptDISP7SEG->write_point */
    byte_to_write = byte_to_write|(ptDISP7SEG->write_point & 0x01);
   
    sys_req = spi_transfer(ptDISP7SEG, &byte_to_write, 0, 1);
    fflush(stdout);    
    return sys_req;
}