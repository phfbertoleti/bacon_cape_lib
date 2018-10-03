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
#include "acc_ctrl.h"
#include "acc_regs.h"
#include "general_functions.h"

/** @addtogroup Accelerometer_control
@{
*/


/* General / i2c defines */
#define ACCELEROMETER_FILE          "/dev/i2c-2"
#define WAIT_TIME_ACCELEROMETER     10000  /* 10000us = 10ms */
#define ACC_IDENTITY_VALUE          0x2A

/* Local prototypes */
int standby_mode(TACCCtrl * ptACC);
int active_mode(TACCCtrl * ptACC);

/**Stand-by mode\n
        This function configures accelerometer's operation mode to stand-by mode.

        @param[in] ptACC - pointer to accelerometer structure variable
        @return Success or fail (ACC_CTRL_SUCCESS or ACC_CTRL_ERROR)
*/
int standby_mode(TACCCtrl * ptACC)
{
    char buffer_standby[2] = {0};
    int bytes_written = 0;
    int ret = ACC_CTRL_SUCCESS;

    buffer_standby[0] = RED_ADDR_CTRL_REG1;
    buffer_standby[1] = 0x00;
    bytes_written = write(ptACC->i2c_descriptor, buffer_standby, 2);

    if (bytes_written != 2)
        ret = ACC_CTRL_ERROR;

    return ret;
}

/**Active mode\n
        This function configures accelerometer's operation mode to active mode.

        @param[in] ptACC - pointer to accelerometer structure variable
        @return Success or fail (ACC_CTRL_SUCCESS or ACC_CTRL_ERROR)
*/
int active_mode(TACCCtrl * ptACC)
{
    char buffer_active[2] = {0};
    int bytes_written = 0;
    int ret = ACC_CTRL_SUCCESS;

    buffer_active[0] = RED_ADDR_CTRL_REG1;
    buffer_active[1] = 0x01;
    bytes_written = write(ptACC->i2c_descriptor, buffer_active, 2);

    if (bytes_written != 2)
        ret = ACC_CTRL_ERROR;

    return ret; 
}

/**Setup accelerometer\n
        This function initializes accelerometer communication (I2C). This function must be called before any other accelerometer function.

        @param[in] ptACC - pointer to accelerometer structure variable
        @return Success or fail (ACC_CTRL_SUCCESS or ACC_CTRL_ERROR)
*/
int setup_accelerometer(TACCCtrl * ptACC)
{
    int ret[20];
    int ret_final;
    int idx_ret = 0;

    /* If accelerometer has been already configured, there's nothing to do here */
    if (ptACC->accelerometer_config != 0)
        return ACC_CTRL_ERROR;

    /* Check accelerometer access (considering read and write operations) */
    if ((ptACC->i2c_descriptor = open(ACCELEROMETER_FILE, O_RDWR)) < 0)
    {
        ptACC->accelerometer_config = 0;
        return ACC_CTRL_ERROR;
    }
    else
    {
        ptACC->accelerometer_addr = ACCELEROMETER_ADDR;

        /* Try to obtain I2C bus access */
        if (ioctl(ptACC->i2c_descriptor, I2C_SLAVE, ptACC->accelerometer_addr) < 0)
            return ACC_CTRL_ERROR;
        else
        {
            /* So far, so good */
            ptACC->accelerometer_config = 1;

            ret[idx_ret] = standby_mode(ptACC);
            idx_ret++;

            ret[idx_ret] = active_mode(ptACC);
            idx_ret++;

            ptACC->accelerometer_x_raw = 0;
            ptACC->accelerometer_y_raw = 0;
            ptACC->accelerometer_z_raw = 0;
            ptACC->accelerations_ready = AccStatus_not_ready;
        }
    }

    ret_final = ret_min(ret,idx_ret);

    if (ret_final == ACC_CTRL_ERROR)
        ptACC->accelerometer_config = 0;

    return ret_final;
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

/**Read raw accelerations (X, Y and Z axis)\n
        This function reads raw accelerations (X, Y and Z axis) and stores these information in accelerometer structure variable.\n
        It only return success when there's acceleration to be read from MMA8452Q device.

        @param[in] ptACC - pointer to accelerometer structure variable
        @return Success or fail (ACC_CTRL_SUCCESS or ACC_CTRL_ERROR)
*/
int read_raw_accelerations_x_y_z(TACCCtrl * ptACC)
{
    int ret = ACC_CTRL_SUCCESS;
    char reg[1] = {REG_ADDR_STATUS};
    char data[7] = {0};
    int bytes_written = 0;
    int acc_x = 0;
    int acc_y = 0;
    int acc_z = 0;

    bytes_written = write(ptACC->i2c_descriptor, reg, 1);

    if (bytes_written != 1)
        return ACC_CTRL_ERROR;

    if (read(ptACC->i2c_descriptor, data, 7) != 7)
        ret = ACC_CTRL_ERROR;
    else
    {
        acc_x = ((data[1] * 256) + data[2]) / 16;

        /* check if acc_x should be negative */
        if(acc_x > 2047)
            acc_x -= 4096;

        acc_y = ((data[3] * 256) + data[4]) / 16;

        /* check if acc_y should be negative */
        if(acc_y > 2047)
            acc_y -= 4096;

        acc_z = ((data[5] * 256) + data[6]) / 16;

        /* check if acc_z should be negative */
        if(acc_z > 2047)
            acc_z -= 4096;

        /* store accelerations in accelerometer structure variable */
        ptACC->accelerometer_x_raw = acc_x;
        ptACC->accelerometer_y_raw = acc_y;
        ptACC->accelerometer_z_raw = acc_z;
        ret = ACC_CTRL_SUCCESS;
    }

    return ret;
}

/** @} */