#include "systick.h"
#include "stdio.h"
/**counter definition**/

void Enable_Systick()
{
  int *systick_control_register= (int *)(0xE000E010);
  int enable_bit_no = 0x5;
  int tmp;


  tmp = *systick_control_register;
  tmp = tmp | enable_bit_no;

  *systick_control_register = tmp;
  
}


void Systick_Reload()
{

    int *systick_reload_register= (int *)(0xE000E014);
    int Reload_val = 0xFFFFFF;  // The max value is equal to 0xFFFFFF
    *systick_reload_register = Reload_val;
}

int systick_curr_reg_read()
{
    int *systick_current_register= (int *)(0xE000E018);
    int value;
    value =  *systick_current_register;
    return value;
}

void systick_curr_reg_clean()
{
int *systick_current_register= (int *)(0xE000E018);
*systick_current_register = (int)(0x0FFFFFF);
}


/**  counter usage **/

