/** @brief Accelerometer (MMA8452Q) control module  (bacon cape)\n
    Link to MMA8452Q datasheet: https://www.nxp.com/docs/en/data-sheet/MMA8452Q.pdf
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
#include "accelerometer_ctrl.h"
#include "general_functions.h"

/** @addtogroup Accelerometer_control
@{
*/

/* General / i2c defines */
#define ACCELEROMETER_FILE          "/dev/i2c-2"
#define WAIT_TIME_ACCELEROMETER     10000  /* 10000us = 10ms */
#define ACC_IDENTITY_VALUE          0x2A

/* Register addresses defines */
#define REG_ADDR_STATUS                0x00
#define REG_ADDR_OUT_X_MSB             0x01
#define REG_ADDR_OUT_X_LSB             0x02
#define REG_ADDR_OUT_Y_MSB             0x03
#define REG_ADDR_OUT_Y_LSB             0x04
#define REG_ADDR_OUT_Z_MSB             0x05
#define REG_ADDR_OUT_Z_LSB             0x06
#define REG_ADDR_SYSMOD                0x0B
#define REG_ADDR_INT_SOURCE            0x0C
#define REG_ADDR_WHO_AM_I              0x0D
#define RED_ADDR_XYZ_DATA_CFG          0x0E
#define RED_ADDR_CTRL_REG1             0x2A

/* Status Register defines */
#define REG_STATUS_MASK_X_ACC_AVAILABLE        0b00000001
#define REG_STATUS_MASK_Y_ACC_AVAILABLE        0b00000010
#define REG_STATUS_MASK_Z_ACC_AVAILABLE        0b00000100
#define REG_STATUS_MASK_XYZ_ACC_READY          0b00001000
#define REG_STATUS_MASK_X_ACC_OVERWRITE        0b00010000
#define REG_STATUS_MASK_Y_ACC_OVERWRITE        0b00100000
#define REG_STATUS_MASK_Z_ACC_OVERWRITE        0b01000000
#define REG_STATUS_MASK_XYZ_ACC_OVERWRITE      0b10000000

/* Interruption source Register defines */
#define REG_INT_SRC_MASK_DATA_READY            0b00000001
#define REG_INT_SRC_MASK_FREEFALL_MOTION       0b00000100
#define REG_INT_SRC_MASK_PULSE                 0b00001000
#define REG_INT_SRC_MASK_LANDSCAPE_PORTRAIT    0b00010000
#define REG_INT_SRC_MASK_TRANSIENT             0b00100000
#define REG_INT_SRC_MASK_AUTOSLEEP_WAKE        0b10000000

/* XYZ_DATA_CFG Register defines */
#define REG_XYZ_DATA_CFG_FS0            0b00000001
#define REG_XYZ_DATA_CFG_FS1            0b00000010
#define REG_XYZ_DATA_CFG_HPF            0b00010000

/** @brief Accelerometer status register (parsed register)
*/
/* Status register structure */
typedef struct {
    char x_new_data_available;  //bit 0
    char y_new_data_available;  //bit 1
    char z_new_data_available;  //bit 2
    char xyz_new_data_ready;    //bit 3
    char x_data_overwrite;      //bit 4
    char y_data_overwrite;      //bit 5
    char z_data_overwrite;      //bit 6
    char xyz_data_overwrite;    //bit 7
}TACCStatusReg;

/*
 *  Local prototypes
 */
void parse_status_register_value(TACCStatusReg * ptStatusReg, unsigned char reg_value);
void parse_int_src_register_value(TACCCtrl * ptACC, unsigned char reg_value);
void parse_xyz_data_cfg_register_value(TACCCtrl * ptACC, unsigned char reg_value);
int single_byte_read_register(TACCCtrl * ptACC, unsigned char register_to_read, unsigned char * ptRegValue);
int single_byte_write_register(TACCCtrl * ptACC, unsigned char register_to_write, unsigned char byte_to_write);
int set_stand_by_mode(TACCCtrl * ptACC);
int set_active_mode(TACCCtrl * ptACC);
int change_ODR(TACCCtrl * ptACC);

/**Parse information of Status register\n
        Parse information of Status register and stores parsed data in accelerometer structure variable

        @param[in] ptStatusReg - pointer to accelerometer structure variable
        @param[in] reg_value - Status Register value/byte
        @return None
*/
void parse_status_register_value(TACCStatusReg * ptStatusReg, unsigned char reg_value)
{
    ptStatusReg->x_new_data_available = reg_value & REG_STATUS_MASK_X_ACC_AVAILABLE;
    ptStatusReg->y_new_data_available = reg_value & REG_STATUS_MASK_Y_ACC_AVAILABLE;
    ptStatusReg->z_new_data_available = reg_value & REG_STATUS_MASK_Z_ACC_AVAILABLE;
    ptStatusReg->xyz_new_data_ready = reg_value & REG_STATUS_MASK_XYZ_ACC_READY;
    ptStatusReg->x_data_overwrite = reg_value & REG_STATUS_MASK_X_ACC_OVERWRITE;
    ptStatusReg->y_data_overwrite = reg_value & REG_STATUS_MASK_Y_ACC_OVERWRITE;
    ptStatusReg->z_data_overwrite = reg_value & REG_STATUS_MASK_Z_ACC_OVERWRITE;
    ptStatusReg->xyz_data_overwrite = reg_value & REG_STATUS_MASK_XYZ_ACC_OVERWRITE;
}

/**Parse information of Interruption Source register\n
        Parse information of Interruption Source register and stores parsed data in accelerometer structure variable

        @param[in] ptACC - pointer to accelerometer structure variable
        @param[in] reg_value - Interruption Source register value/byte
        @return None
*/
void parse_int_src_register_value(TACCCtrl * ptACC, unsigned char reg_value)
{
    ptACC->ACCIntSrcReg.data_ready = reg_value & REG_INT_SRC_MASK_DATA_READY;
    ptACC->ACCIntSrcReg.freefall_motion = reg_value & REG_INT_SRC_MASK_FREEFALL_MOTION;
    ptACC->ACCIntSrcReg.pulse = reg_value & REG_INT_SRC_MASK_PULSE;
    ptACC->ACCIntSrcReg.landscape_portrait = reg_value & REG_INT_SRC_MASK_LANDSCAPE_PORTRAIT;
    ptACC->ACCIntSrcReg.transient = reg_value & REG_INT_SRC_MASK_TRANSIENT;
    ptACC->ACCIntSrcReg.autosleep_wake = reg_value & REG_INT_SRC_MASK_AUTOSLEEP_WAKE;
}

/**Parse information of XYZ Data CFG Source register\n
        Parse information of XYZ Data CFG Source register and stores parsed data in accelerometer structure variable

        @param[in] ptACC - pointer to accelerometer structure variable
        @param[in] reg_value - XYZ Data CFG Source register value/byte
        @return None
*/
void parse_xyz_data_cfg_register_value(TACCCtrl * ptACC, unsigned char reg_value)
{
    char fs0_value;
    char fs1_value;
    char fs_value;

    ptACC->ACCDataCfgReg.HPF_status = reg_value & REG_XYZ_DATA_CFG_HPF;

    fs0_value = reg_value & REG_XYZ_DATA_CFG_FS0;
    fs1_value = reg_value & REG_XYZ_DATA_CFG_FS1;
    fs_value = (fs1_value << 8) | fs0_value;
    ptACC->ACCDataCfgReg.AccSensibility = fs_value;
}

/**Read accelerometer identity\n
        Read accelerometer identity to confirm the destination device is a MMA8452Q device. This identity value shoul be 0x2A

        @param[in] ptACC - pointer to accelerometer structure variable
        @return Success or fail (ACC_CTRL_SUCCESS or ACC_CTRL_ERROR)
*/
int read_accelerometer_identity(TACCCtrl * ptACC)
{
    unsigned char reg_who_am_I_value = 0x00;
    int ret = ACC_CTRL_SUCCESS;

   /*
    * Sequence:
    * - If Accelerometer hasn't been configured yet, the function ends rightaway. Otherwise, it can proceed
    * - Read and parse Status register
    * - Parsed Status register is written in accelerometer structure variable.
    */

   if (ptACC->accelerometer_config == 0)
   {
       printf("\r\nPonto 1\n");
       return ACC_CTRL_ERROR;
   }

   ret = set_stand_by_mode(ptACC);

   if (ret == ACC_CTRL_SUCCESS)
      ret = single_byte_read_register(ptACC, REG_ADDR_WHO_AM_I, &reg_who_am_I_value);
   else 
       printf("\r\nPonto 2\n");

   if (ret == ACC_CTRL_SUCCESS)
   {
       if (reg_who_am_I_value != ACC_IDENTITY_VALUE)
           ret = ACC_CTRL_ERROR;
   }
   else
       printf("\r\nPonto 3\n");

   if (ret == ACC_CTRL_SUCCESS)
      ret = set_active_mode(ptACC);
   else
       printf("\r\nPonto 4  reg_who_am_I_value = %02X \n", reg_who_am_I_value);

   return ret;
}

/**Setup accelerometer\n
        This function initializes accelerometer communication (I2C). This function must be called before any other accelerometer function.

        @param[in] ptACC - pointer to accelerometer structure variable
        @return Success or fail (ACC_CTRL_SUCCESS or ACC_CTRL_ERROR)
*/
int setup_accelerometer(TACCCtrl * ptACC)
{
    int ret = ACC_CTRL_SUCCESS;

    /* If accelerometer has been already configured, there's nothing to do here */
    if (ptACC->accelerometer_config != 0)
        return ACC_CTRL_ERROR;

    /* Check accelerometer access (considering read and write operations) */
    if ((ptACC->i2c_descriptor = open(ACCELEROMETER_FILE, O_RDWR)) < 0)
    {
        ptACC->accelerometer_config = 0;
        ret = ACC_CTRL_ERROR;
    }
    else
    {
        ptACC->accelerometer_addr = ACCELEROMETER_ADDR;
        ret = ACC_CTRL_SUCCESS;

        /* Try to obtain I2C bus access */
        if (ioctl(ptACC->i2c_descriptor, I2C_SLAVE, ptACC->accelerometer_addr) < 0)
            ret = ACC_CTRL_ERROR;
        else
        {
            /* So far, so good */
            ptACC->accelerometer_config = 1;

            /* Change output data rate */
            if (ret == ACC_CTRL_SUCCESS)
                ret = read_accelerometer_identity(ptACC);

            /* Change output data rate */
            if (ret == ACC_CTRL_SUCCESS)
                ret = change_ODR(ptACC);

            /* Change sensibility and HPF */
            if (ret == ACC_CTRL_SUCCESS)
                ret = write_xyz_data_cfg(ptACC);

            /* Set config flag and init raw accelerations variables */
            if (ret == ACC_CTRL_SUCCESS)
                ptACC->accelerometer_config = 1;

            ptACC->accelerometer_x_raw = 0;
            ptACC->accelerometer_y_raw = 0;
            ptACC->accelerometer_z_raw = 0;
            ptACC->accelerations_ready = AccStatus_not_ready;

            /* Set accelerometer active mode (to accelerometer starts sampling) */
            if (ret == ACC_CTRL_SUCCESS)
                ret = set_active_mode(ptACC);
        }
    }

    return ret;
}

/**Close accelerometer communication\n
        This function closes accelerometer communication. This function must be called when accelerometer usage ends.

        @param[in] ptACC - pointer to accelerometer structure variable
        @return Success (0) or fail (!=0)
*/
int close_accelerometer(TACCCtrl * ptACC)
{
    ptACC->accelerometer_config = 0;
    return close(ptACC->i2c_descriptor);
}

/**Single-byte register read\n
        This function reads the value of a register (single-byte value)

        @param[in] ptACC - pointer to accelerometer structure variable
        @param[in] register_to_read - informs which register must be read
        @param[out] ptRegValue - pointer to a variablue that will store register value
        @return Success or fail (ACC_CTRL_SUCCESS or ACC_CTRL_ERROR)
*/
int single_byte_read_register(TACCCtrl * ptACC, unsigned char register_to_read, unsigned char * ptRegValue)
{
    int ret = ACC_CTRL_SUCCESS;
    unsigned char reg_value = 0x00;
    unsigned char reg_to_read = register_to_read;

    if (write(ptACC->i2c_descriptor, &reg_to_read, 1) != 1)
        ret = ACC_CTRL_ERROR;

    /* Wait a little time for receiving accelerometer requested data */
    usleep(WAIT_TIME_ACCELEROMETER);

    /* Read requested data */
    if (ret == 0)
    {
        if (read(ptACC->i2c_descriptor, &reg_value, 1) != 1)
            ret = ACC_CTRL_ERROR;

        *ptRegValue = reg_value;
    }

    return ret;
}

/**Single-byte register write\n
        This function writes a value (byte) to a register (single-byte value)

        @param[in] ptACC - pointer to accelerometer structure variable
        @param[in] register_to_write - informs which register must be written
        @param[in] byte_to_write - value to be write in register
        @return Success or fail (ACC_CTRL_SUCCESS or ACC_CTRL_ERROR)
*/
int single_byte_write_register(TACCCtrl * ptACC, unsigned char register_to_write, unsigned char byte_to_write)
{
    int ret = ACC_CTRL_SUCCESS;
    int bytes_written = 0;
    unsigned char buffer_write[2]={0};

    /* Prepare buffer for writing operation  */
    buffer_write[0] = register_to_write;
    buffer_write[1] = byte_to_write;

    usleep(WAIT_TIME_ACCELEROMETER);

    bytes_written = write(ptACC->i2c_descriptor, buffer_write, 2);

    if (bytes_written != 2)
        ret = ACC_CTRL_ERROR;

    return ret;
}

/**Read raw accelerations (X, Y and Z axis)\n
        This function reads raw accelerations (X, Y and Z axis) and stores these information in accelerometer structure variable.\n
        It only return success when there's acceleration to be read from MMA8452Q device.

        @param[in] ptACC - pointer to accelerometer structure variable
        @return Success or fail (ACC_CTRL_SUCCESS or ACC_CTRL_ERROR)
*/
int read_raw_accelerations_x_y_z(TACCCtrl * ptACC)
{
   TACCStatusReg ACCStatusReg;
   unsigned char reg_status_value = 0x00;
   unsigned char reg_raw_x_msb_value = 0x00;
   unsigned char reg_raw_x_lsb_value = 0x00;
   unsigned char reg_raw_y_msb_value = 0x00;
   unsigned char reg_raw_y_lsb_value = 0x00;
   unsigned char reg_raw_z_msb_value = 0x00;
   unsigned char reg_raw_z_lsb_value = 0x00;
   int ret = ACC_CTRL_SUCCESS;
   int ret_raw_accelerations[6] = {0};

    /*
    * Sequence:
    * - If Accelerometer hasn't been configured yet, the function ends rightaway. Otherwise, it can proceed
    * - Read and parse Status register in order to know if there're ready accelerations for x, y and z axis
    * - Once there're ready raw accelerations for x, y and z axis, they're read one by one and written in
    *   accelerometer structure variable.
    */

    ptACC->accelerations_ready = AccStatus_not_ready;

    if (ptACC->accelerometer_config == 0)
        return ACC_CTRL_ERROR;

    ret = set_stand_by_mode(ptACC);

    if (ret == ACC_CTRL_SUCCESS)
    {
        ret = single_byte_read_register(ptACC, REG_ADDR_STATUS, &reg_status_value);
        parse_status_register_value(&ACCStatusReg, reg_status_value);
    }

    if ((ret == ACC_CTRL_SUCCESS) && (ACCStatusReg.xyz_new_data_ready))
    {
        ret_raw_accelerations[0] = single_byte_read_register(ptACC, REG_ADDR_OUT_X_MSB, &reg_raw_x_msb_value);
        ret_raw_accelerations[1] = single_byte_read_register(ptACC, REG_ADDR_OUT_X_LSB, &reg_raw_x_lsb_value);
        ret_raw_accelerations[2] = single_byte_read_register(ptACC, REG_ADDR_OUT_Y_MSB, &reg_raw_y_msb_value);
        ret_raw_accelerations[3] = single_byte_read_register(ptACC, REG_ADDR_OUT_Y_LSB, &reg_raw_y_lsb_value);
        ret_raw_accelerations[4] = single_byte_read_register(ptACC, REG_ADDR_OUT_Z_MSB, &reg_raw_z_msb_value);
        ret_raw_accelerations[5] = single_byte_read_register(ptACC, REG_ADDR_OUT_Z_LSB, &reg_raw_z_lsb_value);

        if (ret_min(ret_raw_accelerations,6) != ACC_CTRL_ERROR)
        {
            ptACC->accelerometer_x_raw = (reg_raw_x_msb_value << 8) | reg_raw_x_lsb_value;
            ptACC->accelerometer_y_raw = (reg_raw_y_msb_value << 8) | reg_raw_y_lsb_value;
            ptACC->accelerometer_z_raw = (reg_raw_z_msb_value << 8) | reg_raw_z_lsb_value;
            ptACC->accelerations_ready = AccStatus_ready;
            ret = ACC_CTRL_SUCCESS;
        }
        else
            ret = ACC_CTRL_ERROR;
    }
    else
        ret = ACC_CTRL_ERROR;

    if (ret == ACC_CTRL_SUCCESS)
        ret = set_active_mode(ptACC);

    return ret;
}

/**Read accelerometer mode\n
        This function reads the current accelermeter operation mode.

        @param[in] ptACC - pointer to accelerometer structure variable
        @return Success or fail (ACC_CTRL_SUCCESS or ACC_CTRL_ERROR)
*/
int read_accelerometer_mode(TACCCtrl * ptACC)
{
    int ret = ACC_CTRL_SUCCESS;
    unsigned char reg_mode_value = (char)AccMode_Invalid;

   /*
    * Sequence:
    * - If Accelerometer hasn't been configured yet, the function ends rightaway. Otherwise, it can proceed
    * - Check SysMod register in order to get accelerometer current mode
    * - Fill ptACC->accelerometer_mode with accelerometer current mode
    */
    ptACC->accelerometer_mode = AccMode_Invalid;
    if (ptACC->accelerometer_config == 0)
        return ACC_CTRL_ERROR;

    ret = set_stand_by_mode(ptACC);

    if (ret == ACC_CTRL_SUCCESS)
        ret = single_byte_read_register(ptACC, REG_ADDR_SYSMOD, &reg_mode_value);

    if (ret == ACC_CTRL_SUCCESS)
        ptACC->accelerometer_mode = reg_mode_value;

    ret = set_active_mode(ptACC);

    return ret;
}

/**Read interruption source register\n
        This function reads the interruption source register, parse its value and store parsed data to accelerometer structure variable.

        @param[in] ptAcc - pointer to accelerometer structure variable
        @return Success or fail (ACC_CTRL_SUCCESS or ACC_CTRL_ERROR)
*/
int read_int_source(TACCCtrl * ptAcc)
{
    unsigned char reg_int_source_value = 0x00;
    int ret = ACC_CTRL_SUCCESS;

   /*
    * Sequence:
    * - If Accelerometer hasn't been configured yet, the function ends rightaway. Otherwise, it can proceed
    * - Read and parse Status register
    * - Parsed Status register is written in accelerometer structure variable.
    */

    if (ptAcc->accelerometer_config == 0)
        return ACC_CTRL_ERROR;

    ret = set_stand_by_mode(ptAcc);

    if (ret == ACC_CTRL_SUCCESS)
    {
        ret = single_byte_read_register(ptAcc, REG_ADDR_INT_SOURCE, &reg_int_source_value);
        parse_int_src_register_value(ptAcc,reg_int_source_value);
    }

    if (ret == ACC_CTRL_SUCCESS)
        ret = set_active_mode(ptAcc);

    return ret;
}

/**Read xyz data cfg register\n
        This function reads the xyz data cfg register, parse its value and store parsed data to accelerometer structure variable.

        @param[in] ptAcc - pointer to accelerometer structure variable
        @return Success or fail (ACC_CTRL_SUCCESS or ACC_CTRL_ERROR)
*/
int read_xyz_data_cfg(TACCCtrl * ptAcc)
{
    unsigned char reg_xyz_data_cfg_value = 0x00;
    int ret = ACC_CTRL_SUCCESS;

   /*
    * Sequence:
    * - If Accelerometer hasn't been configured yet, the function ends rightaway. Otherwise, it can proceed
    * - Read and parse XYZ Data CFG register
    * - Parsed XYZ Data CFG register is written in accelerometer structure variable.
    */

    if (ptAcc->accelerometer_config == 0)
        return ACC_CTRL_ERROR;

    ret = set_stand_by_mode(ptAcc);

    if (ret == ACC_CTRL_SUCCESS)
    {
        ret = single_byte_read_register(ptAcc, RED_ADDR_XYZ_DATA_CFG, &reg_xyz_data_cfg_value);
        parse_xyz_data_cfg_register_value(ptAcc,reg_xyz_data_cfg_value);
    }

    ret = set_active_mode(ptAcc);

    return ret;
}

/**Write xyz data cfg register\n
        This function writes to the xyz data cfg register.\n
        The data to write (byte) is composed based on a parsed xyz data cfg register structure

        @param[in] ptAcc - pointer to accelerometer structure variable
        @return Success or fail (ACC_CTRL_SUCCESS or ACC_CTRL_ERROR)
*/
int write_xyz_data_cfg(TACCCtrl * ptAcc)
{
    int ret = ACC_CTRL_SUCCESS;
    unsigned char reg_xyz_data_cfg_value = 0x00;

   /*
    * Sequence:
    * - If Accelerometer hasn't been configured yet, the function ends rightaway. Otherwise, it can proceed
    * - Format XYZ Data CFG register
    * - Write XYZ Data CFG register in accelerometer.
    */

    if (ptAcc->accelerometer_config == 0)
        return ACC_CTRL_ERROR;

    ret = set_stand_by_mode(ptAcc);

    if (ptAcc->ACCDataCfgReg.HPF_status)
        reg_xyz_data_cfg_value = reg_xyz_data_cfg_value | REG_XYZ_DATA_CFG_HPF;

    switch (ptAcc->ACCDataCfgReg.AccSensibility)
    {
        case AccSensibility_2g:
            break;

        case AccSensibility_4g:
            reg_xyz_data_cfg_value = reg_xyz_data_cfg_value | 0x01;
            break;

        case AccSensibility_8g:
            reg_xyz_data_cfg_value = reg_xyz_data_cfg_value | 0x02;
            break;

        default:
            return ACC_CTRL_ERROR;
    }

    if (ret == ACC_CTRL_SUCCESS)
        ret = single_byte_write_register(ptAcc, RED_ADDR_XYZ_DATA_CFG, reg_xyz_data_cfg_value);

    if (ret == ACC_CTRL_SUCCESS)
        ret = set_active_mode(ptAcc);

    return ret;
}

/**Set stand by mode on\n
        This function sets accelerometer's stand-by mode on.
        @param[in] ptAcc - pointer to accelerometer structure variable
        @return Success or fail (ACC_CTRL_SUCCESS or ACC_CTRL_ERROR)
*/
int set_stand_by_mode(TACCCtrl * ptACC)
{
    unsigned char reg_ctrl_reg1_value = 0x00;
    int ret = ACC_CTRL_SUCCESS;

    ret = single_byte_read_register(ptACC, RED_ADDR_CTRL_REG1, &reg_ctrl_reg1_value);

    if (ret == ACC_CTRL_SUCCESS)
    {
        reg_ctrl_reg1_value = reg_ctrl_reg1_value & 0xFE;
        ret = single_byte_write_register(ptACC, RED_ADDR_CTRL_REG1, reg_ctrl_reg1_value);
    }

    return ret;
}

/**Set active mode on\n
        This function sets accelerometer's active mode on.
        @param[in] ptAcc - pointer to accelerometer structure variable
        @return Success or fail (ACC_CTRL_SUCCESS or ACC_CTRL_ERROR)
*/
int set_active_mode(TACCCtrl * ptACC)
{
    unsigned char reg_ctrl_reg1_value = 0x00;
    int ret = ACC_CTRL_SUCCESS;

    ret = single_byte_read_register(ptACC, RED_ADDR_CTRL_REG1, &reg_ctrl_reg1_value);

    if (ret == ACC_CTRL_SUCCESS)
    {
        reg_ctrl_reg1_value = reg_ctrl_reg1_value | 0x01;
        ret = single_byte_write_register(ptACC, RED_ADDR_CTRL_REG1, reg_ctrl_reg1_value);
    }

    return ret;
}

/**Change ODR (output Data Rate\n
        This function changes the ODR accordingly accelerometer structure variable
        @param[in] ptAcc - pointer to accelerometer structure variable
        @return Success or fail (ACC_CTRL_SUCCESS or ACC_CTRL_ERROR)
*/
int change_ODR(TACCCtrl * ptACC)
{
    int ret = ACC_CTRL_SUCCESS;
    unsigned char reg_ctrl_reg1_value = 0x00;

    ret = set_stand_by_mode(ptACC);

    if (ret == ACC_CTRL_SUCCESS)
       ret = single_byte_read_register(ptACC, RED_ADDR_CTRL_REG1, &reg_ctrl_reg1_value);

    if (ret == ACC_CTRL_SUCCESS)
    {
        reg_ctrl_reg1_value = reg_ctrl_reg1_value & 0xC7;
        reg_ctrl_reg1_value = reg_ctrl_reg1_value | (ptACC->AccODR << 3);
        ret = single_byte_write_register(ptACC, RED_ADDR_CTRL_REG1, reg_ctrl_reg1_value);
    }

     if (ret == ACC_CTRL_SUCCESS)
        ret = set_active_mode(ptACC);

     return ret;
}
/** @} */