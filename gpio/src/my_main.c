/* my_main.c: main file for monitor */
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

#include "common.h"
/* This include will give us the CubeMX generated defines */
#include "main.h"
/* This function is called from the CubeMX generated main.c, after all
 * the HAL peripherals have been initialized. */
void my_init(void)
{
  /* Initialize the terminal system */
  TerminalInit();

  /* Print out reset source */
  WDTCheckReset();
  
  /* Initialize the task system */
  TaskingInit();

  my_Init();

}

/* This function is called from inside the CubeMX generated main.c,
 * inside the while(1) loop. */
void my_main(void)
{

  TaskingRun();  /* Run all registered tasks */
  my_Loop();

  WDTFeed();
}

ParserReturnVal_t Gpio(int mode) // Gets gpio pin and led state from user
{
  uint32_t rc,onoff;
  int pin = 0;
  int status_pin = 0; 
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

  rc = fetch_uint32_arg(&onoff);
  status_pin = onoff % 10; //split last digit from number
  pin = onoff / 10; //divide num by 10. num /= 10 also a valid one 
  onoff = status_pin;
 
 gpio(pin, onoff); 

  if(rc) {
    printf("Enter correct gpio pin!\n");
    return CmdReturnBadParameter1;
  }

  return CmdReturnOk;
}

ADD_CMD("gpio",Gpio,"0 | 1           Control LED")

