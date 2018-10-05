/* Program sample for lib's validation only */
/*
 * Author: Pedro Bertoleti
 */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../bacon_cape_lib/acc_ctrl.h"

#define MAX_ACCELERATION   (4096/2)  /* 12-bits of resolution (dual polarity) */

/* constants  */
const int acc_factor[3] = {2,4,8}; /* 2g, 4g and 8g factors */

/* Main program */
int main(void)
{
   TACCCtrl Acc;
   float acc_x = 0.0;
   float acc_y = 0.0;
   float acc_z = 0.0;

   /* Fill information about accelerometer */
   Acc.accelerometer_config = 0;
   Acc.accelerometer_addr = ACCELEROMETER_ADDR;
   Acc.ACCDataCfgReg.AccSensibility = AccSensibility_2g;
   Acc.ACCDataCfgReg.HPF_status = 0;


   /* Setup accelerometer */
   printf("\n\r[STATUS] Setting up accelerometer...\n\n");
   if (setup_accelerometer(&Acc) != ACC_CTRL_SUCCESS)
   {
       printf("\n\r[ERROR] cannot setup accelerometer communication\n\n");
       return -1;
   }
   printf("\n\r[STATUS] Accelerometer up and running\n\n");

   printf("\n\r[ACCELEROMETER TEST] - press ctrl + c to exit\n\n");

   while(1)
   {
        /* read raw accelerations and print them where they were successfully read */
        while (read_raw_accelerations_x_y_z(&Acc) != ACC_CTRL_SUCCESS);

        /* calculate the read acceleration */
        acc_x = ((float)Acc.accelerometer_x_raw/MAX_ACCELERATION)*acc_factor[Acc.ACCDataCfgReg.AccSensibility];
        acc_y = ((float)Acc.accelerometer_y_raw/MAX_ACCELERATION)*acc_factor[Acc.ACCDataCfgReg.AccSensibility];
        acc_z = ((float)Acc.accelerometer_z_raw/MAX_ACCELERATION)*acc_factor[Acc.ACCDataCfgReg.AccSensibility];

        printf("\rAcc X = %1.2fg ; Acc Y = %1.2fg ; Acc Z = %1.2fg ", acc_x, acc_y, acc_z);
        //printf("\rRaw Acc. X = %04d ; Raw Acc. Y = %04d ; Raw Acc. Z = %04d ", Acc.accelerometer_x_raw, Acc.accelerometer_y_raw, Acc.accelerometer_z_raw);
   }

   close_accelerometer(&Acc);
   return 0;
}
