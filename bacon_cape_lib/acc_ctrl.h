/** @brief Header file from Accelerometer (MMA8452Q) control module  (bacon cape)\n\n

    Link to MMA8452Q datasheet: https://www.nxp.com/docs/en/data-sheet/MMA8452Q.pdf
    @author Pedro Bertoleti
    @date September, 2018
*/
#ifndef ACCCTRL_H
#define ACCCTRL_H

#define ACCELEROMETER_BASE_ADDR  0x1C
#define ADDR_SELECT              0x00  /* select = 0x00 cfor 0x1C address and select = 0x01 for 0x1D address */
#define ACCELEROMETER_ADDR       ACCELEROMETER_BASE_ADDR|ADDR_SELECT

#define ACC_CTRL_ERROR         -1
#define ACC_CTRL_SUCCESS        0

/* Sensibility values */
#define ACC_SENSIBILITY_2G      0
#define ACC_SENSIBILITY_4G      1
#define ACC_SENSIBILITY_8G      2

/** @brief Accelerometer ready status
*/
/* Enum - accelerations */
typedef enum
{
/** AccStatus_ready indicates that accelerometer is ready to operate
*/
    AccStatus_ready = 0,
/** AccStatus_not_ready indicates that accelerometer isn't ready to operate
*/
    AccStatus_not_ready
}TAccReadyStatus;





/** @brief Accelerometer operation modes
*/
/* Enum - accelerometer mode */
typedef enum
{
/** AccMode_standby indicates that accelerometer is in stand-by operation mode
*/
    AccMode_standby = 0,
/** AccMode_wake indicates that accelerometer is in wake-up operation mode
*/
    AccMode_wake,
/** AccMode_sleep indicates that accelerometer is in sleep operation mode
*/
    AccMode_sleep,
    AccMode_Invalid
}TAccMode;



/** @brief Accelerometer available sensibility configururations
*/
/* Enum - accelerometer sensibility */
typedef enum
{
/** AccSensibility_2g corresponds to the configuration of +-2g max acceleration allowed
*/
    AccSensibility_2g = 0,
/** AccSensibility_4g corresponds to the configuration of +-4g max acceleration allowed
*/
    AccSensibility_4g,
/** AccSensibility_8g corresponds to the configuration of +-8g max acceleration allowed
*/
    AccSensibility_8g,
    AccSensibility_invalid,
}TAccSensibility;


/** @brief Accelerometer available ODR (Output Data Rate)
*/
/* Enum - accelerometer sensibility */
typedef enum
{
/** ODR_800HZ corresponds to the 800Hz ODR
*/
    AccODR_800HZ = 0,
/** ODR_400HZ corresponds to the 400Hz ODR
*/
    AccODR_400HZ,
/** ODR_200HZ corresponds to the 200Hz ODR
*/
    AccODR_200HZ,
/** ODR_100HZ corresponds to the 100Hz ODR
*/
    AccODR_100HZ,
/** ODR_50HZ corresponds to the 50Hz ODR
*/
    AccODR_50HZ,
/** ODR_12_5_HZ corresponds to the 12.5Hz ODR
*/
    ODR_12_5_HZ,
/** ODR_6_25_HZ corresponds to the 6.25Hz ODR
*/
    ODR_6_25_HZ,
/** ODR_1_56_HZ corresponds to the 1.56Hz ODR
*/
    ODR_1_56_HZ,
    AccODR_invalid,
}TAccODR;



/** @brief Accelerometer interrupt source register (parsed register)
*/
/* Interrupt source register structure */
typedef struct {
/** data_ready indicates if a interrupt from new accelerometer data ready was triggered
*/
    char data_ready;         //bit 0
                             //bit 1 - not used
/** freefall_motion indicates if a free-fall or motion interrupt was triggered
*/
    char freefall_motion;    //bit 2
/** pulse indicates if a pulse interrupt was triggered
*/
    char pulse;              //bit 3
/** landscape_portrait indicates if a orientation change interrupt (landscape/portrait) was triggered
*/
    char landscape_portrait; //bit 4
/** transient indicates if a transient interrupt was triggered
*/
    char transient;          //bit 5
                             //bit 6 - not used
/** autosleep_wake indicates if a auto-sleep or wake interrupt was triggered
*/
    char autosleep_wake;     //bit 7
}TACCIntSrcReg;





/** @brief Accelerometer XYZ_DATA_CFG (parsed register)
*/
/* XYZ_DATA_CFG register structure */
typedef struct {
/** AccSensibility contains the sensibility configuration to write or read from accelerometer
*/
    TAccSensibility AccSensibility;
/** HPF_status contains the high-pass filter configuration to write or read from accelerometer
*/
    char HPF_status;
}TACCDataCfgReg;





/** @brief Accelerometer control structure.\n\n
    This is the accelerometer control structure, a structure that defines everything regarding to its configuration and usage.
*/
typedef struct {
/** accelerometer_config is the accelerometer config flag. It indicates if accelerometer communication (I2C) is already configured (1) or not (0).\n
    Operations on accelerometer will only take action when accelerometer_config is equal to 1.
*/
    int accelerometer_config;
/** accelerometer_addr is the accelerometer I2C Address.\n
    It's configured by user program, allowing multiple accelerometers handling (if necessary)
*/
    int accelerometer_addr;
/** i2c_descriptor for the accelerometer I2C file.
*/
    int i2c_descriptor;
/** accelerometer_mode stores the most recent accelerometer mode read from accelerometer device
*/
    int accelerometer_mode;
/** accelerations_ready indicates if accelerometer is ready to operate or not
*/
    int accelerations_ready;
/** accelerometer_x_raw stores the most recent raw X-axis acceleration read from accelerometer device
*/
    int accelerometer_x_raw;
/** accelerometer_y_raw stores the most recent raw Y-axis acceleration read from accelerometer device
*/
    int accelerometer_y_raw;
/** accelerometer_z_raw stores the most recent raw Z-axis acceleration read from accelerometer device
*/
    int accelerometer_z_raw;
/** ACCIntSrcReg stores the most recent interrupt source value read from accelerometer interrupt source register
*/
    TACCIntSrcReg ACCIntSrcReg;
/** ACCDataCfgReg stores the most recent XYZ_DATA_CFG value read from accelerometer XYZ_DATA_CFG register
*/
    TACCDataCfgReg ACCDataCfgReg;
/** AccODR stores the Output Data Rate (ODR) of accelerometer
*/
    TAccODR AccODR;
}TACCCtrl;



/* Prototypes */
int setup_accelerometer(TACCCtrl * ptACC);
int close_accelerometer(TACCCtrl * ptACC);
int read_raw_accelerations_x_y_z(TACCCtrl * ptACC);
int write_xyz_data_cfg(TACCCtrl * ptACC);
#endif
