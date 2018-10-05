/** @brief Header file containing the registers (and co-relates definitions) from Accelerometer (MMA8452Q) \n\n
    @author Pedro Bertoleti
    @date September, 2018
*/
#ifndef ACCREGS_H
#define ACCREGS_H

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
#define REG_ADDR_XYZ_DATA_CFG          0x0E
#define REG_ADDR_CTRL_REG1             0x2A

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

#endif
