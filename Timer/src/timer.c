/* example.c:
 *
 *   Template code for implementing a simple task, using the ADD_TASK()
 *   macro.  Also contains template code for a simple monitor command.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx_it.h"
#include "common.h"
#include "stm32f4xx_hal_def.h"

TIM_HandleTypeDef htim3;

void ExampleInit(void *data)
{

  /* Place Initialization things here.  This function gets called once
   * at startup.
   */

}

void ExampleTask(void *data)
{

  /* Place your task functionality in this function.  This function
   * will be called repeatedly, as if you placed it inside the main
   * while(1){} loop.
   */

}

void delay_us(uint16_t us)
{
  TIM1->CNT = 0; //Set the counter to 0
  while(TIM1->CNT < us);
  printf("%ld\n",TIM1->CNT);
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {      
  // Check which version of the timer triggered this callback and toggle LED
  if (htim == &htim3 )
  {
  }
}
void delay_ms(uint16_t ms)
{
  for(uint16_t i =0;i<ms;i++)
  {
    delay_us(1000);
   // HAL_TIM_Base_Start_IT(&htim3);
  }
}

void TIM1_Init()
 {
  RCC->APB2ENR |= (1<<0); //Enable timer 1 clock

  TIM1->PSC = 100-1; //100mhz/100 -- 1 us delay

  TIM1->ARR = 0xffff; //Max ARR

  TIM1->CR1 |= (1<<0);//Enable the counter

  while (!(TIM1->SR & (1<<0)));//Update interrupt flag

  RCC->AHB1ENR |= (1<<0); //Clock enable for GPIOA
  GPIOA->MODER |= (1<<10);//pin PA8 is set as output
  GPIOA->OTYPER &= ~(1<<8);
  GPIOA->OSPEEDR |= (1<<17);
  GPIOA->PUPDR &= ~((1<<16) | (1<<17));
  
  printf("Timer 1 successfully initialized!");
 }

void TIM3_Init()
{
  RCC->APB1ENR |= (1<<1);

  TIM3->PSC = 100-1; //100mhz/100 -- 1 us delay
  

  TIM3->ARR = 0xffff; //Max ARR

  TIM3->CR1 |= (1<<0);//Enable the counter


  while (!(TIM3->SR & (1<<0)));//Update interrupt flag 
  RCC->AHB1ENR |= (1<<0); //Clock enable for GPIOA
  GPIOA->MODER |= (1<<12);//pin PA6 is set as output
  GPIOA->OTYPER &= ~(1<<6);//output typer register as pa6
  GPIOA->OSPEEDR |= (1<<12);
  GPIOA->PUPDR &= ~((1<<12) | (1<<13));
   
 // HAL_NVIC_SetPriority();
 // HAL_NVIC_EnableIRQ();

}


ADD_TASK(ExampleTask,  /* This is the name of the function for the task */
	 ExampleInit,  /* This is the initialization function */
	 NULL,         /* This pointer is passed as 'data' to the functions */
	 0,            /* This is the number of milliseconds between calls */
	 "This is the help text for the task")
  
ParserReturnVal_t TimerInitialize(int mode)
{
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

   TIM1_Init();
   TIM3_Init();
  /* Put your command implementation here */
  printf("Timer is Initialize!\n");
  printf("\nEnter the Timer Delay!\n");
  
  return CmdReturnOk;
}
ADD_CMD("timerinit",TimerInitialize,"                Timer Initialize")

ParserReturnVal_t Timer_Delay(int mode)
{
  uint32_t rc=0;
  uint16_t delayset;
  uint16_t pinState;

  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

  /* Put your command implementation here */
  rc =  fetch_uint16_arg(&delayset);
  if(rc){
  printf("Example Command\n");
  return CmdReturnBadParameter1;
  }

  while(1)
  {
    pinState = ~GPIOC->IDR;
    
    if(pinState & (1<<13)){
      break;
    }
    IWDG->KR |= 0x0000AAAA; 

    GPIOA->BSRR |= (1<<5);
    delay_ms(delayset);
    GPIOA->BSRR |= (1<<21);  
    delay_ms(delayset);

    }
  return CmdReturnOk;
}



ADD_CMD("timerdelay",Timer_Delay,"                Example Command")
