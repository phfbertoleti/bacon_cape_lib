/** @brief Gerneral/common functions to all bacon cape lib modules
    @author Pedro Bertoleti
    @date September, 2018
*/

/** @addtogroup General_functions
@{
*/

/**Return minimal system call return value\n
        This function return the minimal system call return value (of an array of these values)\n
        This is used to determine if one or more system calls has been failed.

        @param[in] SysReqs - pointer to array containing system call returns
        @param[in] length - number of syscall returns to analyse
        @return Mnimal value present in system calls returns array
*/
int ret_min(int * SysReqs, int length)
{
    int i;
    int min = SysReqs[0];

    for(i=1; i<length; i++)
    {
         if (SysReqs[i] < min)
         	min = SysReqs[i];
    }

    return min;
}
/** @} */