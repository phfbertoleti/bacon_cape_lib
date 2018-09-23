/* Program sample for lib's validation only */
/*
 * Author: Pedro Bertoleti
 */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../bacon_cape_lib/accelerometer_ctrl.h"


/* Main program */
int main(void)
{
   TACCCtrl Acc;

   /* Fill information about accelerometer */
   Acc.accelerometer_config = 0;
   Acc.accelerometer_addr = ACCELEROMETER_ADDR;
   Acc.AccODR = ODR_1_56_HZ;
   Acc.ACCDataCfgReg.AccSensibility = AccSensibility_2g;

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
        printf("\rrRaw Acc. X = %d ; Raw Acc. Y = %d ; Raw Acc. Z = %d", Acc.accelerometer_x_raw, Acc.accelerometer_y_raw, Acc.accelerometer_z_raw);
   }

   close_accelerometer(&Acc);
   return 0;
}
